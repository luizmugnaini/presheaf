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

#include <psh/streams.h>

#include <psh/core.h>
#include <stdio.h>

#if defined(PSH_OS_WINDOWS_32)
#    include <Windows.h>
#else
#    include <unistd.h>
#endif

// TODO(luiz): Substitute the `perror` calls with `psh::log_fmt` taking the error strings via a
//       thread safe alternative to `strerror`.

namespace psh {
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
                   (flag == OpenFileFlag::READ_TEXT_EXTENDED) ||
                   (flag == OpenFileFlag::READ_BIN) ||
                   (flag == OpenFileFlag::READ_BIN_EXTENDED) ||
                   (flag == OpenFileFlag::WRITE_EXTENDED);
        }
    }  // namespace impl_streams

    FileReadResult read_file(Arena* arena, strptr path, ReadFileFlag flag) noexcept {
        FILE* fhandle = fopen(path, impl_streams::OPEN_FILE_FLAG_TO_STR_MAP[static_cast<usize>(static_cast<OpenFileFlag>(flag))]);
        if (psh_unlikely(fhandle == nullptr)) {
            return FileReadResult{.status = FileStatus::FAILED_TO_OPEN};
        }

        if (psh_unlikely(fseek(fhandle, 0, SEEK_END) == -1)) {
            perror("Couldn't seek end of file.\n");
            return FileReadResult{.status = FileStatus::FAILED_TO_READ};
        }

        isize file_size = ftell(fhandle);
        if (psh_unlikely(file_size == -1)) {
            perror("Couldn't tell the size of the file.\n");
            return FileReadResult{.status = FileStatus::SIZE_UNKNOWN};
        }
        usize size = static_cast<usize>(file_size);

        if (psh_unlikely(fseek(fhandle, 0, SEEK_SET) == -1)) {
            perror("Couldn't seek start of file.\n");
            return FileReadResult{.status = FileStatus::FAILED_TO_READ};
        }

        psh_assert_msg(arena != nullptr, "Invalid arena");
        ArenaCheckpoint arena_checkpoint = arena->make_checkpoint();

        Array<u8> content{arena, size};
        usize     read_count = fread(content.buf, sizeof(u8), content.size, fhandle);

        if (psh_unlikely(ferror(fhandle) != 0)) {
            perror("Couldn't read file.\n");

            arena->restore_state(arena_checkpoint);
            return FileReadResult{.status = FileStatus::FAILED_TO_READ};
        }

        i32 res = fclose(reinterpret_cast<FILE*>(fhandle));
        if (psh_unlikely(res == EOF)) {
            psh_error_fmt("File %s failed to be closed.", path);
        }

        return FileReadResult{
            .content = content,
            .status  = FileStatus::OK,
        };
    }

    String read_stdin(Arena* arena, u32 initial_buf_size, u32 read_chunk_size) noexcept {
        ArenaCheckpoint arena_checkpoint = arena->make_checkpoint();
        String          content{arena, initial_buf_size};

#if defined(PSH_OS_WINDOWS_32)
        HANDLE handle_stdin = GetStdHandle(STD_INPUT_HANDLE);
        if (handle_stdin == INVALID_HANDLE_VALUE) {
            psh_error("Unable to acquire the handle to the stdin stream.");

            arena->restore_state(arena_checkpoint);
            return String{};
        }

        for (;;) {
            if (content.data.size + read_chunk_size > content.data.capacity) {
                content.data.resize(content.data.size + read_chunk_size);
            }

            DWORD bytes_read;
            BOOL  success = ReadFile(handle_stdin, content.data.end(), read_chunk_size, &bytes_read, nullptr);
            content.data.size += bytes_read;
            if (psh_unlikely(!success)) {
                psh_error("Unable to read from the stdin stream.");

                arena->restore_state(arena_checkpoint);
                return String{};
            }

            if (bytes_read < read_chunk_size) {
                break;
            }
        }
#else
        for (;;) {
            if (content.data.size + read_chunk_size > content.data.capacity) {
                content.data.resize(content.data.size + read_chunk_size);
            }

            isize bytes_read = read(STDIN_FILENO, content.data.end(), read_chunk_size);

            if (psh_unlikely(bytes_read == -1)) {
                psh_error("Unable to read from the stdin stream.");

                arena->restore_state(arena_checkpoint);
                return String{};
            }

            content.data.size += static_cast<usize>(bytes_read);
            if (static_cast<usize>(bytes_read) < read_chunk_size) {
                break;
            }
        }
#endif

        // Add null terminator to the end of the string.
        if (content.data.size == content.data.capacity) {
            content.data.resize(content.data.size + 1);
        }
        content.data.buf[content.data.size] = 0;

        return content;
    }
    }
}  // namespace psh
