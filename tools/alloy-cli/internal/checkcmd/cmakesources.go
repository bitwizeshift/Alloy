package checkcmd

import (
	"context"
	"os"

	"github.com/bitwizeshift/go-cli"
	"github.com/bitwizeshift/go-cli/arg"

	"github.com/bitwizeshift/alloy/tools/internal/check"
	"github.com/bitwizeshift/alloy/tools/internal/cmakelists"
	"github.com/bitwizeshift/alloy/tools/internal/root"
)

// CMakeSourcesBuilder builds a [CheckRunner] that checks that the files named
// on the command line are listed in the CMakeLists.txt that builds them.
type CMakeSourcesBuilder struct {
	// Rooter reports the repository root that file paths are relative to, and
	// that the CMakeLists.txt files are found under.
	Rooter root.Rooter

	paths []string
}

// RegisterArgs registers the files to check.
func (csb *CMakeSourcesBuilder) RegisterArgs(cl *arg.CommandLine) {
	cl.Add(
		arg.Unmatched("file", &csb.paths,
			arg.Usage("the files to check, relative to the repository root"),
			arg.Required(),
			arg.CompleteFilesMatching("h", "hpp", "c", "cpp"),
		),
	)
}

// Build returns a [CheckRunner] over the parsed files, with a
// [cmakelists.Checker] that reads the CMakeLists.txt files relative to the
// repository root.
//
// An error is returned if the repository root cannot be determined.
func (csb *CMakeSourcesBuilder) Build(ctx context.Context) (cli.Runner, error) {
	dir, err := csb.Rooter.Root(ctx)
	if err != nil {
		return nil, err
	}
	return &CheckRunner{
		Checker: &cmakelists.Checker{FS: os.DirFS(dir)},
		FS:      check.OS{Dir: dir},
		Paths:   csb.paths,
	}, nil
}

var (
	_ cli.Builder   = (*CMakeSourcesBuilder)(nil)
	_ arg.Registrar = (*CMakeSourcesBuilder)(nil)
)
