// Package guard finds and fixes C and C++ headers whose include guards do not
// follow the repository's layout.
//
// The name of a guard is derived from the path of its header, so that every
// header in the repository has a distinct guard.
package guard
