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
    enum struct OpenFileFlag {
        /// Open a text file for reading operations.
        ///
        /// The file stream is positioned at the start of the file.
        READ_TEXT = 0,

        /// Open a text file for reading and writing.
        ///
        /// The file stream will be placed at the start of the file and the existing contents of the
        /// file, if any, will remain intact.
        READ_TEXT_EXTENDED = 1,

        /// Open a non-text file for reading operations.
        ///
        /// The file stream is positioned at the start of the file.
        READ_BIN = 2,

        /// Open a non-text file for reading and writing.
        ///
        /// The file stream will be placed at the start of the file and the existing contents of the
        /// file, if any, will remain intact.
        READ_BIN_EXTENDED = 3,

        /// Open a file for writing.
        ///
        /// If the file exists, all file contents are overwritten and the stream is placed at
        /// the start of the file. Otherwise, the file will be created and the file stream will be
        /// placed at the start of the file.
        WRITE = 4,

        /// Open a file for reading and writing.
        ///
        /// If the file exists, all existing file contents are overwritten and the file stream is
        /// positioned at the start of the file. Otherwise, the file will be created and the file
        /// stream will be placed at the start of the file.
        WRITE_EXTENDED = 5,

        /// Open file for writing to the end of the file stream.
        ///
        /// The file stream is placed at the end of the file and all output is written to the
        /// current end of the file. If the file doesn't exist, it is created.
        APPEND = 6,

        // NOTE: the append extended flag won't be supported due to its non-portable behaviour. The
        //       user may use the WRITE_EXTENDED instead.
    };

    enum struct ReadFileFlag {
        READ_TEXT          = 0,
        READ_TEXT_EXTENDED = 1,
        READ_BIN           = 2,
        READ_BIN_EXTENDED  = 3,
    };

    enum struct WriteFileFlag {
        WRITE          = 4,
        WRITE_EXTENDED = 5,
        APPEND         = 6,
    };

    enum struct FileStatus {
        FAILED_TO_OPEN,
        FAILED_TO_READ,
        OUT_OF_MEMORY,
        SIZE_UNKNOWN,
        OK,
    };

    struct File {
        String       path;              ///< Path to the file.
        void*        handle = nullptr;  ///< File stream.
        usize        size   = 0;        ///< Size of the file.
        OpenFileFlag flag;              ///< Read/write Permission flags.
        FileStatus   status;            ///< Indicates the result of the opening operation.

        /// Open a file.
        ///
        /// Parameters:
        ///     * arena: The arena allocator that will carry the contents of the path string.
        ///     * path: The path to the file to be opened.
        ///     * flag: The permission flag used to open the file.
        ///
        /// Note: Before proceeding, consider checking the `File::status` flag to confirm that the
        ///       operation was successful.
        File(Arena* arena, StringView path, OpenFileFlag flag) noexcept;
        ~File() noexcept;
    };

    struct FileReadResult {
        String     content = {};  ///< The contents pertaining to the file.
        FileStatus status;        ///< Status of the read operation.
    };

    /// Read file contents to a string.
    ///
    /// Parameters:
    ///     * arena: The arena allocator that will carry the contents of the resulting string.
    ///     * file: A file instance whose flag has read permission.
    FileReadResult read_file(Arena* arena, File const& f) noexcept;

    /// Read file contents to a string.
    ///
    /// Parameters:
    ///     * arena: The arena allocator that will carry the contents of the resulting string.
    ///     * path: A zero-terminated string containing the path to the file to be read.
    ///     * flag: Can be any flag with read permission.
    FileReadResult read_file(Arena* arena, strptr path, ReadFileFlag flag) noexcept;
}  // namespace psh
