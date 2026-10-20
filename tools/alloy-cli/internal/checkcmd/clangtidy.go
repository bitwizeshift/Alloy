package checkcmd

import (
	"context"
	"errors"
	"fmt"
	"io/fs"
	"os"
	"path/filepath"
	"runtime"
	"strconv"
	"sync"

	"github.com/bitwizeshift/go-cli"
	"github.com/bitwizeshift/go-cli/arg"

	"github.com/bitwizeshift/alloy/tools/alloy-cli/internal/stream"
	"github.com/bitwizeshift/alloy/tools/internal/clangtidy"
	"github.com/bitwizeshift/alloy/tools/internal/diff"
	"github.com/bitwizeshift/alloy/tools/internal/root"
)

// contextLines is the number of unchanged lines around each change that
// clang-tidy diagnostics are also reported on.
const contextLines = 5

var (
	// ErrClangTidy is returned by [ClangTidyRunner.Run] when clang-tidy fails
	// for any file.
	ErrClangTidy = errors.New("clang-tidy reported problems")

	// ErrJobs is returned when parsing a --jobs value that is less than one.
	ErrJobs = errors.New("must be at least 1")
)

// Differ reports the changes made to files.
type Differ interface {
	// Changes returns each of paths that has changes, with the line ranges
	// that the changes cover.
	Changes(ctx context.Context, paths ...string) ([]diff.File, error)
}

// Tidier runs clang-tidy over a file.
type Tidier interface {
	// Tidy runs clang-tidy over the file at path, reporting only diagnostics
	// within ranges, or on every line when ranges is empty. It returns the
	// output of clang-tidy, and an error if it reported a failure.
	Tidy(ctx context.Context, path string, ranges []diff.Range) ([]byte, error)
}

// ClangTidyBuilder builds a [ClangTidyRunner] for the C and C++ files named on
// the command line.
type ClangTidyBuilder struct {
	// Rooter reports the repository root that file paths and the build
	// directory are relative to.
	Rooter root.Rooter

	fix      bool
	buildDir string
	jobs     int
	paths    []string
}

// RegisterArgs registers the --fix, --build-dir, and --jobs flags, and the
// files to check.
func (cb *ClangTidyBuilder) RegisterArgs(cl *arg.CommandLine) {
	fix := arg.Flag("fix", &cb.fix,
		arg.Usage("apply the suggested fixes in place"),
	)
	buildDir := arg.Flag("build-dir", &cb.buildDir,
		arg.Shorthand("p"),
		arg.ValueLabel("dir"),
		arg.Usage("the directory containing compile_commands.json, relative to the repository root"),
		arg.DefaultValue("build"),
		arg.CompleteDirs(),
	)
	jobs := arg.Flag("jobs", &cb.jobs,
		arg.Shorthand("j"),
		arg.ValueLabel("count"),
		arg.Usage("the number of files to check at once (default: the number of CPUs)"),
		arg.DefaultFromFunc(cb.defaultJobs),
		arg.Callback(cb.validateJobs),
	)
	files := arg.Unmatched("file", &cb.paths,
		arg.Usage("the C and C++ files to check, relative to the repository root"),
		arg.Required(),
		arg.CompleteFilesMatching("h", "hpp", "c", "cpp"),
	)
	cl.Add(fix, buildDir, jobs, files)
	arg.Group("clang-tidy Flags", buildDir, jobs)
}

func (*ClangTidyBuilder) defaultJobs(context.Context) (string, error) {
	return strconv.Itoa(runtime.NumCPU()), nil
}

func (*ClangTidyBuilder) validateJobs(jobs int) error {
	if jobs < 1 {
		return fmt.Errorf("jobs %d: %w", jobs, ErrJobs)
	}
	return nil
}

