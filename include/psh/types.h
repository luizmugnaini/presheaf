#pragma once

#include <cstdint>

#if defined(PSH_NAMESPACED_TYPES)
namespace psh {
#endif
    /// Unsigned integer type.
    using u8    = std::uint8_t;
    using u16   = std::uint16_t;
    using u32   = std::uint32_t;
    using u64   = std::uint64_t;
    using usize = u64;

    /// Signed integer type.
    using i8    = std::int8_t;
    using i16   = std::int16_t;
    using i32   = std::int32_t;
    using i64   = std::int64_t;
    using isize = i64;

    /// Memory-address types.
    using uptr = u64;
    using iptr = i64;

    /// Floating-point types.
    using f32 = float;
    using f64 = double;

    /// Immutable zero-terminated string type
    ///
    /// A pointer to a contiguous array of constant character values.
    using StrPtr = char const*;

    /// String literal type.
    struct StringLiteral {
        StrPtr str;

        template <usize N>
        consteval StringLiteral(char const (&_str)[N]) : str{_str} {}
    };
#if defined(PSH_NAMESPACED_TYPES)
}
#endif
