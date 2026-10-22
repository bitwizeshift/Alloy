package checkcmd_test

import (
	"context"
	"errors"
	"os"
	"testing"

	"github.com/google/go-cmp/cmp"
	"github.com/google/go-cmp/cmp/cmpopts"

	"github.com/bitwizeshift/go-cli"
	"github.com/bitwizeshift/go-cli/arg"
	"github.com/bitwizeshift/go-cli/arg/argtest"

	"github.com/bitwizeshift/alloy/tools/alloy-cli/internal/checkcmd"
	"github.com/bitwizeshift/alloy/tools/internal/check"
	"github.com/bitwizeshift/alloy/tools/internal/cmakelists"
	"github.com/bitwizeshift/alloy/tools/internal/root/roottest"
)

func TestCMakeSourcesBuilderRegisterArgs(t *testing.T) {
	t.Parallel()

	// Arrange
	cl := argtest.NewCommandLine()
	sut := &checkcmd.CMakeSourcesBuilder{Rooter: roottest.Rooter("/repo")}

	// Act
	arg.Register(cl, sut)

	// Assert
	flags := argtest.LongFlags(cl)
	unmatched := argtest.GetUnmatched(cl)
	if got, want := flags, []string(nil); !cmp.Equal(got, want, cmpopts.EquateEmpty()) {
		t.Errorf("CMakeSourcesBuilder.RegisterArgs() flags = %v, want %v", got, want)
	}
	wantUnmatched := &argtest.Unmatched{Name: "file", Required: true}
	if got, want := unmatched, wantUnmatched; !cmp.Equal(got, want, cmpopts.IgnoreFields(argtest.Unmatched{}, "Usage")) {
		t.Errorf("CMakeSourcesBuilder.RegisterArgs() unmatched = %v, want %v", got, want)
	}
}

func TestCMakeSourcesBuilderBuild(t *testing.T) {
	t.Parallel()

	testErr := errors.New("test error")
	testCases := []struct {
		name    string
		builder *checkcmd.CMakeSourcesBuilder
		args    []string
		want    cli.Runner
		wantErr error
	}{
		{
			name:    "files",
			builder: &checkcmd.CMakeSourcesBuilder{Rooter: roottest.Rooter("/repo")},
			args:    []string{"a.cpp", "b.hpp"},
			want: &checkcmd.CheckRunner{
				Checker: &cmakelists.Checker{FS: os.DirFS("/repo")},
				FS:      check.OS{Dir: "/repo"},
				Paths:   []string{"a.cpp", "b.hpp"},
			},
			wantErr: nil,
		}, {
			name:    "rooter error",
			builder: &checkcmd.CMakeSourcesBuilder{Rooter: roottest.ErrRooter(testErr)},
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
				t.Fatalf("CMakeSourcesBuilder.Build() = %v, want %v", got, want)
			}
			if got, want := runner, tc.want; !cmp.Equal(got, want) {
				t.Errorf("CMakeSourcesBuilder.Build() = %v, want %v", got, want)
			}
		})
	}
}
