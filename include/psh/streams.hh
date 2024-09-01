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
/// Description: File system management utilities.
/// Author: Luiz G. Mugnaini A. <luizmugnaini@gmail.com>

#pragma once

#include <psh/buffer.hh>
#include <psh/core.hh>
#include <psh/option.hh>
#include <psh/string.hh>

namespace psh {
    // -----------------------------------------------------------------------------
    // - Standard Input Stream -
    // -----------------------------------------------------------------------------

    /// Read the standard input stream bytes to a string.
    String read_stdin(Arena* arena) noexcept;

    // -----------------------------------------------------------------------------
    // - OS file stream -
    // -----------------------------------------------------------------------------

    enum struct OpenFileFlag {
        /// Open a text file for reading operations.
        ///
        /// The file stream is positioned at the start of the file.
        READ_TEXT,

        /// Open a text file for reading and writing.
        ///
        /// The file stream will be placed at the start of the file and the existing contents of the
        /// file, if any, will remain intact.
        READ_TEXT_EXTENDED,

        /// Open a non-text file for reading operations.
        ///
        /// The file stream is positioned at the start of the file.
        READ_BIN,

        /// Open a non-text file for reading and writing.
        ///
        /// The file stream will be placed at the start of the file and the existing contents of the
        /// file, if any, will remain intact.
        READ_BIN_EXTENDED,

        /// Open a file for writing.
        ///
        /// If the file exists, all file contents are overwritten and the stream is placed at
        /// the start of the file. Otherwise, the file will be created and the file stream will be
        /// placed at the start of the file.
        WRITE,

        /// Open a file for reading and writing.
        ///
        /// If the file exists, all existing file contents are overwritten and the file stream is
        /// positioned at the start of the file. Otherwise, the file will be created and the file
        /// stream will be placed at the start of the file.
        WRITE_EXTENDED,

        /// Open file for writing to the end of the file stream.
        ///
        /// The file stream is placed at the end of the file and all output is written to the
        /// current end of the file. If the file doesn't exist, it is created.
        APPEND,

        FLAG_COUNT,

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
