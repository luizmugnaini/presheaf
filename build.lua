--                          Presheaf Library
--    Copyright (C) 2024 Luiz Gustavo Mugnaini Anselmo
--
--    This program is free software; you can redistribute it and/or modify
--    it under the terms of the GNU General Public License as published by
--    the Free Software Foundation; either version 2 of the License, or
--    (at your option) any later version.
--
--    This program is distributed in the hope that it will be useful,
--    but WITHOUT ANY WARRANTY; without even the implied warranty of
--    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
--    GNU General Public License for more details.
--
--    You should have received a copy of the GNU General Public License along
--    with this program; if not, write to the Free Software Foundation, Inc.,
--    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
--
-- Description: Build system for the presheaf library.
-- Author: Luiz G. Mugnaini A. <luizmugnaini@gmail.com>
--
-- Running the build system:  lua build.lua [options]
-- Example:                   lua build.lua fmt clang mold test

local start_time = os.clock()

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
	local acc = is_prefix and (join .. arr[1]) or arr[1]
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
		cc = {
			exe = "clang++",
			opt = {
				std = "-std=",
				no_link = "-c",
				include = "-I",
				define = "-D",
				out_obj = "-o",
				out_exe = "-o",
			},
			flags = {
				common = "-pedantic -Wall -Wextra -Wpedantic -Wuninitialized -Wcast-align -Wconversion -Wnull-pointer-arithmetic -Wnull-dereference -Wformat=2 -Wno-unused-variable -Wno-compat -Wno-unsafe-buffer-usage -fno-rtti -fno-exceptions",
				debug = "-Werror -g -O0 -fsanitize=address -fsanitize=pointer-compare -fsanitize=pointer-subtract -fsanitize=undefined -fstack-protector-strong -fsanitize=leak",
				release = "-O2",
			},
		},
		ar = {
			exe = "llvm-ar",
			out = "-o",
			flags = "rcs",
		},
	},
	gcc = {
		cc = {
			exe = "g++",
			opt = {
				std = "-std=",
				no_link = "-c",
				include = "-I",
				define = "-D",
				out_obj = "-o",
				out_exe = "-o",
			},
			flags = {
				common = "-pedantic -Wall -Wextra -Wpedantic -Wuninitialized -Wcast-align -Wconversion -Wnull-dereference -Wformat=2 -Wno-unused-variable -fno-rtti -fno-exceptions",
				debug = "-Werror -g -O0 -fsanitize=address -fsanitize=pointer-compare -fsanitize=pointer-subtract -fsanitize=undefined -fstack-protector-strong -fsanitize=leak",
				release = "-O2",
			},
		},
		ar = {
			exe = "ar",
			out = "-o",
			flags = "rcs",
		},
	},
	msvc = {
		cc = {
			exe = "cl",
			opt = {
				include = "/I",
				define = "/D",
				std = "/std:",
				no_link = "/c",
				out_obj = "/Fo:",
				out_exe = "/Fe:",
			},
			flags = {
				common = "-nologo -Oi -TP -MP -FC -GF -GA /fp:except- -GR- -EHsc- /INCREMENTAL:NO /W3",
				debug = "/Ob0 /Od /Oy- /Z7 /RTC1 /MTd",
				release = "/O2 /MT",
			},
		},
		ar = {
			exe = "lib",
			out = "/out:",
			flags = "/nologo",
		},
	},
	clang_cl = {
		cc = {
			exe = "clang-cl",
			opt = {
				include = "/I",
				define = "/D",
				std = "/std:",
				no_link = "-c",
				out_obj = "-o",
				out_exe = "-o",
			},
			flags = {
				common = "/TP -Wall -Wextra -Wconversion -Wuninitialized -Wnull-pointer-arithmetic -Wnull-dereference -Wcast-align -Wformat=2 -Wno-unused-variable -Wno-missing-prototypes -Wno-unsafe-buffer-usage -Wno-c++20-compat -Wno-c++98-compat-pedantic",
				debug = "-Ob0 /Od /Oy- /Z7 /RTC1 -g /MTd",
				release = "-O2 /MT",
			},
		},
		ar = {
			exe = "llvm-lib",
			out = "/out:",
			flags = "/nologo",
		},
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

function presheaf_flags(cc)
	return string.format(
		"%s %s %s %s %s %s",
		cc.opt.std .. presheaf.std,
		cc.flags.common,
		options.release and cc.flags.release or cc.flags.debug,
		concat(presheaf.defines, " " .. cc.opt.define, true),
		options.release and "" or concat(presheaf.debug_defines, " " .. cc.opt.define, true),
		cc.opt.include .. presheaf.include_dir
	)
end

-- -----------------------------------------------------------------------------
-- Toolchain
-- -----------------------------------------------------------------------------

local tc = { cc = nil, ar = nil, ld = nil }

if options.clang then
	if os_windows then
		tc.cc = compilers.clang_cl.cc
		tc.ar = compilers.clang_cl.ar
	else
		tc.cc = compilers.clang.cc
		tc.ar = compilers.clang.ar
	end
elseif options.gcc then
	assert(not os_windows, "GCC build not supported in Windows")
	tc.cc = compilers.gcc.cc
	tc.ar = compilers.gcc.ar
elseif options.msvc then
	tc.cc = compilers.msvc.cc
	tc.ar = compilers.msvc.ar
else
	if os_windows then
		tc.cc = compilers.msvc.cc
		tc.ar = compilers.msvc.ar
	else
		tc.cc = compilers.gcc.cc
		tc.ar = compilers.gcc.ar
	end
end

-- -----------------------------------------------------------------------------
-- Execute build instructions
-- -----------------------------------------------------------------------------

if options.fmt then
	exec("clang-format -i include/psh/*.h src/*.cc")
end

local out_dir = "build"
local obj_out = out_dir .. os_info.path_sep .. presheaf.lib .. os_info.obj_ext
local lib_out = out_dir .. os_info.path_sep .. presheaf.lib .. os_info.lib_ext
exec("mkdir " .. out_dir, true)

-- Compile without linking.
exec(
	string.format(
		"%s %s %s %s %s",
		tc.cc.exe,
		tc.cc.opt.no_link,
		presheaf_flags(tc.cc, options.release),
		tc.cc.opt.out_obj .. obj_out,
		presheaf.src,
		tc.ar.exe,
		tc.ar.flags,
		tc.ar.out .. lib_out,
		obj_out
	)
)
-- Archive objs into a library.
exec(string.format("%s %s %s %s", tc.ar.exe, tc.ar.flags, tc.ar.out .. lib_out, obj_out))

if options.test then
	-- Compile tests with debug flags.
	local test_exe_out = out_dir .. os_info.path_sep .. presheaf.test_exe .. os_info.exe_ext
	exec(
		string.format(
			"%s %s %s %s %s",
			tc.cc.exe,
			presheaf_flags(tc.cc, false),
			tc.cc.opt.out_obj .. out_dir .. os_info.path_sep .. presheaf.test_exe .. os_info.obj_ext,
			tc.cc.opt.out_exe .. test_exe_out,
			presheaf.test_src
		)
	)
	-- Run tests.
	exec(test_exe_out)
end

print(string.format("\x1b[1;35mtime elapsed ::\x1b[0m %.5f seconds", os.clock() - start_time))
