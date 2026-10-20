package checktest

import (
	"io"
	"io/fs"

	"github.com/bitwizeshift/alloy/tools/internal/check"
)

// MemFS is an in-memory [check.FS] that maps file names to their contents.
type MemFS map[string]string

// ReadFile returns the contents of the file name, or an error wrapping
// [fs.ErrNotExist] if there is no such file.
func (m MemFS) ReadFile(name string) ([]byte, error) {
	content, ok := m[name]
	if !ok {
		return nil, &fs.PathError{Op: "open", Path: name, Err: fs.ErrNotExist}
	}
	return []byte(content), nil
}

// WriteFile replaces the contents of the file name with data, or returns an
// error wrapping [fs.ErrNotExist] if there is no such file.
func (m MemFS) WriteFile(name string, data []byte) error {
	if _, ok := m[name]; !ok {
		return &fs.PathError{Op: "open", Path: name, Err: fs.ErrNotExist}
	}
	m[name] = string(data)
	return nil
}

var _ check.FS = MemFS(nil)

// ReadOnlyFS returns a [check.FS] that reads the given files, and fails every
// write with err.
func ReadOnlyFS(files map[string]string, err error) check.FS {
	return &readOnlyFS{files: MemFS(files), err: err}
}

type readOnlyFS struct {
	files MemFS
	err   error
}

func (r *readOnlyFS) ReadFile(name string) ([]byte, error) {
	return r.files.ReadFile(name)
}

func (r *readOnlyFS) WriteFile(string, []byte) error {
	return r.err
}

// Checker returns a [check.Checker] that reports findings for every file.
func Checker(findings ...check.Finding) check.Checker {
	return checker(func(string, io.Reader) ([]check.Finding, error) {
		return findings, nil
	})
}

// ErrChecker returns a [check.Checker] that fails every file with err.
func ErrChecker(err error) check.Checker {
	return checker(func(string, io.Reader) ([]check.Finding, error) {
		return nil, err
	})
}

type checker func(path string, r io.Reader) ([]check.Finding, error)

func (c checker) Check(path string, r io.Reader) ([]check.Finding, error) {
	return c(path, r)
}

// Fixer returns a [check.Fixer] that replaces the contents of every file with
// content.
func Fixer(content string) check.Fixer {
	return fixer(func(_ string, _ io.Reader, w io.Writer) error {
		_, err := io.WriteString(w, content)
		return err
	})
}

// ErrFixer returns a [check.Fixer] that fails every file with err.
func ErrFixer(err error) check.Fixer {
	return fixer(func(string, io.Reader, io.Writer) error {
		return err
	})
}

type fixer func(path string, r io.Reader, w io.Writer) error

func (f fixer) Fix(path string, r io.Reader, w io.Writer) error {
	return f(path, r, w)
}
