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
/// Description: File system management utilities.
/// Author: Luiz G. Mugnaini A. <luizmugnaini@gmail.com>

#pragma once

#include <psh/buffer.h>
#include <psh/option.h>
#include <psh/string.h>
#include <psh/types.h>

namespace psh {
    enum class FileStatus {
        FailedToRead,
        FailedToOpen,
        FailedToClose,
        OutOfMemory,
        SizeUnknown,
        OK,
    };

    struct FileReadResult {
        String     content{};
        FileStatus status = FileStatus::FailedToRead;
    };

    struct File {
        std::FILE* handle = nullptr;
        String     path{};
        usize      size     = 0;
        FileStatus validity = FileStatus::FailedToRead;

        File(Arena* arena, StringView path_, strptr flags_ = "rb") noexcept;
        ~File() noexcept;

        FileReadResult read(Arena* arena) noexcept;
    };
}  // namespace psh
