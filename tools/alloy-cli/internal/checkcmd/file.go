package checkcmd

import (
	"context"

	"github.com/bitwizeshift/go-cli"
	"github.com/bitwizeshift/go-cli/arg"

	"github.com/bitwizeshift/alloy/tools/alloy-cli/internal/stream"
	"github.com/bitwizeshift/alloy/tools/internal/check"
	"github.com/bitwizeshift/alloy/tools/internal/guard"
	"github.com/bitwizeshift/alloy/tools/internal/newline"
	"github.com/bitwizeshift/alloy/tools/internal/root"
	"github.com/bitwizeshift/alloy/tools/internal/whitespace"
)

// HeaderGuard returns a [FileBuilder] for the check that C and C++ headers
// have an include guard named after their path relative to an include or src
// directory.
func HeaderGuard(rooter root.Rooter) *FileBuilder {
	return &FileBuilder{
		Rooter:  rooter,
		Checker: &guard.Checker{Roots: []string{"include", "src"}},
	}
}

// TrailingNewline returns a [FileBuilder] for the check that files end with a
// newline.
func TrailingNewline(rooter root.Rooter) *FileBuilder {
	return &FileBuilder{
		Rooter:  rooter,
		Checker: newline.Checker{},
	}
}

// Whitespace returns a [FileBuilder] for the check that lines have no trailing
// whitespace.
func Whitespace(rooter root.Rooter) *FileBuilder {
	return &FileBuilder{
		Rooter:  rooter,
		Checker: whitespace.Checker{},
	}
}

// FileBuilder builds a [FileRunner] that applies a [check.CheckFixer] to the
// files named on the command line.
type FileBuilder struct {
	// Rooter reports the repository root that file paths are relative to.
	Rooter root.Rooter

	// Checker is the check applied to each file.
	Checker check.CheckFixer

	fix   bool
	paths []string
}

// RegisterArgs registers the --fix flag and the files to check.
func (fb *FileBuilder) RegisterArgs(cl *arg.CommandLine) {
	cl.Add(
		arg.Flag("fix", &fb.fix,
			arg.Usage("fix files in place instead of reporting problems"),
		),
		arg.Unmatched("file", &fb.paths,
			arg.Usage("the files to check, relative to the repository root"),
			arg.Required(),
			arg.CompleteFiles(),
		),
	)
}

// Build returns a [FileRunner] over the parsed files, reading and writing them
// relative to the repository root.
//
// An error is returned if the repository root cannot be determined.
func (fb *FileBuilder) Build(ctx context.Context) (cli.Runner, error) {
	dir, err := fb.Rooter.Root(ctx)
	if err != nil {
		return nil, err
	}
	return &FileRunner{
		Checker: fb.Checker,
		FS:      check.OS{Dir: dir},
		Fix:     fb.fix,
		Paths:   fb.paths,
	}, nil
}

var (
	_ cli.Builder   = (*FileBuilder)(nil)
	_ arg.Registrar = (*FileBuilder)(nil)
)

// FileRunner applies a [check.CheckFixer] to a list of files.
type FileRunner struct {
	// Checker is the check applied to each file.
	Checker check.CheckFixer

	// FS is the filesystem that the files are read from and written to.
	FS check.FS

	// Fix selects fixing each file in place, instead of reporting problems.
	Fix bool

	// Paths are the files to check.
	Paths []string
}

// Run checks each file and writes each problem found to the error stream, or
// with Fix, fixes each file and writes the path of each changed file to the
// output stream.
//
// [check.ErrFindings] is returned if a check reports any problem. Any other
// error is returned if a file cannot be read, checked, fixed, or written.
func (fr *FileRunner) Run(ctx context.Context) error {
	if fr.Fix {
		runner := &check.Runner{FS: fr.FS, Out: stream.Verbatim(cli.OutStream(ctx))}
		return runner.Fix(fr.Checker, fr.Paths...)
	}
	runner := &CheckRunner{Checker: fr.Checker, FS: fr.FS, Paths: fr.Paths}
	return runner.Run(ctx)
}

var _ cli.Runner = (*FileRunner)(nil)

// CheckRunner reports the problems that a [check.Checker] finds in a list of
// files.
type CheckRunner struct {
	// Checker is the check applied to each file.
	Checker check.Checker

	// FS is the filesystem that the files are read from.
	FS check.FS

	// Paths are the files to check.
	Paths []string
}

// Run checks each file and writes each problem found to the error stream.
//
// [check.ErrFindings] is returned if the check reports any problem. Any other
// error is returned if a file cannot be read or checked.
func (cr *CheckRunner) Run(ctx context.Context) error {
	runner := &check.Runner{FS: cr.FS, Out: stream.Verbatim(cli.ErrStream(ctx))}
	return runner.Check(cr.Checker, cr.Paths...)
}

var _ cli.Runner = (*CheckRunner)(nil)
