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

// DatabaseWriter writes the compilation database that clang-tidy reads.
type DatabaseWriter interface {
	// WriteDatabase returns the directory that contains the compilation
	// database, and a function that removes it once it is no longer needed.
	// It returns an error if the database cannot be written.
	WriteDatabase() (dir string, remove func() error, err error)

	// Covers reports whether the written database holds the compile commands
	// that the file at path is checked with.
	Covers(path string) bool
}

// Tidier runs clang-tidy over a file.
type Tidier interface {
	// Tidy runs clang-tidy over the file at path with the compilation database
	// in buildDir, reporting only diagnostics within ranges, or on every line
	// when ranges is empty. It returns the output of clang-tidy, and an error
	// if it reported a failure.
	Tidy(ctx context.Context, buildDir, path string, ranges []diff.Range) ([]byte, error)
}

// FixApplier applies the fixes that a [Tidier] suggested.
type FixApplier interface {
	// ApplyFixes applies the fixes suggested for every file checked so far.
	// It returns an error if they cannot be applied.
	ApplyFixes(ctx context.Context) error
}

// includeCleanerChecks selects only the clang-tidy check that reports
// includes that are unused, and symbols whose header is not directly included.
const includeCleanerChecks = "-*,misc-include-cleaner"

// IncludeCleaner returns a [ClangTidyBuilder] for the check that C and C++
// files directly include the headers of the symbols they use, and no others.
//
// An include is made unused by changes to the lines that use it, rather than
// to the include itself, so diagnostics are reported on every line. Fixes are
// applied without formatting, since formatting re-sorts and realigns every
// include around each added or removed one.
func IncludeCleaner(rooter root.Rooter) *ClangTidyBuilder {
	return &ClangTidyBuilder{
		Rooter:      rooter,
		Checks:      includeCleanerChecks,
		FormatStyle: "none",
		WholeFiles:  true,
	}
}

// ClangTidyBuilder builds a [ClangTidyRunner] for the C and C++ files named on
// the command line.
type ClangTidyBuilder struct {
	// Rooter reports the repository root that file paths and the build
	// directory are relative to.
	Rooter root.Rooter

	// Checks is the glob of checks that clang-tidy runs, in place of the
	// checks enabled by the .clang-tidy configuration. The configured checks
	// are run when Checks is empty.
	Checks string

	// FormatStyle is the clang-format style that the code around applied fixes
	// is formatted with, or "none" to apply them as they are, in place of the
	// FormatStyle of the .clang-tidy configuration. The configured style is
	// used when FormatStyle is empty.
	FormatStyle string

	// WholeFiles selects reporting the diagnostics on every line of each
	// file, instead of only those near its changes.
	WholeFiles bool

	fix      bool
	buildDir string
	config   string
	jobs     int
	paths    []string
}

// RegisterArgs registers the --fix, --build-dir, --config, and --jobs flags,
// and the files to check.
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
	config := arg.Flag("config", &cb.config,
		arg.ValueLabel("name"),
		arg.Usage("the build configuration whose compile commands are used"),
		arg.DefaultValue("Debug"),
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
	cl.Add(fix, buildDir, config, jobs, files)
	arg.Group("clang-tidy Flags", buildDir, config, jobs)
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
	tidier := &clangtidy.Command{
		Dir:         dir,
		Fix:         cb.fix,
		Checks:      cb.Checks,
		FormatStyle: cb.FormatStyle,
	}
	var fixer FixApplier
	if cb.fix {
		fixer = tidier
	}
	return &ClangTidyRunner{
		Differ: cb.differ(dir),
		Database: &clangtidy.ConfigDatabase{
			Dir:      dir,
			BuildDir: cb.buildDir,
			Config:   cb.config,
		},
		Tidier: tidier,
		Fixer:  fixer,
		Jobs:   cb.jobs,
		Paths:  cb.paths,
	}, nil
}

func (cb *ClangTidyBuilder) differ(dir string) Differ {
	if cb.WholeFiles {
		return nil
	}
	return diff.Git{Dir: dir, Context: contextLines}
}

func (cb *ClangTidyBuilder) warnMissingDatabase(ctx context.Context, dir string) {
	database := filepath.Join(cb.buildDir, clangtidy.DatabaseFile)
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
	// Differ reports the changed lines of each file. Every file is checked in
	// full when Differ is nil.
	Differ Differ

	// Database writes the compilation database that Tidier reads.
	Database DatabaseWriter

	// Tidier runs clang-tidy over each file.
	Tidier Tidier

	// Fixer applies the fixes that Tidier suggests, once every file has been
	// checked. No fixes are applied when Fixer is nil.
	Fixer FixApplier

	// Jobs is the number of files checked at once.
	Jobs int

	// Paths are the files to check.
	Paths []string
}

// Run runs clang-tidy over each file, reporting only the diagnostics within
// the ranges from Differ; a file without changes, or any file when Differ is
// nil, is checked in full. The output for each file that clang-tidy fails on
// is written to the error stream, in the order of Paths.
//
// Files that the database does not cover, such as those of a library that is
// only built on another platform, are skipped with a notice on the error
// stream, since clang-tidy cannot compile them. Once every file has been
// checked, the fixes suggested for all of them are applied by Fixer.
//
// [ErrClangTidy] is returned if clang-tidy fails for any file. Any other error
// is returned if the changes cannot be read from Differ, if the compilation
// database cannot be written or removed by Database, or if Fixer cannot apply
// the fixes.
func (cr *ClangTidyRunner) Run(ctx context.Context) (err error) {
	ranges, err := cr.ranges(ctx)
	if err != nil {
		return err
	}
	buildDir, remove, err := cr.Database.WriteDatabase()
	if err != nil {
		return err
	}
	defer func() { err = errors.Join(err, remove()) }()

	out := stream.Verbatim(cli.ErrStream(ctx))
	var paths []string
	for _, path := range cr.Paths {
		if !cr.Database.Covers(path) {
			fmt.Fprintf(out, "%s: skipped, not covered by the compilation database\n", path)
			continue
		}
		paths = append(paths, path)
	}
	failed := false
	for _, result := range cr.tidy(ctx, buildDir, paths, ranges) {
		if result.err == nil {
			continue
		}
		failed = true
		out.Write(result.output)
		fmt.Fprintf(out, "%s: %v\n", result.path, result.err)
	}
	if cr.Fixer != nil {
		if err := cr.Fixer.ApplyFixes(ctx); err != nil {
			return err
		}
	}
	if failed {
		return ErrClangTidy
	}
	return nil
}

func (cr *ClangTidyRunner) ranges(ctx context.Context) (map[string][]diff.Range, error) {
	if cr.Differ == nil {
		return nil, nil
	}
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

// tidy runs the Tidier over each of paths with up to Jobs running at once, and
// returns the results in the same order as paths.
func (cr *ClangTidyRunner) tidy(ctx context.Context, buildDir string, paths []string, ranges map[string][]diff.Range) []tidyResult {
	results := make([]tidyResult, len(paths))
	limit := make(chan struct{}, max(cr.Jobs, 1))
	var wg sync.WaitGroup
	for i, path := range paths {
		wg.Go(func() {
			limit <- struct{}{}
			defer func() { <-limit }()

			output, err := cr.Tidier.Tidy(ctx, buildDir, path, ranges[path])
			results[i] = tidyResult{path: path, output: output, err: err}
		})
	}
	wg.Wait()
	return results
}

var _ cli.Runner = (*ClangTidyRunner)(nil)
