package checkcmd_test

import (
	"context"
	"errors"
	"runtime"
	"strings"
	"testing"

	"github.com/google/go-cmp/cmp"
	"github.com/google/go-cmp/cmp/cmpopts"

	"github.com/bitwizeshift/go-cli"
	"github.com/bitwizeshift/go-cli/arg"
	"github.com/bitwizeshift/go-cli/arg/argtest"
	"github.com/bitwizeshift/go-cli/clitest"

	"github.com/bitwizeshift/alloy/tools/alloy-cli/internal/checkcmd"
	"github.com/bitwizeshift/alloy/tools/internal/clangtidy"
	"github.com/bitwizeshift/alloy/tools/internal/diff"
	"github.com/bitwizeshift/alloy/tools/internal/root/roottest"
)

func TestClangTidyBuilderRegisterArgs(t *testing.T) {
	t.Parallel()

	// Arrange
	cl := argtest.NewCommandLine()
	sut := &checkcmd.ClangTidyBuilder{Rooter: roottest.Rooter("/repo")}

	// Act
	arg.Register(cl, sut)

	// Assert
	flags := argtest.AllFlags(cl)
	unmatched := argtest.GetUnmatched(cl)
	wantFlags := []*argtest.Flag{
		{Long: "build-dir", Shorthand: "p", ValueLabel: "dir", Group: "clang-tidy Flags"},
		{Long: "config", ValueLabel: "name", Group: "clang-tidy Flags"},
		{Long: "fix", ValueLabel: "bool"},
		{Long: "jobs", Shorthand: "j", ValueLabel: "count", Group: "clang-tidy Flags"},
	}
	if got, want := flags, wantFlags; !cmp.Equal(got, want, cmpopts.EquateEmpty()) {
		t.Errorf("ClangTidyBuilder.RegisterArgs() flags = %v, want %v\n%s", got, want, cmp.Diff(want, got, cmpopts.EquateEmpty()))
	}
	wantUnmatched := &argtest.Unmatched{Name: "file", Required: true}
	if got, want := unmatched, wantUnmatched; !cmp.Equal(got, want, cmpopts.IgnoreFields(argtest.Unmatched{}, "Usage")) {
		t.Errorf("ClangTidyBuilder.RegisterArgs() unmatched = %v, want %v", got, want)
	}
}

func TestClangTidyBuilderRegisterArgs_NonPositiveJobs_ReturnsError(t *testing.T) {
	t.Parallel()

	// Arrange
	cl := argtest.NewCommandLine()
	sut := &checkcmd.ClangTidyBuilder{Rooter: roottest.Rooter("/repo")}
	arg.Register(cl, sut)

	// Act
	err := argtest.Parse(cl, "--jobs", "0", "a.cpp")

	// Assert
	if got, want := err, checkcmd.ErrJobs; !cmp.Equal(got, want, cmpopts.EquateErrors()) {
		t.Fatalf("argtest.Parse() = %v, want %v", got, want)
	}
}

