package copyright

import (
	"bytes"
	"fmt"
	"os/exec"
)

// History reports the years in which files were modified.
type History interface {
	// Years returns the years in which the file at path was modified, in any
	// order. A file without any history has no years.
	Years(path string) ([]int, error)
}

// Git is a [History] that reads the commit dates of a file from the Git log of
// the repository containing Dir, following the file across renames.
type Git struct {
	// Dir is the directory git is run in, which relative paths are resolved
	// against. The working directory is used when Dir is empty.
	Dir string
}

// Years returns the year of every commit that modified the file at path.
//
// An error is returned if git cannot be run, or if its output cannot be parsed
// with [ParseYears].
func (g Git) Years(path string) ([]int, error) {
	cmd := exec.Command(
		"git", "log",
		"--date=format:%Y",
		"--pretty=%ad",
		"--follow",
		"--no-show-signature",
		"--", path,
	)
	cmd.Dir = g.Dir
	out, err := cmd.Output()
	if err != nil {
		return nil, fmt.Errorf("git log %s: %w", path, err)
	}
	return ParseYears(bytes.NewReader(out))
}

var _ History = Git{}
