///                          Presheaf Library
///    Copyright (C) 2024 Luiz Gustavo Mugnaini Anselmo
///
///    This program is free software; you can redistribute it and/or modify
///    it under the terms of the GNU General Public License as published by
///    the Free Software Foundation; either version 2 of the License, or
///    (at your option) any later version.
///
///    This program is distributed in the hope that it will be useful,
///    but WITHOUT ANY WARRANTY; without even the implied warranty of
///    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///    GNU General Public License for more details.
///
///    You should have received a copy of the GNU General Public License along
///    with this program; if not, write to the Free Software Foundation, Inc.,
///    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
///
/// Description: Implementation of the file system management utilities.
/// Author: Luiz G. Mugnaini A. <luizmuganini@gmail.com>

#include <psh/file_system.h>

#include <psh/intrinsics.h>
#include <cstdio>

// TODO(luiz): Substitute the `std::perror` calls with `psh::log_fmt` taking the error strings via a
//       thread safe alternative to `std::strerror`.

namespace psh {
    namespace {
        constexpr strptr open_file_flag(OpenFileFlag f) {
            strptr s;
            switch (f) {
                case OpenFileFlag::READ_TEXT:          s = "r"; break;
                case OpenFileFlag::READ_TEXT_EXTENDED: s = "r+"; break;
                case OpenFileFlag::READ_BIN:           s = "rb"; break;
                case OpenFileFlag::READ_BIN_EXTENDED:  s = "rb+"; break;
                case OpenFileFlag::WRITE:              s = "w"; break;
                case OpenFileFlag::WRITE_EXTENDED:     s = "w+"; break;
                case OpenFileFlag::APPEND:             s = "a"; break;
            }
            return s;
        }

        constexpr bool has_read_permission(OpenFileFlag flag) noexcept {
            return (flag == OpenFileFlag::READ_TEXT) ||
                   (flag == OpenFileFlag::READ_TEXT_EXTENDED) || (flag == OpenFileFlag::READ_BIN) ||
                   (flag == OpenFileFlag::READ_BIN_EXTENDED) ||
                   (flag == OpenFileFlag::WRITE_EXTENDED);
        }
    }  // namespace

    File::File(Arena* arena, StringView _path, OpenFileFlag _flag) noexcept {
        path.init(arena, _path);
        flag = _flag;

        handle = std::fopen(path.data.buf, open_file_flag(flag));
        if (psh_unlikely(handle == nullptr)) {
            status = FileStatus::FAILED_TO_OPEN;
            return;
        }

        FILE* fhandle = reinterpret_cast<FILE*>(handle);

        if (psh_unlikely(std::fseek(fhandle, 0, SEEK_END) == -1)) {
            std::perror("Couldn't seek end of file.");
            status = FileStatus::FAILED_TO_READ;
            return;
        }

        isize file_size = std::ftell(fhandle);
        if (psh_unlikely(file_size == -1)) {
            std::perror("Couldn't tell the size of the file.\n");
            status = FileStatus::SIZE_UNKNOWN;
            return;
        }

        size = static_cast<usize>(file_size);

        if (psh_unlikely(std::fseek(fhandle, 0, SEEK_SET) == -1)) {
            std::perror("Couldn't seek start of file.\n");
            status = FileStatus::FAILED_TO_READ;
            return;
        }

        status = FileStatus::OK;
    }

    File::~File() noexcept {
        if (psh_unlikely(handle == nullptr)) {
            return;
        }

        i32 res = std::fclose(reinterpret_cast<FILE*>(handle));
        if (psh_unlikely(res == EOF)) {
            psh_error_fmt("File %s failed to be closed.", path.data.buf);
        }
    }

    FileReadResult read_file(File file, Arena* arena) noexcept {
        psh_assert_msg(
            has_read_permission(file.flag),
            "read_file cannot read File whose reading permission is disabled.");

        if (psh_unlikely(file.status != FileStatus::OK)) {
            return {.status = FileStatus::FAILED_TO_READ};
        }

        // Acquire memory for the buffer.
        char* buf = arena->alloc<char>(file.size + 1);
        if (psh_unlikely(buf == nullptr)) {
            return {.status = FileStatus::OUT_OF_MEMORY};
        }

        // Read the whole file into the buffer.
        FILE* fhandle    = reinterpret_cast<FILE*>(file.handle);
        usize read_count = std::fread(buf, 1, file.size, fhandle);
        if (psh_unlikely(std::ferror(fhandle) != 0)) {
            std::perror("Couldn't read file.\n");
            return {.status = FileStatus::FAILED_TO_READ};
        }

        buf[read_count] = 0;  // Ensure the string is null terminated.

        return FileReadResult{
            .content = String{arena, StringView{buf, file.size}},
            .status  = FileStatus::OK,
        };
    }

    FileReadResult read_file(Arena* arena, strptr path, ReadFileFlag flag) noexcept {
        FILE* fhandle = std::fopen(path, open_file_flag(static_cast<OpenFileFlag>(flag)));
        if (psh_unlikely(fhandle == nullptr)) {
            return {.status = FileStatus::FAILED_TO_OPEN};
        }

        if (psh_unlikely(std::fseek(fhandle, 0, SEEK_END) == -1)) {
            std::perror("Couldn't seek end of file.");
            return {.status = FileStatus::FAILED_TO_READ};
        }

        isize file_size = std::ftell(fhandle);
        if (psh_unlikely(file_size == -1)) {
            std::perror("Couldn't tell the size of the file.\n");
            return {.status = FileStatus::SIZE_UNKNOWN};
        }
        usize size = static_cast<usize>(file_size);

        if (psh_unlikely(std::fseek(fhandle, 0, SEEK_SET) == -1)) {
            std::perror("Couldn't seek start of file.\n");
            return {.status = FileStatus::FAILED_TO_READ};
        }

        // Acquire memory for the buffer.
        char* buf = arena->alloc<char>(size + 1);
        if (psh_unlikely(buf == nullptr)) {
            return {.status = FileStatus::OUT_OF_MEMORY};
        }

        // Read the whole file into the buffer.
        usize read_count = std::fread(buf, 1, size, fhandle);
        if (psh_unlikely(std::ferror(fhandle) != 0)) {
            std::perror("Couldn't read file.\n");
            return {.status = FileStatus::FAILED_TO_READ};
        }

        buf[read_count] = 0;  // Ensure the string is null terminated.

        return FileReadResult{
            .content = String{arena, StringView{buf, size}},
            .status  = FileStatus::OK,
        };
    }
}  // namespace psh
