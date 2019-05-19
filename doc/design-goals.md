# Design Goals

**Alloy** tries to achieve several design goals, outlined below:

1. [Types don't force allocation requirements](#types-dont-force-allocation-requirements)
2. [No working constructors](#no-working-constructors)
3. [Functions are marked `noexcept` wherever possible](#functions-are-marked-noexcept-wherever-possible)
4. [Invariants and fundamental preconditions/postconditions are asserted](#invariants-and-fundamental-preconditions-postconditions-are-asserted)
5. [Output parameters are pointers](#output-parameters-are-pointers)
6. [Stored non-owning reference parameters are pointers](#stored-non-owning-reference-parameters-are-pointers)
7. [No Unambiguous Ownership](#no-unambiguous-ownership)

## <a name="types-dont-force-allocation-requirements"></a>Types don't force allocation requirements

In order to provide the most flexibility, none of the types in
**Alloy** impose any allocation requirements (e.g. does not define member
`new` operator).

This allows the consumers to make decisions on memory requirements --
whether it be on the stack, heap, etc.

## <a name="no-working-constructors"></a>No working constructors

Constructors that perform work have various drawbacks:
* They may deceivingly take time, despite looking like simple constructions
* The more complexity is introduced, the more room for error exists
* Constructor bodies execute after members are initialized, which necessitates
  setting default state that will never be used

By preferring simple, non-working constructors that assign the complete
contents of all class members, it allows for providing certain guarantees,
for example:
* Constructors can now be `noexcept`
* Constructors can be marked `constexpr` (provided all members are trivial or
  `constexpr` as well)


To allow for this design, any logic that would typically be in a working
constructor is, instead, restructured into `static` functions which act as
named factories. This provides no additional overhead to the typical
working-constructor approach, and also provides more clarity at the point
of construction.

It is much easier to read:
```c++
auto q = alloy::core::quaternion::from_angles(y, p, r);
```
compared to:
```c++
auto q = alloy::core::quaternion{y, p, r};
```

## <a name="functions-are-marked-noexcept-wherever-possible"></a>Functions are marked `noexcept` wherever possible

Error codes are preferred over throwing exceptions in order to avoid
the complexity that comes with handling the exceptions (e.g.
stack-unwinding).

By following this practice, most functions are marked `noexcept`
which provides API guarantees that the functions will not throw.

## <a name="invariants-and-fundamental-preconditions-postconditions-are-asserted"></a>Invariants and fundamental preconditions/postconditions are asserted

Assertions are used strategically through **Alloy** to provide guarantees on
the algorithmic invariants, as well as fundamental preconditions and postconditions.

**Note:** Assertions are _not_ used to convey recoverable error conditions when
validating preconditions, as this would make it unrecoverable.
Examples of _fundamental_ preconditions:
* Asserting a parameter cannot be null
* Asserting a parameter cannot be outside a threshold

## <a name="output-parameters-are-pointers"></a>Output parameters are pointers

In order to provide consistent semantics to the API, all output-parameters
are passed as pointers, rather than references.

Using references can lead to unobvious or accidental mutations:
```c++
auto mat = alloy::core::matrix4{};
q.extract_matrix(mat);
```
Compared to:
```c++
auto mat = alloy::core::matrix4{};
q.extract_matrix(&mat);
```

## <a name="stored-non-owning-reference-parameters-are-pointers"></a>Stored non-owning reference parameters are pointers

All APIs that capture non-owning references to objects capture the references
by pointers on the API, in an effort to distinguish things that might outlive
the scope.

For example:
```c++
{
  auto listener = ::listener{};
  subsystem.register_listener(listener); // did this copy? did this move?
}
```
Compared to:
```c++
{
  auto listener = ::listener{};
  subsystem.register_listener(&listener);
} // obvious scope issue
```

## <a name="no-unambiguous-ownership"></a>No Unambiguous Ownership

All types in **Alloy** provide explicit support for copy or move semantics
as they are required. Types that require unique-ownership only expose
move-semantics, types that do not allow new owners do not expose any
copy/move constructors, and value types expose both.

Anything that references other entities are referred to by specific
terminology that indicate the nature of the ownership (e.g.
`*_handle` types are non-owning)