package license_test

import (
	"errors"
	"io"
	"regexp"
	"strings"
	"testing"
	"testing/iotest"

	"github.com/google/go-cmp/cmp"
	"github.com/google/go-cmp/cmp/cmpopts"

	"github.com/bitwizeshift/alloy/tools/internal/check"
	"github.com/bitwizeshift/alloy/tools/internal/license"
)

var rules = []license.Rule{
	{Path: regexp.MustCompile(`(^|/)test/`), Header: "// test header\n"},
	{Path: regexp.MustCompile(`\.cpp$`), Header: "// source header\n"},
}

func TestHeaderCheckerCheck(t *testing.T) {
	t.Parallel()

	testErr := errors.New("test error")
	testCases := []struct {
		name    string
		path    string
		reader  io.Reader
		want    []check.Finding
		wantErr error
	}{
		{
			name:    "header",
			path:    "src/a.cpp",
			reader:  strings.NewReader("// source header\n\nint x;\n"),
			want:    nil,
			wantErr: nil,
		}, {
			name:    "first matching rule",
			path:    "test/a.cpp",
			reader:  strings.NewReader("// test header\nint x;\n"),
			want:    nil,
			wantErr: nil,
		}, {
			name:    "header of another rule",
			path:    "test/a.cpp",
			reader:  strings.NewReader("// source header\nint x;\n"),
			want:    []check.Finding{{Line: 1, Message: "missing license header"}},
			wantErr: nil,
		}, {
			name:    "header after content",
			path:    "src/a.cpp",
			reader:  strings.NewReader("int x;\n// source header\n"),
			want:    []check.Finding{{Line: 1, Message: "missing license header"}},
			wantErr: nil,
		}, {
			name:    "empty",
			path:    "src/a.cpp",
			reader:  strings.NewReader(""),
			want:    []check.Finding{{Line: 1, Message: "missing license header"}},
			wantErr: nil,
		}, {
			name:    "no matching rule",
			path:    "README.md",
			reader:  strings.NewReader("# Alloy\n"),
			want:    nil,
			wantErr: nil,
		}, {
			name:    "read error",
			path:    "src/a.cpp",
			reader:  iotest.ErrReader(testErr),
			want:    nil,
			wantErr: testErr,
		},
	}

	for _, tc := range testCases {
		t.Run(tc.name, func(t *testing.T) {
			t.Parallel()

			// Arrange
			sut := &license.HeaderChecker{Rules: rules}

			// Act
			findings, err := sut.Check(tc.path, tc.reader)

			// Assert
			if got, want := err, tc.wantErr; !cmp.Equal(got, want, cmpopts.EquateErrors()) {
				t.Fatalf("HeaderChecker.Check() = %v, want %v", got, want)
			}
			if got, want := findings, tc.want; !cmp.Equal(got, want) {
				t.Errorf("HeaderChecker.Check() = %v, want %v", got, want)
			}
		})
	}
}
