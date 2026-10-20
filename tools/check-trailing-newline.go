//go:build ignore

// Command check-trailing-newline reports files given as arguments that do not
// end with a newline, or appends one with -fix.
//
// Usage:
//
//	go -C tools run ./check-trailing-newline.go [-fix] file...
//
// File paths are relative to the repository root.
package main

import (
	"os"

	"github.com/bitwizeshift/alloy/tools/internal/check"
	"github.com/bitwizeshift/alloy/tools/internal/newline"
)

func main() {
	os.Exit(check.Main(newline.Checker{}))
}
