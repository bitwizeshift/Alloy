# Design Goals

**Alloy** tries to achieve several design goals

## Explicit Copying

TODO: elaborate

## Explicit Conversions

TODO: elaborate

## Unambiguous Ownership

TODO: elaborate

## Types don't force allocation requirements

* Assert preconditions and postconditions of APIs with `ALLOY_ASSERT`
* Prefer error codes over exceptions (e.g. `expected`)
* Prefer composition over inheritance
* Only use `virtual` functions on types that are guaranteed to not be
  in a hot path
* Never enforce allocation requirements in a type