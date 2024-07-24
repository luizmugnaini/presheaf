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
/// Author: Luiz G. Mugnaini A. <luizmugnaini@gmail.com>

#include <psh/streams.h>

#include <psh/intrinsics.h>
#include <cstdio>

#if defined(PSH_OS_WINDOWS_32)
#    include <Windows.h>
#else
#    include <unistd.h>
#endif

// TODO(luiz): Substitute the `std::perror` calls with `psh::log_fmt` taking the error strings via a
//       thread safe alternative to `std::strerror`.

namespace psh {
    // -----------------------------------------------------------------------------
    // - Implementation of the stdin handling -
    // -----------------------------------------------------------------------------

    String read_stdin(Arena* arena) noexcept {
        constexpr u32 INITIAL_SIZE = 128;
        constexpr u32 CHUNK_SIZE   = 64;
        String        s{arena, INITIAL_SIZE};

#if defined(PSH_OS_WINDOWS_32)
        HANDLE handle_stdin = GetStdHandle(STD_INPUT_HANDLE);
        if (handle_stdin == INVALID_HANDLE_VALUE) {
            psh_error("Unable to acquire the handle to the stdin stream.");
            return s;
        }

        for (;;) {
            if (s.data.size + CHUNK_SIZE > s.data.capacity) {
                s.data.resize(s.data.size + CHUNK_SIZE);
            }

            DWORD bytes_read;
            BOOL  success = ReadFile(handle_stdin, s.data.end(), CHUNK_SIZE, &bytes_read, nullptr);
            s.data.size += bytes_read;
            if (psh_unlikely(!success)) {
                psh_error("Unable to read from the stdin stream.");
                return s;
            }

            if (bytes_read < CHUNK_SIZE) {
                break;
            }
        }
#else
        for (;;) {
            if (s.data.size + CHUNK_SIZE > s.data.capacity) {
                s.data.resize(s.data.size + CHUNK_SIZE);
            }

            isize bytes_read = read(STDIN_FILENO, s.data.end(), CHUNK_SIZE);

            if (psh_unlikely(bytes_read == -1)) {
                psh_error("Unable to read from the stdin stream.");
                return s;
            }

            s.data.size += static_cast<usize>(bytes_read);
            if (static_cast<usize>(bytes_read) < CHUNK_SIZE) {
                break;
            }
        }
#endif

        // Add null terminator to the end of the string.
        if (s.data.size == s.data.capacity) {
            s.data.resize(s.data.size + 1);
        }
        s.data.buf[s.data.size] = 0;

        return s;
    }

    // -----------------------------------------------------------------------------
    // - Implementation of the OS file stream handling -
    // -----------------------------------------------------------------------------

    namespace streams_impl {
        constexpr strptr open_file_flag(OpenFileFlag f) noexcept {
            constexpr strptr FLAG_STR[static_cast<u32>(OpenFileFlag::FLAG_COUNT)]{
                "r",
                "r+",
                "rb",
                "rb+",
                "w",
                "w+",
                "a",
            };
            return FLAG_STR[static_cast<u32>(f)];
        }

        constexpr bool has_read_permission(OpenFileFlag flag) noexcept {
            return (flag == OpenFileFlag::READ_TEXT) ||
                   (flag == OpenFileFlag::READ_TEXT_EXTENDED) || (flag == OpenFileFlag::READ_BIN) ||
                   (flag == OpenFileFlag::READ_BIN_EXTENDED) ||
                   (flag == OpenFileFlag::WRITE_EXTENDED);
        }
    }  // namespace streams_impl

    File::File(Arena* arena, StringView _path, OpenFileFlag _flag) noexcept {
        path.init(arena, _path);
        flag = _flag;

        handle = std::fopen(path.data.buf, streams_impl::open_file_flag(flag));
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

    FileReadResult read_file(Arena* arena, File const& file) noexcept {
        psh_assert_msg(
            streams_impl::has_read_permission(file.flag),
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
        FILE* fhandle =
            std::fopen(path, streams_impl::open_file_flag(static_cast<OpenFileFlag>(flag)));
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
