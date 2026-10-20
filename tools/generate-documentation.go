//go:build ignore

// Command generate-documentation generates the API documentation by running
// doxygen with the .codedocs configuration at the repository root.
//
// Usage:
//
//	go -C tools run ./generate-documentation.go
package main

import (
	"errors"
	"fmt"
	"os"
	"os/exec"

	"github.com/bitwizeshift/alloy/tools/internal/repo"
)

func main() {
	if err := repo.Chdir(); err != nil {
		fmt.Fprintln(os.Stderr, err)
		os.Exit(1)
	}

	cmd := exec.Command("doxygen", ".codedocs")
	cmd.Stdout = os.Stdout
	cmd.Stderr = os.Stderr
	if err := cmd.Run(); err != nil {
		if exitErr, ok := errors.AsType[*exec.ExitError](err); ok {
			os.Exit(exitErr.ExitCode())
		}
		fmt.Fprintln(os.Stderr, err)
		os.Exit(1)
	}
}
