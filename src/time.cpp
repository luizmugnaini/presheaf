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
#    include <time.h>
#endif

namespace psh {
    Option<f64> current_time_in_seconds() noexcept {
        Option<f64> curr_time = {};
#if defined(PSH_OS_WINDOWS)
        LARGE_INTEGER frequency;
        LARGE_INTEGER counter;
        if (psh_likely(
                (QueryPerformanceFrequency(&frequency) != 0) &&
                (QueryPerformanceCounter(&counter) != 0))) {
            curr_time = static_cast<f64>(counter.QuadPart) / static_cast<f64>(frequency.QuadPart);
        }
#elif defined(PSH_OS_UNIX)
        timespec time_spec;
        if (psh_likely(clock_gettime(CLOCK_MONOTONIC, &time_spec) == 0)) {
            curr_time = static_cast<f64>(time_spec.tv_sec) + (static_cast<f64>(time_spec.tv_nsec) / 1e9L);
        }
#endif
        return curr_time;
    }

    void sleep_milliseconds(u32 millis) noexcept {
#if defined(PSH_OS_WINDOWS)
        Sleep(millis);
#elif defined(PSH_OS_UNIX)
#    error "TODO: implement sleep_milliseconds"
        // timespec request_sleep = {
        //     .tv_sec  = 0,
        //     .tv_nsec = static_cast<f64>(millis) / 1000L,
        // };
        // timespec remaining_sleep = {};
        //
        // i32 status = nanosleep(&request_sleep, &remaining_sleep);
        // psh_assert(status != -1);  // TODO: proper error handling.
#endif
    }  // namespace psh
}  // namespace psh
