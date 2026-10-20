package diff_test

import (
	"errors"
	"io"
	"strings"
	"testing"
	"testing/iotest"

	"github.com/google/go-cmp/cmp"
	"github.com/google/go-cmp/cmp/cmpopts"

	"github.com/bitwizeshift/alloy/tools/internal/diff"
)

const modified = `diff --git src/a.cpp src/a.cpp
index 1111111..2222222 100644
--- src/a.cpp
+++ src/a.cpp
@@ -1,3 +1,4 @@
 int a;
+int b;
 int c;
 int d;
@@ -20 +21,2 @@ void f()
-old
+new
+++x
`

const added = `diff --git src/new.hpp src/new.hpp
new file mode 100644
index 0000000..3333333
--- /dev/null
+++ src/new.hpp
@@ -0,0 +1,2 @@
+#pragma once
+int x;
`

const deleted = `diff --git src/old.hpp src/old.hpp
deleted file mode 100644
index 4444444..0000000
--- src/old.hpp
+++ /dev/null
@@ -1,2 +0,0 @@
-#pragma once
--- int y;
`

const deletionOnly = `diff --git src/d.cpp src/d.cpp
index 5555555..6666666 100644
--- src/d.cpp
+++ src/d.cpp
@@ -4,2 +3,0 @@ int a;
-int b;
-int c;
\ No newline at end of file
`

const spaced = "diff --git \"src/a b.cpp\" \"src/a b.cpp\"\n" +
	"--- src/a b.cpp\t\n" +
	"+++ src/a b.cpp\t\n" +
	"@@ -1 +1 @@\n" +
	"-x\n" +
	"+y\n"

func TestParse(t *testing.T) {
	t.Parallel()

	testErr := errors.New("test error")
	testCases := []struct {
		name    string
		reader  io.Reader
		want    []diff.File
		wantErr error
	}{
		{
			name:    "empty",
			reader:  strings.NewReader(""),
			want:    nil,
			wantErr: nil,
		}, {
			name:   "modified file",
			reader: strings.NewReader(modified),
			want: []diff.File{{
				Path: "src/a.cpp",
				Ranges: []diff.Range{
					{Start: 1, End: 4},
					{Start: 21, End: 22},
				},
			}},
			wantErr: nil,
		}, {
			name:   "added file",
			reader: strings.NewReader(added),
			want: []diff.File{{
				Path:   "src/new.hpp",
				Ranges: []diff.Range{{Start: 1, End: 2}},
			}},
			wantErr: nil,
		}, {
			name:    "deleted file",
			reader:  strings.NewReader(deleted),
			want:    nil,
			wantErr: nil,
		}, {
			name:    "deletion-only hunk",
			reader:  strings.NewReader(deletionOnly),
			want:    []diff.File{{Path: "src/d.cpp", Ranges: nil}},
			wantErr: nil,
		}, {
			name:   "path with spaces",
			reader: strings.NewReader(spaced),
			want: []diff.File{{
				Path:   "src/a b.cpp",
				Ranges: []diff.Range{{Start: 1, End: 1}},
			}},
			wantErr: nil,
		}, {
			name:   "multiple files",
			reader: strings.NewReader(deleted + modified + added),
			want: []diff.File{{
				Path: "src/a.cpp",
				Ranges: []diff.Range{
					{Start: 1, End: 4},
					{Start: 21, End: 22},
				},
			}, {
				Path:   "src/new.hpp",
				Ranges: []diff.Range{{Start: 1, End: 2}},
			}},
			wantErr: nil,
		}, {
			name:    "malformed hunk",
			reader:  strings.NewReader("+++ src/a.cpp\n@@ -1 +x @@\n"),
			want:    nil,
			wantErr: diff.ErrBadHunk,
		}, {
			name:    "hunk without file",
			reader:  strings.NewReader("@@ -1 +1 @@\n-x\n+y\n"),
			want:    nil,
			wantErr: diff.ErrBadHunk,
		}, {
			name:    "read error",
			reader:  iotest.ErrReader(testErr),
			want:    nil,
			wantErr: testErr,
		},
	}

	for _, tc := range testCases {
		t.Run(tc.name, func(t *testing.T) {
			t.Parallel()

			// Act
			files, err := diff.Parse(tc.reader)

			// Assert
			if got, want := err, tc.wantErr; !cmp.Equal(got, want, cmpopts.EquateErrors()) {
				t.Fatalf("Parse() = %v, want %v", got, want)
			}
			if got, want := files, tc.want; !cmp.Equal(got, want) {
				t.Errorf("Parse() mismatch (-got +want):\n%s", cmp.Diff(got, want))
			}
		})
	}
}
