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

#include "psh_string.hpp"

#include <string.h>
#include "psh_memory.hpp"

// Due to STB code being public domain, we separate its implementation from the psh_string module.
#include "psh_impl_stb_sprintf.cpp"

namespace psh {
    psh_proc usize cstring_length(cstring str) psh_no_except {
        usize length = 0;
        if (psh_likely(str != nullptr)) {
            length = strlen(str);
        }
        return length;
    }

    psh_proc StringCompareResult string_compare(String lhs, String rhs) psh_no_except {
        i32                 cmp = memcmp(lhs.buf, rhs.buf, psh_min_value(lhs.count, rhs.count));
        StringCompareResult result;
        if (cmp == 0) {
            result = StringCompareResult::EQUAL;
        } else if (cmp < 0) {
            result = StringCompareResult::LESS_THAN;
        } else {
            result = StringCompareResult::GREATER_THAN;
        }
        return result;
    }

    psh_proc bool string_equal(String lhs, String rhs) psh_no_except {
        usize length    = lhs.count;
        bool  are_equal = true;

        are_equal &= (rhs.count == length);
        if (are_equal) {
            are_equal &= (memcmp(lhs.buf, rhs.buf, length) == 0);
        }

        return are_equal;
    }

    psh_proc Status join_strings(DynamicString& target, FatPtr<String const> join_strings, String join_element) psh_no_except {
        bool previously_empty = (target.count == 0);

        // Resize the string ahead of time.
        {
            // Get the number of characters that will be added to the string.
            usize additional_length = 1;
            {
                // Add the size accumulated by the join element.
                if (join_element.count != 0) {
                    additional_length += previously_empty ? ((join_strings.count - 1u) * join_element.count)
                                                          : (join_strings.count * join_element.count);
                }

                // Account for the size of each of the joining strings.
                for (String const& s : join_strings) {
                    additional_length += s.count;
                }
            }

            usize new_capacity = target.count + additional_length;
            if (target.capacity < new_capacity) {
                if (psh_unlikely(!dynamic_array_reserve(&target, new_capacity))) {
                    return STATUS_FAILED;
                }
            }
        }

        u8*   string_buf        = reinterpret_cast<u8*>(target.buf);
        usize new_string_length = target.count;
        if (join_element.count != 0) {
            usize first_idx = 0;

            // If the string was empty, join the first string without accounting for the joining element.
            if (previously_empty) {
                String const& first_js = join_strings[0];

                memory_copy(string_buf + new_string_length, reinterpret_cast<u8 const*>(first_js.buf), first_js.count);
                new_string_length += first_js.count;

                ++first_idx;
            }

            // Join remaining strings.
            for (usize idx = first_idx; idx < join_strings.count; ++idx) {
                String const& js = join_strings[idx];

                usize previous_size = new_string_length;
                memory_copy(string_buf + previous_size, reinterpret_cast<u8 const*>(join_element.buf), join_element.count);
                memory_copy(string_buf + previous_size + join_element.count, reinterpret_cast<u8 const*>(js.buf), js.count);
                new_string_length += join_element.count + js.count;
            }

            target.count = new_string_length;
        } else {
            for (String const& js : join_strings) {
                memory_copy(string_buf + new_string_length, reinterpret_cast<u8 const*>(js.buf), js.count);
                new_string_length += js.count;
            }
        }

        target.buf[new_string_length] = 0;
        target.count                  = new_string_length;

        return STATUS_OK;
    }

    // -------------------------------------------------------------------------------------------------
    // String formatting wrapper functions.
    // -------------------------------------------------------------------------------------------------

    psh_proc psh_attribute_disable_asan i32 string_format_list(char* buf, i32 count, cstring fmt, va_list va) psh_no_except {
        impl::SprintfContext c;

        if ((count == 0) && !buf) {
            c.length = 0;

            impl::string_format_list_with_callback(impl::count_clamp_callback, &c, c.tmp, fmt, va);
        } else {
            c.buf    = buf;
            c.count  = count;
            c.length = 0;

            impl::string_format_list_with_callback(impl::clamp_callback, &c, impl::clamp_callback(nullptr, &c, 0), fmt, va);

            // zero-terminate
            i32 l = static_cast<i32>(c.buf - buf);
            if (l >= count) {
                // should never be greater, only equal (or less) than count
                l = count - 1;
            }
            buf[l] = 0;
        }

        return c.length;
    }

    psh_proc psh_attribute_disable_asan i32 string_format(char* buf, i32 count, cstring fmt, ...) psh_no_except {
        va_list va;
        va_start(va, fmt);

        i32 result = string_format_list(buf, count, fmt, va);

        va_end(va);
        return result;
    }
}  // namespace psh
