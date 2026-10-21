// Package clangtidy runs clang-tidy over a file, restricting its diagnostics
// to selected line ranges.
//
// The compilation database that clang-tidy reads can be narrowed to a single
// configuration, since multi-config generators write an entry per
// configuration for every file and clang-tidy checks each of them.
package clangtidy
