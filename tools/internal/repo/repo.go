package repo

import (
	"fmt"
	"os"
	"os/exec"
	"strings"
)

// Root returns the absolute path to the top-level directory of the Git working
// tree that contains the current working directory.
//
// An error is returned if the working directory is not inside a Git working
// tree, or if git cannot be run.
func Root() (string, error) {
	out, err := exec.Command("git", "rev-parse", "--show-toplevel").Output()
	if err != nil {
		return "", fmt.Errorf("repo root: %w", err)
	}
	return strings.TrimSpace(string(out)), nil
}

// Chdir changes the current working directory to the value of [Root].
//
// An error is returned if the root cannot be determined or entered.
func Chdir() error {
	root, err := Root()
	if err != nil {
		return err
	}
	return os.Chdir(root)
}
