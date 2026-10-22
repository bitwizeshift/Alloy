package cmakelists

import (
	"errors"
	"fmt"
	"io"
	"io/fs"
	"path"
	"slices"
	"strings"

	"github.com/bitwizeshift/alloy/tools/internal/check"
)

// Name is the name of the file that lists the sources of a directory.
const Name = "CMakeLists.txt"

// prefixes are the variables that an argument may start a path relative to
// the directory of its CMakeLists.txt with.
var prefixes = []string{
	"${CMAKE_CURRENT_SOURCE_DIR}/",
	"${CMAKE_CURRENT_LIST_DIR}/",
}

// Checker reports files that are not listed in the CMakeLists.txt that
// builds them.
type Checker struct {
	// FS is the filesystem that the CMakeLists.txt files are read from, rooted
	// at the directory that file paths are relative to.
	FS fs.FS
}

// Check returns a single [check.Finding] for the whole file if the nearest
// CMakeLists.txt in the directory of file or above it has no argument naming
// file, either by its path relative to that directory or prefixed with
// ${CMAKE_CURRENT_SOURCE_DIR}/ or ${CMAKE_CURRENT_LIST_DIR}/. The contents of
// the file are not read.
//
// An error is returned if file is not a valid path in FS, or a CMakeLists.txt
// cannot be read.
func (c *Checker) Check(file string, _ io.Reader) ([]check.Finding, error) {
	if !fs.ValidPath(file) {
		return nil, &fs.PathError{Op: "check", Path: file, Err: fs.ErrInvalid}
	}
	list, data, err := c.nearest(file)
	if errors.Is(err, fs.ErrNotExist) {
		return []check.Finding{{Message: "not built by any " + Name}}, nil
	}
	if err != nil {
		return nil, err
	}
	if c.lists(Arguments(string(data)), c.relative(file, list)) {
		return nil, nil
	}
	return []check.Finding{{Message: fmt.Sprintf("not listed in %s", list)}}, nil
}

var _ check.Checker = (*Checker)(nil)

// nearest returns the path and contents of the CMakeLists.txt closest to
// file, searching from its directory up to the root of FS.
//
// An error wrapping [fs.ErrNotExist] is returned if there is none.
func (c *Checker) nearest(file string) (string, []byte, error) {
	dir := path.Dir(file)
	for {
		list := path.Join(dir, Name)
		data, err := fs.ReadFile(c.FS, list)
		if err == nil {
			return list, data, nil
		}
		if !errors.Is(err, fs.ErrNotExist) || dir == "." {
			return "", nil, err
		}
		dir = path.Dir(dir)
	}
}

// relative returns the path of file relative to the directory of list.
func (*Checker) relative(file, list string) string {
	dir := path.Dir(list)
	if dir == "." {
		return file
	}
	return strings.TrimPrefix(file, dir+"/")
}

// lists reports whether one of args names the file at rel.
func (*Checker) lists(args []string, rel string) bool {
	return slices.ContainsFunc(args, func(arg string) bool {
		for _, prefix := range prefixes {
			arg = strings.TrimPrefix(arg, prefix)
		}
		return arg == rel
	})
}
