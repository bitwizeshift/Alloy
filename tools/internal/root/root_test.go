package root_test

import (
	"context"
	"errors"
	"path/filepath"
	"testing"

	"github.com/google/go-cmp/cmp"
	"github.com/google/go-cmp/cmp/cmpopts"

	"github.com/bitwizeshift/alloy/tools/internal/root"
	"github.com/bitwizeshift/alloy/tools/internal/root/roottest"
)

func repoRoot(t *testing.T) string {
	t.Helper()
	dir, err := filepath.Abs(filepath.Join("..", "..", ".."))
	if err != nil {
		t.Fatalf("filepath.Abs() = %v", err)
	}
	resolved, err := filepath.EvalSymlinks(dir)
	if err != nil {
		t.Fatalf("filepath.EvalSymlinks() = %v", err)
	}
	return resolved
}

func primeRoot(t *testing.T, sut root.Rooter) {
	t.Helper()
	ctx := context.Background()
	_, _ = sut.Root(ctx)
}

func TestGitRooterRoot(t *testing.T) {
	t.Parallel()

	testCases := []struct {
		name    string
		dir     string
		want    string
		wantErr error
	}{
		{
			name:    "working directory",
			dir:     "",
			want:    repoRoot(t),
			wantErr: nil,
		}, {
			name:    "nested directory",
			dir:     "roottest",
			want:    repoRoot(t),
			wantErr: nil,
		}, {
			name:    "outside working tree",
			dir:     "/",
			want:    "",
			wantErr: cmpopts.AnyError,
		},
	}

	for _, tc := range testCases {
		t.Run(tc.name, func(t *testing.T) {
			t.Parallel()

			// Arrange
			ctx := context.Background()
			sut := root.GitRooter{Dir: tc.dir}

			// Act
			dir, err := sut.Root(ctx)

			// Assert
			if got, want := err, tc.wantErr; !cmp.Equal(got, want, cmpopts.EquateErrors()) {
				t.Fatalf("GitRooter.Root() = %v, want %v", got, want)
			}
			if got, want := dir, tc.want; !cmp.Equal(got, want) {
				t.Errorf("GitRooter.Root() = %q, want %q", got, want)
			}
		})
	}
}

func TestCachedRooterRoot(t *testing.T) {
	t.Parallel()

	testErr := errors.New("test error")
	testCases := []struct {
		name    string
		rooter  root.Rooter
		want    string
		wantErr error
	}{
		{
			name:    "rooter succeeds",
			rooter:  roottest.Rooter("/repo"),
			want:    "/repo",
			wantErr: nil,
		}, {
			name:    "rooter fails",
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
			sut := &root.CachedRooter{Rooter: tc.rooter}

			// Act
			dir, err := sut.Root(ctx)

			// Assert
			if got, want := err, tc.wantErr; !cmp.Equal(got, want, cmpopts.EquateErrors()) {
				t.Fatalf("CachedRooter.Root() = %v, want %v", got, want)
			}
			if got, want := dir, tc.want; !cmp.Equal(got, want) {
				t.Errorf("CachedRooter.Root() = %q, want %q", got, want)
			}
		})
	}
}

func TestCachedRooterRoot_AfterSuccess_ReturnsCachedRoot(t *testing.T) {
	t.Parallel()

	// Arrange
	ctx := context.Background()
	sut := &root.CachedRooter{Rooter: roottest.Rooter("/repo")}
	primeRoot(t, sut)
	sut.Rooter = roottest.ErrRooter(errors.New("test error"))

	// Act
	dir, err := sut.Root(ctx)

	// Assert
	if got, want := err, error(nil); !cmp.Equal(got, want, cmpopts.EquateErrors()) {
		t.Fatalf("CachedRooter.Root() = %v, want %v", got, want)
	}
	if got, want := dir, "/repo"; !cmp.Equal(got, want) {
		t.Errorf("CachedRooter.Root() = %q, want %q", got, want)
	}
}

func TestCachedRooterRoot_AfterFailure_ConsultsRooter(t *testing.T) {
	t.Parallel()

	// Arrange
	ctx := context.Background()
	sut := &root.CachedRooter{Rooter: roottest.ErrRooter(errors.New("test error"))}
	primeRoot(t, sut)
	sut.Rooter = roottest.Rooter("/repo")

	// Act
	dir, err := sut.Root(ctx)

	// Assert
	if got, want := err, error(nil); !cmp.Equal(got, want, cmpopts.EquateErrors()) {
		t.Fatalf("CachedRooter.Root() = %v, want %v", got, want)
	}
	if got, want := dir, "/repo"; !cmp.Equal(got, want) {
		t.Errorf("CachedRooter.Root() = %q, want %q", got, want)
	}
}