func TestClangTidyBuilderBuild(t *testing.T) {
	t.Parallel()

	testErr := errors.New("test error")
	testCases := []struct {
		name    string
		builder *checkcmd.ClangTidyBuilder
		args    []string
		want    cli.Runner
		wantErr error
	}{
		{
			name:    "defaults",
			builder: &checkcmd.ClangTidyBuilder{Rooter: roottest.Rooter("/repo")},
			args:    []string{"a.cpp", "b.cpp"},
			want: &checkcmd.ClangTidyRunner{
				Differ: diff.Git{Dir: "/repo", Context: 5},
				Database: &clangtidy.ConfigDatabase{
					Dir:      "/repo",
					BuildDir: "build",
					Config:   "Debug",
				},
				Tidier: &clangtidy.Command{Dir: "/repo", Fix: false},
				Jobs:   runtime.NumCPU(),
				Paths:  []string{"a.cpp", "b.cpp"},
			},
			wantErr: nil,
		}, {
			name:    "flags",
			builder: &checkcmd.ClangTidyBuilder{Rooter: roottest.Rooter("/repo")},
			args:    []string{"--fix", "-p", "out", "--config", "Release", "-j", "3", "a.cpp"},
			want: &checkcmd.ClangTidyRunner{
				Differ: diff.Git{Dir: "/repo", Context: 5},
				Database: &clangtidy.ConfigDatabase{
					Dir:      "/repo",
					BuildDir: "out",
					Config:   "Release",
				},
				Tidier: &clangtidy.Command{Dir: "/repo", Fix: true},
				Fixer:  &clangtidy.Command{Dir: "/repo", Fix: true},
				Jobs:   3,
				Paths:  []string{"a.cpp"},
			},
			wantErr: nil,
		}, {
			name:    "include cleaner",
			builder: checkcmd.IncludeCleaner(roottest.Rooter("/repo")),
			args:    []string{"--fix", "a.hpp"},
			want: &checkcmd.ClangTidyRunner{
				Differ: nil,
				Database: &clangtidy.ConfigDatabase{
					Dir:      "/repo",
					BuildDir: "build",
					Config:   "Debug",
				},
				Tidier: &clangtidy.Command{
					Dir:         "/repo",
					Fix:         true,
					Checks:      "-*,misc-include-cleaner",
					FormatStyle: "none",
				},
				Fixer: &clangtidy.Command{
					Dir:         "/repo",
					Fix:         true,
					Checks:      "-*,misc-include-cleaner",
					FormatStyle: "none",
				},
				Jobs:  runtime.NumCPU(),
				Paths: []string{"a.hpp"},
			},
			wantErr: nil,
		}, {
			name:    "rooter error",
			builder: &checkcmd.ClangTidyBuilder{Rooter: roottest.ErrRooter(testErr)},
			args:    []string{"a.cpp"},
			want:    nil,
			wantErr: testErr,
		},
	}

	for _, tc := range testCases {
		t.Run(tc.name, func(t *testing.T) {
			t.Parallel()

			// Arrange
			ctx, _ := clitest.WithCaptureWriters(context.Background())
			sut := tc.builder
			parseArgs(t, sut, tc.args...)

			// Act
			runner, err := sut.Build(ctx)

			// Assert
			if got, want := err, tc.wantErr; !cmp.Equal(got, want, cmpopts.EquateErrors()) {
				t.Fatalf("ClangTidyBuilder.Build() = %v, want %v", got, want)
			}
			opts := cmpopts.IgnoreUnexported(clangtidy.ConfigDatabase{}, clangtidy.Command{})
			if got, want := runner, tc.want; !cmp.Equal(got, want, opts) {
				t.Errorf("ClangTidyBuilder.Build() = %v, want %v\n%s", got, want, cmp.Diff(want, got, opts))
			}
		})
	}
}

func TestClangTidyBuilderBuild_MissingDatabase_Warns(t *testing.T) {
	t.Parallel()

	// Arrange
	ctx, output := clitest.WithCaptureWriters(context.Background())
	sut := &checkcmd.ClangTidyBuilder{Rooter: roottest.Rooter(absPath(t, "testdata/repo"))}
	parseArgs(t, sut, "a.cpp")

	// Act
	_, err := sut.Build(ctx)

	// Assert
	if got, want := err, error(nil); !cmp.Equal(got, want, cmpopts.EquateErrors()) {
		t.Fatalf("ClangTidyBuilder.Build() = %v, want %v", got, want)
	}
	warned := strings.Contains(output.Stderr.String(), absPath(t, "testdata/repo/build/compile_commands.json"))
	if got, want := warned, true; !cmp.Equal(got, want) {
		t.Errorf("ClangTidyBuilder.Build() stderr = %q, want a warning naming the database", output.Stderr)
	}
}

