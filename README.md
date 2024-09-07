# Presheaf Library

> WARNING: This library is currently in a very volatile state. Breaking changes will happen all the time
> until I think the API is good enough.

This is a C++ library that I use across my projects as an alternative to the STL. It's written in
C++20 and its only dependence is a compiler supporting the language features. Compilers that are
ensured to work are: Clang, GCC, MSVC.

The code is written with simplicity of use in mind and does not adhere to the principles of the so
called "modern" C++. There is no use of exceptions, inheritance, and only a few standard headers are
used for more information, check the [style guide](./STYLE_GUIDE.md).

# Development

The library has a bundled compilation unit `src/all.cc` which may be used if you wish to compile as
a unity build. This is as simple as, e.g.:
```sh
# Build static library.
clang++ -c -std=c++20 -Iinclude src/all.cc -o presheaf.o && llvm-ar rc presheaf.a presheaf.o
# Build all library tests.
clang++ -std=c+20 -Iinclude tests/test_all.cc -o test
```

Another option is to use the `build.lua` script, which will manage to build the project with many
custom options that may be viewed in the file itself. With that said, Lua is, optionally, the only
dependency of the whole project - being only required if you want the convenience of running the build 
script.

# Integrating with another project

You may either create a static or shared library binary or you can simply add the include directory
`include` and the source file `src/all.cc` to your compilation command. If you are using CMake (may
your soul be forgiven), you can include the following lines to add Presheaf as a library:
```cmake
add_library(presheaf src/all.cc)
target_include_directories(presheaf PUBLIC include)
set_property(TARGET presheaf PROPERTY CXX_STANDARD 20)
```
# Library compile-time options

The following `#define` macros can be used to tweak the behaviour of the library as you want it:
- `psh_malloc` and `psh_free`: Function-like macros for overriding the default allocation functions.
- `PSH_DISABLE_LOGGING`: Silence all logging calls.
- `PSH_DISABLE_ASSERTS`: Silence all assert calls.
- `PSH_CHECK_BOUNDS`: Check the bounds of index-based memory access.
- `PSH_CHECK_MEMCPY_OVERLAP`: Check if the pointer ranges overlap before calling `memcpy`.
- `PSH_DEBUG`: Enables all debug checks.
- `PSH_ABORT_AT_MEMORY_ERROR`: Aborts the program at any memory (re)allocation errors, for debugging
  purposes. Not enabled by `PSH_DEBUG` due to its intrusiveness on the behaviour of fault tolerancy.
- `PSH_DEFINE_SHORT_NAMES`: Define shorter names for function-like macros defined in `psh/core.h`,
  `psh/log.h`, and `psh/assert.h`.
