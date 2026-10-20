package checkcmd_test

import (
	"context"
	"errors"
	"io/fs"
	"regexp"
	"testing"
	"time"

	"github.com/google/go-cmp/cmp"
	"github.com/google/go-cmp/cmp/cmpopts"

	"github.com/bitwizeshift/go-cli"
	"github.com/bitwizeshift/go-cli/arg"
	"github.com/bitwizeshift/go-cli/arg/argtest"
	"github.com/bitwizeshift/go-cli/clitest"

	"github.com/bitwizeshift/alloy/tools/alloy-cli/internal/checkcmd"
	"github.com/bitwizeshift/alloy/tools/internal/check"
	"github.com/bitwizeshift/alloy/tools/internal/check/checktest"
	"github.com/bitwizeshift/alloy/tools/internal/copyright"
	"github.com/bitwizeshift/alloy/tools/internal/newline"
	"github.com/bitwizeshift/alloy/tools/internal/root/roottest"
	"github.com/bitwizeshift/alloy/tools/internal/whitespace"
)

func TestFileBuilderRegisterArgs(t *testing.T) {
	t.Parallel()

	// Arrange
	cl := argtest.NewCommandLine()
	sut := checkcmd.Whitespace(roottest.Rooter("/repo"))

	// Act
	arg.Register(cl, sut)

	// Assert
	flags := argtest.LongFlags(cl)
	unmatched := argtest.GetUnmatched(cl)
	if got, want := flags, []string{"fix"}; !cmp.Equal(got, want) {
		t.Errorf("FileBuilder.RegisterArgs() flags = %v, want %v", got, want)
	}
	wantUnmatched := &argtest.Unmatched{Name: "file", Required: true}
	if got, want := unmatched, wantUnmatched; !cmp.Equal(got, want, cmpopts.IgnoreFields(argtest.Unmatched{}, "Usage")) {
		t.Errorf("FileBuilder.RegisterArgs() unmatched = %v, want %v", got, want)
	}
}

func TestFileBuilderBuild(t *testing.T) {
	t.Parallel()

	testErr := errors.New("test error")
	testCases := []struct {
		name    string
		builder *checkcmd.FileBuilder
		args    []string
		want    cli.Runner
		wantErr error
	}{
		{
			name:    "check",
			builder: checkcmd.Whitespace(roottest.Rooter("/repo")),
			args:    []string{"a.cpp", "b.cpp"},
			want: &checkcmd.FileRunner{
				Checker: whitespace.Checker{},
				FS:      check.OS{Dir: "/repo"},
				Fix:     false,
				Paths:   []string{"a.cpp", "b.cpp"},
			},
			wantErr: nil,
		}, {
			name:    "fix",
			builder: checkcmd.TrailingNewline(roottest.Rooter("/repo")),
			args:    []string{"--fix", "a.cpp"},
			want: &checkcmd.FileRunner{
				Checker: newline.Checker{},
				FS:      check.OS{Dir: "/repo"},
				Fix:     true,
				Paths:   []string{"a.cpp"},
			},
			wantErr: nil,
		}, {
			name:    "copyright",
			builder: checkcmd.LicenseCopyright(roottest.Rooter("/repo")),
			args:    []string{"a.cpp"},
			want: &checkcmd.FileRunner{
				Checker: &copyright.Checker{
					Pattern: regexp.MustCompile(`Copyright \(c\) (.*) Matthew Rodusek All rights reserved\.`),
					History: copyright.Git{Dir: "/repo"},
					Year:    time.Now().Year(),
				},
				FS:    check.OS{Dir: "/repo"},
				Fix:   false,
				Paths: []string{"a.cpp"},
			},
			wantErr: nil,
		}, {
			name:    "rooter error",
			builder: checkcmd.Whitespace(roottest.ErrRooter(testErr)),
			args:    []string{"a.cpp"},
			want:    nil,
			wantErr: testErr,
		},
	}

	for _, tc := range testCases {
		t.Run(tc.name, func(t *testing.T) {
			t.Parallel()

			// Arrange
			ctx := context.Background()
			sut := tc.builder
			parseArgs(t, sut, tc.args...)

			// Act
			runner, err := sut.Build(ctx)

			// Assert
			if got, want := err, tc.wantErr; !cmp.Equal(got, want, cmpopts.EquateErrors()) {
				t.Fatalf("FileBuilder.Build() = %v, want %v", got, want)
			}
			if got, want := runner, tc.want; !cmp.Equal(got, want, compareRegexps()) {
				t.Errorf("FileBuilder.Build() = %v, want %v\n%s", got, want, cmp.Diff(want, got, compareRegexps()))
			}
		})
	}
}

