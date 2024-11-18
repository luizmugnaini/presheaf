# Rules

- No STL (and no third-party code in general).
- No inclusion of C++ wrappers for libc headers.
- No exceptions.
- No RTTI.
- No smart pointers.
- No template logic or template overuse.
- No encapsulation.
- No classes nor inheritance.
- No private members.

This library is created to be as hackable as possible as it only aims to provide the basic building
block of a program (where C failed to deliver). The users are always treated as having better
understanding of their program than the library, so everything is open for them to enjoy and
exploit.

# Good practices

- Strive for POD structures.
- Minimal use of destructors and operator overloads.
- Pointers should **never** indicate ownership, only the possibility that a reference does not exist.
- Don't be afraid of pointers, know how to use them and be careful, that's all.
- Always provide useful documentation. Note however that some docs may not be really that relevant:
  for instance, the method `psh::Array::size_bytes` is obvious, it simply returns the size of the
  array in bytes.
- Don't write in a comment what can be clearly stated in code.
- Don't try to be clever, be pragmatic and simple.
- Use the following comment tags for an easier way to search them later: @BUG, @FIXME, @TODO, @NOTE.
- Write fault tolerant code, if some value fails to meet some expectation, try to use a default value
  whenever possible.
- In case there is no possible default value, don't be shy of asserts! If you **are** going to fail,
  fail as fast as possible. This makes debugging way easier for both the users and the library devs.
- Always use the type aliases declared in `<psh/core.h>`. These provide uniformity for the codebase.

# Formatting

- For C and C++ files: just use `clang-format` with the custom `.clang-format` configuration file.

# Regarding `auto`

Don't do `auto`, simple as that. Why?
- Many may argue that it improves refactoring, but the fact is: If your return type changed,
  the behaviour of the object changed - and you should probably change the code anyway. So no
  refactoring improvement is gained by using `auto` - in fact, it makes it worse to refactor
  by obscuring the type.
- No one has the obligation to use an LSP just because you use.

# The `const` qualifier

- The `const` qualifier should *always* be put to the right of the type, which allows one to read
  any type seamlessly from right to left. For instance, `int const` is a constant integer, while
  `int const* const` is a constant pointer to an array of constant integers.
- Don't use `const` when there is absolutely no need to, it just clutters the code and decreases
  readability. Moreover, C++ has pointers, you can still modify them if you want.

# The `constexpr` qualifier

Prefer using `constexpr` over a `#define` as it permits type safety. You should, however, not abuse
the use of `constexpr`'s for functions that will clearly never be invoked at compile time. The latter
will only make the code harder to read and give you the false feeling that that codepath is optimised
in some way. Always remember that the compiler gives you **no guarantee** that the `constexpr`
function **will** be evaluated at compile time, just that it **can**.

# The `[[nodiscard]]` attribute

I opted out of the use of the `nodiscard` attribute simply because it litters the codebase and
decreases significantly the readability of the code - it adds too much  noise. Moreover, the kinds of
bugs that `nodiscard` prevents are actually pretty uninteresting and should be uncommon in any well
written codebase.

# Naming Conventions

We mostly follow the advice given by the [C++ Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#S-naming).

## General Advice

- Have clear names over succinct ones.
- Variables and functions should be named using snake case: `my_variable`.
- Structs should be named using pascal case: `MyStructFoo`.
- Global constants (declared with `const` or `constexpr`) should be all caps and in snake case:
  `MY_CONSTANT_VAR`.
- Enum types should be named with pascal case and their members should be all caps with snake case,
  as they are constant expressions.

## Classes and Structs

- Always use the keyword `struct`, the keyword `class` is useless and just adds another word to the grammar.
- Private variables should *not* be used, everything should be accessible. The user is responsible for their
  usage, not the library - if one wants to break some implicit contract, let it be.
- Regarding **method argument names**, if you need to avoid name collisions with member variables, you can
  put an underscore to the start of the argument: `arg_avoiding_collision_`.

# Enumerations

- Prefer enumeration structs as they provide namespaced constants.
  ```cpp
  enum struct MyEnum {
      ENUM_MEMBER_A,
      ENUM_MEMBER_B,
      ENUM_MEMBER_C,
  };
  ```

# Error Handling

> **Exceptions are not allowed at all.**

- Handle all of your errors through enumerations and switch statements.
- Always document if a function or method may return an error, and which errors might be returned.
- Don't ignore possible errors.

# Recommended reads

- [Orthodox C++ principles](https://gist.github.com/bkaradzic/2e39896bc7d8c34e042b) has some good insights,
  although we don't follow all of them in this codebase.
