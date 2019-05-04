# Alloy Style-Guide

This document describes the C++ coding standards that all contributors are
expected to adhere to when presenting contributions to the Alloy project.

This guide is not meant to be the strict set of definitive rules about
where to put braces, or how many blank lines to leave between functions.
Rather, this guide defines the expected best-practices, and the
bare-minimum that the project is expected to follow.

As with all rules, there is a time and place where they may need to be
broken -- but this should be reached after discussion (ideally during
a code-review from a pull-request).

## <a name="naming"></a>Naming

* Class identifiers should be named in `lower_snake_case`. \
  **example:**
  ```c++
  class foo_system;
  ```
* Variable names should be named in `lower_snake_case`. \
  **example:**
  ```c++
  auto counter = 1;
  ```
* Function names should be in `lower_snake_case`. \
  **example:**
  ```c++
  void do_something();
  ```
* Template types and variables should be in `PascalCase` (upper camel-case). \
  **example:**
  ```c++
  template<typename T, typename...Args>
  T make(Args&&...args);
  ```

## <a name="style"></a>Style

## <a name="project-structure"></a>Project Structure

Each sub-project within **Alloy** receives its own unique sub-`namespace` under
`namespace alloy` (e.g. `alloy::core` for `alloy-core`).

Subdirectories of an `include` directory _do not_ and _should not_ match the
include structure after the name of the primary project (e.g.
`alloy/core/math/matrix/matrix3.hpp` should be `alloy::core` and not
`alloy::core::math::matrix`).

Namespaces in C++ are _not_ package identifiers like in languages
like Java. Using a 'package-path' like namespace encourages bad behavior, like
using a `using` directive to pull in types (which leads to maintenance
nightmares). Additionally, a `use`d symbol can often lose its readability if
the `namespace` identifier encodes the information. For example:

```
using namespace org::system::config;
using namespace org::filesystem::file;

if (set_contents("foo.bar", value)) {
  std::cout << "Success" << std::endl>>;
}
```

The usage of `set_contents` is ambiguous. Is it
`org::system::config::set_contents`, where it sets the configuration `foo.bar`
to `value`? Or is it `org::filesystem::file::set_contents` where it sets the
contents of the file `foo.bar` to `value`?

Additionally, `namespace`s in C++ have different semantics than java
package-paths. Symbols in C++ are looked up from every namespace starting with
the current most nested namespace, all the way up to the global-namespace.
Combine this with header include search-paths, and it can lead to symbols
being accidentally used based on changing header paths.

## Restrictions

* Avoid `using` directives
  * Except if used to pull in an `inline namespace`, like `std::literals`