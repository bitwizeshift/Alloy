package includes

import (
	"cmp"
	"fmt"
	"io"
	"path"
	"slices"
	"strings"

	"github.com/bitwizeshift/alloy/tools/internal/check"
)

// group is where an included header comes from, relative to the file that
// includes it. The includes of a file are ordered by group.
type group int

const (
	ownGroup group = iota
	subsystemGroup
	libraryGroup
	otherLibraryGroup
	thirdPartyGroup
	standardGroup
)

func (g group) String() string {
	switch g {
	case ownGroup:
		return "own header"
	case subsystemGroup:
		return "same subsystem"
	case libraryGroup:
		return "same library"
	case otherLibraryGroup:
		return "another library"
	case thirdPartyGroup:
		return "third-party"
	}
	return "standard library"
}

// Checker reports and fixes C and C++ files whose includes do not follow the
// layout:
//
//	#include "alloy/core/math/quaternion.hpp"
//
//	#include "alloy/core/math/math.hpp"
//	#include "alloy/core/math/trigonometry.hpp"
//
//	#include "alloy/core/assert.hpp"
//
//	#include "alloy/platform/window.hpp"
//
//	#include <SDL3/SDL.h>
//
//	#include <cstdint>
//	#include <vector>
//
// The includes are grouped by where their headers come from: the header of the
// source file itself, the same subsystem, another subsystem of the same
// library, another library, a third-party library, and the standard library.
// The groups are in that order and separated by a blank line, and the includes
// of each group are sorted by path. The subsystem of a header is the directory
// below its library that it is in.
//
// Library headers are included with quotes from within a library, and with
// angle brackets from other targets, such as tests. Every other header is
// included with angle brackets, except for those included with quotes relative
// to the file that includes them, which are taken to be from the same
// subsystem.
//
// Includes within conditional blocks are not checked, and a conditional block
// may separate two groups in place of the blank line.
type Checker struct {
	// Roots are the names of the directories that sources are included
	// relative to. A file is placed in its library and subsystem by the part of
	// its path after the first of these directories.
	Roots []string

	// Targets are the names of the directories that hold targets other than
	// libraries, such as tests and examples. A file under any of these
	// includes library headers with angle brackets.
	Targets []string

	// Libraries are the paths that the headers of each library are included
	// under, such as "alloy/core".
	Libraries []string
}

// Check returns a [check.Finding] for each include of the file at path, read
// from r, that is out of order, is not separated from the includes around it
// as its group requires, or is delimited with the wrong quotes. An error is
// returned only if r cannot be read.
func (c *Checker) Check(path string, r io.Reader) ([]check.Finding, error) {
	lines, err := readLines(r)
	if err != nil {
		return nil, err
	}
	f := c.file(path)
	s := parseSection(lines)
	var findings []check.Finding
	var prev *directive
	for _, d := range s.directives() {
		if f.angled(d) != d.angled {
			findings = append(findings, finding(d, "%v must be included as %s", d, d.spelling(!d.angled)))
		}
		if prev != nil {
			findings = append(findings, f.compare(s, lines, prev, d)...)
		}
		prev = d
	}
	return findings, nil
}

// Fix copies r to w with the includes of the file at path grouped, sorted, and
// delimited as [Checker.Check] requires. Conditional blocks follow the group
// of the last include above them. A file with a comment between its includes
// is copied as is, since there is no telling which includes the comment
// describes. An error is returned if r cannot be read, or w cannot be written.
func (c *Checker) Fix(path string, r io.Reader, w io.Writer) error {
	lines, err := readLines(r)
	if err != nil {
		return err
	}
	f := c.file(path)
	s := parseSection(lines)
	if len(s.items) > 0 && !s.comments {
		lines = slices.Concat(lines[:s.start], f.render(s, lines), lines[s.end:])
	}
	_, err = io.WriteString(w, strings.Join(lines, "\n"))
	return err
}

var _ check.CheckFixer = (*Checker)(nil)

// compare returns the findings for the include d that follows the include
// prev.
func (f *file) compare(s *section, lines []string, prev, d *directive) []check.Finding {
	pg, g := f.group(prev), f.group(d)
	switch {
	case pg > g || pg == g && prev.path > d.path:
		if pg == g {
			return []check.Finding{finding(d, "%v must be included before %v", d, prev)}
		}
		return []check.Finding{finding(d, "%v (%v) must be included before %v (%v)", d, g, prev, pg)}
	case pg == g && d.line != prev.line+1:
		return []check.Finding{finding(d, "%v must directly follow %v (both %v)", d, prev, g)}
	case pg != g && !s.hasConditional(prev.line+1, d.line) && !isSeparator(lines[prev.line+1:d.line]):
		return []check.Finding{finding(d, "%v (%v) must be separated from %v (%v) by a blank line", d, g, prev, pg)}
	}
	return nil
}

func isSeparator(lines []string) bool {
	return len(lines) == 1 && strings.TrimSpace(lines[0]) == ""
}

func finding(d *directive, format string, args ...any) check.Finding {
	return check.Finding{Line: d.line + 1, Message: fmt.Sprintf(format, args...)}
}

// file is a file whose includes are checked, as placed in the repository.
type file struct {
	// path is the path of the file relative to its root.
	path string

	// library is the library that the file is part of, and is empty if it is
	// part of none.
	library string

	// target reports whether the file is part of a target other than a
	// library.
	target bool

	// source reports whether the file is a source file, rather than a header.
	source bool

	libraries []string
}

