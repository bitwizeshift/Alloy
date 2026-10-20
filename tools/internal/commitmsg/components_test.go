package commitmsg_test

import (
	"io/fs"
	"testing"
	"testing/fstest"

	"github.com/google/go-cmp/cmp"
	"github.com/google/go-cmp/cmp/cmpopts"

	"github.com/bitwizeshift/alloy/tools/internal/commitmsg"
)

func TestComponents(t *testing.T) {
	t.Parallel()

	testCases := []struct {
		name    string
		fs      fs.FS
		want    []string
		wantErr error
	}{
		{
			name: "libraries and extras",
			fs: fstest.MapFS{
				"lib/alloy-core/CMakeLists.txt":       {},
				"lib/alloy-render-gl/CMakeLists.txt":  {},
				"lib/CMakeLists.txt":                  {},
				"lib/other/CMakeLists.txt":            {},
				"extra/logging/CMakeLists.txt":        {},
				"extra/sdl2-bindings/CMakeLists.txt":  {},
				"extra/CMakeLists.txt":                {},
				"extra/tools/CMakeLists.txt":          {},
				"tools/check-whitespace.go":           {},
				"third_party/catch/CMakeLists.txt":    {},
				"lib/alloy-core/include/alloy/core.h": {},
			},
			want:    []string{"core", "logging", "render-gl", "sdl2-bindings", "tools"},
			wantErr: nil,
		}, {
			name: "missing lib",
			fs: fstest.MapFS{
				"extra/logging/CMakeLists.txt": {},
			},
			want:    nil,
			wantErr: fs.ErrNotExist,
		}, {
			name: "missing extra",
			fs: fstest.MapFS{
				"lib/alloy-core/CMakeLists.txt": {},
			},
			want:    nil,
			wantErr: fs.ErrNotExist,
		},
	}

	for _, tc := range testCases {
		t.Run(tc.name, func(t *testing.T) {
			t.Parallel()

			// Act
			components, err := commitmsg.Components(tc.fs)

			// Assert
			if got, want := err, tc.wantErr; !cmp.Equal(got, want, cmpopts.EquateErrors()) {
				t.Fatalf("Components() = %v, want %v", got, want)
			}
			if got, want := components, tc.want; !cmp.Equal(got, want) {
				t.Errorf("Components() = %v, want %v", got, want)
			}
		})
	}
}
