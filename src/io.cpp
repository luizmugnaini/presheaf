/// Implementation of the IO handling.
///
/// Author: Luiz G. Mugnaini A. <luizmugnaini@gmail.com>
#include <psh/io.h>

#include <psh/assert.h>
#include <psh/types.h>

#include <cstdio>
#include <cstdlib>

namespace psh {
    void abort_program() noexcept {
        psh_discard(std::fprintf(stderr, "Aborting program...\n"));
        std::abort();
    }

    StrPtr log_level_str(LogLevel level) {
        StrPtr s;
        switch (level) {
            case LogLevel::Fatal:   s = "\x1b[1;41m[FATAL]\x1b[0m";
            case LogLevel::Error:   s = "\x1b[1;31m[ERROR]\x1b[0m";
            case LogLevel::Warning: s = "\x1b[1;33m[WARNING]\x1b[0m";
            case LogLevel::Info:    s = "\x1b[1;32m[INFO]\x1b[0m";
            case LogLevel::Debug:   s = "\x1b[1;34m[DEBUG]\x1b[0m";
        }
        return s;
    }

    void log(LogInfo&& info, StrPtr msg) {
#if defined(PSH_DEBUG) || defined(PSH_ENABLE_LOGGING)
        psh_discard(std::fprintf(
            stderr,
            "%s [%s:%d] %s\n",
            log_level_str(info.lvl),
            info.file,
            info.line,
            msg));
#endif
    }
}  // namespace psh