func compareRegexps() cmp.Option {
	return cmp.Comparer(func(lhs, rhs *regexp.Regexp) bool {
		return lhs.String() == rhs.String()
	})
}

func TestFileRunnerRun(t *testing.T) {
	t.Parallel()

	testErr := errors.New("test error")
	testCases := []struct {
		name       string
		fs         checktest.MemFS
		checker    check.CheckFixer
		fix        bool
		paths      []string
		wantStdout string
		wantStderr string
		wantFS     checktest.MemFS
		wantErr    error
	}{
		{
			name:       "check passes",
			fs:         checktest.MemFS{"a.cpp": "old"},
			checker:    checkFixer{Checker: checktest.Checker(), Fixer: checktest.Fixer("new")},
			fix:        false,
			paths:      []string{"a.cpp"},
			wantStdout: "",
			wantStderr: "",
			wantFS:     checktest.MemFS{"a.cpp": "old"},
			wantErr:    nil,
		}, {
			name: "check finds problems",
			fs:   checktest.MemFS{"[fg:red]a.cpp": "old"},
			checker: checkFixer{
				Checker: checktest.Checker(check.Finding{Line: 2, Message: "bad [/fg]"}),
				Fixer:   checktest.Fixer("new"),
			},
			fix:        false,
			paths:      []string{"[fg:red]a.cpp"},
			wantStdout: "",
			wantStderr: "[fg:red]a.cpp:2: bad [/fg]\n",
			wantFS:     checktest.MemFS{"[fg:red]a.cpp": "old"},
			wantErr:    check.ErrFindings,
		}, {
			name:       "fix",
			fs:         checktest.MemFS{"a.cpp": "old", "b.cpp": "new"},
			checker:    checkFixer{Checker: checktest.Checker(), Fixer: checktest.Fixer("new")},
			fix:        true,
			paths:      []string{"a.cpp", "b.cpp"},
			wantStdout: "a.cpp\n",
			wantStderr: "",
			wantFS:     checktest.MemFS{"a.cpp": "new", "b.cpp": "new"},
			wantErr:    nil,
		}, {
			name: "fix fails",
			fs:   checktest.MemFS{"a.cpp": "old"},
			checker: checkFixer{
				Checker: checktest.Checker(),
				Fixer:   checktest.ErrFixer(testErr),
			},
			fix:        true,
			paths:      []string{"a.cpp"},
			wantStdout: "",
			wantStderr: "",
			wantFS:     checktest.MemFS{"a.cpp": "old"},
			wantErr:    testErr,
		}, {
			name:       "missing file",
			fs:         checktest.MemFS{},
			checker:    checkFixer{Checker: checktest.Checker(), Fixer: checktest.Fixer("new")},
			fix:        false,
			paths:      []string{"a.cpp"},
			wantStdout: "",
			wantStderr: "",
			wantFS:     checktest.MemFS{},
			wantErr:    fs.ErrNotExist,
		},
	}

	for _, tc := range testCases {
		t.Run(tc.name, func(t *testing.T) {
			t.Parallel()

			// Arrange
			ctx, output := clitest.WithCaptureWriters(context.Background())
			sut := &checkcmd.FileRunner{
				Checker: tc.checker,
				FS:      tc.fs,
				Fix:     tc.fix,
				Paths:   tc.paths,
			}

			// Act
			err := sut.Run(ctx)

			// Assert
			if got, want := err, tc.wantErr; !cmp.Equal(got, want, cmpopts.EquateErrors()) {
				t.Fatalf("FileRunner.Run() = %v, want %v", got, want)
			}
			if got, want := output.Stdout.String(), tc.wantStdout; !cmp.Equal(got, want) {
				t.Errorf("FileRunner.Run() stdout = %q, want %q", got, want)
			}
			if got, want := output.Stderr.String(), tc.wantStderr; !cmp.Equal(got, want) {
				t.Errorf("FileRunner.Run() stderr = %q, want %q", got, want)
			}
			if got, want := tc.fs, tc.wantFS; !cmp.Equal(got, want, cmpopts.EquateEmpty()) {
				t.Errorf("FileRunner.Run() files = %v, want %v", got, want)
			}
		})
	}
}
