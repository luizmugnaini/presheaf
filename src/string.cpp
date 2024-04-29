/// Implementation of the string.h header.
///
/// Author: Luiz G. Mugnaini A. <luizmugnaini@gmail.com>
#include <psh/string.h>

#include <cstring>

namespace psh {
    StrCmpResult str_cmp(StrPtr lhs, StrPtr rhs) {
        i32 const cmp = std::strcmp(lhs, rhs);

        StrCmpResult res;
        if (cmp == 0) {
            res = StrCmpResult::Equal;
        } else if (cmp < 0) {
            res = StrCmpResult::LessThan;
        } else {
            res = StrCmpResult::GreaterThan;
        }

        return res;
    }

    bool str_equal(StrPtr lhs, StrPtr rhs) {
        return (std::strcmp(lhs, rhs) == 0);
    }
}  // namespace psh
