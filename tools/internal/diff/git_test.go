package diff_test

import (
	"context"
	"testing"

	"github.com/google/go-cmp/cmp"
	"github.com/google/go-cmp/cmp/cmpopts"

	"github.com/bitwizeshift/alloy/tools/internal/diff"
)

func TestGitChanges(t *testing.T) {
	t.Parallel()

	testCases := []struct {
		name    string
		dir     string
		paths   []string
		want    []diff.File
		wantErr error
	}{
		{
			name:    "untracked path",
			dir:     "",
			paths:   []string{"testdata/missing.txt"},
			want:    nil,
			wantErr: nil,
		}, {
			name:    "outside working tree",
			dir:     "/",
			paths:   []string{"missing.txt"},
			want:    nil,
			wantErr: cmpopts.AnyError,
		},
	}

	for _, tc := range testCases {
		t.Run(tc.name, func(t *testing.T) {
			t.Parallel()

			// Arrange
			ctx := context.Background()
			sut := diff.Git{Dir: tc.dir, Context: 5}

			// Act
			files, err := sut.Changes(ctx, tc.paths...)

			// Assert
			if got, want := err, tc.wantErr; !cmp.Equal(got, want, cmpopts.EquateErrors()) {
				t.Fatalf("Git.Changes() = %v, want %v", got, want)
			}
			if got, want := files, tc.want; !cmp.Equal(got, want, cmpopts.EquateEmpty()) {
				t.Errorf("Git.Changes() = %v, want %v", got, want)
			}
		})
	}
}
