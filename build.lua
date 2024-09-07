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

for i = 1, #arg do
    options[arg[i]] = true
end

local os_windows = (package.config:sub(1, 1) == "\\")
local os_info = {}
if os_windows then
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

function exec(cmd_str, quiet)
    local cmd_res = (not (quiet or options.quiet)) and cmd_str or (cmd_str .. os_info.silence_cmd)
    if not options.quiet then
        print("\x1b[1;35mexecuting ::\x1b[0m " .. cmd_res)
    end
    os.execute(cmd_res)
end

function concat(arr, join, is_prefix)
    local acc = nil
    if is_prefix then
        acc = join .. arr[1]
    else
        acc = arr[1]
    end

    for i = 2, #arr do
        acc = acc .. join .. arr[i]
    end
    return acc
end

-- -----------------------------------------------------------------------------
-- Project configuration
-- -----------------------------------------------------------------------------

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
    src = "src/all.cc",
    test_src = "tests/test_all.cc",
    include_dir = "include",
    defines = { "_CRT_SECURE_NO_WARNINGS" },
    debug_defines = { "PSH_DEBUG" },
    lib = "libpresheaf",
    test_exe = "test_all",
    std = "c++20",
}

function presheaf_flags(tc)
    return string.format(
        "%s %s %s %s %s %s",
        tc.opt_std .. presheaf.std,
        tc.flags_common,
        options.release and tc.flags_release or tc.flags_debug,
        concat(presheaf.defines, " " .. tc.opt_define, true),
        options.release and "" or concat(presheaf.debug_defines, " " .. tc.opt_define, true),
        tc.opt_include .. presheaf.include_dir
    )
end

-- -----------------------------------------------------------------------------
-- Toolchain
-- -----------------------------------------------------------------------------

local tc = os_windows and compilers.msvc or compilers.gcc
if options.clang then
    tc = os_windows and compilers.clang_cl or compilers.clang
elseif options.gcc then
    assert(not os_windows, "GCC build not supported in Windows")
    tc = compilers.gcc
elseif options.msvc then
    tc = compilers.msvc
end

-- -----------------------------------------------------------------------------
-- Execute build instructions
-- -----------------------------------------------------------------------------

if options.fmt then
    exec("clang-format -i include/psh/*.h src/*.cc tests/*.h tests/*.cc")
end

local out_dir = "build"
local obj_out = out_dir .. os_info.path_sep .. presheaf.lib .. os_info.obj_ext
local lib_out = out_dir .. os_info.path_sep .. presheaf.lib .. os_info.lib_ext
exec("mkdir " .. out_dir, true)

-- Compile without linking.
exec(
    string.format(
        string.rep("%s ", 9),
        tc.cc,
        tc.opt_no_link,
        tc.opt_std .. presheaf.std,
        tc.flags_common,
        options.release and tc.flags_release or tc.flags_debug,
        concat(presheaf.defines, " " .. tc.opt_define, true),
        tc.opt_include .. presheaf.include_dir,
        tc.opt_out_obj .. obj_out,
        presheaf.src
    )
)
-- Archive objs into a library.
exec(string.format("%s %s %s %s", tc.ar, tc.ar_flags, tc.ar_out .. lib_out, obj_out))

if options.test then
    -- Compile tests with debug flags.
    local test_exe_out = out_dir .. os_info.path_sep .. presheaf.test_exe .. os_info.exe_ext
    exec(
        string.format(
            string.rep("%s ", 10),
            tc.cc,
            tc.opt_std .. presheaf.std,
            tc.flags_common,
            tc.flags_debug,
            concat(presheaf.defines, " " .. tc.opt_define, true),
            concat(presheaf.debug_defines, " " .. tc.opt_define, true),
            tc.opt_include .. presheaf.include_dir,
            tc.opt_out_obj .. out_dir .. os_info.path_sep .. presheaf.test_exe .. os_info.obj_ext,
            tc.opt_out_exe .. test_exe_out,
            presheaf.test_src
        )
    )
    -- Run tests.
    exec(test_exe_out)
end

print(string.format("\x1b[1;35mtime elapsed ::\x1b[0m %.5f seconds", os.difftime(os.time(), start_time)))
