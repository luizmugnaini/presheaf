--                             Presheaf library
-- Copyright (C) 2024 Luiz Gustavo Mugnaini Anselmo
--
-- Permission is hereby granted, free of charge, to any person obtaining a copy of
-- this software and associated documentation files (the “Software”), to deal in
-- the Software without restriction, including without limitation the rights to
-- use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
-- of the Software, and to permit persons to whom the Software is furnished to do
-- so, subject to the following conditions:
--
-- The above copyright notice and this permission notice shall be included in all
-- copies or substantial portions of the Software.
--
-- THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
-- IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
-- FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
-- AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
-- LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
-- OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
-- SOFTWARE.
--
-- Description: Build system for the presheaf library.
-- Author: Luiz G. Mugnaini A. <luizmugnaini@gmail.com>
--
-- Running the build system:  lua build.lua [options]
-- Example:                   lua build.lua fmt clang test
--
-- You can also pass ad-hoc flags directly to the compiler command. Any argument passed
-- after `--` will be directed to the compilation command. For instance:
--
-- lua build.lua clang -- -fsanitize=address -DPSH_ABORT_AT_MEMORY_ERROR
--

local start_time = os.time()

-- -----------------------------------------------------------------------------
-- Available command line options
-- -----------------------------------------------------------------------------

local options = {
    dll     = { on = false, description = "Build as a DLL (shared object)." },
    release = { on = false, description = "Release build type (on by default)." },
    debug   = { on = false, description = "Debug build type (off by default)." },
    test    = { on = false, description = "Build and run tests." },
    fmt     = { on = false, description = "Format source files with clang-format before building." },
    clang   = {
        on = false,
        description = "Use the Clang compiler (Clang-cl on Windows). If you want to build with clang-cl on Linux, use: `-msvc -clang`",
    },
    gcc     = { on = false, description = "Use the GCC compiler." },
    msvc    = { on = false, description = "Use the Visual C++ compiler." },
    help    = { on = false, description = "Print the help message for the build script." },
    quiet   = { on = false, description = "Don't print information about the build process." },
}

-- -----------------------------------------------------------------------------
-- Collect command line arguments
-- -----------------------------------------------------------------------------

-- Collect build options.
local custom_flags_idx = nil
for i = 1, #arg do
    if arg[i] == "--" then
        custom_flags_idx = i + 1
        break
    end
    local start_idx = string.find(arg[i], "[^-]")
    local opt       = string.sub(arg[i], start_idx)
    options[opt].on = true
end

if not options.release.on and not options.debug.on then
    options.release.on = true
end

-- Collect flags to be directly passed to the compiler.
local custom_compiler_flags = {}
if custom_flags_idx ~= nil then
    for i = custom_flags_idx, #arg do
        custom_compiler_flags[i - custom_flags_idx + 1] = arg[i]
    end
end

-- Print the help message.
if options.help.on then
    print("Usage:\n    lua " .. arg[0] .. " [options] -- [custom compiler flags]\n\nOptions:")
    for k, v in pairs(options) do
        print(string.format("    -%-15s" .. "%s", k, v.description))
    end
    print(
        "\nExample: Build the library with Clang, run all tests, and directly specify compiler flags:\n"
        .. "    lua build.lua -clang -test -- -fsanitize=address -DPSH_ABORT_AT_MEMORY_ERROR"
    )
    return
end

-- -----------------------------------------------------------------------------
-- Operating system information
-- -----------------------------------------------------------------------------

local os_info = {
    windows = (package.config:sub(1, 1) == "\\"),
    linux   = false,
    darwin  = false,
}

if not os_info.windows then
    local handle = io.popen("uname")
    local result = handle:read("*a")
    handle:close()

    if string.match(result, "Linux") then
        os_info.linux = true
    elseif string.match(result, "Darwin") then
        os_info.darwin = true
    end
end

if os_info.windows then
    os_info.path_sep    = "\\"
    os_info.silence_cmd = " > NUL 2>&1"
else
    os_info.path_sep    = "/"
    os_info.silence_cmd = " > /dev/null 2>&1"
end

local os_file_info = {
    unix = {
        obj        = ".o",
        exe        = "",
        dll        = ".so",
        lib        = ".a",
        lib_prefix = "lib",
    },
    windows = {
        dbg        = ".pdb",
        obj        = ".obj",
        exe        = ".exe",
        dll        = ".dll",
        lib        = ".lib",
        lib_prefix = "",
    },
}

-- -----------------------------------------------------------------------------
-- Utility functions
-- -----------------------------------------------------------------------------

local function log_info(msg)
    if not options.quiet.on then
        print("\x1b[1;35m[Presheaf INFO]\x1b[0m " .. msg)
    end
end

local function exec(cmd_str, quiet)
    if quiet then
        cmd_str = cmd_str .. os_info.silence_cmd
    else
        log_info("Executing: " .. cmd_str)
    end

    os.execute(cmd_str)
end

