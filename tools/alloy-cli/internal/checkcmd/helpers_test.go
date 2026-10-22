package checkcmd_test

import (
	"context"
	"fmt"
	"path/filepath"
	"slices"
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

// databaseWriter returns dir as the directory of the compilation database,
// fails to remove it with removeErr, and covers every file but those in
// uncovered.
type databaseWriter struct {
	dir       string
	removeErr error
	uncovered []string
}

func (dw databaseWriter) WriteDatabase() (string, func() error, error) {
	return dw.dir, func() error { return dw.removeErr }, nil
}

func (dw databaseWriter) Covers(path string) bool {
	return !slices.Contains(dw.uncovered, path)
}

// errDatabaseWriter fails to write the compilation database.
type errDatabaseWriter struct {
	err error
}

func (dw errDatabaseWriter) WriteDatabase() (string, func() error, error) {
	return "", nil, dw.err
}

func (errDatabaseWriter) Covers(string) bool {
	return true
}

// noOpTidier succeeds for every file without output.
type noOpTidier struct{}

func (noOpTidier) Tidy(context.Context, string, string, []diff.Range) ([]byte, error) {
	return nil, nil
}

// echoTidier fails for every file, echoing the build directory, path, and
// ranges it was given as its output.
type echoTidier struct {
	err error
}

func (et echoTidier) Tidy(_ context.Context, buildDir, path string, ranges []diff.Range) ([]byte, error) {
	return fmt.Appendf(nil, "%s %s %v\n", buildDir, path, ranges), et.err
}

// fixApplier fails to apply fixes with err.
type fixApplier struct {
	err error
}

func (fa fixApplier) ApplyFixes(context.Context) error {
	return fa.err
}
