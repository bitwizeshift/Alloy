# `Alloy::Render`

## Description

The render library of the **Alloy** game engine.

This library is broken into various sub-libraries:
* The primary API/interfaces
* The actual implementation libraries

## Concerns

* Abstracting the render system into a robust/composable interface
* Creating / managing Shader objects
* Creating model handles and rendering those handles

**Note:** The actual graphics/modeling mathematics is not handled in this
library; it is handled in the `alloy-core` library. This library only
deals with translating models into a render call.