local function concat(arr, join, is_prefix, no_space)
    if #arr < 1 then
        return ""
    end

    if not join then
        join = ""
    end

    local space = " "
    if no_space then
        space = ""
    end

    local acc = arr[1] .. space
    for i = 2, #arr do
        if arr[i] and (arr[i] ~= "") then
            acc = acc .. join .. arr[i] .. space
        end
    end

    if is_prefix then
        acc = join .. acc -- Add the prefix to the start of the string
    end

    if not no_space then
        acc = string.sub(acc, 1, #acc - 1) -- Remove the extraneous space at the end.
    end

    return acc
end

local function make_path(arr)
    return concat(arr, os_info.path_sep, false, true)
end

local function get_script_dir()
    local info = debug.getinfo(1, "S").source:sub(2)
    return info:match("(.*)[/\\]")
end

-- -----------------------------------------------------------------------------
-- Project configuration
-- -----------------------------------------------------------------------------

local root_dir = get_script_dir()
if not root_dir then
    root_dir = "."
end

local compilers = {
    clang = {
        cc            = "clang++",
        opt_include   = "-I",
        opt_define    = "-D",
        opt_dll       = "-shared",
        opt_std       = "-std=",
        opt_no_link   = "-c",
        opt_out_obj   = "-o",
        opt_out_exe   = "-o",
        flags_common  = "-pedantic -Wall -Wextra -Wpedantic -Wuninitialized -Wcast-align -Wconversion -Wnull-pointer-arithmetic -Wnull-dereference -Wformat=2 -Wpointer-arith -Wno-unsafe-buffer-usage -fno-rtti -fno-exceptions -Werror=implicit-function-declaration",
        flags_debug   = "-Werror -Wno-unused-variable -Werror -g -O0 -fsanitize=address -fsanitize=pointer-compare -fsanitize=pointer-subtract -fsanitize=undefined -fstack-protector-strong -fsanitize=leak",
        flags_release = "-Wunused -O2",
        ar            = "llvm-ar",
        ar_out        = "",
        ar_flags      = "rcs",
    },
    gcc = {
        cc            = "g++",
        opt_include   = "-I",
        opt_define    = "-D",
        opt_dll       = "-shared",
        opt_std       = "-std=",
        opt_no_link   = "-c",
        opt_out_obj   = "-o",
        opt_out_exe   = "-o",
        flags_common  = "-pedantic -Wall -Wextra -Wpedantic -Wuninitialized -Wcast-align -Wconversion -Wnull-dereference -Wformat=2 -fno-rtti -fno-exceptions",
        flags_debug   = "-Werror -g -O0 -fsanitize=address -fsanitize=pointer-compare -fsanitize=pointer-subtract -fsanitize=undefined -fstack-protector-strong -fsanitize=leak",
        flags_release = "-O2",
        ar            = "ar",
        ar_out        = "",
        ar_flags      = "rcs",
    },
    msvc = {
        cc                   = "cl",
        opt_include          = "-I",
        opt_define           = "-D",
        opt_link_flags_start = "/link",
        opt_dll              = "/DLL",
        opt_std              = "/std:",
        opt_no_link          = "/c",
        opt_out_obj          = "/Fo",
        opt_out_exe          = "/Fe",
        opt_out_pdb          = "/Fd",
        flags_common         = "-nologo /INCREMENTAL:NO -Oi -TP -MP -FC -GF -GA /fp:except- -GR- -EHa-",
        flags_debug          = "/Ob1 /Od /Oy- /Z7 /RTC1 /MTd /W4 /WX /fsanitize=address",
        flags_release        = "/O2 /MT",
        ar                   = "lib",
        ar_out               = "/out:",
        ar_flags             = "/nologo",
    },
    clang_cl = {
        cc                   = "clang-cl",
        opt_include          = "-I",
        opt_define           = "-D",
        opt_link_flags_start = "/link",
        opt_dll              = "/DLL",
        opt_std              = "/std:",
        opt_no_link          = "-c",
        opt_out_obj          = "-o",
        opt_out_exe          = "-o",
        flags_common         = "/TP /INCREMENTAL:NO -EHa- -Wno-unsafe-buffer-usage -Wno-c++20-compat -Wno-c++98-compat-pedantic -Wno-documentation-unknown-command",
        flags_debug          = "-Ob1 /Od /Oy- /Z7 /RTC1 -g /MTd -Wall -Wextra -Werror -Wconversion -Wuninitialized -Wnull-pointer-arithmetic -Wnull-dereference -Wcast-align -Wformat=2",
        flags_release        = "-O2 /MT",
        ar                   = "llvm-lib",
        ar_out               = "/out:",
        ar_flags             = "/nologo",
    },
}

local presheaf = {
    src              = make_path({ root_dir, "src", "presheaf.cpp" }),
    test_src         = make_path({ root_dir, "tests", "test_presheaf.cpp" }),
    include_dir      = make_path({ root_dir, "include" }),
    dll_build_define = "PSH_BUILD_DLL",
    debug_defines    = { "PSH_DEBUG" },
    test_defines     = { "PSH_DEBUG", "PSH_ENABLE_ANSI_COLORS" },
    lib              = "presheaf",
    test_exe         = "presheaf_tests",
    std              = "c++20",
    out_dir          = make_path({ ".", "build" }),
}

-- -----------------------------------------------------------------------------
-- Toolchain
-- -----------------------------------------------------------------------------

local target_windows = os_info.windows or options.msvc.on

-- Defaults per platform.
local toolchain = target_windows and compilers.msvc or compilers.gcc
local os_ext    = target_windows and os_file_info.windows or os_file_info.unix

if options.clang.on then
    toolchain = target_windows and compilers.clang_cl or compilers.clang
elseif options.gcc.on then
    assert(not os_info.windows, "GCC build not supported on Windows")
    toolchain = compilers.gcc
elseif options.msvc.on then
    toolchain = compilers.msvc
end

-- -----------------------------------------------------------------------------
-- Execute build instructions
-- -----------------------------------------------------------------------------

local function prepare_output_target()
    local mkdir_cmd = os_info.windows and "mkdir" or "mkdir -p"
    exec(mkdir_cmd .. " " .. presheaf.out_dir, true)
end

local function format_source_files()
    log_info("Formatting source files...")
    exec(concat({
        "clang-format -i",
        make_path({ root_dir, "include", "psh", "*.hpp" }),
        make_path({ root_dir, "src", "*.cpp" }),
        make_path({ root_dir, "tests", "*.hpp" }),
        make_path({ root_dir, "tests", "*.cpp" }),
    }))
end

local function build_presheaf_lib(tc, custom_flags_)
    log_info("Building the presheaf library...")

    local custom_flags = concat(custom_flags_)
    local default_flags = tc.flags_common .. " " .. (options.release.on and tc.flags_release or tc.flags_debug)
    local defines = options.debug.on and concat(presheaf.debug_defines, tc.opt_define, true) or nil

    local output_artifacts_flags = ""
    if tc.cc == "cl" then
        output_artifacts_flags = tc.opt_out_obj
            .. make_path({ presheaf.out_dir, presheaf.lib .. os_ext.obj })
            .. " "
            .. tc.opt_out_pdb
            .. make_path({ presheaf.out_dir, presheaf.lib .. os_ext.dbg })
    elseif not options.dll.on then
        output_artifacts_flags = tc.opt_out_obj .. make_path({ presheaf.out_dir, presheaf.lib .. os_ext.obj })
    end

    if options.dll.on then
        local dll_flags
        local dll_lib_out_flag -- Windows-only thing, library with the DLL symbols to be imported by the user.
        if target_windows then
            dll_flags        = "/LD"
            dll_lib_out_flag = "/IMPLIB:" .. make_path({ presheaf.out_dir, presheaf.lib .. "dll" .. ".lib" })
        else
            dll_flags = "-shared -fPIC"
        end

        local dll_out = make_path({ presheaf.out_dir, os_ext.lib_prefix .. presheaf.lib .. os_ext.dll })
        exec(concat({
            tc.cc,
            tc.opt_std .. presheaf.std,
            default_flags,
            custom_flags,
            defines,
            tc.opt_define  .. presheaf.dll_build_define,
            tc.opt_include .. presheaf.include_dir,
            output_artifacts_flags,
            dll_lib_out_flag,
            tc.opt_out_exe .. dll_out,
            dll_flags,
            presheaf.src,
        }))
    else
        local obj_out = make_path({ presheaf.out_dir, presheaf.lib .. os_ext.obj })

        -- Compile without linking.
        exec(concat({
            tc.cc,
            tc.opt_no_link,
            tc.opt_std .. presheaf.std,
            default_flags,
            custom_flags,
            defines,
            tc.opt_include .. presheaf.include_dir,
            output_artifacts_flags,
            presheaf.src,
        }))

        -- Archive objs into a library.
        local lib_out = make_path({ presheaf.out_dir, os_ext.lib_prefix .. presheaf.lib .. os_ext.lib })
        exec(concat({ tc.ar, tc.ar_flags, tc.ar_out .. lib_out, obj_out }))
    end
end

local function build_presheaf_tests(tc)
    log_info("Building the presheaf library tests...")

    local default_flags = tc.flags_common .. " " .. tc.flags_debug
    local custom_flags  = concat(custom_compiler_flags)
    local defines       = concat(presheaf.test_defines, tc.opt_define, true)

    local out_obj_flag = ""
    if tc.cc == "cl" then
        out_obj_flag = tc.opt_out_obj .. make_path({ presheaf.out_dir, presheaf.test_exe .. os_ext.obj })
    end

    local test_exe_out = make_path({ presheaf.out_dir, presheaf.test_exe .. os_ext.exe })
    exec(concat({
        tc.cc,
        tc.opt_std .. presheaf.std,
        default_flags,
        custom_flags,
        defines,
        tc.opt_include .. presheaf.include_dir,
        out_obj_flag,
        tc.opt_out_exe .. test_exe_out,
        presheaf.test_src,
    }))
    return test_exe_out
end

if options.fmt.on then
    format_source_files()
end

prepare_output_target()
build_presheaf_lib(toolchain, custom_compiler_flags)

if options.test.on then
    local test_exe = build_presheaf_tests(toolchain)
    exec(test_exe)
end

local end_time = os.time()
log_info(string.format("Time elapsed: %.5f seconds", os.difftime(end_time, start_time)))
