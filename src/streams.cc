///                             Presheaf library
/// Copyright (C) 2024 Luiz Gustavo Mugnaini Anselmo
///
/// Permission is hereby granted, free of charge, to any person obtaining a copy of
/// this software and associated documentation files (the “Software”), to deal in
/// the Software without restriction, including without limitation the rights to
/// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
/// of the Software, and to permit persons to whom the Software is furnished to do
/// so, subject to the following conditions:
///
/// The above copyright notice and this permission notice shall be included in all
/// copies or substantial portions of the Software.
///
/// THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
/// SOFTWARE.
///
/// Description: Implementation of the file system management utilities.
/// Author: Luiz G. Mugnaini A. <luizmugnaini@gmail.com>

#include <psh/streams.hh>

#include <cstdio>
#include <psh/core.hh>

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

    namespace impl_streams {
        constexpr strptr OPEN_FILE_FLAG_TO_STR_MAP[static_cast<usize>(OpenFileFlag::FLAG_COUNT)] = {
            "r",
            "r+",
            "rb",
            "rb+",
            "w",
            "w+",
            "a",
        };

        constexpr bool has_read_permission(OpenFileFlag flag) noexcept {
            return (flag == OpenFileFlag::READ_TEXT) ||
                   (flag == OpenFileFlag::READ_TEXT_EXTENDED) || (flag == OpenFileFlag::READ_BIN) ||
                   (flag == OpenFileFlag::READ_BIN_EXTENDED) ||
                   (flag == OpenFileFlag::WRITE_EXTENDED);
        }
    }  // namespace impl_streams

    File::File(Arena* arena, StringView _path, OpenFileFlag _flag) noexcept {
        path.init(arena, _path);
        flag = _flag;

        handle = std::fopen(
            path.data.buf,
            impl_streams::OPEN_FILE_FLAG_TO_STR_MAP[static_cast<usize>(flag)]);
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
            impl_streams::has_read_permission(file.flag),
            "read_file cannot read File whose reading permission is disabled.");

        if (psh_unlikely(file.status != FileStatus::OK)) {
            return {.status = FileStatus::FAILED_TO_READ};
        }

        u8* buf = arena->alloc<u8>(file.size + 1);
        if (psh_unlikely(buf == nullptr)) {
            return {.status = FileStatus::OUT_OF_MEMORY};
        }

        FILE* fhandle    = reinterpret_cast<FILE*>(file.handle);
        usize read_count = std::fread(buf, 1, file.size, fhandle);
        if (psh_unlikely(std::ferror(fhandle) != 0)) {
            std::perror("Couldn't read file.\n");
            return {.status = FileStatus::FAILED_TO_READ};
        }

        buf[read_count] = 0;  // Ensure the string is null terminated.

        return FileReadResult{
            .content = Array{FatPtr{buf, file.size}, arena},
            .status  = FileStatus::OK,
        };
    }

    FileReadResult read_file(Arena* arena, strptr path, ReadFileFlag flag) noexcept {
        FILE* fhandle = std::fopen(path, impl_streams::OPEN_FILE_FLAG_TO_STR_MAP[static_cast<usize>(static_cast<OpenFileFlag>(flag))]);
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

        u8* buf = arena->alloc<u8>(size + 1);
        if (psh_unlikely(buf == nullptr)) {
            return {.status = FileStatus::OUT_OF_MEMORY};
        }

        usize read_count = std::fread(buf, 1, size, fhandle);
        if (psh_unlikely(std::ferror(fhandle) != 0)) {
            std::perror("Couldn't read file.\n");
            return {.status = FileStatus::FAILED_TO_READ};
        }

        buf[read_count] = 0;  // Ensure the string is null terminated.

        i32 res = std::fclose(reinterpret_cast<FILE*>(fhandle));
        if (psh_unlikely(res == EOF)) {
            psh_error_fmt("File %s failed to be closed.", path);
        }

        return FileReadResult{
            .content = Array{FatPtr{buf, size}, arena},
            .status  = FileStatus::OK,
        };
    }
}  // namespace psh
