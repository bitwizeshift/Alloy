package checkcmd_test

import (
	"context"
	"fmt"
	"path/filepath"
	"testing"

	"github.com/bitwizeshift/go-cli/arg"
	"github.com/bitwizeshift/go-cli/arg/argtest"

	"github.com/bitwizeshift/alloy/tools/internal/check"
	"github.com/bitwizeshift/alloy/tools/internal/diff"
)

// parseArgs registers the arguments of registrar and parses args into them.
func parseArgs(t *testing.T, registrar arg.Registrar, args ...string) {
	t.Helper()
	cl := argtest.NewCommandLine()
	arg.Register(cl, registrar)
	argtest.MustParse(t, cl, args...)
}

func absPath(t *testing.T, path string) string {
	t.Helper()
	abs, err := filepath.Abs(path)
	if err != nil {
		t.Fatalf("filepath.Abs() = %v", err)
	}
	return abs
}

type checkFixer struct {
	check.Checker
	check.Fixer
}

// differ reports the same changed files for any paths.
type differ []diff.File

func (d differ) Changes(context.Context, ...string) ([]diff.File, error) {
	return d, nil
}

// errDiffer fails for any paths.
type errDiffer struct {
	err error
}

func (d errDiffer) Changes(context.Context, ...string) ([]diff.File, error) {
	return nil, d.err
}

// noOpTidier succeeds for every file without output.
type noOpTidier struct{}

func (noOpTidier) Tidy(context.Context, string, []diff.Range) ([]byte, error) {
	return nil, nil
}

// echoTidier fails for every file, echoing the path and ranges it was given as
// its output.
type echoTidier struct {
	err error
}

func (et echoTidier) Tidy(_ context.Context, path string, ranges []diff.Range) ([]byte, error) {
	return fmt.Appendf(nil, "%s %v\n", path, ranges), et.err
}
