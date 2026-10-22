package includes

import (
	"errors"
	"io/fs"
	"path"
	"slices"
	"strings"
)

// Libraries returns the paths that the headers of each library in fsys are
// included under, in sorted order. Each directory matched by one of patterns
// is a library, while files that they match are ignored, and each of roots is a directory of a library that its
// sources are included relative to.
//
// The path of a library is the run of directories, below each of its roots,
// that hold nothing but the next directory in the run, as "alloy/core" does
// in lib/alloy-core/include/alloy/core. Where the roots of a library differ,
// the path is the part of the runs that they all share. A library whose
// sources are directly in its roots has no path, and is left out.
//
// An error is returned if a pattern is malformed, or a root cannot be read.
func Libraries(fsys fs.FS, roots []string, patterns ...string) ([]string, error) {
	var libraries []string
	for _, pattern := range patterns {
		dirs, err := fs.Glob(fsys, pattern)
		if err != nil {
			return nil, err
		}
		for _, dir := range dirs {
			info, err := fs.Stat(fsys, dir)
			if err != nil {
				return nil, err
			}
			if !info.IsDir() {
				continue
			}
			library, err := libraryPath(fsys, dir, roots)
			if err != nil {
				return nil, err
			}
			if library != "" {
				libraries = append(libraries, library)
			}
		}
	}
	slices.Sort(libraries)
	return slices.Compact(libraries), nil
}

// libraryPath returns the path of the library in dir, as described by
// [Libraries].
func libraryPath(fsys fs.FS, dir string, roots []string) (string, error) {
	var runs [][]string
	for _, root := range roots {
		run, err := directoryRun(fsys, path.Join(dir, root))
		if errors.Is(err, fs.ErrNotExist) {
			continue
		}
		if err != nil {
			return "", err
		}
		runs = append(runs, run)
	}
	if len(runs) == 0 {
		return "", nil
	}
	shared := runs[0]
	for _, run := range runs[1:] {
		n := 0
		for n < len(shared) && n < len(run) && shared[n] == run[n] {
			n++
		}
		shared = shared[:n]
	}
	return strings.Join(shared, "/"), nil
}

// directoryRun returns the names of the directories below dir that each hold
// nothing but the next.
func directoryRun(fsys fs.FS, dir string) ([]string, error) {
	var run []string
	for {
		entries, err := fs.ReadDir(fsys, dir)
		if err != nil {
			return nil, err
		}
		if len(entries) != 1 || !entries[0].IsDir() {
			return run, nil
		}
		run = append(run, entries[0].Name())
		dir = path.Join(dir, entries[0].Name())
	}
}
