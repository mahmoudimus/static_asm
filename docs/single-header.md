# Techniques for Creating a Single-Header Library

An amalgamation tool can combine source files, but it cannot guarantee that the
result has correct linkage and shared state across translation units. Check
these C++ rules when preparing a single-header distribution.

This project runs [quom](https://github.com/Viatorus/quom) from
[scripts/amalgamate.sh](../scripts/amalgamate.sh). CI runs
[clang-tidy](https://clang.llvm.org/extra/clang-tidy/)
with the `google-build-using-namespace` check; that check covers the first
rule, not the linkage and state rules below.

## 1. Avoid global `using namespace` in amalgamated source

`using namespace` at global scope in a `.cpp` file affects every translation
unit if that file's contents are later included in a header. It can introduce
name collisions in consumers' code.

**Recommended patterns instead:**

```cpp
namespace MyLib {
    struct Foo { Foo(); };
    inline Foo::Foo() = default;
}
```

or

```cpp
namespace MyLib { struct Foo { Foo(); }; }
inline MyLib::Foo::Foo() = default;
```

## 2. Place implementation details in a nested namespace

Public APIs should live in the main namespace. Put implementation details in
a nested namespace such as `detail` or `impl`. This marks the API boundary for
readers; it does not make those names inaccessible to C++ callers.

**Common conventions:**

```cpp
namespace MyLib {
    namespace detail {           // very widely used
        // internal classes, functions, etc.
    }
}
```

or

```cpp
namespace MyLib::impl {          // shorter, also common
    // internal implementation details
}
```

C++17 added the compact nested namespace syntax:

```cpp
namespace MyLib::detail {
    class InternalHelper { /* ... */ };
}
```

## 3. Give shared header state one definition

Moving a file-scope `static` variable into a header gives each translation
unit its **own copy**. That is legal C++, but mutable state can then diverge
between callers. If the state must be shared program-wide, use a C++17 `inline`
variable, such as a `static inline` class member.

**Modern (C++17+) solution:**

```cpp
// Before (in .cpp)
namespace MyLib {
    static int s_counter = 0;

    int next_id() {
        return ++s_counter;
    }
}
```

```cpp
// After (safe for header)
namespace MyLib {
    namespace detail {
        struct Globals {
            static inline int counter = 0;
        };
    }

    inline int next_id() {
        return ++detail::Globals::counter;
    }
}
```

The `static inline` class member denotes one program-wide entity across
translation units. An `inline` namespace-scope variable can serve the same
purpose without the wrapper class.

## 4. Mark header-defined external functions `inline`

Mark non-template functions with external linkage `inline` when their
definitions appear in a header included by multiple translation units. This
includes out-of-class member definitions. Functions defined inside a class,
`constexpr` functions, and `consteval` functions are already inline.

For header-only code, write `inline` directly:

```cpp
namespace MyLib::detail {
    struct Helper {
        void do_work();
    };

    inline void Helper::do_work() {
        // implementation
    }
}
```

If a project uses an `inline_t` placeholder in separately compiled `.cpp`
files, its amalgamation step must explicitly turn that placeholder into
`inline`. This repository's script does not perform such a rewrite.

## Summary: the four rules

1. Keep global `using namespace` directives out of amalgamated implementation files.
2. Put implementation details in a nested namespace such as `detail` or `impl`.
3. Use an `inline` variable when mutable state must be shared across translation units.
4. Mark header-defined, non-template functions with external linkage `inline`.

After generating a single header, include it from two separate translation
units and link them together. A one-file build cannot detect duplicate
external definitions. If shared state matters, call into it from both
translation units to check that they observe the same object.
