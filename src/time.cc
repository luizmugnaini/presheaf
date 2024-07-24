///                            Presheaf Library
///    Copyright (C) 2024 Luiz Gustavo Mugnaini Anselmo
///
///    This program is free software; you can redistribute it and/or modify
///    it under the terms of the GNU General Public License as published by
///    the Free Software Foundation; either version 2 of the License, or
///    (at your option) any later version.
///
///    This program is distributed in the hope that it will be useful,
///    but WITHOUT ANY WARRANTY; without even the implied warranty of
///    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///    GNU General Public License for more details.
///
///    You should have received a copy of the GNU General Public License along
///    with this program; if not, write to the Free Software Foundation, Inc.,
///    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
///
/// Description: Implementation of the system time interface.
/// Author: Luiz G. Mugnaini A. <luizmuganini@gmail.com>

#include <psh/time.h>

#if defined(PSH_OS_WINDOWS_32)
#    include <Windows.h>
#elif defined(PSH_OS_UNIX)
#    include <time.h>
#endif

namespace psh {
    Option<f64> current_time_in_seconds() noexcept {
        Option<f64> curr_time = {};
#if defined(PSH_OS_WINDOWS_32)
        LARGE_INTEGER frequency;
        LARGE_INTEGER counter;
        if (psh_likely(
                (QueryPerformanceFrequency(&frequency) != 0) &&
                (QueryPerformanceCounter(&counter) != 0))) {
            curr_time = static_cast<f64>(counter.QuadPart) / static_cast<f64>(frequency.QuadPart);
        }
#elif defined(PSH_OS_UNIX)
        timespec time_spec;
        if (psh_likely(clock_gettime(CLOCK_MONOTONIC, &time_spec) != 0) {
            curr_time =
                static_cast<f64>(time_spec.tv_sec) + (static_cast<f64>(time_spec.tv_nsec) / 1e9);
        }
#endif
        return curr_time;
    }
}  // namespace psh
