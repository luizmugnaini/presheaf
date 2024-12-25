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

// @TODO(luiz): Substitute the perror calls with psh::log_fmt taking the error strings via a
//       thread safe alternative to strerror.

namespace psh {
    namespace impl::streams {
        // @TODO(luiz): To be honest this is quite ugly, OpenFileFlag is just a concatenation of
        // ReadFileFlag and WriteFileFlag. It is not exposed, but is still a weak maintainance point.
        enum struct OpenFileFlag : u32 {
            READ_TEXT = 0,
            READ_TEXT_EXTENDED,
            READ_BIN,
            READ_BIN_EXTENDED,
            WRITE,
            WRITE_EXTENDED,
            APPEND,
            FLAG_COUNT,
        };
        constexpr strptr OPEN_FILE_FLAG_TO_STR_MAP[static_cast<usize>(OpenFileFlag::FLAG_COUNT)] = {
            "r",    // ReadFileFlag::READ_TEXT
            "r+",   // ReadFileFlag::READ_TEXT_EXTENDED
            "rb",   // ReadFileFlag::READ_BIN
            "rb+",  // ReadFileFlag::READ_BIN_EXTENDED
            "w",    // WriteFileFlag::WRITE
            "w+",   // WriteFileFlag::WRITE_EXTENDED
            "a",    // WriteFileFlag::APPEND
        };

        constexpr bool has_read_permission(OpenFileFlag flag) psh_no_except {
            return (flag == OpenFileFlag::READ_TEXT) ||
                   (flag == OpenFileFlag::READ_TEXT_EXTENDED) ||
                   (flag == OpenFileFlag::READ_BIN) ||
                   (flag == OpenFileFlag::READ_BIN_EXTENDED) ||
                   (flag == OpenFileFlag::WRITE_EXTENDED);
        }
    }  // namespace impl::streams

    FileReadResult read_file(Arena* arena, strptr path, ReadFileFlag flag) psh_no_except {
        strptr mode = impl::streams::OPEN_FILE_FLAG_TO_STR_MAP[static_cast<u32>(flag)];
        FILE*  fhandle;

#if defined(PSH_OS_WINDOWS)
        fopen_s(&fhandle, path, mode);
#else
        fhandle = fopen(path, mode);
#endif

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

        psh_assert_not_null(arena);
        ArenaCheckpoint arena_checkpoint = arena->make_checkpoint();

        Array<u8> content{arena, size};

        usize read_count = fread(content.buf, sizeof(u8), content.count, fhandle);
        psh_discard_value(read_count);  // @TODO: Maybe we should check this.

        if (psh_unlikely(ferror(fhandle) != 0)) {
            perror("Couldn't read file.\n");

            arena->restore_checkpoint(arena_checkpoint);
            return FileReadResult{.status = FileStatus::FAILED_TO_READ};
        }

        i32 res = fclose(reinterpret_cast<FILE*>(fhandle));
        if (psh_unlikely(res == EOF)) {
            psh_log_error_fmt("File %s failed to be closed.", path);
        }

        return FileReadResult{
            .content = content,
            .status  = FileStatus::OK,
        };
    }

    String read_stdin(Arena* arena, u32 initial_buf_size, u32 read_chunk_size) psh_no_except {
        ArenaCheckpoint arena_checkpoint = arena->make_checkpoint();

        String content{arena, initial_buf_size};

#if defined(PSH_OS_WINDOWS)
        HANDLE handle_stdin = GetStdHandle(STD_INPUT_HANDLE);
        if (handle_stdin == INVALID_HANDLE_VALUE) {
            psh_log_error("Unable to acquire the handle to the stdin stream.");

            arena->restore_checkpoint(arena_checkpoint);
            return {};
        }

        for (;;) {
            if (content.count + read_chunk_size > content.capacity) {
                content.resize(content.count + read_chunk_size);
            }

            DWORD bytes_read;
            BOOL  success = ReadFile(handle_stdin, content.end(), read_chunk_size, &bytes_read, nullptr);
            content.count += bytes_read;
            if (psh_unlikely(!success)) {
                psh_log_error("Unable to read from the stdin stream.");

                arena->restore_checkpoint(arena_checkpoint);
                return {};
            }

            if (bytes_read < read_chunk_size) {
                break;
            }
        }
#else
        for (;;) {
            if (content.count + read_chunk_size > content.capacity) {
                content.resize(content.count + read_chunk_size);
            }

            isize bytes_read = read(STDIN_FILENO, content.end(), read_chunk_size);

            if (psh_unlikely(bytes_read == -1)) {
                psh_log_error("Unable to read from the stdin stream.");

                arena->restore_checkpoint(arena_checkpoint);
                return {};
            }

            content.count += static_cast<usize>(bytes_read);
            if (static_cast<usize>(bytes_read) < read_chunk_size) {
                break;
            }
        }
#endif

        // Add null terminator to the end of the string.
        if (content.count == content.capacity) {
            content.resize(content.count + 1);
        }
        content.buf[content.count] = 0;

        return content;
    }

    String absolute_path(Arena* arena, strptr file_path) psh_no_except {
        psh_assert_not_null(arena);
        ArenaCheckpoint arena_checkpoint = arena->make_checkpoint();

        String abs_path{arena, PSH_IMPL_PATH_MAX_CHAR_COUNT};

#if defined(PSH_OS_WINDOWS)
        DWORD result = GetFullPathName(file_path, PSH_IMPL_PATH_MAX_CHAR_COUNT, abs_path.buf, nullptr);
        if (result == 0) {
            psh_log_error_fmt(
                "Unable to obtain the full path of %s due to the error: %lu",
                file_path,
                GetLastError());

            arena->restore_checkpoint(arena_checkpoint);
            return {};
        }
#else
        char* result = realpath(file_path, abs_path.buf);
        if (result == nullptr) {
            psh_log_error_fmt("Unable to obtain the full path of %s due to the error:", file_path);
            perror(nullptr);

            arena->restore_checkpoint(arena_checkpoint);
            return {};
        }
#endif

        return abs_path;
    }
}  // namespace psh
