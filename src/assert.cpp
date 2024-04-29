#include <psh/assert.h>

namespace psh {
    void assert_(bool expr_res, StrPtr expr_str, StrPtr msg, LogInfo&& info) {
#if defined(PSH_DEBUG) || defined(PSH_ENABLE_ASSERTS)
        if (!expr_res) {
            log_fmt(info, "Assertion failed: %s, msg: %s", expr_str, msg);
            abort_program();
        }
#endif
    }
}  // namespace psh