func (c *Checker) file(p string) *file {
	parts := strings.Split(p, "/")
	f := &file{
		path:      p,
		source:    slices.Contains([]string{".c", ".cpp"}, path.Ext(p)),
		libraries: c.Libraries,
	}
	for i, part := range parts[:len(parts)-1] {
		f.target = f.target || slices.Contains(c.Targets, part)
		if slices.Contains(c.Roots, part) {
			f.path = strings.Join(parts[i+1:], "/")
			break
		}
	}
	f.library = f.libraryOf(f.path)
	return f
}

// libraryOf returns the library that the header at p is included from, or an
// empty string if it is from none.
func (f *file) libraryOf(p string) string {
	library := ""
	for _, l := range f.libraries {
		if (p == l || strings.HasPrefix(p, l+"/")) && len(l) > len(library) {
			library = l
		}
	}
	return library
}

// resolve returns the path of the header that d includes, relative to the
// roots.
func (f *file) resolve(d *directive) string {
	if d.angled || f.libraryOf(d.path) != "" {
		return d.path
	}
	return path.Join(path.Dir(f.path), d.path)
}

// angled reports whether d must be delimited with angle brackets.
func (f *file) angled(d *directive) bool {
	if f.libraryOf(d.path) != "" {
		return f.target
	}
	return d.angled
}

func (f *file) group(d *directive) group {
	header := f.resolve(d)
	library := f.libraryOf(header)
	switch {
	case f.isOwn(header):
		return ownGroup
	case library != "" && library == f.library:
		if subsystem(header, library) == subsystem(f.path, library) {
			return subsystemGroup
		}
		return libraryGroup
	case library != "":
		return otherLibraryGroup
	case !d.angled:
		return subsystemGroup
	case standard[d.path]:
		return standardGroup
	}
	return thirdPartyGroup
}

// isOwn reports whether header is the header of the source file: the header
// in the same directory with the same name, up to its first dot. The header of
// "quaternion.cpp" and "quaternion.test.cpp" is "quaternion.hpp".
func (f *file) isOwn(header string) bool {
	if !f.source || path.Dir(header) != path.Dir(f.path) {
		return false
	}
	if ext := path.Ext(header); ext != ".h" && ext != ".hpp" {
		return false
	}
	return stem(header) == stem(f.path)
}

// render returns the lines of the section s, with its includes grouped,
// sorted, and delimited as required, each group followed by the conditional
// blocks anchored to it.
func (f *file) render(s *section, lines []string) []string {
	type entry struct {
		group group
		item  item
	}
	var includes, blocks []entry
	anchor := ownGroup
	for _, it := range s.items {
		if it.include == nil {
			blocks = append(blocks, entry{group: anchor, item: it})
			continue
		}
		g := f.group(it.include)
		anchor = max(anchor, g)
		includes = append(includes, entry{group: g, item: it})
	}
	slices.SortStableFunc(includes, func(lhs, rhs entry) int {
		return cmp.Or(cmp.Compare(lhs.group, rhs.group), strings.Compare(lhs.item.include.path, rhs.item.include.path))
	})

	var chunks [][]string
	for g := ownGroup; g <= standardGroup; g++ {
		var members []*directive
		for _, e := range includes {
			if e.group == g {
				members = append(members, e.item.include)
			}
		}
		if len(members) > 0 {
			chunks = append(chunks, f.renderGroup(members))
		}
		for _, e := range blocks {
			if e.group == g {
				chunks = append(chunks, lines[e.item.start:e.item.end])
			}
		}
	}
	var rendered []string
	for i, chunk := range chunks {
		if i > 0 {
			rendered = append(rendered, "")
		}
		rendered = append(rendered, chunk...)
	}
	return rendered
}

// renderGroup returns the lines of the sorted directives of a group. If the
// directives are not already on consecutive lines in this order, their
// trailing comments are aligned to the same column, as moving them leaves
// them misaligned.
func (f *file) renderGroup(directives []*directive) []string {
	inPlace := true
	width := 0
	for i, d := range directives {
		inPlace = inPlace && (i == 0 || d.line == directives[i-1].line+1)
		width = max(width, len(d.prefix)+len(d.spelling(f.angled(d))))
	}
	rendered := make([]string, len(directives))
	for i, d := range directives {
		angled := f.angled(d)
		comment := strings.TrimLeft(d.suffix, " \t")
		if inPlace || !strings.HasPrefix(comment, "//") {
			rendered[i] = d.text(angled)
			continue
		}
		head := d.prefix + d.spelling(angled)
		rendered[i] = head + strings.Repeat(" ", width-len(head)+1) + comment
	}
	return rendered
}

// subsystem returns the subsystem of the header at p in library: the first
// directory below the library, or an empty string for a header directly in
// it.
func subsystem(p, library string) string {
	rest := strings.TrimPrefix(p, library+"/")
	dir, _, found := strings.Cut(rest, "/")
	if !found {
		return ""
	}
	return dir
}

func stem(p string) string {
	name, _, _ := strings.Cut(path.Base(p), ".")
	return name
}

func readLines(r io.Reader) ([]string, error) {
	data, err := io.ReadAll(r)
	if err != nil {
		return nil, err
	}
	return strings.Split(string(data), "\n"), nil
}
