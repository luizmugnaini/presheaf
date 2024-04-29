#pragma once

#include <psh/string.h>
#include <psh/types.h>

namespace psh {
    /// Read a file to a string buffer.
    ///
    /// If the operation fails, an empty string is returned.
    String read_file(Arena* arena, StrPtr path) noexcept;
}  // namespace psh
