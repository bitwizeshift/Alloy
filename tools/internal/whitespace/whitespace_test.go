package whitespace_test

import (
	"errors"
	"io"
	"strings"
	"testing"
	"testing/iotest"

	"github.com/google/go-cmp/cmp"
	"github.com/google/go-cmp/cmp/cmpopts"

	"github.com/bitwizeshift/alloy/tools/internal/check"
	"github.com/bitwizeshift/alloy/tools/internal/whitespace"
)

func TestCheckerCheck(t *testing.T) {
	t.Parallel()

	testErr := errors.New("test error")
	testCases := []struct {
		name    string
		reader  io.Reader
		want    []check.Finding
		wantErr error
	}{
		{
			name:    "empty",
			reader:  strings.NewReader(""),
			want:    nil,
			wantErr: nil,
		}, {
			name:    "clean lines",
			reader:  strings.NewReader("int x;\n  int y;\n"),
			want:    nil,
			wantErr: nil,
		}, {
			name:    "crlf endings",
			reader:  strings.NewReader("int x;\r\nint y;\r\n"),
			want:    nil,
			wantErr: nil,
		}, {
			name:   "trailing spaces and tabs",
			reader: strings.NewReader("int x; \nint y;\nint z;\t\n"),
			want: []check.Finding{
				{Line: 1, Message: "trailing whitespace"},
				{Line: 3, Message: "trailing whitespace"},
			},
			wantErr: nil,
		}, {
			name:   "whitespace-only line",
			reader: strings.NewReader("int x;\n   \nint y;\n"),
			want: []check.Finding{
				{Line: 2, Message: "trailing whitespace"},
			},
			wantErr: nil,
		}, {
			name:   "final line without newline",
			reader: strings.NewReader("int x;\nint y; "),
			want: []check.Finding{
				{Line: 2, Message: "trailing whitespace"},
			},
			wantErr: nil,
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

			// Arrange
			sut := whitespace.Checker{}

			// Act
			findings, err := sut.Check("a.cpp", tc.reader)

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

func TestCheckerFix(t *testing.T) {
	t.Parallel()

	testErr := errors.New("test error")
	testCases := []struct {
		name    string
		reader  io.Reader
		want    string
		wantErr error
	}{
		{
			name:    "empty",
			reader:  strings.NewReader(""),
			want:    "",
			wantErr: nil,
		}, {
			name:    "trailing spaces and tabs",
			reader:  strings.NewReader("int x; \nint y;\nint z;\t\n"),
			want:    "int x;\nint y;\nint z;\n",
			wantErr: nil,
		}, {
			name:    "preserves crlf endings",
			reader:  strings.NewReader("int x; \r\nint y;\r\n"),
			want:    "int x;\r\nint y;\r\n",
			wantErr: nil,
		}, {
			name:    "final line without newline",
			reader:  strings.NewReader("int x;\nint y; "),
			want:    "int x;\nint y;",
			wantErr: nil,
		}, {
			name:    "read error",
			reader:  iotest.ErrReader(testErr),
			want:    "",
			wantErr: testErr,
		},
	}

	for _, tc := range testCases {
		t.Run(tc.name, func(t *testing.T) {
			t.Parallel()

			// Arrange
			sut := whitespace.Checker{}
			var w strings.Builder

			// Act
			err := sut.Fix("a.cpp", tc.reader, &w)

			// Assert
			if got, want := err, tc.wantErr; !cmp.Equal(got, want, cmpopts.EquateErrors()) {
				t.Fatalf("Checker.Fix() = %v, want %v", got, want)
			}
			if got, want := w.String(), tc.want; !cmp.Equal(got, want) {
				t.Errorf("Checker.Fix() output = %q, want %q", got, want)
			}
		})
	}
}

type errWriter struct {
	err error
}

func (ew errWriter) Write([]byte) (int, error) {
	return 0, ew.err
}

func TestCheckerFix_WriteFails_ReturnsError(t *testing.T) {
	t.Parallel()

	// Arrange
	testErr := errors.New("test error")
	sut := whitespace.Checker{}
	r := strings.NewReader("int x; \n")
	w := errWriter{err: testErr}

	// Act
	err := sut.Fix("a.cpp", r, w)

	// Assert
	if got, want := err, testErr; !cmp.Equal(got, want, cmpopts.EquateErrors()) {
		t.Fatalf("Checker.Fix() = %v, want %v", got, want)
	}
}
