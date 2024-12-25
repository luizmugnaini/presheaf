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
/// Description: Implementation of the system time interface.
/// Author: Luiz G. Mugnaini A. <luizmuganini@gmail.com>

#include <psh/time.hpp>

#if defined(PSH_OS_WINDOWS)
#    include <Windows.h>
#elif defined(PSH_OS_UNIX)
#    include <errno.h>
#    include <string.h>
#    include <time.h>
#endif

namespace psh {
    f64 current_time_in_seconds() psh_no_except {
        f64 curr_time = -1.0;

#if defined(PSH_OS_WINDOWS)
        LARGE_INTEGER frequency;
        LARGE_INTEGER counter;

        bool success = (QueryPerformanceFrequency(&frequency) != 0);
        success &= (QueryPerformanceCounter(&counter) != 0);

        if (psh_likely(success)) {
            curr_time = static_cast<f64>(counter.QuadPart) / static_cast<f64>(frequency.QuadPart);
        }
#elif defined(PSH_OS_UNIX)
        timespec time_spec;

        bool success = (clock_gettime(CLOCK_MONOTONIC, &time_spec) == 0);

        if (psh_likely(success)) {
            curr_time = static_cast<f64>(time_spec.tv_sec) + (static_cast<f64>(time_spec.tv_nsec) / 1e9);
        }
#endif

        return curr_time;
    }

    void sleep_milliseconds(f64 ms) psh_no_except {
#if defined(PSH_OS_WINDOWS)
        u32 ms_count = ((0.0 < ms) && (ms < 1.0)) ? 1 : static_cast<u32>(ms);
        Sleep(ms_count);
#elif defined(PSH_OS_UNIX)
        timespec request_sleep;
        request_sleep.tv_sec     = static_cast<time_t>(ms / 1000);
        request_sleep.tv_nsec    = (static_cast<long>(ms) - static_cast<long>(request_sleep.tv_sec) * 1000) * 1'000'000;
        timespec remaining_sleep = {};

        i32 status = nanosleep(&request_sleep, &remaining_sleep);
        if (status == -1) {
            psh_log_error_fmt("Failed to sleep due to error: %s", strerror(errno));
        }
#endif
    }
}  // namespace psh
