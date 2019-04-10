![Alloy Engine](doc/src/images/alloy-logo.png)

[![Release](https://img.shields.io/github/release/bitwizeshift/alloy.svg)](https://github.com/bitwizeshift/alloy/releases/latest)
[![License](https://img.shields.io/badge/license-MIT-blue.svg)](https://raw.githubusercontent.com/bitwizeshift/alloy/master/LICENSE)
[![Github Issues](https://img.shields.io/github/issues/bitwizeshift/alloy.svg)](http://github.com/bitwizeshift/alloy/issues)

## <a name="summary"></a>Summary

**Alloy** is a flexible game-engine and development kit under active
development written in C++.

It is characterized by upholding a strict set of design-goals which
center around maintainability and readability through proper
software engineering practices. The overall goal for this project
is to be a well-rounded and easy-to-use, efficient game-engine.

It is inspired by many great open-source projects that
have come before it, the complete list can be seen
[below](#inspirations).

## Table of Contents

* Consumers
  * [Features](#features) \
    A summary of all existing features for the Alloy engine
  * [**Alloy** API Reference](https://bitwizeshift.github.io/alloy/api/latest/manual.html) \
    For doxygen-generated API information
  * [**Alloy** Tutorials](https://bitwizeshift.github.io/alloy/api/latest/tutorials.html) \
    For tutorials on how to use `alloy`
  * [Legal](doc/legal.md) \
    Information about how to attribute this project
* Contributors
  * [Building the **Alloy** library](doc/building.md) \
    Detailed instructions for how to build the `alloy` library
  * [Contributing Guidelines](.github/CONTRIBUTING.md) \
    Guidelines that must be followed in order to contribute to the project

## Features

TODO

## <a name="inspirations"></a>Inspirations

**Alloy** was inspired by a variety of existing open-source projects.
Some of these projects are listed below:

- [Godot](https://github.com/godotengine/godot)
- [Blade](https://github.com/crazii/blade)
- [Ogre](https://github.com/OGRECave/ogre/pulls)
- [Nomad](https://github.com/taurheim/NomadECS)
- [easy-engine](https://github.com/callebstrom/easy-engine)

**Note:** _inspired_ does not mean that any code was taken without following
proper license attribution. It simply means that certain designs and concepts
may have been adjusted to align with what other existing projects are
already doing.

## <a name="tested-compilers"></a>Tested Compilers

The following compilers are currently being tested through
[continuous integration](#continuous-integration) with
[Travis](https://travis-ci.org/bitwizeshift/alloy) and
[AppVeyor](https://ci.appveyor.com/project/bitwizeshift/alloy/)

| Compiler              | Operating System                   |
|-----------------------|------------------------------------|
| g++ 7.3.0             | Ubuntu 14.04.1                     |
| g++ 8.1.0             | Ubuntu 14.04.1                     |
| clang++ 5.0.2         | Ubuntu 14.04.1                     |
| clang++ 6.0.1         | Ubuntu 14.04.1                     |
| clang xcode 10        | Darwin Kernel 17.7.0               |
| Visual Studio 2017    | Windows Server 2016 (x86)          |
| Visual Studio 2017    | Windows Server 2016 (x64)          |

## <a name="continuous-integration"></a>Continuous Integration

| **Build**     | **Status**      |
|---------------|-----------------|
| Ubuntu, macOS | [![Build Status](https://travis-ci.org/bitwizeshift/alloy.svg?branch=master)](https://travis-ci.org/bitwizeshift/alloy) |
| MSVC          | [![Build status](https://ci.appveyor.com/api/projects/status/ou5sraydky6tjxv9?svg=true)](https://ci.appveyor.com/project/bitwizeshift/alloy) |


## <a name="packaging"></a>Current Releases

| **Host**            | **Release**      |
|---------------------|------------------|
| Github              | [![Release](https://img.shields.io/github/release/bitwizeshift/alloy.svg)](https://github.com/bitwizeshift/alloy/releases/latest) |
| Conan               | [![Release](https://img.shields.io/github/release/bitwizeshift/alloy.svg)](#)

## <a name="license"></a>License

<img align="right" src="http://opensource.org/trademarks/opensource/OSI-Approved-License-100x137.png">

**Alloy** is licensed under the
[MIT License](http://opensource.org/licenses/MIT):

> Copyright &copy; 2018 Matthew Rodusek
>
> Permission is hereby granted, free of charge, to any person obtaining a copy
> of this software and associated documentation files (the "Software"), to deal
> in the Software without restriction, including without limitation the rights
> to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
> copies of the Software, and to permit persons to whom the Software is
> furnished to do so, subject to the following conditions:
>
> The above copyright notice and this permission notice shall be included in all
> copies or substantial portions of the Software.
>
> THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
> IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
> FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
> AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
> LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
> OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
> SOFTWARE.
