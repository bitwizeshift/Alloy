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
	libraryGroup
	otherLibraryGroup
	thirdPartyGroup
	standardGroup
)

func (g group) String() string {
	switch g {
	case ownGroup:
		return "own header"
	case libraryGroup:
		return "same library"
	case otherLibraryGroup:
		return "another library"
	case thirdPartyGroup:
		return "third-party"
	}
	return "standard library"
}

// origin is the library that an included header comes from. Includes from
// the same origin are kept together, and includes from different ones are
// separated by a blank line.
type origin struct {
	group group

	// library is the library or third-party library of the header, and is
	// empty for the header of the source file itself, the standard library,
	// and third-party headers that are in no directory.
	library string
}

func (o origin) String() string {
	if o.library != "" {
		return o.library
	}
	return o.group.String()
}

// Checker reports and fixes C and C++ files whose includes do not follow the
// layout:
//
//	#include "alloy/core/math/quaternion.hpp"
//
//	#include "alloy/core/assert.hpp"
//	#include "alloy/core/math/math.hpp"
//	#include "alloy/core/math/trigonometry.hpp"
//
//	#include "alloy/platform/window.hpp"
//
//	#include "alloy/render/device.hpp"
//
//	#include <SDL3/SDL.h>
//
//	#include <cstdint>
//	#include <vector>
//
// The includes are grouped by where their headers come from: the header of the
// source file itself, the same library, another library, a third-party
// library, and the standard library. The groups are in that order, and the
// includes of each group are sorted by library and then by path, except that
// headers included relative to the file come first in their library. The
// library of a third-party header is the first directory of its path.
// Third-party headers that are in no directory, such as <Windows.h>, are taken
// to be from the same library, as their paths do not tell them apart.
//
// A blank line separates the header of the source file from the rest, and the
// includes of each library from those of the next.
//
// Library headers are included with quotes from within a library, and with
// angle brackets from other targets, such as tests. Every other header is
// included with angle brackets, except for those included with quotes relative
// to the file that includes them, which are taken to be from the same
// library.
//
// A header may be included only once.
//
// Includes within conditional blocks are not checked, and a conditional block
// may separate two groups in place of the blank line.
type Checker struct {
	// Roots are the names of the directories that sources are included
	// relative to. A file is placed in its library by the part of its path
	// after the first of these directories.
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
// as its group requires, is delimited with the wrong quotes, or includes a
// header that is already included. An error is returned only if r cannot be
// read.
func (c *Checker) Check(path string, r io.Reader) ([]check.Finding, error) {
	lines, err := readLines(r)
	if err != nil {
		return nil, err
	}
	f := c.file(path)
	s := parseSection(lines)
	var findings []check.Finding
	var prev *directive
	seen := map[string]*directive{}
	for _, d := range s.directives() {
		if first, ok := seen[f.resolve(d)]; ok {
			findings = append(findings, finding(d, "%v is already included on line %d", d, first.line+1))
			continue
		}
		seen[f.resolve(d)] = d
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
	po, o := f.origin(prev), f.origin(d)
	switch {
	case f.compareDirectives(prev, d) > 0:
		if pg == g {
			return []check.Finding{finding(d, "%v must be included before %v", d, prev)}
		}
		return []check.Finding{finding(d, "%v (%v) must be included before %v (%v)", d, g, prev, pg)}
	case po == o && d.line != prev.line+1:
		return []check.Finding{finding(d, "%v must directly follow %v (both %v)", d, prev, o)}
	case po != o && !s.hasConditional(prev.line+1, d.line) && !isSeparator(lines[prev.line+1:d.line]):
		return []check.Finding{finding(d, "%v (%v) must be separated from %v (%v) by a blank line", d, o, prev, po)}
	}
	return nil
}

// compareDirectives orders the directives lhs and rhs by group, then by the
// library of their origin, then with relative includes first, and then by
// path.
func (f *file) compareDirectives(lhs, rhs *directive) int {
	return cmp.Or(
		cmp.Compare(f.group(lhs), f.group(rhs)),
		strings.Compare(f.origin(lhs).library, f.origin(rhs).library),
		compareBool(f.isRelative(rhs), f.isRelative(lhs)),
		strings.Compare(lhs.path, rhs.path),
	)
}

// compareBool orders false before true.
func compareBool(lhs, rhs bool) int {
	switch {
	case lhs == rhs:
		return 0
	case lhs:
		return 1
	}
	return -1
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
	if !f.isRelative(d) {
		return d.path
	}
	return path.Join(path.Dir(f.path), d.path)
}

// isRelative reports whether d includes a header relative to the file.
func (f *file) isRelative(d *directive) bool {
	return !d.angled && f.libraryOf(d.path) == ""
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
		return libraryGroup
	case library != "":
		return otherLibraryGroup
	case !d.angled:
		return libraryGroup
	case standard[d.path]:
		return standardGroup
	}
	return thirdPartyGroup
}

func (f *file) origin(d *directive) origin {
	switch g := f.group(d); g {
	case libraryGroup:
		return origin{group: g, library: f.library}
	case otherLibraryGroup:
		return origin{group: g, library: f.libraryOf(f.resolve(d))}
	case thirdPartyGroup:
		library, _, found := strings.Cut(d.path, "/")
		if !found {
			return origin{group: g}
		}
		return origin{group: g, library: library}
	default:
		return origin{group: g}
	}
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
// blocks anchored to it. Only the first include of each header is kept.
func (f *file) render(s *section, lines []string) []string {
	type entry struct {
		group group
		item  item
	}
	var includes, blocks []entry
	anchor := ownGroup
	seen := map[string]bool{}
	for _, it := range s.items {
		if it.include == nil {
			blocks = append(blocks, entry{group: anchor, item: it})
			continue
		}
		header := f.resolve(it.include)
		if seen[header] {
			continue
		}
		seen[header] = true
		g := f.group(it.include)
		anchor = max(anchor, g)
		includes = append(includes, entry{group: g, item: it})
	}
	slices.SortStableFunc(includes, func(lhs, rhs entry) int {
		return f.compareDirectives(lhs.item.include, rhs.item.include)
	})

	// A chunk is either the includes of one origin, or a conditional block.
	type chunk struct {
		origin     origin
		directives []*directive
		block      []string
	}
	var chunks []chunk
	for g := ownGroup; g <= standardGroup; g++ {
		for _, e := range includes {
			if e.group != g {
				continue
			}
			o := f.origin(e.item.include)
			if n := len(chunks); n > 0 && chunks[n-1].block == nil && chunks[n-1].origin == o {
				chunks[n-1].directives = append(chunks[n-1].directives, e.item.include)
				continue
			}
			chunks = append(chunks, chunk{origin: o, directives: []*directive{e.item.include}})
		}
		for _, e := range blocks {
			if e.group == g {
				chunks = append(chunks, chunk{block: lines[e.item.start:e.item.end]})
			}
		}
	}
	var rendered []string
	for i, c := range chunks {
		if i > 0 {
			rendered = append(rendered, "")
		}
		if c.block != nil {
			rendered = append(rendered, c.block...)
			continue
		}
		rendered = append(rendered, f.renderGroup(c.directives)...)
	}
	return rendered
}

// renderGroup returns the lines of the sorted directives of an origin. If the
// directives are not already on consecutive lines in this order, the trailing
// comments of each run of consecutive directives that have one are aligned to
// the same column, as moving them leaves them misaligned.
func (f *file) renderGroup(directives []*directive) []string {
	inPlace := true
	rendered := make([]string, len(directives))
	for i, d := range directives {
		inPlace = inPlace && (i == 0 || d.line == directives[i-1].line+1)
		rendered[i] = d.text(f.angled(d))
	}
	if inPlace {
		return rendered
	}
	for start := 0; start < len(directives); start++ {
		end, width := start, 0
		for ; end < len(directives) && trailingComment(directives[end]) != ""; end++ {
			d := directives[end]
			width = max(width, len(d.prefix)+len(d.spelling(f.angled(d))))
		}
		for i := start; i < end; i++ {
			d := directives[i]
			head := d.prefix + d.spelling(f.angled(d))
			rendered[i] = head + strings.Repeat(" ", width-len(head)+1) + trailingComment(d)
		}
		start = end
	}
	return rendered
}

// trailingComment returns the // comment after the path of d, or an empty
// string if it has none.
func trailingComment(d *directive) string {
	comment := strings.TrimLeft(d.suffix, " \t")
	if !strings.HasPrefix(comment, "//") {
		return ""
	}
	return comment
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
