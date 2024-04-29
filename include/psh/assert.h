#pragma once

#include <psh/io.h>

namespace psh {
    // Internal definition of the assert macros
    void assert_(
        bool      expr_res,
        StrPtr    expr_str,
        StrPtr    msg  = "",
        LogInfo&& info = LogInfo{LogLevel::Fatal});
}  // namespace psh

/// Assert that an expression evaluates to true.
#define psh_assert(expr) psh::assert_(expr, #expr)

/// Assert that an expression evaluates to true and write a descriptive message in case of failure.
#define psh_assert_msg(expr, msg) psh::assert_(expr, #expr, msg)

/// Macro used to mark code-paths as unreachable.
#define psh_unreachable()                                                  \
    do {                                                                   \
        psh::log(psh::LogLevel::Fatal, "Codepath should be unreachable!"); \
        psh::abort_program();                                              \
    } while (0);

/// Simple macro discarding an unused result.
#define psh_discard(x) (void)x
