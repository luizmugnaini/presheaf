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

#include <psh/streams.hpp>

#include <stdio.h>
#include <psh/core.hpp>

#if defined(PSH_OS_WINDOWS)
#    include <Windows.h>
#    define PSH_IMPL_PATH_MAX_CHAR_COUNT MAX_PATH
#else
#    include <limits.h>
#    include <unistd.h>
#    define PSH_IMPL_PATH_MAX_CHAR_COUNT PATH_MAX
#endif

// @TODO(luiz): Substitute the perror calls with psh_log_fmt taking the error strings via a
//       thread safe alternative to strerror.

namespace psh::impl {
    constexpr cstring OPEN_FILE_FLAG_TO_STR_MAP[OPEN_FILE_FLAG_COUNT] = {
        "r",    // OPEN_FILE_FLAG_READ_TEXT
        "r+",   // OPEN_FILE_FLAG_READ_TEXT_EXTENDED
        "rb",   // OPEN_FILE_FLAG_READ_BIN
        "rb+",  // OPEN_FILE_FLAG_READ_BIN_EXTENDED
        "w",    // OPEN_FILE_FLAG_WRITE
        "w+",   // OPEN_FILE_FLAG_WRITE_EXTENDED
        "a",    // OPEN_FILE_FLAG_APPEND
    };

    constexpr bool has_read_permission(OpenFileFlag flag) psh_no_except {
        return (flag == OPEN_FILE_FLAG_READ_TEXT)
               || (flag == OPEN_FILE_FLAG_READ_TEXT_EXTENDED)
               || (flag == OPEN_FILE_FLAG_READ_BIN)
               || (flag == OPEN_FILE_FLAG_READ_BIN_EXTENDED)
               || (flag == OPEN_FILE_FLAG_WRITE_EXTENDED);
    }
}  // namespace psh::impl

namespace psh {
    FileReadResult read_file(Arena* arena, cstring path, OpenFileFlag flag) psh_no_except {
        psh_validate_usage({
            psh_assert_not_null(arena);
            psh_assert_not_null(path);
            psh_assert_msg(impl::has_read_permission(flag), "Cannot read file without opening with read permissions.");
        });

        cstring mode = impl::OPEN_FILE_FLAG_TO_STR_MAP[flag];
        FILE*   fhandle;

#if defined(PSH_OS_WINDOWS)
        fopen_s(&fhandle, path, mode);
#else
        fhandle = fopen(path, mode);
#endif

        if (psh_unlikely(fhandle == nullptr)) {
            return FileReadResult{.status = FILE_STATUS_FAILED_TO_OPEN};
        }

        if (psh_unlikely(fseek(fhandle, 0, SEEK_END) == -1)) {
            perror("Couldn't seek end of file.\n");
            return FileReadResult{.status = FILE_STATUS_FAILED_TO_READ};
        }

        isize file_size = ftell(fhandle);
        if (psh_unlikely(file_size == -1)) {
            perror("Couldn't tell the size of the file.\n");
            return FileReadResult{.status = FILE_STATUS_SIZE_UNKNOWN};
        }
        usize size = static_cast<usize>(file_size);

        if (psh_unlikely(fseek(fhandle, 0, SEEK_SET) == -1)) {
            perror("Couldn't seek start of file.\n");
            return FileReadResult{.status = FILE_STATUS_FAILED_TO_READ};
        }

        ArenaCheckpoint arena_checkpoint = make_arena_checkpoint(arena);

        Array<u8> content = make_array<u8>(arena, size);

        usize read_count = fread(content.buf, sizeof(u8), content.count, fhandle);
        psh_discard_value(read_count);  // @TODO: Maybe we should check this.

        if (psh_unlikely(ferror(fhandle) != 0)) {
            perror("Couldn't read file.\n");

            arena_checkpoint_restore(arena_checkpoint);
            return FileReadResult{.status = FILE_STATUS_FAILED_TO_READ};
        }

        i32 res = fclose(reinterpret_cast<FILE*>(fhandle));
        if (psh_unlikely(res == EOF)) {
            psh_log_error_fmt("File %s failed to be closed.", path);
        }

        return FileReadResult{
            .content = content,
            .status  = FILE_STATUS_OK,
        };
    }

    String read_stdin(Arena* arena, u32 initial_buf_size, u32 read_chunk_size) psh_no_except {
        psh_validate_usage(psh_assert_not_null(arena));

        ArenaCheckpoint arena_checkpoint = make_arena_checkpoint(arena);
        String          content          = make_string(arena, initial_buf_size);

#if defined(PSH_OS_WINDOWS)
        HANDLE handle_stdin = GetStdHandle(STD_INPUT_HANDLE);
        if (handle_stdin == INVALID_HANDLE_VALUE) {
            psh_log_error("Unable to acquire the handle to the stdin stream.");

            arena_checkpoint_restore(arena_checkpoint);
            return String{};
        }

        for (;;) {
            if (content.count + read_chunk_size > content.capacity) {
                dynamic_array_reserve(&content, content.count + read_chunk_size);
            }

            DWORD bytes_read;
            BOOL  success = ReadFile(handle_stdin, content.buf + content.count, read_chunk_size, &bytes_read, nullptr);
            content.count += bytes_read;
            if (psh_unlikely(!success)) {
                psh_log_error("Unable to read from the stdin stream.");

                arena_checkpoint_restore(arena_checkpoint);
                return String{};
            }

            if (bytes_read < read_chunk_size) {
                break;
            }
        }
#else
        for (;;) {
            if (content.count + read_chunk_size > content.capacity) {
                dynamic_array_reserve(&content, content.count + read_chunk_size);
            }

            isize bytes_read = read(STDIN_FILENO, content.buf + content.count, read_chunk_size);

            if (psh_unlikely(bytes_read == -1)) {
                psh_log_error("Unable to read from the stdin stream.");

                arena_checkpoint_restore(arena_checkpoint);
                return String{};
            }

            content.count += static_cast<usize>(bytes_read);
            if (static_cast<usize>(bytes_read) < read_chunk_size) {
                break;
            }
        }
#endif

        // Add null terminator to the end of the string.
        if (content.count == content.capacity) {
            dynamic_array_reserve(&content, content.count + 1);
        }
        content.buf[content.count] = 0;

        return content;
    }

    String absolute_path(Arena* arena, cstring file_path) psh_no_except {
        psh_paranoid_validate_usage(psh_assert_not_null(file_path));
        psh_validate_usage(psh_assert_not_null(arena));

        ArenaCheckpoint arena_checkpoint = make_arena_checkpoint(arena);

        String abs_path = make_string(arena, PSH_IMPL_PATH_MAX_CHAR_COUNT);

#if defined(PSH_OS_WINDOWS)
        DWORD result = GetFullPathName(file_path, PSH_IMPL_PATH_MAX_CHAR_COUNT, abs_path.buf, nullptr);
        if (result == 0) {
            psh_log_error_fmt(
                "Unable to obtain the full path of %s due to the error: %lu",
                file_path,
                GetLastError());

            arena_checkpoint_restore(arena_checkpoint);
            return String{};
        }
#else
        char const* result = realpath(file_path, abs_path.buf);
        if (result == nullptr) {
            psh_log_error_fmt("Unable to obtain the full path of %s due to the error:", file_path);
            perror(nullptr);

            arena_checkpoint_restore(arena_checkpoint);
            return String{};
        }
#endif

        return abs_path;
    }
}  // namespace psh
