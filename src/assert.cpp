#include <psh/assert.h>

namespace psh {
    void assert_(bool expr_res, StrPtr expr_str, StrPtr msg, LogInfo&& info) {
        if constexpr (ASSERTS_ENABLED) {
            if (!expr_res) {
                log_fmt(info, "Assertion failed: %s, msg: %s", expr_str, msg);
                abort_program();
            }
        }
    }
}  // namespace psh
