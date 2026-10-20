package check_test

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

func TestRunnerCheck(t *testing.T) {
	t.Parallel()

	testErr := errors.New("test error")
	testCases := []struct {
		name    string
		fs      check.FS
		checker check.Checker
		paths   []string
		want    string
		wantErr error
	}{
		{
			name:    "no findings",
			fs:      checktest.MemFS{"a.cpp": "", "b.cpp": ""},
			checker: checktest.Checker(),
			paths:   []string{"a.cpp", "b.cpp"},
			want:    "",
			wantErr: nil,
		}, {
			name: "findings in every file",
			fs:   checktest.MemFS{"a.cpp": "", "b.cpp": ""},
			checker: checktest.Checker(
				check.Finding{Line: 3, Message: "bad line"},
				check.Finding{Line: 0, Message: "bad file"},
			),
			paths:   []string{"a.cpp", "b.cpp"},
			want:    "a.cpp:3: bad line\na.cpp: bad file\nb.cpp:3: bad line\nb.cpp: bad file\n",
			wantErr: check.ErrFindings,
		}, {
			name:    "missing file",
			fs:      checktest.MemFS{},
			checker: checktest.Checker(),
			paths:   []string{"a.cpp"},
			want:    "",
			wantErr: fs.ErrNotExist,
		}, {
			name:    "checker error",
			fs:      checktest.MemFS{"a.cpp": ""},
			checker: checktest.ErrChecker(testErr),
			paths:   []string{"a.cpp"},
			want:    "",
			wantErr: testErr,
		},
	}

	for _, tc := range testCases {
		t.Run(tc.name, func(t *testing.T) {
			t.Parallel()

			// Arrange
			var out strings.Builder
			sut := &check.Runner{FS: tc.fs, Out: &out}

			// Act
			err := sut.Check(tc.checker, tc.paths...)

			// Assert
			if got, want := err, tc.wantErr; !cmp.Equal(got, want, cmpopts.EquateErrors()) {
				t.Fatalf("Runner.Check() = %v, want %v", got, want)
			}
			if got, want := out.String(), tc.want; !cmp.Equal(got, want) {
				t.Errorf("Runner.Check() output = %q, want %q", got, want)
			}
		})
	}
}

func TestRunnerFix(t *testing.T) {
	t.Parallel()

	testErr := errors.New("test error")
	testCases := []struct {
		name    string
		fs      checktest.MemFS
		fixer   check.Fixer
		paths   []string
		want    string
		wantFS  checktest.MemFS
		wantErr error
	}{
		{
			name:    "changes every file",
			fs:      checktest.MemFS{"a.cpp": "old", "b.cpp": "old"},
			fixer:   checktest.Fixer("new"),
			paths:   []string{"a.cpp", "b.cpp"},
			want:    "a.cpp\nb.cpp\n",
			wantFS:  checktest.MemFS{"a.cpp": "new", "b.cpp": "new"},
			wantErr: nil,
		}, {
			name:    "skips unchanged files",
			fs:      checktest.MemFS{"a.cpp": "new", "b.cpp": "old"},
			fixer:   checktest.Fixer("new"),
			paths:   []string{"a.cpp", "b.cpp"},
			want:    "b.cpp\n",
			wantFS:  checktest.MemFS{"a.cpp": "new", "b.cpp": "new"},
			wantErr: nil,
		}, {
			name:    "missing file",
			fs:      checktest.MemFS{},
			fixer:   checktest.Fixer("new"),
			paths:   []string{"a.cpp"},
			want:    "",
			wantFS:  checktest.MemFS{},
			wantErr: fs.ErrNotExist,
		}, {
			name:    "fixer error",
			fs:      checktest.MemFS{"a.cpp": "old"},
			fixer:   checktest.ErrFixer(testErr),
			paths:   []string{"a.cpp"},
			want:    "",
			wantFS:  checktest.MemFS{"a.cpp": "old"},
			wantErr: testErr,
		},
	}

	for _, tc := range testCases {
		t.Run(tc.name, func(t *testing.T) {
			t.Parallel()

			// Arrange
			var out strings.Builder
			sut := &check.Runner{FS: tc.fs, Out: &out}

			// Act
			err := sut.Fix(tc.fixer, tc.paths...)

			// Assert
			if got, want := err, tc.wantErr; !cmp.Equal(got, want, cmpopts.EquateErrors()) {
				t.Fatalf("Runner.Fix() = %v, want %v", got, want)
			}
			if got, want := out.String(), tc.want; !cmp.Equal(got, want) {
				t.Errorf("Runner.Fix() output = %q, want %q", got, want)
			}
			if got, want := tc.fs, tc.wantFS; !cmp.Equal(got, want, cmpopts.EquateEmpty()) {
				t.Errorf("Runner.Fix() files = %v, want %v", got, want)
			}
		})
	}
}

func TestRunnerFix_WriteFails_ReturnsError(t *testing.T) {
	t.Parallel()

	// Arrange
	testErr := errors.New("test error")
	var out strings.Builder
	sut := &check.Runner{
		FS:  checktest.ReadOnlyFS(map[string]string{"a.cpp": "old"}, testErr),
		Out: &out,
	}
	fixer := checktest.Fixer("new")

	// Act
	err := sut.Fix(fixer, "a.cpp")

	// Assert
	if got, want := err, testErr; !cmp.Equal(got, want, cmpopts.EquateErrors()) {
		t.Fatalf("Runner.Fix() = %v, want %v", got, want)
	}
	if got, want := out.String(), ""; !cmp.Equal(got, want) {
		t.Errorf("Runner.Fix() output = %q, want %q", got, want)
	}
}
