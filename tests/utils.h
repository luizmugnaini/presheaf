/// Utility functions for writing tests.
///
/// @author Luiz G. Mugnaini. A. <luizmugnaini@gmail.com>
#pragma once

#include <cstdio>

/// Log to the console that the test passed.
///
/// * context:  String representing the context of the test, for instance, the name of the test
///             function that should be logged.
inline void log_passed(char const* const context) {
    std::printf("\x1b[1;32m[PASSED]\x1b[0m: %s.\n", context);
}
