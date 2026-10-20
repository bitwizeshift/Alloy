package checkcmd_test

import (
	"context"
	"errors"
	"io/fs"
	"strings"
	"testing"

	"github.com/google/go-cmp/cmp"
	"github.com/google/go-cmp/cmp/cmpopts"

	"github.com/bitwizeshift/go-cli"
	"github.com/bitwizeshift/go-cli/arg"
	"github.com/bitwizeshift/go-cli/arg/argtest"
	"github.com/bitwizeshift/go-cli/clitest"

	"github.com/bitwizeshift/alloy/tools/alloy-cli/internal/checkcmd"
	"github.com/bitwizeshift/alloy/tools/internal/commitmsg"
	"github.com/bitwizeshift/alloy/tools/internal/root/roottest"
)

func TestCommitMsgBuilderRegisterArgs(t *testing.T) {
	t.Parallel()

	// Arrange
	cl := argtest.NewCommandLine()
	sut := &checkcmd.CommitMsgBuilder{Rooter: roottest.Rooter("/repo")}

	// Act
	arg.Register(cl, sut)

	// Assert
	flags := argtest.LongFlags(cl)
	positionals := argtest.AllPositionals(cl)
	if got, want := flags, []string(nil); !cmp.Equal(got, want, cmpopts.EquateEmpty()) {
		t.Errorf("CommitMsgBuilder.RegisterArgs() flags = %v, want %v", got, want)
	}
	wantPositionals := []*argtest.Positional{{Index: 0, Name: "file", Required: true}}
	if got, want := positionals, wantPositionals; !cmp.Equal(got, want, cmpopts.IgnoreFields(argtest.Positional{}, "Usage")) {
		t.Errorf("CommitMsgBuilder.RegisterArgs() positionals = %v, want %v", got, want)
	}
}

func TestCommitMsgBuilderBuild(t *testing.T) {
	t.Parallel()

	testErr := errors.New("test error")
	repo := absPath(t, "testdata/repo")
	wantRunner := &checkcmd.CommitMsgRunner{
		Policy: checkcmd.CommitPolicy([]string{"core", "logging", "tools"}),
		Message: &commitmsg.Message{
			Subject: "Add a thing",
			Lines: []string{
				"",
				"Body line.",
				"",
				"Change-Category: chore",
				"Change-Impact: none",
			},
			Trailers: []commitmsg.Trailer{
				{Key: "Change-Category", Value: "chore"},
				{Key: "Change-Impact", Value: "none"},
			},
		},
	}
	testCases := []struct {
		name    string
		builder *checkcmd.CommitMsgBuilder
		args    []string
		want    cli.Runner
		wantErr error
	}{
		{
			name:    "relative to root",
			builder: &checkcmd.CommitMsgBuilder{Rooter: roottest.Rooter(repo)},
			args:    []string{"COMMIT_EDITMSG"},
			want:    wantRunner,
			wantErr: nil,
		}, {
			name:    "absolute path",
			builder: &checkcmd.CommitMsgBuilder{Rooter: roottest.Rooter(repo)},
			args:    []string{absPath(t, "testdata/repo/COMMIT_EDITMSG")},
			want:    wantRunner,
			wantErr: nil,
		}, {
			name:    "missing message",
			builder: &checkcmd.CommitMsgBuilder{Rooter: roottest.Rooter(repo)},
			args:    []string{"MISSING"},
			want:    nil,
			wantErr: fs.ErrNotExist,
		}, {
			name:    "missing components",
			builder: &checkcmd.CommitMsgBuilder{Rooter: roottest.Rooter(absPath(t, "testdata"))},
			args:    []string{"repo/COMMIT_EDITMSG"},
			want:    nil,
			wantErr: fs.ErrNotExist,
		}, {
			name:    "rooter error",
			builder: &checkcmd.CommitMsgBuilder{Rooter: roottest.ErrRooter(testErr)},
			args:    []string{"COMMIT_EDITMSG"},
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
				t.Fatalf("CommitMsgBuilder.Build() = %v, want %v", got, want)
			}
			if got, want := runner, tc.want; !cmp.Equal(got, want) {
				t.Errorf("CommitMsgBuilder.Build() = %v, want %v\n%s", got, want, cmp.Diff(want, got))
			}
		})
	}
}

func TestCommitMsgRunnerRun(t *testing.T) {
	t.Parallel()

	testCases := []struct {
		name           string
		message        *commitmsg.Message
		wantViolations int
		wantErr        error
	}{
		{
			name: "valid message",
			message: &commitmsg.Message{
				Subject: "Add a thing",
				Lines:   []string{"", "Change-Category: chore", "Change-Impact: none"},
				Trailers: []commitmsg.Trailer{
					{Key: "Change-Category", Value: "chore"},
					{Key: "Change-Impact", Value: "none"},
				},
			},
			wantViolations: 0,
			wantErr:        nil,
		}, {
			name: "invalid message",
			message: &commitmsg.Message{
				Subject:  "Add a thing",
				Lines:    nil,
				Trailers: nil,
			},
			wantViolations: 2,
			wantErr:        checkcmd.ErrCommitMsg,
		},
	}

	for _, tc := range testCases {
		t.Run(tc.name, func(t *testing.T) {
			t.Parallel()

			// Arrange
			ctx, output := clitest.WithCaptureWriters(context.Background())
			sut := &checkcmd.CommitMsgRunner{
				Policy:  checkcmd.CommitPolicy([]string{"tools"}),
				Message: tc.message,
			}

			// Act
			err := sut.Run(ctx)

			// Assert
			if got, want := err, tc.wantErr; !cmp.Equal(got, want, cmpopts.EquateErrors()) {
				t.Fatalf("CommitMsgRunner.Run() = %v, want %v", got, want)
			}
			violations := strings.Count(output.Stderr.String(), "commit message: ")
			if got, want := violations, tc.wantViolations; !cmp.Equal(got, want) {
				t.Errorf("CommitMsgRunner.Run() violations = %d, want %d\n%s", got, want, output.Stderr)
			}
		})
	}
}
