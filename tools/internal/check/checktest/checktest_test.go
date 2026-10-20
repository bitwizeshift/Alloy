package checktest_test

import (
	"errors"
	"io/fs"
	"strings"
	"testing"

	"github.com/google/go-cmp/cmp"
	"github.com/google/go-cmp/cmp/cmpopts"

	"github.com/bitwizeshift/alloy/tools/internal/check"
	"github.com/bitwizeshift/alloy/tools/internal/check/checktest"
)

func TestMemFSReadFile(t *testing.T) {
	t.Parallel()

	testCases := []struct {
		name    string
		fs      checktest.MemFS
		file    string
		want    []byte
		wantErr error
	}{
		{
			name:    "existing file",
			fs:      checktest.MemFS{"a.cpp": "content"},
			file:    "a.cpp",
			want:    []byte("content"),
			wantErr: nil,
		}, {
			name:    "missing file",
			fs:      checktest.MemFS{},
			file:    "a.cpp",
			want:    nil,
			wantErr: fs.ErrNotExist,
		},
	}

	for _, tc := range testCases {
		t.Run(tc.name, func(t *testing.T) {
			t.Parallel()

			// Arrange
			sut := tc.fs

			// Act
			data, err := sut.ReadFile(tc.file)

			// Assert
			if got, want := err, tc.wantErr; !cmp.Equal(got, want, cmpopts.EquateErrors()) {
				t.Fatalf("MemFS.ReadFile() = %v, want %v", got, want)
			}
			if got, want := data, tc.want; !cmp.Equal(got, want) {
				t.Errorf("MemFS.ReadFile() = %q, want %q", got, want)
			}
		})
	}
}

func TestMemFSWriteFile(t *testing.T) {
	t.Parallel()

	testCases := []struct {
		name    string
		fs      checktest.MemFS
		file    string
		data    []byte
		want    checktest.MemFS
		wantErr error
	}{
		{
			name:    "existing file",
			fs:      checktest.MemFS{"a.cpp": "old"},
			file:    "a.cpp",
			data:    []byte("new"),
			want:    checktest.MemFS{"a.cpp": "new"},
			wantErr: nil,
		}, {
			name:    "missing file",
			fs:      checktest.MemFS{},
			file:    "a.cpp",
			data:    []byte("new"),
			want:    checktest.MemFS{},
			wantErr: fs.ErrNotExist,
		},
	}

	for _, tc := range testCases {
		t.Run(tc.name, func(t *testing.T) {
			t.Parallel()

			// Arrange
			sut := tc.fs

			// Act
			err := sut.WriteFile(tc.file, tc.data)

			// Assert
			if got, want := err, tc.wantErr; !cmp.Equal(got, want, cmpopts.EquateErrors()) {
				t.Fatalf("MemFS.WriteFile() = %v, want %v", got, want)
			}
			if got, want := sut, tc.want; !cmp.Equal(got, want, cmpopts.EquateEmpty()) {
				t.Errorf("MemFS.WriteFile() files = %v, want %v", got, want)
			}
		})
	}
}

func TestReadOnlyFS(t *testing.T) {
	t.Parallel()

	// Arrange
	testErr := errors.New("test error")
	sut := checktest.ReadOnlyFS(map[string]string{"a.cpp": "content"}, testErr)

	// Act
	data, readErr := sut.ReadFile("a.cpp")
	writeErr := sut.WriteFile("a.cpp", []byte("new"))

	// Assert
	if got, want := readErr, error(nil); !cmp.Equal(got, want, cmpopts.EquateErrors()) {
		t.Fatalf("ReadOnlyFS().ReadFile() = %v, want %v", got, want)
	}
	if got, want := writeErr, testErr; !cmp.Equal(got, want, cmpopts.EquateErrors()) {
		t.Fatalf("ReadOnlyFS().WriteFile() = %v, want %v", got, want)
	}
	if got, want := data, []byte("content"); !cmp.Equal(got, want) {
		t.Errorf("ReadOnlyFS().ReadFile() = %q, want %q", got, want)
	}
}

func TestChecker(t *testing.T) {
	t.Parallel()

	testErr := errors.New("test error")
	testCases := []struct {
		name    string
		checker check.Checker
		want    []check.Finding
		wantErr error
	}{
		{
			name:    "findings",
			checker: checktest.Checker(check.Finding{Line: 1, Message: "bad"}),
			want:    []check.Finding{{Line: 1, Message: "bad"}},
			wantErr: nil,
		}, {
			name:    "error",
			checker: checktest.ErrChecker(testErr),
			want:    nil,
			wantErr: testErr,
		},
	}

	for _, tc := range testCases {
		t.Run(tc.name, func(t *testing.T) {
			t.Parallel()

			// Arrange
			sut := tc.checker
			r := strings.NewReader("content")

			// Act
			findings, err := sut.Check("a.cpp", r)

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

func TestFixer(t *testing.T) {
	t.Parallel()

	testErr := errors.New("test error")
	testCases := []struct {
		name    string
		fixer   check.Fixer
		want    string
		wantErr error
	}{
		{
			name:    "replaces content",
			fixer:   checktest.Fixer("new"),
			want:    "new",
			wantErr: nil,
		}, {
			name:    "error",
			fixer:   checktest.ErrFixer(testErr),
			want:    "",
			wantErr: testErr,
		},
	}

	for _, tc := range testCases {
		t.Run(tc.name, func(t *testing.T) {
			t.Parallel()

			// Arrange
			sut := tc.fixer
			r := strings.NewReader("old")
			var w strings.Builder

			// Act
			err := sut.Fix("a.cpp", r, &w)

			// Assert
			if got, want := err, tc.wantErr; !cmp.Equal(got, want, cmpopts.EquateErrors()) {
				t.Fatalf("Fixer.Fix() = %v, want %v", got, want)
			}
			if got, want := w.String(), tc.want; !cmp.Equal(got, want) {
				t.Errorf("Fixer.Fix() output = %q, want %q", got, want)
			}
		})
	}
}
