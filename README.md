# Presheaf Library

This is a C++ library that I use across my projects as an alternative to the STL. It's written in
C++20 and its only dependence is a compiler supporting the language features. Compilers that are
ensured to work are: Clang, GCC, MSVC.

The code is written with simplicity of use in mind and does not adhere to the principles of the so
called "modern" C++. There is no use of exceptions, inheritance, and only a few standard headers are
used (i.e. `type_traits`, `stdint`, etc) for more information, check the [style guide](./STYLE_GUIDE.md).

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
your soul be forgiven), you can add the following two lines to add Presheaf as a library:
```cmake
add_library(presheaf src/all.cc)
target_compile_options(presheaf PUBLIC cxx_std_20)
```
# Library compile-time options

The following `#define` macros can be used to tweak the behaviour of the library as you want it:
- `PSH_NAMESPACED_TYPES`: add the library namespace to all fundamental types such as `i32` (which
  would become `psh::i32`).
- `PSH_DISABLE_LOGGING`: silence all logging calls.
- `PSH_DISABLE_ASSERTS`: silence all assert calls.
- `PSH_CHECK_BOUNDS`: check the bounds of index-based memory access.
- `PSH_DEBUG`: Enables all debug checks.

