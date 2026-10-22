// Package cmakelists finds C and C++ files that are not listed in the
// CMakeLists.txt that builds them.
//
// Sources are listed explicitly rather than globbed, so a file that is added
// without being listed is silently left out of the build. The CMakeLists.txt
// that builds a file is the nearest one in its directory or above it, and
// lists the file by its path relative to that directory.
package cmakelists
