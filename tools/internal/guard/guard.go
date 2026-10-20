package guard

import (
	"fmt"
	"io"
	"strings"

	"github.com/bitwizeshift/alloy/tools/internal/check"
)

// pragmaOnce is the block that follows the #define of every include guard.
var pragmaOnce = []string{"#ifdef _MSC_VER", "# pragma once", "#endif"}

// Checker reports and fixes headers whose include guard does not follow the
// layout:
//
//	#ifndef NAME
//	#define NAME
//
//	#ifdef _MSC_VER
//	# pragma once
//	#endif
//
//	...
//
//	#endif /* NAME */
//
// The #ifndef is the first line after the comments that open the header, and
// the closing #endif is its last line.
type Checker struct {
	// Roots are the names of the directories that header paths are relative
	// to. The guard name is formed from the part of the path after the first
	// of these directories, or from the whole path if it contains none.
	Roots []string
}

// Name returns the guard name of the header at path: the path relative to the
// first of Roots, in uppercase, with every character that cannot appear in an
// identifier replaced with an underscore.
func (c *Checker) Name(path string) string {
	parts := strings.Split(path, "/")
	for i, part := range parts {
		if c.isRoot(part) {
			parts = parts[i+1:]
			break
		}
	}
	return strings.Map(func(r rune) rune {
		switch {
		case 'a' <= r && r <= 'z':
			return r - 'a' + 'A'
		case 'A' <= r && r <= 'Z', '0' <= r && r <= '9':
			return r
		}
		return '_'
	}, strings.Join(parts, "/"))
}

// Check returns a [check.Finding] for each part of the include guard of the
// header read from r that does not follow the layout for path. An error is
// returned only if r cannot be read.
func (c *Checker) Check(path string, r io.Reader) ([]check.Finding, error) {
	h, err := c.read(path, r)
	if err != nil {
		return nil, err
	}
	var findings []check.Finding
	if h.line(h.open) != h.ifndef() {
		findings = append(findings, h.finding(h.open, "include guard must open with %q", h.ifndef()))
	}
	if h.line(h.open+1) != h.define() {
		findings = append(findings, h.finding(h.open+1, "include guard must continue with %q", h.define()))
	}
	if !h.hasPragmaOnce() {
		findings = append(findings, h.finding(h.pragma, "include guard must be followed by %q holding %q", pragmaOnce[0], pragmaOnce[1]))
	}
	if h.line(h.close) != h.endif() {
		findings = append(findings, h.finding(h.close, "include guard must close with %q", h.endif()))
	}
	return findings, nil
}

// Fix copies r to w with the include guard rewritten to follow the layout for
// path. The rest of the header is copied unchanged, and a header whose guard
// is not an #ifndef and #define pair closed by an #endif is copied as is. An
// error is returned if r cannot be read, or w cannot be written.
func (c *Checker) Fix(path string, r io.Reader, w io.Writer) error {
	h, err := c.read(path, r)
	if err != nil {
		return err
	}
	if h.isGuarded() {
		h.fix()
	}
	_, err = io.WriteString(w, strings.Join(h.lines, "\n"))
	return err
}

var _ check.CheckFixer = (*Checker)(nil)

func (c *Checker) isRoot(dir string) bool {
	for _, root := range c.Roots {
		if dir == root {
			return true
		}
	}
	return false
}

func (c *Checker) read(path string, r io.Reader) (*header, error) {
	data, err := io.ReadAll(r)
	if err != nil {
		return nil, err
	}
	h := &header{name: c.Name(path), lines: strings.Split(string(data), "\n")}
	h.locate()
	return h, nil
}

// header is a header split into lines, with the indices of the lines that make
// up its include guard.
type header struct {
	name  string
	lines []string

	// open is the index of the first line after the opening comments.
	open int

	// pragma and pragmaEnd delimit the #pragma once block after the #define,
	// and are equal where the block is to be inserted if there is none.
	pragma, pragmaEnd int

	// close is the index of the last line that is not blank.
	close int
}

