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
-- Example:                   lua build.lua fmt clang mold test
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

-- Available command line options.
local options = {
    -- Format source files.
    fmt = false,
    -- Build type (default: debug build).
    release = false,
    -- Compiler of choice (default: MSVC on Windows and GCC on Linux).
    clang = false,
    gcc = false,
    msvc = false,
    -- Build and run tests.
    test = false,
    -- Whether or not to print the commands ran by the build script and their output.
    quiet = false,
}

local custom_flags_idx = nil
for i = 1, #arg do
    if arg[i] == "--" then
        custom_flags_idx = i + 1
        break
    end
    options[arg[i]] = true
end

local custom_compiler_flags = {}
if custom_flags_idx ~= nil then
    for i = custom_flags_idx, #arg do
        custom_compiler_flags[i - custom_flags_idx + 1] = arg[i]
    end
end

-- -----------------------------------------------------------------------------
-- Operating system information
-- -----------------------------------------------------------------------------

local os_info = {
    windows = (package.config:sub(1, 1) == "\\"),
    linux = false,
    darwin = false,
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
    os_info.path_sep = "\\"
    os_info.silence_cmd = " > NUL 2>&1"
    os_info.obj_ext = ".obj"
    os_info.lib_ext = ".lib"
    os_info.exe_ext = ".exe"
else
    os_info.path_sep = "/"
    os_info.silence_cmd = " > /dev/null 2>&1"
    os_info.obj_ext = ".o"
    os_info.lib_ext = ".a"
    os_info.exe_ext = ""
end

-- -----------------------------------------------------------------------------
-- Utility functions
-- -----------------------------------------------------------------------------

local function log_info(msg)
    if not options.quiet then
        print("\x1b[1;35m[INFO]\x1b[0m " .. msg)
    end
end

local function exec(cmd_str, quiet)
    if quiet then
        os.execute(cmd_str .. os_info.silence_cmd)
    else
        log_info("Executing: " .. cmd_str)
        os.execute(cmd_str)
    end
end

local function concat(arr, join, is_prefix)
    if #arr < 1 then
        return ""
    end

    local acc = arr[1]
    for i = 2, #arr do
        if arr[i] and (arr[i] ~= "") then -- Skip empty strings
            acc = acc .. join .. arr[i]
        end
    end

    if is_prefix then
        acc = join .. acc
    end

    return acc
end

local function make_path(arr)
    return concat(arr, os_info.path_sep, false)
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
        cc = "clang++",
        opt_include = "-I",
        opt_define = "-D",
        opt_std = "-std=",
        opt_no_link = "-c",
        opt_out_obj = "-o",
        opt_out_exe = "-o",
        flags_common = "-pedantic -Wall -Wextra -Wpedantic -Wuninitialized -Wcast-align -Wconversion -Wnull-pointer-arithmetic -Wnull-dereference -Wformat=2 -Wpointer-arith -Wno-unsafe-buffer-usage -fno-rtti -fno-exceptions -Werror=implicit-function-declaration",
        flags_debug = "-Wno-unused-variable -Werror -g -O0 -fsanitize=address -fsanitize=pointer-compare -fsanitize=pointer-subtract -fsanitize=undefined -fstack-protector-strong -fsanitize=leak",
        flags_release = "-Wunused -O2",
        ar = "llvm-ar",
        ar_out = "",
        ar_flags = "rcs",
    },
    gcc = {
        cc = "g++",
        opt_include = "-I",
        opt_define = "-D",
        opt_std = "-std=",
        opt_no_link = "-c",
        opt_out_obj = "-o",
        opt_out_exe = "-o",
        flags_common = "-pedantic -Wall -Wextra -Wpedantic -Wuninitialized -Wcast-align -Wconversion -Wnull-dereference -Wformat=2 -Wno-unused-variable -fno-rtti -fno-exceptions",
        flags_debug = "-Werror -g -O0 -fsanitize=address -fsanitize=pointer-compare -fsanitize=pointer-subtract -fsanitize=undefined -fstack-protector-strong -fsanitize=leak",
        flags_release = "-O2",
        ar = "ar",
        ar_out = "",
        ar_flags = "rcs",
    },
    msvc = {
        cc = "cl",
        opt_include = "/I",
        opt_define = "/D",
        opt_std = "/std:",
        opt_no_link = "/c",
        opt_out_obj = "/Fo:",
        opt_out_exe = "/Fe:",
        flags_common = "-nologo -Oi -TP -MP -FC -GF -GA /fp:except- -GR- -EHsc- /INCREMENTAL:NO /W3",
        flags_debug = "/Ob0 /Od /Oy- /Z7 /RTC1 /MTd",
        flags_release = "/O2 /MT",
        ar = "lib",
        ar_out = "/out:",
        ar_flags = "/nologo",
    },
    clang_cl = {
        cc = "clang-cl",
        opt_include = "/I",
        opt_define = "/D",
        opt_std = "/std:",
        opt_no_link = "-c",
        opt_out_obj = "-o",
        opt_out_exe = "-o",
        flags_common = "/TP -Wall -Wextra -Wconversion -Wuninitialized -Wnull-pointer-arithmetic -Wnull-dereference -Wcast-align -Wformat=2 -Wno-unused-variable -Wno-missing-prototypes -Wno-unsafe-buffer-usage -Wno-c++20-compat -Wno-c++98-compat-pedantic",
        flags_debug = "-Ob0 /Od /Oy- /Z7 /RTC1 -g /MTd",
        flags_release = "-O2 /MT",
        ar = "llvm-lib",
        ar_out = "/out:",
        ar_flags = "/nologo",
    },
}

