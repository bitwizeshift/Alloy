//go:build ignore

// Command check-license-copyright reports copyright notices in the files given
// as arguments whose years do not match the years in which each file was
// modified, or corrects them with -fix.
//
// Usage:
//
//	go -C tools run ./check-license-copyright.go [-fix] file...
//
// File paths are relative to the repository root.
package main

import (
	"os"
	"regexp"
	"time"

	"github.com/bitwizeshift/alloy/tools/internal/check"
	"github.com/bitwizeshift/alloy/tools/internal/copyright"
)

var pattern = regexp.MustCompile(`Copyright \(c\) (.*) Matthew Rodusek All rights reserved\.`)

func main() {
	os.Exit(check.Main(&copyright.Checker{
		Pattern: pattern,
		History: copyright.Git{},
		Year:    time.Now().Year(),
	}))
}
