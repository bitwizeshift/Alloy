package generatecmd

import (
	"context"
	"os/exec"

	"github.com/bitwizeshift/go-cli"

	"github.com/bitwizeshift/alloy/tools/alloy-cli/internal/stream"
	"github.com/bitwizeshift/alloy/tools/internal/root"
)

// DocumentationBuilder builds a [DocumentationRunner] for the repository.
type DocumentationBuilder struct {
	// Rooter reports the repository root that holds the doxygen configuration.
	Rooter root.Rooter
}

// Build returns a [DocumentationRunner] that runs from the repository root.
//
// An error is returned if the repository root cannot be determined.
func (db *DocumentationBuilder) Build(ctx context.Context) (cli.Runner, error) {
	dir, err := db.Rooter.Root(ctx)
	if err != nil {
		return nil, err
	}
	return &DocumentationRunner{Dir: dir}, nil
}

var _ cli.Builder = (*DocumentationBuilder)(nil)

// DocumentationRunner generates the API documentation by running doxygen with
// the .codedocs configuration.
type DocumentationRunner struct {
	// Dir is the directory holding the .codedocs configuration, which doxygen
	// is run in.
	Dir string
}

// Run runs doxygen, writing its output to the output and error streams.
//
// An error is returned if doxygen cannot be run, or if it fails.
func (dr *DocumentationRunner) Run(ctx context.Context) error {
	cmd := exec.CommandContext(ctx, "doxygen", ".codedocs")
	cmd.Dir = dr.Dir
	cmd.Stdout = stream.Verbatim(cli.OutStream(ctx))
	cmd.Stderr = stream.Verbatim(cli.ErrStream(ctx))
	return cmd.Run()
}

var _ cli.Runner = (*DocumentationRunner)(nil)
