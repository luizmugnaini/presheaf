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

#include <psh/string.h>

#include <psh/memory_utils.h>
#include <psh/option.h>
#include <cstring>
#include <string>

namespace psh {
    // -----------------------------------------------------------------------------
    // - String utilities implementation -
    // -----------------------------------------------------------------------------

    usize str_size(strptr str) noexcept {
        usize res = 0;
        if (psh_likely(str != nullptr)) {
            res = std::char_traits<char>::length(str);
        }
        return res;
    }

    StrCmpResult str_cmp(strptr lhs, strptr rhs) noexcept {
        i32          cmp = std::strcmp(lhs, rhs);
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
        return (std::strcmp(lhs, rhs) == 0);
    }

    bool is_utf8(char c) noexcept {
        return (0x1F < c && c < 0x7F);
    }

    // -----------------------------------------------------------------------------
    // - String view implementation -
    // -----------------------------------------------------------------------------

    StringView::StringView(strptr str) noexcept
        : data{str, str_size(str)} {}

    // -----------------------------------------------------------------------------
    // - String implementation -
    // -----------------------------------------------------------------------------

    void String::init(Arena* arena, usize capacity) noexcept {
        this->data.init(arena, capacity);
    }

    void String::init(Arena* arena, StringView sv) noexcept {
        this->data.init(arena, sv.data.size + 1);
        memory_copy(
            reinterpret_cast<u8*>(this->data.buf),
            reinterpret_cast<u8 const*>(sv.data.buf),
            sv.data.size);
        this->data.buf[sv.data.size + 1] = 0;
        this->data.size                  = sv.data.size;
    }

    String::String(Arena* _arena, usize _capacity) noexcept {
        this->init(_arena, _capacity);
    }

    String::String(Arena* arena, StringView sv) noexcept {
        this->init(arena, sv);
    }

    StringView String::view() const noexcept {
        return StringView{this->data.buf, this->data.size};
    }

    Status String::join(FatPtr<StringView const> strs, strptr join_cstr) noexcept {
        Option<StringView> join_sv{join_cstr};
        bool const         was_empty = (this->data.size == 0);

        usize additional_size = 1;  // Account for the null terminator.
        {
            if (join_sv.has_val) {
                // Regarding the join string: if the string is currently empty, we are only going to
                // use the total number of `strs` to be joined minus one. Otherwise, we shall add a
                // join string to the end of the current string data, and just then join `strs`.
                additional_size += was_empty ? ((strs.size - 1) * join_sv.val.data.size)
                                             : (strs.size * join_sv.val.data.size);
            }

            // Account for the size of each of the strings.
            for (StringView const& s : strs) {
                additional_size += s.data.size;
            }
        }
        usize new_capacity = this->data.size + additional_size;

        if (this->data.capacity < new_capacity) {
            if (psh_unlikely(this->data.resize(new_capacity) == Status::FAILED)) {
                return Status::FAILED;
            }
        }

        if (join_sv.has_val && (join_sv.val.data.size != 0)) {
            usize init_idx = 0;

            // If the string was empty, we omit the first `sjoin`.
            if (was_empty) {
                StringView const& s0 = strs[0];
                std::memcpy(this->data.buf + this->data.size, s0.data.buf, s0.data.size);
                this->data.size += s0.data.size;
                init_idx += 1;
            }

            for (usize idx = init_idx; idx < strs.size; ++idx) {
                StringView const& si = strs[idx];
                std::memcpy(this->data.buf + this->data.size, join_sv.val.data.buf, join_sv.val.data.size);
                std::memcpy(
                    this->data.buf + this->data.size + join_sv.val.data.size,
                    si.data.buf,
                    si.data.size);
                this->data.size += join_sv.val.data.size + si.data.size;
            }
        } else {
            for (StringView const& s : strs) {
                std::memcpy(this->data.buf + this->data.size, s.data.buf, s.data.size);
                this->data.size += s.data.size;
            }
        }

        // Append a null terminator.
        this->data.buf[this->data.size] = 0;

        return Status::OK;
    }

    Status String::join(std::initializer_list<StringView> strs, strptr sjoin) noexcept {
        return this->join(FatPtr<StringView const>{strs.begin(), strs.size()}, sjoin);
    }
}  // namespace psh
