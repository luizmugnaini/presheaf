# Presheaf Library

This is a C++ library that I use across my projects as an alternative to the STL. It's written in
C++20 and its only dependence is a compiler supporting the language features. Compilers that are
ensured to work are: Clang, GCC, MSVC, Clang-cl (MSVC-based Clang).

There is no use of exceptions, inheritance, and only a few STL headers are used (i.e.
`type_traits`, `cstdio`, etc).

## Development

You can build the project in the classic way any CMake project (should) work:

```
cmake -S . -B build
cmake --build build
```

On the other hand you can also use the simple Python script `mk.py` that manages generating the CMake
build system, building the project, building the documentation, running tests, and running the
project binaries in general.

The script requires Python 3.10 or higher. Here are some *optional* dependencies used by default:

- [Python](https://www.python.org/) (version 3.10 or greater).
- [Ninja](https://ninja-build.org/) (version 1.11.1 or greater).
- [Mold](https://github.com/rui314/mold) (version 2.4 or greater).
- [Clang-Format](https://clang.llvm.org/docs/ClangFormat.html).
- [codespell](https://github.com/codespell-project/codespell).

These defaults may be easily changed by tweaking their corresponding constants at the top of `mk.py`.

If you run the `python mk.py --help` command, you'll be prompted with all of the functionality of the
script.
