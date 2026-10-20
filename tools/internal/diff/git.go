package diff

import (
	"bytes"
	"context"
	"fmt"
	"os/exec"
	"strconv"
)

// Git reads the changes in the working tree of the Git repository that
// contains Dir, relative to HEAD.
type Git struct {
	// Dir is the directory git is run in, which relative paths are resolved
	// against. The working directory is used when Dir is empty.
	Dir string

	// Context is the number of unchanged lines included around each change.
	Context int
}

// Changes returns each of paths that differs from HEAD, or from the empty tree
// before the first commit, with the ranges of lines that the changes cover in
// the working tree. Paths without changes are omitted.
//
// An error is returned if git cannot be run, or if its output cannot be read
// with [Parse].
func (g Git) Changes(ctx context.Context, paths ...string) ([]File, error) {
	base, err := g.base(ctx)
	if err != nil {
		return nil, err
	}
	args := []string{
		"-c", "core.quotePath=false",
		"diff", base,
		"--no-color", "--no-ext-diff", "--no-prefix",
		"--unified=" + strconv.Itoa(g.Context),
		"--",
	}
	out, err := g.git(ctx, append(args, paths...)...).Output()
	if err != nil {
		return nil, fmt.Errorf("git diff: %w", err)
	}
	return Parse(bytes.NewReader(out))
}

// base returns the revision that changes are measured from.
func (g Git) base(ctx context.Context) (string, error) {
	if g.git(ctx, "rev-parse", "--verify", "--quiet", "HEAD").Run() == nil {
		return "HEAD", nil
	}
	out, err := g.git(ctx, "hash-object", "-t", "tree", "/dev/null").Output()
	if err != nil {
		return "", fmt.Errorf("git hash-object: %w", err)
	}
	return string(bytes.TrimSpace(out)), nil
}

func (g Git) git(ctx context.Context, args ...string) *exec.Cmd {
	cmd := exec.CommandContext(ctx, "git", args...)
	cmd.Dir = g.Dir
	return cmd
}
