# `Alloy::Core`

## Description

The core library of the **Alloy** game engine.
This library consists of mostly semantic/value-types which make up the
foundation of the rest of the system.

All types in this library are agnostic of the underlying rendering engine,
audio engine, etc. They are intended to be infrastructure that
will be leveraged by the various other libraries instead.

## Concerns

* core types (e.g. `half`,`real`, `string_view`, etc)
* mathematical types / functionality (`trigonometry`, `vector[2,3,4]`, etc)
* geometric types (e.g. `aabb`, `line`, `ray`, etc)
* model types (`mesh`, `transform`, `vertex`, `camera`, `color`, etc)

Extensions may be added in the future for 'container' types, as well as
allocator / memory-management types.

