package generatecmd_test

import (
	"context"
	"errors"
	"testing"

	"github.com/google/go-cmp/cmp"
	"github.com/google/go-cmp/cmp/cmpopts"

	"github.com/bitwizeshift/go-cli"

	"github.com/bitwizeshift/alloy/tools/alloy-cli/internal/generatecmd"
	"github.com/bitwizeshift/alloy/tools/internal/root"
	"github.com/bitwizeshift/alloy/tools/internal/root/roottest"
)

func TestDocumentationBuilderBuild(t *testing.T) {
	t.Parallel()

	testErr := errors.New("test error")
	testCases := []struct {
		name    string
		rooter  root.Rooter
		want    cli.Runner
		wantErr error
	}{
		{
			name:    "repository root",
			rooter:  roottest.Rooter("/repo"),
			want:    &generatecmd.DocumentationRunner{Dir: "/repo"},
			wantErr: nil,
		}, {
			name:    "rooter error",
			rooter:  roottest.ErrRooter(testErr),
			want:    nil,
			wantErr: testErr,
		},
	}

	for _, tc := range testCases {
		t.Run(tc.name, func(t *testing.T) {
			t.Parallel()

			// Arrange
			ctx := context.Background()
			sut := &generatecmd.DocumentationBuilder{Rooter: tc.rooter}

			// Act
			runner, err := sut.Build(ctx)

			// Assert
			if got, want := err, tc.wantErr; !cmp.Equal(got, want, cmpopts.EquateErrors()) {
				t.Fatalf("DocumentationBuilder.Build() = %v, want %v", got, want)
			}
			if got, want := runner, tc.want; !cmp.Equal(got, want) {
				t.Errorf("DocumentationBuilder.Build() = %v, want %v", got, want)
			}
		})
	}
}
