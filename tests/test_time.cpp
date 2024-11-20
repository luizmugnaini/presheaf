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

namespace psh::test::time {
    psh_internal void fetch_system_time() {
        f64 time = current_time_in_seconds();
        psh_assert(time > 0.0);

        report_test_successful();
    }

    psh_internal void system_sleep() {
        f64 milliseconds_to_sleep = 45.6;

        f64 sleep_start = current_time_in_seconds();
        sleep_milliseconds(milliseconds_to_sleep);
        f64 sleep_end = current_time_in_seconds();

        f64 sleep_duration_milliseconds = (sleep_end - sleep_start) * 1000;
        psh_assert(sleep_duration_milliseconds >= milliseconds_to_sleep);

        report_test_successful();
    }

    psh_internal void run_all() {
        fetch_system_time();
        system_sleep();
    }
}  // namespace psh::test::time

#if !defined(PSH_TEST_NOMAIN)
int main() {
    psh::test::vec::run_all();
    return 0;
}
#endif
