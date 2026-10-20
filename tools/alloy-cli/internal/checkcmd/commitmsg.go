package checkcmd

import (
	"context"
	"errors"
	"fmt"
	"os"
	"path/filepath"

	"github.com/bitwizeshift/go-cli"
	"github.com/bitwizeshift/go-cli/arg"

	"github.com/bitwizeshift/alloy/tools/alloy-cli/internal/stream"
	"github.com/bitwizeshift/alloy/tools/internal/commitmsg"
	"github.com/bitwizeshift/alloy/tools/internal/root"
)

// ErrCommitMsg is returned by [CommitMsgRunner.Run] when the commit message
// violates the commit standards.
var ErrCommitMsg = errors.New("commit message does not follow doc/commit-standards.md")

// CommitMsgBuilder builds a [CommitMsgRunner] for the commit message in the
// file named on the command line.
type CommitMsgBuilder struct {
	// Rooter reports the repository root, which relative paths are resolved
	// against and which the valid Component trailers are read from.
	Rooter root.Rooter

	path string
}

// RegisterArgs registers the file that holds the commit message.
func (cb *CommitMsgBuilder) RegisterArgs(cl *arg.CommandLine) {
	cl.Add(
		arg.Positional("file", 0, &cb.path,
			arg.Usage("the file holding the commit message, as passed to the commit-msg hook"),
			arg.Required(),
			arg.CompleteFiles(),
		),
	)
}

// Build reads the commit message, and returns a [CommitMsgRunner] that
// validates it against the commit standards for the repository.
//
// An error is returned if the repository root cannot be determined, or if the
// message or the repository's components cannot be read.
func (cb *CommitMsgBuilder) Build(ctx context.Context) (cli.Runner, error) {
	dir, err := cb.Rooter.Root(ctx)
	if err != nil {
		return nil, err
	}
	message, err := cb.readMessage(dir)
	if err != nil {
		return nil, err
	}
	components, err := commitmsg.Components(os.DirFS(dir))
	if err != nil {
		return nil, err
	}
	return &CommitMsgRunner{
		Policy:  CommitPolicy(components),
		Message: message,
	}, nil
}

func (cb *CommitMsgBuilder) readMessage(dir string) (*commitmsg.Message, error) {
	path := cb.path
	if !filepath.IsAbs(path) {
		path = filepath.Join(dir, path)
	}
	file, err := os.Open(path)
	if err != nil {
		return nil, err
	}
	defer file.Close()
	return commitmsg.Parse(file)
}

var (
	_ cli.Builder   = (*CommitMsgBuilder)(nil)
	_ arg.Registrar = (*CommitMsgBuilder)(nil)
)

// CommitPolicy returns the [commitmsg.Policy] described by
// doc/commit-standards.md, accepting each of components as a Component
// trailer.
func CommitPolicy(components []string) *commitmsg.Policy {
	return &commitmsg.Policy{
		SubjectWidth: 50,
		BodyWidth:    72,
		Categories: []string{
			"feature",
			"fix",
			"refactor",
			"docs",
			"tests",
			"deps",
			"perf",
			"security",
			"chore",
		},
		Impacts:    []string{"breaking", "additive", "corrective", "none"},
		Components: components,
	}
}

// CommitMsgRunner validates a commit message against a [commitmsg.Policy].
type CommitMsgRunner struct {
	// Policy is the set of rules the message must satisfy.
	Policy *commitmsg.Policy

	// Message is the commit message to validate.
	Message *commitmsg.Message
}

// Run writes each violation of Policy to the error stream.
//
// [ErrCommitMsg] is returned if there is any violation.
func (cr *CommitMsgRunner) Run(ctx context.Context) error {
	errs := cr.Policy.Validate(cr.Message)
	out := stream.Verbatim(cli.ErrStream(ctx))
	for _, err := range errs {
		fmt.Fprintf(out, "commit message: %v\n", err)
	}
	if len(errs) > 0 {
		return ErrCommitMsg
	}
	return nil
}

var _ cli.Runner = (*CommitMsgRunner)(nil)
