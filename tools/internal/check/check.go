package check

import (
	"bytes"
	"errors"
	"fmt"
	"io"
	"os"
	"path/filepath"
)

// ErrFindings is returned by [Runner.Check] when at least one file has a
// [Finding].
var ErrFindings = errors.New("check found problems")

// Finding is a single problem detected in a file.
type Finding struct {
	// Line is the 1-based line number of the problem, or 0 if the problem
	// applies to the file as a whole.
	Line int

	// Message describes the problem.
	Message string
}

// Checker detects problems in the contents of a file.
type Checker interface {
	// Check reads the contents of the file at path from r, and returns a
	// [Finding] for each problem found. An error is returned only if the
	// contents could not be checked.
	Check(path string, r io.Reader) ([]Finding, error)
}

// Fixer rewrites the contents of a file to resolve the problems that its
// paired [Checker] detects.
type Fixer interface {
	// Fix reads the contents of the file at path from r, and writes the fixed
	// contents to w.
	Fix(path string, r io.Reader, w io.Writer) error
}

// CheckFixer is a [Checker] that is also a [Fixer].
type CheckFixer interface {
	Checker
	Fixer
}

// FS reads and writes whole files.
type FS interface {
	// ReadFile returns the contents of the file name.
	ReadFile(name string) ([]byte, error)

	// WriteFile replaces the contents of the existing file name with data.
	WriteFile(name string, data []byte) error
}

// OS is an [FS] backed by the operating system's filesystem.
type OS struct {
	// Dir is the directory that relative names are resolved against. The
	// working directory is used when Dir is empty.
	Dir string
}

// ReadFile returns the contents of the file name.
func (o OS) ReadFile(name string) ([]byte, error) {
	return os.ReadFile(o.path(name))
}

// WriteFile replaces the contents of the existing file name with data,
// preserving its permissions.
func (o OS) WriteFile(name string, data []byte) error {
	path := o.path(name)
	info, err := os.Stat(path)
	if err != nil {
		return err
	}
	return os.WriteFile(path, data, info.Mode().Perm())
}

func (o OS) path(name string) string {
	if filepath.IsAbs(name) {
		return name
	}
	return filepath.Join(o.Dir, name)
}

var _ FS = OS{}

// Runner applies a check to a list of files.
type Runner struct {
	// FS is the filesystem that files are read from and written to.
	FS FS

	// Out receives one line per [Finding], or per fixed file.
	Out io.Writer
}

// Check runs c on each of paths, and writes each [Finding] to Out in the form
// "path:line: message", or "path: message" for a finding without a line.
//
// [ErrFindings] is returned if any finding was reported. Any other error is
// returned if a file could not be read or checked, in which case the remaining
// files are not checked.
func (r *Runner) Check(c Checker, paths ...string) error {
	found := false
	for _, path := range paths {
		findings, err := r.check(c, path)
		if err != nil {
			return err
		}
		for _, f := range findings {
			r.report(path, f)
		}
		found = found || len(findings) > 0
	}
	if found {
		return ErrFindings
	}
	return nil
}

// Fix runs f on each of paths, writing back only the files whose contents
// change, and writes the path of each changed file to Out.
//
// An error is returned if a file could not be read, fixed, or written, in
// which case the remaining files are not fixed.
func (r *Runner) Fix(f Fixer, paths ...string) error {
	for _, path := range paths {
		changed, err := r.fix(f, path)
		if err != nil {
			return err
		}
		if changed {
			fmt.Fprintln(r.Out, path)
		}
	}
	return nil
}

func (r *Runner) check(c Checker, path string) ([]Finding, error) {
	data, err := r.FS.ReadFile(path)
	if err != nil {
		return nil, err
	}
	findings, err := c.Check(path, bytes.NewReader(data))
	if err != nil {
		return nil, fmt.Errorf("check %s: %w", path, err)
	}
	return findings, nil
}

func (r *Runner) fix(f Fixer, path string) (bool, error) {
	data, err := r.FS.ReadFile(path)
	if err != nil {
		return false, err
	}
	var fixed bytes.Buffer
	if err := f.Fix(path, bytes.NewReader(data), &fixed); err != nil {
		return false, fmt.Errorf("fix %s: %w", path, err)
	}
	if bytes.Equal(data, fixed.Bytes()) {
		return false, nil
	}
	if err := r.FS.WriteFile(path, fixed.Bytes()); err != nil {
		return false, err
	}
	return true, nil
}

func (r *Runner) report(path string, f Finding) {
	if f.Line == 0 {
		fmt.Fprintf(r.Out, "%s: %s\n", path, f.Message)
		return
	}
	fmt.Fprintf(r.Out, "%s:%d: %s\n", path, f.Line, f.Message)
}
