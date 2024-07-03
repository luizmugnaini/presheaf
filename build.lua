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

-- Available command line options.
local options = {
	-- Format source files.
	fmt = false,
	-- Build type (default: debug build).
	release = false,
	-- Compiler of choice (default: clang).
	clang = false,
	gcc = false,
	msvc = false,
	-- Linker of choice (default: compiler chooses)
	ld = false,
	lld = false,
	mold = false, -- Only available on unix builds.
	-- Build tests.
	test = false,
	-- Whether or not to print the commands ran by the build script and their output.
	quiet = false,
}
for i = 1, #arg do
	options[arg[i]] = true
end

local os_windows = (package.config:sub(1, 1) == "\\")
local silence_cmd = os_windows and " > NUL 2>&1" or " > /dev/null 2>&1"

function exec(cmd_str, show_out)
	if not options.quiet then
		print("\x1b[1;35mexecuting ::\x1b[0m " .. cmd_str)
	end

	local cmd_res = (show_out and not options.quiet) and cmd_str or (cmd_str .. silence_cmd)
	os.execute(cmd_res)
end

-- -----------------------------------------------------------------------------
-- - Library configuration -
-- -----------------------------------------------------------------------------

-- File extensions.
local obj_ext = os_windows and ".obj" or ".o"
local exe_ext = os_windows and ".exe" or ""
local lib_ext = os_windows and ".lib" or ".a"

local out_dir = "build/"

local lib = {
	src = "src/all.cc",
	test = "tests/test_all.cc",
	include_dir = "include",
	lib_obj = out_dir .. "presheaf" .. obj_ext,
	lib_bin = out_dir .. "presheaf" .. lib_ext,
	test_exe = out_dir .. "test" .. exe_ext,
	std = "c++20",
}

local compiler_config = {
	clang = {
		compiler = os_windows and "clang-cl" or "clang++",
		archiver = "llvm-ar rc",
		include_flag = "-I",
		std_flag = "-std=",
		no_link_flag = "-c",
		link_flag = "-L",
		linker_flag = "-fuse-ld=",
		out_flag = "-o",
		cflags = "-Wall -Wextra -pedantic -Wuninitialized -Wswitch -Wcovered-switch-default -Wshadow -Wcast-align -Wold-style-cast -Wpedantic -Wconversion -Wsign-conversion -Wnull-dereference -Wdouble-promotion -Wmisleading-indentation -Wformat=2 -Wno-unused-variable -fno-rtti -fno-exceptions -fno-cxx-exceptions -fcolor-diagnostics -fno-force-emit-vtables",
		debug_cflags = "-g -DPSH_DEBUG",
		san_cflags = "-fsanitize=address -fsanitize=pointer-compare -fsanitize=pointer-subtract -fsanitize=undefined -fstack-protector-strong -fsanitize=leak",
	},
	msvc = {
		compiler = "cl",
		archiver = "lib",
		include_flag = "/I",
		std_flag = "/std:",
		no_link_flag = "/c",
		link_flag = "/link",
		linker_flag = "/linker:",
		out_flag = "/o",
		cflags = "/W3 /fp:except- /GR- /GA /nologo",
		debug_cflags = "/Zi /Oy- /DPSH_DEBUG",
		san_cflags = "",
	},
	gcc = {
		compiler = "g++",
		archiver = "ar rcs",
		include_flag = "-I",
		std_flag = "-std=",
		no_link_flag = "-c",
		link_flag = "-L",
		linker_flag = "-fuse-ld=",
		out_flag = "-o",
		cflags = "-Wall -Wextra -pedantic -Wuninitialized -Wswitch -Wshadow -Wcast-align -Wold-style-cast -Wpedantic -Wconversion -Wsign-conversion -Wnull-dereference -Wdouble-promotion -Wmisleading-indentation -Wformat=2 -Wno-unused-variable -fno-rtti -fno-exceptions -fno-exceptions",
		debug_cflags = "-g -DPSH_DEBUG",
		san_cflags = "-fsanitize=address -fsanitize=pointer-compare -fsanitize=pointer-subtract -fsanitize=undefined -fstack-protector-strong -fsanitize=leak",
	},
}

local linker_config = {
	ld = "ld",
	mold = "mold",
	lld = os_windows and "lld-link" or "lld",
}

-- -----------------------------------------------------------------------------
-- - Pre-compilation step -
-- -----------------------------------------------------------------------------

-- Decide which compiler to use.
local cc = compiler_config.clang
if options.gcc then
	cc = compiler_config.gcc
elseif options.msvc then
	cc = compiler_config.msvc
end

-- Decide which linker to use.
local ld = nil
if options.lld then
	ld = linker_config.lld
elseif options.mold then
	assert(not os_windows, "mold linker not available")
	ld = linker_config.mold
elseif options.ld then
	ld = linker_config.ld
end

-- Format source files if requested.
if options.fmt then
	exec("clang-format -i include/psh/*.h src/*.cc")
end

-- Create the directory where the compiler output will be written.
exec("mkdir " .. out_dir)

-- -----------------------------------------------------------------------------
-- - Compile and archive the presheaf library -
-- -----------------------------------------------------------------------------

local cflags_lib =
	string.format("%s %s%s %s %s%s", cc.no_link_flag, cc.std_flag, lib.std, cc.cflags, cc.include_flag, lib.include_dir)
if not options.release then
	cflags_lib = cflags_lib .. " " .. cc.debug_cflags .. " " .. cc.san_cflags
end

exec(string.format("%s %s %s %s %s", cc.compiler, cflags_lib, lib.src, cc.out_flag, lib.lib_obj), true)
exec(string.format("%s %s %s", cc.archiver, lib.lib_bin, lib.lib_obj), true)

-- -----------------------------------------------------------------------------
-- - Compile and run the tests -
-- -----------------------------------------------------------------------------

if options.test then
	local cflags_test = string.format(
		"%s%s %s %s %s %s%s",
		cc.std_flag,
		lib.std,
		cc.cflags,
		cc.debug_cflags,
		cc.san_cflags,
		cc.include_flag,
		lib.include_dir
	)
	if ld ~= nil then
		cflags_test = cflags_test .. " " .. cc.linker_flag .. ld
	end

	exec(string.format("%s %s %s %s %s", cc.compiler, lib.test, cflags_test, cc.out_flag, lib.test_exe), true)
end

print(string.format("\x1b[1;35mtime elapsed ::\x1b[0m %.5f seconds", os.clock() - start_time))
