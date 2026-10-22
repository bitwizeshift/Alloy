package checkcmd_test

import (
	"context"
	"errors"
	"testing"

	"github.com/google/go-cmp/cmp"
	"github.com/google/go-cmp/cmp/cmpopts"

	"github.com/bitwizeshift/go-cli"
	"github.com/bitwizeshift/go-cli/arg"
	"github.com/bitwizeshift/go-cli/arg/argtest"

	"github.com/bitwizeshift/alloy/tools/alloy-cli/internal/checkcmd"
	"github.com/bitwizeshift/alloy/tools/internal/check"
	"github.com/bitwizeshift/alloy/tools/internal/includes"
	"github.com/bitwizeshift/alloy/tools/internal/root/roottest"
)

func TestIncludeOrderBuilderRegisterArgs(t *testing.T) {
	t.Parallel()

	// Arrange
	cl := argtest.NewCommandLine()
	sut := &checkcmd.IncludeOrderBuilder{Rooter: roottest.Rooter("/repo")}

	// Act
	arg.Register(cl, sut)

	// Assert
	flags := argtest.LongFlags(cl)
	unmatched := argtest.GetUnmatched(cl)
	if got, want := flags, []string{"fix"}; !cmp.Equal(got, want) {
		t.Errorf("IncludeOrderBuilder.RegisterArgs() flags = %v, want %v", got, want)
	}
	wantUnmatched := &argtest.Unmatched{Name: "file", Required: true}
	if got, want := unmatched, wantUnmatched; !cmp.Equal(got, want, cmpopts.IgnoreFields(argtest.Unmatched{}, "Usage")) {
		t.Errorf("IncludeOrderBuilder.RegisterArgs() unmatched = %v, want %v", got, want)
	}
}

func TestIncludeOrderBuilderBuild(t *testing.T) {
	t.Parallel()

	testErr := errors.New("test error")
	repo := absPath(t, "testdata/repo")
	checker := &includes.Checker{
		Roots:     []string{"include", "src"},
		Targets:   []string{"test", "benchmark", "example", "bin"},
		Libraries: []string{"alloy/core"},
	}
	testCases := []struct {
		name    string
		builder *checkcmd.IncludeOrderBuilder
		args    []string
		want    cli.Runner
		wantErr error
	}{
		{
			name:    "check",
			builder: &checkcmd.IncludeOrderBuilder{Rooter: roottest.Rooter(repo)},
			args:    []string{"a.cpp", "b.hpp"},
			want: &checkcmd.FileRunner{
				Checker: checker,
				FS:      check.OS{Dir: repo},
				Fix:     false,
				Paths:   []string{"a.cpp", "b.hpp"},
			},
			wantErr: nil,
		}, {
			name:    "fix",
			builder: &checkcmd.IncludeOrderBuilder{Rooter: roottest.Rooter(repo)},
			args:    []string{"--fix", "a.cpp"},
			want: &checkcmd.FileRunner{
				Checker: checker,
				FS:      check.OS{Dir: repo},
				Fix:     true,
				Paths:   []string{"a.cpp"},
			},
			wantErr: nil,
		}, {
			name:    "rooter error",
			builder: &checkcmd.IncludeOrderBuilder{Rooter: roottest.ErrRooter(testErr)},
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
				t.Fatalf("IncludeOrderBuilder.Build() = %v, want %v", got, want)
			}
			if got, want := runner, tc.want; !cmp.Equal(got, want) {
				t.Errorf("IncludeOrderBuilder.Build() = %v, want %v", got, want)
			}
		})
	}
}
