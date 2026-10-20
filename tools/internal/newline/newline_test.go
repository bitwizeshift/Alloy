package newline_test

import (
	"errors"
	"io"
	"strings"
	"testing"
	"testing/iotest"

	"github.com/google/go-cmp/cmp"
	"github.com/google/go-cmp/cmp/cmpopts"

	"github.com/bitwizeshift/alloy/tools/internal/check"
	"github.com/bitwizeshift/alloy/tools/internal/newline"
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
			name:    "ends with newline",
			reader:  strings.NewReader("int x;\n"),
			want:    nil,
			wantErr: nil,
		}, {
			name:    "missing newline",
			reader:  strings.NewReader("int x;\nint y;"),
			want:    []check.Finding{{Line: 0, Message: "missing trailing newline"}},
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
			sut := newline.Checker{}

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
			name:    "ends with newline",
			reader:  strings.NewReader("int x;\n"),
			want:    "int x;\n",
			wantErr: nil,
		}, {
			name:    "missing newline",
			reader:  strings.NewReader("int x;\nint y;"),
			want:    "int x;\nint y;\n",
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
			sut := newline.Checker{}
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