// Build returns a [ClangTidyRunner] that runs git and clang-tidy from the
// repository root. A warning is written to the error stream if the build
// directory has no compilation database, since clang-tidy cannot then see how
// files are compiled.
//
// An error is returned if the repository root cannot be determined.
func (cb *ClangTidyBuilder) Build(ctx context.Context) (cli.Runner, error) {
	dir, err := cb.Rooter.Root(ctx)
	if err != nil {
		return nil, err
	}
	cb.warnMissingDatabase(ctx, dir)
	return &ClangTidyRunner{
		Differ: diff.Git{Dir: dir, Context: contextLines},
		Tidier: &clangtidy.Command{Dir: dir, BuildDir: cb.buildDir, Fix: cb.fix},
		Jobs:   cb.jobs,
		Paths:  cb.paths,
	}, nil
}

func (cb *ClangTidyBuilder) warnMissingDatabase(ctx context.Context, dir string) {
	database := filepath.Join(cb.buildDir, "compile_commands.json")
	if !filepath.IsAbs(database) {
		database = filepath.Join(dir, database)
	}
	if _, err := os.Stat(database); errors.Is(err, fs.ErrNotExist) {
		fmt.Fprintf(cli.ErrStream(ctx), "[theme:warning]warning:[/theme] compilation database not found at [richtext:off]%s[/richtext]\n", database)
	}
}

var (
	_ cli.Builder   = (*ClangTidyBuilder)(nil)
	_ arg.Registrar = (*ClangTidyBuilder)(nil)
)

// ClangTidyRunner runs clang-tidy over files, limiting its diagnostics to the
// lines near their changes.
type ClangTidyRunner struct {
	// Differ reports the changed lines of each file.
	Differ Differ

	// Tidier runs clang-tidy over each file.
	Tidier Tidier

	// Jobs is the number of files checked at once.
	Jobs int

	// Paths are the files to check.
	Paths []string
}

// Run runs clang-tidy over each file, reporting only the diagnostics within
// the ranges from Differ; a file without changes is checked in full. The
// output for each file that clang-tidy fails on is written to the error
// stream, in the order of Paths.
//
// [ErrClangTidy] is returned if clang-tidy fails for any file. Any other error
// is returned if the changes cannot be read from Differ.
func (cr *ClangTidyRunner) Run(ctx context.Context) error {
	ranges, err := cr.ranges(ctx)
	if err != nil {
		return err
	}
	out := stream.Verbatim(cli.ErrStream(ctx))
	failed := false
	for _, result := range cr.tidy(ctx, ranges) {
		if result.err == nil {
			continue
		}
		failed = true
		out.Write(result.output)
		fmt.Fprintf(out, "%s: %v\n", result.path, result.err)
	}
	if failed {
		return ErrClangTidy
	}
	return nil
}

func (cr *ClangTidyRunner) ranges(ctx context.Context) (map[string][]diff.Range, error) {
	files, err := cr.Differ.Changes(ctx, cr.Paths...)
	if err != nil {
		return nil, err
	}
	ranges := make(map[string][]diff.Range, len(files))
	for _, file := range files {
		ranges[file.Path] = file.Ranges
	}
	return ranges, nil
}

type tidyResult struct {
	path   string
	output []byte
	err    error
}

// tidy runs the Tidier over each of Paths with up to Jobs running at once, and
// returns the results in the same order as Paths.
func (cr *ClangTidyRunner) tidy(ctx context.Context, ranges map[string][]diff.Range) []tidyResult {
	results := make([]tidyResult, len(cr.Paths))
	limit := make(chan struct{}, max(cr.Jobs, 1))
	var wg sync.WaitGroup
	for i, path := range cr.Paths {
		wg.Go(func() {
			limit <- struct{}{}
			defer func() { <-limit }()

			output, err := cr.Tidier.Tidy(ctx, path, ranges[path])
			results[i] = tidyResult{path: path, output: output, err: err}
		})
	}
	wg.Wait()
	return results
}

var _ cli.Runner = (*ClangTidyRunner)(nil)
