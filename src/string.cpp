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
/// Description: Implementation of the string related types and utility functions.
/// Author: Luiz G. Mugnaini A. <luizmugnaini@gmail.com>

#include <psh/string.hpp>

#include <string.h>
#include <initializer_list>
#include <psh/memory_utils.hpp>
#include <psh/option.hpp>

namespace psh {
    // -----------------------------------------------------------------------------
    // String utilities implementation.
    // -----------------------------------------------------------------------------

    usize str_length(strptr str) noexcept {
        usize res = 0;
        if (psh_likely(str != nullptr)) {
            res = strlen(str);
        }
        return res;
    }

    StrCmpResult str_cmp(strptr lhs, strptr rhs) noexcept {
        i32          cmp = strcmp(lhs, rhs);
        StrCmpResult res;
        if (cmp == 0) {
            res = StrCmpResult::EQUAL;
        } else if (cmp < 0) {
            res = StrCmpResult::LESS_THAN;
        } else {
            res = StrCmpResult::GREATER_THAN;
        }
        return res;
    }

    bool str_equal(strptr lhs, strptr rhs) noexcept {
        return (strcmp(lhs, rhs) == 0);
    }

    // -----------------------------------------------------------------------------
    // String implementation.
    // -----------------------------------------------------------------------------

    void String::init(Arena* arena, StringView sv) noexcept {
        usize sv_size = sv.data.size;
        this->data.init(arena, sv_size + 1);

        memory_copy(reinterpret_cast<u8*>(this->data.buf), reinterpret_cast<u8 const*>(sv.data.buf), sv_size);
        this->data.buf[sv_size + 1] = 0;
        this->data.size             = sv_size;
    }

    Status String::join(FatPtr<StringView const> join_strings, StringView join_element) noexcept {
        bool previously_empty = (this->data.size == 0);

        // Resize the string ahead of time if necessary.
        {
            // Get the number of characters that will be added to the string.
            usize additional_size = 1;
            {
                // Add the size accumulated by the join element.
                if (join_element.data.size != 0) {
                    additional_size += previously_empty ? ((join_strings.size - 1) * join_element.data.size)
                                                        : (join_strings.size * join_element.data.size);
                }

                // Account for the size of each of the joining strings.
                for (StringView const& s : join_strings) {
                    additional_size += s.data.size;
                }
            }

            usize new_capacity = this->data.size + additional_size;
            if (this->data.capacity < new_capacity) {
                if (psh_unlikely(!this->data.resize(new_capacity))) {
                    return STATUS_FAILED;
                }
            }
        }

        char* string_buf = this->data.buf;
        if (join_element.data.size != 0) {
            usize first_idx = 0;

            // If the string was empty, join the first string without accounting for the joining element.
            if (previously_empty) {
                StringView const& first_js = join_strings[0];

                memory_copy(string_buf + this->data.size, first_js.data.buf, first_js.data.size);
                this->data.size += first_js.data.size;

                ++first_idx;
            }

            // Join remaining strings.
            for (usize idx = first_idx; idx < join_strings.size; ++idx) {
                StringView const& js = join_strings[idx];

                usize previous_size = this->data.size;
                memory_copy(string_buf + previous_size, join_element.data.buf, join_element.data.size);
                memory_copy(string_buf + previous_size + join_element.data.size, js.data.buf, js.data.size);
                this->data.size += join_element.data.size + js.data.size;
            }
        } else {
            for (StringView const& js : join_strings) {
                memory_copy(this->data.buf + this->data.size, js.data.buf, js.data.size);
                this->data.size += js.data.size;
            }
        }

        // Append a null terminator.
        this->data.buf[this->data.size] = 0;

        return STATUS_OK;
    }
}  // namespace psh
