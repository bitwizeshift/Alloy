package checkcmd

import (
	"context"
	"os"

	"github.com/bitwizeshift/go-cli"
	"github.com/bitwizeshift/go-cli/arg"

	"github.com/bitwizeshift/alloy/tools/internal/check"
	"github.com/bitwizeshift/alloy/tools/internal/includes"
	"github.com/bitwizeshift/alloy/tools/internal/root"
)

// IncludeOrderBuilder builds a [FileRunner] that checks that the C and C++
// files named on the command line group and order their includes by where
// their headers come from, among the libraries under lib/ and extra/, and
// follow the rules of [includes.Checker] for how headers are included.
type IncludeOrderBuilder struct {
	// Rooter reports the repository root that file paths are relative to, and
	// that the libraries are found under.
	Rooter root.Rooter

	fix   bool
	paths []string
}

// RegisterArgs registers the --fix flag and the files to check.
func (iob *IncludeOrderBuilder) RegisterArgs(cl *arg.CommandLine) {
	cl.Add(
		arg.Flag("fix", &iob.fix,
			arg.Usage("fix files in place instead of reporting problems"),
		),
		arg.Unmatched("file", &iob.paths,
			arg.Usage("the files to check, relative to the repository root"),
			arg.Required(),
			arg.CompleteFilesMatching("h", "hpp", "c", "cpp"),
		),
	)
}

// Build returns a [FileRunner] over the parsed files, reading and writing them
// relative to the repository root, with an [includes.Checker] for the
// libraries found under it. Files under a test, benchmark, example or bin
// directory are checked as targets other than libraries.
//
// An error is returned if the repository root cannot be determined, or its
// libraries cannot be read.
func (iob *IncludeOrderBuilder) Build(ctx context.Context) (cli.Runner, error) {
	dir, err := iob.Rooter.Root(ctx)
	if err != nil {
		return nil, err
	}
	roots := []string{"include", "src"}
	libraries, err := includes.Libraries(os.DirFS(dir), roots, "lib/*", "extra/*")
	if err != nil {
		return nil, err
	}
	return &FileRunner{
		Checker: &includes.Checker{
			Roots:     roots,
			Targets:   []string{"test", "benchmark", "example", "bin"},
			Libraries: libraries,
		},
		FS:    check.OS{Dir: dir},
		Fix:   iob.fix,
		Paths: iob.paths,
	}, nil
}

var (
	_ cli.Builder   = (*IncludeOrderBuilder)(nil)
	_ arg.Registrar = (*IncludeOrderBuilder)(nil)
)
