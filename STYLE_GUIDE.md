# Good Practices

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
  auto *oops = reinterpret_cast<char *>(&my_foo);
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
- Unfortunately, some STL constructs (`std::initializer_list` for instance) are compiler intrinsic
  are mingled inside of the STL. 
- Prefer the C++ wrappers for the libc headers, like `<cstdio>`. This keeps the namespace clean.
- Don't use third-party code when you can easily write it yourself.
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
- Always use the type aliases declared in `<psh/types.h>`.

# Formatting

- For C and C++ files: just use `clang-format` with the custom `.clang-format` configuration file.

# Regarding `auto`

Almost never `auto`, however in case you have a cast you may use `auto` to avoid repetition:
```cpp
auto* const memory = reinterpret_cast<u8*>(some_ptr);
```
Notice that `auto` should only be used for the **base** type, that is, qualifiers such as `*` or
`const` should be written **explicitly**.

# The `const` qualifier

- The `const` qualifier should *always* be put to the right of the type, which allows one to read
  any type seamlessly from right to left. For instance, `int const` is a constant integer, while
  `int const* const` is a constant pointer to an array of constant integers.

# The `constexpr` qualifier

Prefer using `constexpr` over a `#define` as it permits type safety. You should, however, not abuse
the use of `constexpr`'s for functions that will clearly never be invoked at compile time.

# The `[[nodiscard]]` attribute

I opted out of the use of the `nodiscard` attribute simply because it litters the codebase and
decreases significantly the readability of the code just because of noise. Moreover, the kinds of
bugs that `nodiscard` prevents are actually pretty uninteresting and should be uncommon in any well
written code.

# Strings

- Raw string pointers should always have the type `char const* const`, that is, a constant pointer
  to an array of constant characters. If a string is being returned from a function, the return type
  should be `char const*` since we can't ensure that the caller won't change the pointer itself.

# Naming Conventions

We mostly follow the advice given by the [C++ Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#S-naming).

## General Advice

- Have clear names over succinct ones.

## Variables

- **Common variables** should be named using snake case: `my_variable`.
- **Global constants** (declared with `const` and `constexpr`) should be all
  caps and in snake case: `MY_CONSTANT_VAR`.
- **Global mutable variables** should be named with a starting `g` and in a snake case style:
  `g_my_mutable_global_var`.

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

# Enumerations

- Prefer enumeration structs as they provide some safety.
- **Enums and its members** should be capitalized in each first letter of a word:
  ```cpp
  enum struct MyEnum {
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

# Recommended reads

- [Orthodox C++ principles](https://gist.github.com/bkaradzic/2e39896bc7d8c34e042b).
