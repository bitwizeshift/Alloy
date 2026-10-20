package root

import (
	"context"
	"fmt"
	"os/exec"
	"strings"
	"sync"
)

// Rooter reports the root directory of a repository.
type Rooter interface {
	// Root returns the absolute path to the root directory of the repository.
	Root(ctx context.Context) (string, error)
}

// GitRooter is a [Rooter] that reports the top-level directory of the Git
// working tree containing Dir.
type GitRooter struct {
	// Dir is the directory git is run in. The working directory is used when
	// Dir is empty.
	Dir string
}

// Root returns the absolute path to the top-level directory of the Git working
// tree that contains Dir.
//
// An error is returned if Dir is not inside a Git working tree, or if git
// cannot be run.
func (gr GitRooter) Root(ctx context.Context) (string, error) {
	cmd := exec.CommandContext(ctx, "git", "rev-parse", "--show-toplevel")
	cmd.Dir = gr.Dir
	out, err := cmd.Output()
	if err != nil {
		return "", fmt.Errorf("git rev-parse: %w", err)
	}
	return strings.TrimSpace(string(out)), nil
}

var _ Rooter = GitRooter{}

// CachedRooter is a [Rooter] that remembers the first root that Rooter reports,
// and returns it from every later call without consulting Rooter again. An
// error is not remembered, so the call after a failure consults Rooter anew.
//
// A CachedRooter is safe for concurrent use, and must not be copied after its
// first use.
type CachedRooter struct {
	// Rooter is the source of the root that is cached.
	Rooter Rooter

	mu   sync.Mutex
	root string
}

// Root returns the root cached from an earlier call, or else the root reported
// by Rooter.
//
// An error is returned if no root is cached and Rooter returns an error.
func (cr *CachedRooter) Root(ctx context.Context) (string, error) {
	cr.mu.Lock()
	defer cr.mu.Unlock()
	if cr.root != "" {
		return cr.root, nil
	}
	root, err := cr.Rooter.Root(ctx)
	if err != nil {
		return "", err
	}
	cr.root = root
	return root, nil
}

var _ Rooter = (*CachedRooter)(nil)
