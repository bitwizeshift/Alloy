//go:build ignore

// Command check-whitespace reports lines with trailing whitespace in the files
// given as arguments, or removes it with -fix.
//
// Usage:
//
//	go -C tools run ./check-whitespace.go [-fix] file...
//
// File paths are relative to the repository root.
package main

import (
	"os"

	"github.com/bitwizeshift/alloy/tools/internal/check"
	"github.com/bitwizeshift/alloy/tools/internal/whitespace"
)

func main() {
	os.Exit(check.Main(whitespace.Checker{}))
}
