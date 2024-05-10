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

// TODO(luiz): Substitute the `std::perror` calls with `psh::log_fmt` taking the error strings via a
//       thread safe alternative to `std::strerror`.

namespace psh {
    File::File(strptr path_, strptr flags_) noexcept : path{path_}, flags{flags_} {
        if (psh_unlikely(path == nullptr || flags == nullptr)) {
            return;
        }

        handle = std::fopen(path, flags);
        if (psh_unlikely(handle == nullptr)) {
            validity = FileValidity::FailedToOpen;
            return;
        }

        if (psh_unlikely(std::fseek(handle, 0, SEEK_END) == -1)) {
            std::perror("Couldn't seek end of file.");
            validity = FileValidity::FailedToRead;
            return;
        }

        isize const file_size = std::ftell(handle);

        if (psh_unlikely(file_size == -1)) {
            std::perror("Couldn't tell the size of the file.\n");
            validity = FileValidity::SizeUnknown;
            return;
        }
        if (psh_unlikely(std::fseek(handle, 0, SEEK_SET) == -1)) {
            std::perror("Couldn't seek start of file.\n");
            validity = FileValidity::FailedToRead;
            return;
        }
    }

    File::~File() noexcept {
        if (psh_unlikely(handle == nullptr)) {
            return;
        }

        i32 res = std::fclose(handle);
        if (psh_unlikely(res == EOF)) {
            log_fmt(LogLevel::Error, "File %s failed to be closed.", path);
        }
    }

    FileReadResult File::read(Arena* arena) noexcept {
        // Acquire memory for the buffer.
        char* const buf = arena->alloc<char>(size + 1);
        if (psh_unlikely(buf == nullptr)) {
            return {.valid = FileValidity::OutOfMemory};
        }

        // Read the whole file into the buffer.
        usize const read_count = std::fread(buf, 1, size, handle);
        if (psh_unlikely(std::ferror(handle) != 0)) {
            std::perror("Couldn't read file.\n");
            return {.valid = FileValidity::FailedToRead};
        }

        buf[read_count] = 0;  // Ensure the string is null terminated.

        return FileReadResult{
            .content = String{arena, size, buf},
            .valid   = FileValidity::OK,
        };
    }
}  // namespace psh