local presheaf = {
    src = make_path({ root_dir, "src", "all.cpp" }),
    test_src = make_path({ root_dir, "tests", "test_all.cpp" }),
    include_dir = make_path({ root_dir, "include" }),
    debug_defines = { "PSH_DEBUG", "PSH_ABORT_AT_MEMORY_ERROR" },
    lib = os_info.windows and "presheaf" or "libpresheaf",
    test_exe = "test_all",
    std = "c++20",
    out_dir = make_path({ ".", "build" }),
}

-- -----------------------------------------------------------------------------
-- Toolchain
-- -----------------------------------------------------------------------------

local tc = os_info.windows and compilers.msvc or compilers.gcc
if options.clang then
    tc = os_info.windows and compilers.clang_cl or compilers.clang
elseif options.gcc then
    assert(not os_info.windows, "GCC build not supported on Windows")
    tc = compilers.gcc
elseif options.msvc then
    tc = compilers.msvc
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
    }, " "))
end

local function build_presheaf_lib()
    log_info("Building the presheaf library...")

    local default_flags = tc.flags_common .. " " .. (options.release and tc.flags_release or tc.flags_debug)
    local custom_flags = concat(custom_compiler_flags, " ")
    local defines = options.release and "" or concat(presheaf.debug_defines, " " .. tc.opt_define, true)

    -- Compile without linking.
    local obj_out = make_path({ presheaf.out_dir, presheaf.lib .. os_info.obj_ext })
    exec(concat({
        tc.cc,
        tc.opt_no_link,
        tc.opt_std .. presheaf.std,
        default_flags,
        custom_flags,
        defines,
        tc.opt_include .. presheaf.include_dir,
        tc.opt_out_obj .. obj_out,
        presheaf.src,
    }, " "))

    -- Archive objs into a library.
    local lib_out = make_path({ presheaf.out_dir, presheaf.lib .. os_info.lib_ext })
    exec(concat({ tc.ar, tc.ar_flags, tc.ar_out .. lib_out, obj_out }, " "))
end

local function build_presheaf_tests()
    log_info("Building the library tests...")

    local default_flags = tc.flags_common .. " " .. tc.flags_debug
    local custom_flags = concat(custom_compiler_flags, " ")
    local defines = concat(presheaf.debug_defines, " " .. tc.opt_define, true)

    local out_obj_flag = ""
    if tc.cc == "cl" then
        out_obj_flag = tc.opt_out_obj .. make_path({ presheaf.out_dir, presheaf.test_exe .. os_info.obj_ext })
    end

    local test_exe_out = make_path({ presheaf.out_dir, presheaf.test_exe .. os_info.exe_ext })
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
    }, " "))
    return test_exe_out
end

if options.fmt then
    format_source_files()
end

prepare_output_target()
build_presheaf_lib()

if options.test then
    local test_exe = build_presheaf_tests()
    exec(test_exe)
end

local end_time = os.time()
log_info(string.format("Time elapsed: %.5f seconds", os.difftime(end_time, start_time)))
