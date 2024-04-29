# Good Practices

- Your templates should be as simple as possible, and should not be overused.
- Don't use inheritance, instead share behaviour via concepts.
- Use the least amount of STL you can - possibly don't use it at all as it can introduce
  considerable bloat to the software and worsen both the compile time and runtime, as well as a
  larger memory footprint.
- Don't be afraid of pointers, know how to use them and be careful, that's all.
- Prefer the C++ wrappers for the libc headers, like `<cstdio>`. This keeps the namespace clean.
- Don't use third-party code when you can easily write it yourself.
- Always provide useful documentation.
- Don't write in a comment what can be clearly stated in code.
- Don't try to be clever, be pragmatic and simple.
- Use the following comment tags (BUG, FIXME, TODO, NOTE) for an easier way to search them later.
- Write fault tolerant code, if some value fails to meet some expectation, try to find a way to make
  it a default value if possible.
- In case there is no possible default value, don't be shy of asserts! If you **are** going to fail,
  fail as fast as possible and report the error.
- Always use the type aliases declared in `psh/types.h`.
- Don't use mutable global variables. In an application, these might even serve some purpose, but
  not in a library.

# Formatting

- For C and C++ files: just use `clang-format` with the custom `./.clang-format` configuration file.

# Regarding `auto`

Almost never `auto`, however in case you have a cast you may use `auto` to avoid repetition or
trivially known return types:
```cpp
auto scratch_arena = arena.make_scratch();
auto* const memory = reinterpret_cast<u8*>(some_ptr);
```
Notice that `auto` should only be used for the **base** type, that is, qualifiers such as `*` or
`const` should be written **explicitly**.

# The `const` qualifier

- The `const` qualifier should *always* be put to the right of the type, which allows one to read
  any type seamlessly from right to left. For instance, `int const` is a constant integer, while
  `int const* const` is a constant pointer to an array of constant integers.

# The `constexpr` qualifier

Prefer using `constexpr` over a `#define` as it permits type safety.

# Strings

- Raw string pointers should always have the type `char const* const` (`StrPtr` seen in `psh/types.h`),
  that is, a constant pointer to an array of constant characters. If a string is being returned
  from a function, the return type should be `char const*` since we can't ensure that the caller
  won't change the pointer itself.

# Naming Conventions

We mostly follow the advice given by the [C++ Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#S-naming).

## General Advice

- Have clear names over succinct ones.

## Variables

- **Common variables** should be named using snake case: `my_variable`.
- **Global constants** (declared with `const` and `constexpr`) should be all
  caps and in snake case: `MY_CONSTANT_VAR`.

## Functions

- **Functions** should be named using snake case: `my_function`.
- **Arguments** should be named using snake case: `my_function_argument`.

### Internal Functions

Internal functions are those that shouldn't be called directly by the end user and should be
appended by one underscore: `my_internal_function_`.

## Classes and Structs

- Both **classes and structs** should be capitalized in each first letter of a word: `MyClass`.
- **Private member variables** should be named using snake case and end with a trailing underscore:
  `my_private_member_var_`.
- **Public member variables** should be named using snake case: `my_public_member_var`.
- **Methods** should be named using snake case: `my_method`.
- Regarding **method argument names**, if you need to avoid name collisions with public member
  variables, you can put an underscore to the end of the argument: `_arg_avoiding_collision`.

# Enums

- **Enums and its members** should be capitalized in each first letter of a word:
  ```cpp
  enum MyEnum {
      EnumMemberA,
      EnumMemberB,
      EnumMemberC,
  };
  ```

# Error Handling

> **Exceptions are not allowed at all.**

- Handle all of your errors through enumerations and switch statements.
- Always document if a function or method may return an error, and which errors might be returned.
- Don't ignore possible errors.

# Function declarations

We all know that `const` in a declaration doesn't have any effect on the variable, after all, it's
only a declaration. However, it **serves as documentation**. Therefore, if a function definition
uses `const` parameters, its declaration should mention that:
```cpp
int do_some_computation(float const a, int const b);

...

int do_some_computation(float const a, int const b) {

    ...

    return result;
}
```

# Recommended reads

- [Orthodox C++ principles](https://gist.github.com/bkaradzic/2e39896bc7d8c34e042b).
