# Presheaf Library

> WARNING: This library is currently in a very volatile state. Breaking changes will happen all the time
> until I think the API is good enough.

This is a C++ library that I use across my projects as an alternative to the STL. It's written in
C++20 and its only dependency is libc and a few OS headers (see [here](#dependencies) for more info).
Compilers that are ensured to work are: Clang, GCC, MSVC.

The code is written with simplicity and performance in mind, hence it does **not** adhere to the
principles of the so called "modern" C++. There is no use of exceptions, inheritance, smart
pointers, etc. For more information, check the [style guide](./STYLE_GUIDE.md).

# Building

# Custom Build System

The library comes with its custom build system [`build.lua`](./build.lua), which will manage the project
with many custom options. An usage _example_ would be:

```sh
lua build.lua -release -clang -dll -- -DPSH_CHECK_BOUNDS -fsanitize=undefined
```

where you are building the release version of Presheaf as a DLL (or shared object if on Unix)
using Clang. Additionally you are also activating the UB sanitizer for the build, and bounds checking
assertions. The script will make sure to print out the final invoked command for the compilation, so
no mysteries arise in the build process.

For more information, please run the script with the `-help` flag or refer to the file itself.

## Ad-Hoc Approach

The library has a bundled compilation unit `src/all.cpp` which may be used if you wish to compile as
a unity build. This can be as simple as, e.g.:

```sh
# Build object files without linking.
clang++ -c -std=c++20 -Iinclude src/all.cpp -o presheaf.o

# Archive object files into a library.
llvm-ar rc libpresheaf.a presheaf.o
```

# Integrating with another project

Having no inherent build system, this library is extremely easy to integrate to any projects. Since
unity build files such as `presheaf/src/all.cpp` are available, you can also embed the library
directly inside of your project.

You may either create a static or shared library binary or you can simply add the include directory
`presheaf/include` and the source file `presheaf/src/all.cpp` to your compilation command.

If you are using CMake (may your soul be forgiven), you can include the following lines to add
Presheaf as a library:

```cmake
add_library(presheaf "presheaf/src/all.cpp")
target_include_directories(presheaf PUBLIC "presheaf/include")
set_property(TARGET presheaf PROPERTY CXX_STANDARD 20)
```

and of course, you should change the paths to the correct ones for your project structure.

# Library compile-time options

The following `#define` macros can be used to tweak the behaviour of the library as you want it:
- `PSH_CHECK_BOUNDS`: Check the bounds of index-based memory access.
- `PSH_CHECK_MEMCPY_OVERLAP`: Check if the pointer ranges overlap before calling `memcpy`.
- `PSH_DEBUG`: Enables all of the above listed debug checks.
- `PSH_ABORT_AT_MEMORY_ERROR`: Aborts the program at any memory (re)allocation errors, for debugging
  purposes. Not enabled by `PSH_DEBUG` due to its intrusiveness on the behaviour of fault tolerancy.
- `PSH_DISABLE_LOGGING`: Silence all logging calls.
- `PSH_DISABLE_ASSERTS`: Silence all assert calls.
- `PSH_DEFINE_SHORT_NAMES`: Define shorter names for essential macros. Examples: `psh_max_value` to
  `max_value`, `psh_no_alias` to `no_alias`, etc.

# Dependencies

The only dependencies of the Presheaf library are libc and OS headers. Explicitly, the only external
headers included are:

```c
#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>

// On Windows systems:
#include <Windows.h>

// On Unix systems:
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <sys/mman.h>
```
