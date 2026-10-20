package checkcmd

import (
	"context"
	"regexp"
	"time"

	"github.com/bitwizeshift/go-cli"
	"github.com/bitwizeshift/go-cli/arg"

	"github.com/bitwizeshift/alloy/tools/alloy-cli/internal/stream"
	"github.com/bitwizeshift/alloy/tools/internal/check"
	"github.com/bitwizeshift/alloy/tools/internal/copyright"
	"github.com/bitwizeshift/alloy/tools/internal/newline"
	"github.com/bitwizeshift/alloy/tools/internal/root"
	"github.com/bitwizeshift/alloy/tools/internal/whitespace"
)

var copyrightPattern = regexp.MustCompile(`Copyright \(c\) (.*) Matthew Rodusek All rights reserved\.`)

// LicenseCopyright returns a [FileBuilder] for the check that copyright
// notices list the years in which each file was modified.
func LicenseCopyright(rooter root.Rooter) *FileBuilder {
	return &FileBuilder{
		Rooter: rooter,
		NewChecker: func(dir string) check.CheckFixer {
			return &copyright.Checker{
				Pattern: copyrightPattern,
				History: copyright.Git{Dir: dir},
				Year:    time.Now().Year(),
			}
		},
	}
}

// TrailingNewline returns a [FileBuilder] for the check that files end with a
// newline.
func TrailingNewline(rooter root.Rooter) *FileBuilder {
	return &FileBuilder{
		Rooter: rooter,
		NewChecker: func(string) check.CheckFixer {
			return newline.Checker{}
		},
	}
}

// Whitespace returns a [FileBuilder] for the check that lines have no trailing
// whitespace.
func Whitespace(rooter root.Rooter) *FileBuilder {
	return &FileBuilder{
		Rooter: rooter,
		NewChecker: func(string) check.CheckFixer {
			return whitespace.Checker{}
		},
	}
}

// FileBuilder builds a [FileRunner] that applies a [check.CheckFixer] to the
// files named on the command line.
type FileBuilder struct {
	// Rooter reports the repository root that file paths are relative to.
	Rooter root.Rooter

	// NewChecker returns the check to apply to files in the repository rooted
	// at dir.
	NewChecker func(dir string) check.CheckFixer

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
		Checker: fb.NewChecker(dir),
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
	runner := &check.Runner{FS: fr.FS, Out: stream.Verbatim(cli.ErrStream(ctx))}
	return runner.Check(fr.Checker, fr.Paths...)
}

var _ cli.Runner = (*FileRunner)(nil)