func (h *header) ifndef() string { return "#ifndef " + h.name }
func (h *header) define() string { return "#define " + h.name }
func (h *header) endif() string  { return "#endif /* " + h.name + " */" }

func (h *header) line(i int) string {
	if i < 0 || i >= len(h.lines) {
		return ""
	}
	return h.lines[i]
}

// finding returns a [check.Finding] on the line at index i, or on the whole
// header if i is past either end of it.
func (h *header) finding(i int, format string, args ...any) check.Finding {
	f := check.Finding{Message: fmt.Sprintf(format, args...)}
	if i >= 0 && i < len(h.lines) {
		f.Line = i + 1
	}
	return f
}

func (h *header) locate() {
	h.open = h.skipComments()
	h.pragma = h.skipBlank(h.open + 2)
	h.pragmaEnd = h.pragma
	if h.isPragmaOnce(h.pragma) {
		h.pragmaEnd = h.pragma + len(pragmaOnce)
	} else {
		h.pragma = min(h.open+2, len(h.lines))
		h.pragmaEnd = h.pragma
	}
	h.close = len(h.lines) - 1
	for h.close >= 0 && strings.TrimSpace(h.lines[h.close]) == "" {
		h.close--
	}
}

// skipComments returns the index of the first line that is neither blank nor
// part of a comment.
func (h *header) skipComments() int {
	inBlock := false
	for i, line := range h.lines {
		text := strings.TrimSpace(line)
		switch {
		case inBlock:
			inBlock = !strings.Contains(text, "*/")
		case strings.HasPrefix(text, "/*"):
			inBlock = !strings.Contains(text[2:], "*/")
		case text != "" && !strings.HasPrefix(text, "//"):
			return i
		}
	}
	return len(h.lines)
}

func (h *header) skipBlank(i int) int {
	for i < len(h.lines) && strings.TrimSpace(h.lines[i]) == "" {
		i++
	}
	return i
}

// isPragmaOnce reports whether the lines at i are a conditional on _MSC_VER
// that holds only a #pragma once, in any spelling.
func (h *header) isPragmaOnce(i int) bool {
	cond := strings.TrimSpace(h.line(i))
	pragma := strings.Join(strings.Fields(strings.TrimPrefix(h.line(i+1), "#")), " ")
	end := strings.TrimSpace(h.line(i + 2))
	return strings.HasPrefix(cond, "#if") && strings.Contains(cond, "_MSC_VER") &&
		pragma == "pragma once" && strings.HasPrefix(end, "#endif")
}

func (h *header) hasPragmaOnce() bool {
	if h.pragmaEnd-h.pragma != len(pragmaOnce) {
		return false
	}
	for i, want := range pragmaOnce {
		if h.lines[h.pragma+i] != want {
			return false
		}
	}
	return true
}

// isGuarded reports whether the header opens with an #ifndef and #define pair
// and closes with an #endif.
func (h *header) isGuarded() bool {
	return h.close > h.open+1 &&
		strings.HasPrefix(h.line(h.open), "#ifndef ") &&
		strings.HasPrefix(h.line(h.open+1), "#define ") &&
		strings.HasPrefix(h.line(h.close), "#endif")
}

// fix rewrites the lines of the include guard, from the bottom up so that the
// indices of the lines above each change stay valid.
func (h *header) fix() {
	h.lines[h.close] = h.endif()
	block := pragmaOnce
	if h.pragma == h.pragmaEnd {
		block = append([]string{""}, pragmaOnce...)
		if strings.TrimSpace(h.line(h.pragma)) != "" {
			block = append(block, "")
		}
	}
	h.lines = append(h.lines[:h.pragma], append(block, h.lines[h.pragmaEnd:]...)...)
	h.lines[h.open+1] = h.define()
	h.lines[h.open] = h.ifndef()
}
