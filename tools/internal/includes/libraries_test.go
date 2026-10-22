package includes_test

import (
	"path"
	"testing"
	"testing/fstest"

	"github.com/google/go-cmp/cmp"
	"github.com/google/go-cmp/cmp/cmpopts"

	"github.com/bitwizeshift/alloy/tools/internal/includes"
)

func TestLibraries(t *testing.T) {
	t.Parallel()

	fsys := fstest.MapFS{
		"lib/CMakeLists.txt":                     {},
		"lib/alloy-a/include/alloy/a/a.hpp":      {},
		"lib/alloy-a/include/alloy/a/x/x.hpp":    {},
		"lib/alloy-a/src/alloy/a/x/x.cpp":        {},
		"lib/alloy-b-c/include/alloy/b/c/c.hpp":  {},
		"lib/alloy-b-c/src/alloy/b/c/c.cpp":      {},
		"lib/alloy-b/include/alloy/b/b.hpp":      {},
		"lib/alloy-b/include/alloy/b/x/x.hpp":    {},
		"lib/alloy-b-c/test/src/alloy/b/c/c.cpp": {},
		"extra/flat/src/main.cpp":                {},
		"extra/empty/README.md":                  {},
	}
	testCases := []struct {
		name     string
		patterns []string
		want     []string
		wantErr  error
	}{
		{
			name:     "libraries",
			patterns: []string{"lib/*", "extra/*"},
			want:     []string{"alloy/a", "alloy/b", "alloy/b/c"},
			wantErr:  nil,
		}, {
			name:     "duplicate patterns",
			patterns: []string{"lib/alloy-a", "lib/alloy-a"},
			want:     []string{"alloy/a"},
			wantErr:  nil,
		}, {
			name:     "no libraries",
			patterns: []string{"extra/*"},
			want:     nil,
			wantErr:  nil,
		}, {
			name:     "bad pattern",
			patterns: []string{"lib/["},
			want:     nil,
			wantErr:  path.ErrBadPattern,
		},
	}

	for _, tc := range testCases {
		t.Run(tc.name, func(t *testing.T) {
			t.Parallel()

			// Arrange
			roots := []string{"include", "src"}

			// Act
			libraries, err := includes.Libraries(fsys, roots, tc.patterns...)

			// Assert
			if got, want := err, tc.wantErr; !cmp.Equal(got, want, cmpopts.EquateErrors()) {
				t.Fatalf("Libraries() = %v, want %v", got, want)
			}
			if got, want := libraries, tc.want; !cmp.Equal(got, want, cmpopts.EquateEmpty()) {
				t.Errorf("Libraries() = %v, want %v", got, want)
			}
		})
	}
}
