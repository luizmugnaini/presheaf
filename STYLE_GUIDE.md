# Good Practices

- No third-party code.
- Classes & inheritance: no.
- No exceptions.
- No RTTI.
- No shared pointers, manage your lifetimes.
- Don't be afraid of pointers, know how to use them and be careful, that's all.
- Where are my beloved private members? C++ has pointers and even if you have structure with a
  private member you can still easily read and write to its memory address. In other words... private
  members are useless and introduce getters and setters which are horrible design patterns. If you
  are not convinced, please run the following code with any compiler:
```cpp
#include <cstdio>

#define stringify(x) #x

class SuperPrivate {
public:
  SuperPrivate(char const *name, int id) : name{name}, id{id} {}
private:
  char const *name;
  int id;
};

int main() {
  SuperPrivate my_foo{"Super secure private TM", 5};
  char *oops = reinterpret_cast<char *>(&my_foo);
  std::printf("%s { name: %s, id: %d }",
              stringify(my_foo),
              *reinterpret_cast<char const **>(oops),
              *reinterpret_cast<int *>(oops + sizeof(char const *)));
  return 0;
}
```
- Your templates should be as simple as possible, and should not be overused.
- Use the least amount of STL you can - possibly don't use it at all as it can introduce
  considerable bloat to the software and worsen both the compile time and runtime, as well as a
  larger memory footprint.
- Unfortunately, some STL constructs (`std::initializer_list` for instance) are compiler intrinsic so
  we have to include headers providing them if we wish to use these features.
- Prefer the C++ wrappers for the libc headers, like `<cstdio>`. This keeps the namespace clean.
- Always provide useful documentation. Note however that some docs may not be really that relevant:
  for instance, the method `psh::Array::size_bytes` is obvious, it simply returns the size of the
  array in bytes.
- Don't write in a comment what can be clearly stated in code.
- Don't try to be clever, be pragmatic and simple.
- Use the following comment tags (BUG, FIXME, TODO, NOTE) for an easier way to search them later.
- Write fault tolerant code, if some value fails to meet some expectation, try to find a way to make
  it a default value if possible.
- In case there is no possible default value, don't be shy of asserts! If you **are** going to fail,
  fail as fast as possible.
- Always use the type aliases declared in `<psh/core.h>`.

# Formatting

- For C and C++ files: just use `clang-format` with the custom `.clang-format` configuration file.

# Regarding `auto`

Don't do `auto`, simple as that. Why?
- Many may argue that it improves refactoring, but the fact is that if your return type changed,
  the behaviour of the object changed and you should probably change the code anyway. So no
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
will only make the code harder to read and give you the false feeling that that codepath is optimized
in some way. Always remember that the compiler gives you no guarantee that the `constexpr` function
*will* be evaluated at compile time, just that it *can*.

# The `[[nodiscard]]` attribute

I opted out of the use of the `nodiscard` attribute simply because it litters the codebase and
decreases significantly the readability of the code just because of noise. Moreover, the kinds of
bugs that `nodiscard` prevents are actually pretty uninteresting and should be uncommon in any well
written codebase.

# Strings

- Raw string pointers should always have the type `char const* const`, that is, a constant pointer
  to an array of constant characters. If a string is being returned from a function, the return type
  should be `char const*` since we can't ensure that the caller won't change the pointer itself.

# Naming Conventions

We mostly follow the advice given by the [C++ Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#S-naming).

## General Advice

- Have clear names over succinct ones.
- Variables and functions should be named using snake case: `my_variable`.
- Structs should be named using pascal case: `MyStructFoo`. The same applies for enums.
- Global constants (declared with `const` or `constexpr`) should be all caps and in
  snake case: `MY_CONSTANT_VAR`.
- Enum types should be named with pascal case and their members should
  be all caps with snake case, as they are constant expressions.

## Classes and Structs

- Always use the keyword `struct`, the keyword `class` is useless and just adds another word to the grammar.
- Private variables should *not* be used, everything should be accessible. The user is responsible for their
  usage, not the library - if one wants to break some implicit contract, let it be.
- Regarding **method argument names**, if you need to avoid name collisions with member variables, you can
  put an underscore to the start of the argument: `_arg_avoiding_collision`.

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
