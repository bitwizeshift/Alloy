package roottest_test

import (
	"context"
	"errors"
	"testing"

	"github.com/google/go-cmp/cmp"
	"github.com/google/go-cmp/cmp/cmpopts"

	"github.com/bitwizeshift/alloy/tools/internal/root"
	"github.com/bitwizeshift/alloy/tools/internal/root/roottest"
)

func TestRooter(t *testing.T) {
	t.Parallel()

	testErr := errors.New("test error")
	testCases := []struct {
		name    string
		rooter  root.Rooter
		want    string
		wantErr error
	}{
		{
			name:    "root",
			rooter:  roottest.Rooter("/repo"),
			want:    "/repo",
			wantErr: nil,
		}, {
			name:    "error",
			rooter:  roottest.ErrRooter(testErr),
			want:    "",
			wantErr: testErr,
		},
	}

	for _, tc := range testCases {
		t.Run(tc.name, func(t *testing.T) {
			t.Parallel()

			// Arrange
			ctx := context.Background()
			sut := tc.rooter

			// Act
			dir, err := sut.Root(ctx)

			// Assert
			if got, want := err, tc.wantErr; !cmp.Equal(got, want, cmpopts.EquateErrors()) {
				t.Fatalf("Rooter.Root() = %v, want %v", got, want)
			}
			if got, want := dir, tc.want; !cmp.Equal(got, want) {
				t.Errorf("Rooter.Root() = %q, want %q", got, want)
			}
		})
	}
}
