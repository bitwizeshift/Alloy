//go:build ignore

// Command check-commit-msg verifies that a commit message follows the commit
// standards in doc/commit-standards.md.
//
// Usage:
//
//	go -C tools run ./check-commit-msg.go file
//
// The file holds the proposed commit message, as passed to the commit-msg git
// hook. Every violation is written to stderr.
package main

import (
	"fmt"
	"os"
	"path/filepath"

	"github.com/bitwizeshift/alloy/tools/internal/commitmsg"
	"github.com/bitwizeshift/alloy/tools/internal/repo"
)

func main() {
	os.Exit(run())
}

func run() int {
	if len(os.Args) != 2 {
		fmt.Fprintf(os.Stderr, "usage: %s file\n", filepath.Base(os.Args[0]))
		return 2
	}
	path, err := filepath.Abs(os.Args[1])
	if err != nil {
		fmt.Fprintln(os.Stderr, err)
		return 1
	}
	if err := repo.Chdir(); err != nil {
		fmt.Fprintln(os.Stderr, err)
		return 1
	}

	message, err := readMessage(path)
	if err != nil {
		fmt.Fprintln(os.Stderr, err)
		return 1
	}
	components, err := commitmsg.Components(os.DirFS("."))
	if err != nil {
		fmt.Fprintln(os.Stderr, err)
		return 1
	}

	policy := &commitmsg.Policy{
		SubjectWidth: 50,
		BodyWidth:    72,
		Categories: []string{
			"feature",
			"fix",
			"refactor",
			"docs",
			"tests",
			"deps",
			"perf",
			"security",
			"chore",
		},
		Impacts:    []string{"breaking", "additive", "corrective", "none"},
		Components: components,
	}

	errs := policy.Validate(message)
	for _, err := range errs {
		fmt.Fprintf(os.Stderr, "commit message: %v\n", err)
	}
	if len(errs) > 0 {
		return 1
	}
	return 0
}

func readMessage(path string) (*commitmsg.Message, error) {
	file, err := os.Open(path)
	if err != nil {
		return nil, err
	}
	defer file.Close()
	return commitmsg.Parse(file)
}