func TestClangTidyRunnerRun(t *testing.T) {
	t.Parallel()

	testErr := errors.New("test error")
	testCases := []struct {
		name       string
		differ     checkcmd.Differ
		database   checkcmd.DatabaseWriter
		tidier     checkcmd.Tidier
		fixer      checkcmd.FixApplier
		paths      []string
		wantStderr string
		wantErr    error
	}{
		{
			name: "clean files",
			differ: differ{
				{Path: "a.cpp", Ranges: []diff.Range{{Start: 1, End: 3}}},
			},
			database:   databaseWriter{dir: "db"},
			tidier:     noOpTidier{},
			paths:      []string{"a.cpp", "b.cpp"},
			wantStderr: "",
			wantErr:    nil,
		}, {
			name: "failing files",
			differ: differ{
				{
					Path: "b.cpp",
					Ranges: []diff.Range{
						{Start: 1, End: 3},
						{Start: 9, End: 12},
					},
				},
			},
			database: databaseWriter{dir: "db"},
			tidier:   echoTidier{err: testErr},
			paths:    []string{"a.cpp", "b.cpp"},
			wantStderr: "db a.cpp []\n" +
				"a.cpp: test error\n" +
				"db b.cpp [{1 3} {9 12}]\n" +
				"b.cpp: test error\n",
			wantErr: checkcmd.ErrClangTidy,
		}, {
			name:     "whole files",
			differ:   nil,
			database: databaseWriter{dir: "db"},
			tidier:   echoTidier{err: testErr},
			paths:    []string{"a.cpp"},
			wantStderr: "db a.cpp []\n" +
				"a.cpp: test error\n",
			wantErr: checkcmd.ErrClangTidy,
		}, {
			name:     "uncovered files",
			differ:   nil,
			database: databaseWriter{dir: "db", uncovered: []string{"a.cpp"}},
			tidier:   echoTidier{err: testErr},
			paths:    []string{"a.cpp", "b.cpp"},
			wantStderr: "a.cpp: skipped, not covered by the compilation database\n" +
				"db b.cpp []\n" +
				"b.cpp: test error\n",
			wantErr: checkcmd.ErrClangTidy,
		}, {
			name:       "fixes applied",
			differ:     nil,
			database:   databaseWriter{dir: "db"},
			tidier:     noOpTidier{},
			fixer:      fixApplier{err: nil},
			paths:      []string{"a.cpp"},
			wantStderr: "",
			wantErr:    nil,
		}, {
			name:       "fix error",
			differ:     nil,
			database:   databaseWriter{dir: "db"},
			tidier:     noOpTidier{},
			fixer:      fixApplier{err: testErr},
			paths:      []string{"a.cpp"},
			wantStderr: "",
			wantErr:    testErr,
		}, {
			name:       "differ error",
			differ:     errDiffer{err: testErr},
			database:   databaseWriter{dir: "db"},
			tidier:     noOpTidier{},
			paths:      []string{"a.cpp"},
			wantStderr: "",
			wantErr:    testErr,
		}, {
			name:       "database error",
			differ:     differ{},
			database:   errDatabaseWriter{err: testErr},
			tidier:     noOpTidier{},
			paths:      []string{"a.cpp"},
			wantStderr: "",
			wantErr:    testErr,
		}, {
			name:       "database removal error",
			differ:     differ{},
			database:   databaseWriter{dir: "db", removeErr: testErr},
			tidier:     noOpTidier{},
			paths:      []string{"a.cpp"},
			wantStderr: "",
			wantErr:    testErr,
		},
	}

	for _, tc := range testCases {
		t.Run(tc.name, func(t *testing.T) {
			t.Parallel()

			// Arrange
			ctx, output := clitest.WithCaptureWriters(context.Background())
			sut := &checkcmd.ClangTidyRunner{
				Differ:   tc.differ,
				Database: tc.database,
				Tidier:   tc.tidier,
				Fixer:    tc.fixer,
				Jobs:     2,
				Paths:    tc.paths,
			}

			// Act
			err := sut.Run(ctx)

			// Assert
			if got, want := err, tc.wantErr; !cmp.Equal(got, want, cmpopts.EquateErrors()) {
				t.Fatalf("ClangTidyRunner.Run() = %v, want %v", got, want)
			}
			if got, want := output.Stderr.String(), tc.wantStderr; !cmp.Equal(got, want) {
				t.Errorf("ClangTidyRunner.Run() stderr = %q, want %q", got, want)
			}
		})
	}
}
