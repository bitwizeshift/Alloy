package cmakelists_test

import (
	"io/fs"
	"strings"
	"testing"
	"testing/fstest"

	"github.com/google/go-cmp/cmp"
	"github.com/google/go-cmp/cmp/cmpopts"

	"github.com/bitwizeshift/alloy/tools/internal/check"
	"github.com/bitwizeshift/alloy/tools/internal/cmakelists"
)

func TestCheckerCheck(t *testing.T) {
	t.Parallel()

	testCases := []struct {
		name    string
		fs      fs.FS
		path    string
		want    []check.Finding
		wantErr error
	}{
		{
			name: "listed in same directory",
			fs: fstest.MapFS{
				"lib/a/src/CMakeLists.txt": {Data: []byte("set(files\n  a.cpp\n)\n")},
			},
			path:    "lib/a/src/a.cpp",
			want:    nil,
			wantErr: nil,
		}, {
			name: "listed in parent directory",
			fs: fstest.MapFS{
				"lib/a/CMakeLists.txt": {Data: []byte("set(files\n  src/a/a.cpp\n)\n")},
			},
			path:    "lib/a/src/a/a.cpp",
			want:    nil,
			wantErr: nil,
		}, {
			name: "listed in root",
			fs: fstest.MapFS{
				"CMakeLists.txt": {Data: []byte("add_executable(x src/a.cpp)\n")},
			},
			path:    "src/a.cpp",
			want:    nil,
			wantErr: nil,
		}, {
			name: "listed with source directory",
			fs: fstest.MapFS{
				"lib/a/CMakeLists.txt": {Data: []byte(`set(files "${CMAKE_CURRENT_SOURCE_DIR}/src/a.cpp")`)},
			},
			path:    "lib/a/src/a.cpp",
			want:    nil,
			wantErr: nil,
		}, {
			name: "listed with list directory",
			fs: fstest.MapFS{
				"lib/a/CMakeLists.txt": {Data: []byte(`set(files ${CMAKE_CURRENT_LIST_DIR}/src/a.cpp)`)},
			},
			path:    "lib/a/src/a.cpp",
			want:    nil,
			wantErr: nil,
		}, {
			name: "not listed",
			fs: fstest.MapFS{
				"lib/a/CMakeLists.txt": {Data: []byte("set(files\n  src/b.cpp\n)\n")},
			},
			path:    "lib/a/src/a.cpp",
			want:    []check.Finding{{Message: "not listed in lib/a/CMakeLists.txt"}},
			wantErr: nil,
		}, {
			name: "listed only in comment",
			fs: fstest.MapFS{
				"lib/a/CMakeLists.txt": {Data: []byte("set(files\n  # src/a.cpp\n)\n")},
			},
			path:    "lib/a/src/a.cpp",
			want:    []check.Finding{{Message: "not listed in lib/a/CMakeLists.txt"}},
			wantErr: nil,
		}, {
			name: "listed by suffix only",
			fs: fstest.MapFS{
				"lib/a/CMakeLists.txt": {Data: []byte("set(files\n  other/src/a.cpp\n)\n")},
			},
			path:    "lib/a/src/a.cpp",
			want:    []check.Finding{{Message: "not listed in lib/a/CMakeLists.txt"}},
			wantErr: nil,
		}, {
			name: "listed only above nearest",
			fs: fstest.MapFS{
				"lib/a/CMakeLists.txt":      {Data: []byte("set(files\n  test/src/a.cpp\n)\n")},
				"lib/a/test/CMakeLists.txt": {Data: []byte("set(files\n  src/b.cpp\n)\n")},
			},
			path:    "lib/a/test/src/a.cpp",
			want:    []check.Finding{{Message: "not listed in lib/a/test/CMakeLists.txt"}},
			wantErr: nil,
		}, {
			name:    "no CMakeLists.txt",
			fs:      fstest.MapFS{},
			path:    "lib/a/src/a.cpp",
			want:    []check.Finding{{Message: "not built by any CMakeLists.txt"}},
			wantErr: nil,
		}, {
			name: "unreadable CMakeLists.txt",
			fs: fstest.MapFS{
				"lib/a/CMakeLists.txt/b": {Data: nil},
			},
			path:    "lib/a/src/a.cpp",
			want:    nil,
			wantErr: fs.ErrInvalid,
		}, {
			name:    "invalid path",
			fs:      fstest.MapFS{},
			path:    "../a.cpp",
			want:    nil,
			wantErr: fs.ErrInvalid,
		},
	}

	for _, tc := range testCases {
		t.Run(tc.name, func(t *testing.T) {
			t.Parallel()

			// Arrange
			sut := &cmakelists.Checker{FS: tc.fs}

			// Act
			findings, err := sut.Check(tc.path, strings.NewReader(""))

			// Assert
			if got, want := err, tc.wantErr; !cmp.Equal(got, want, cmpopts.EquateErrors()) {
				t.Fatalf("Checker.Check() = %v, want %v", got, want)
			}
			if got, want := findings, tc.want; !cmp.Equal(got, want) {
				t.Errorf("Checker.Check() = %v, want %v", got, want)
			}
		})
	}
}
