package diff

import (
	"bufio"
	"errors"
	"fmt"
	"io"
	"regexp"
	"strconv"
	"strings"
)

// ErrBadHunk is returned by [Parse] when a hunk header is malformed, or
// appears before the header of the file it belongs to.
var ErrBadHunk = errors.New("invalid hunk header")

// Range is an inclusive range of 1-based line numbers.
type Range struct {
	Start int
	End   int
}

// File is a file in the new version of a diff, and the line ranges that its
// hunks cover.
type File struct {
	// Path is the path of the file in the new version.
	Path string

	// Ranges are the ranges covered by each hunk, in the order they appear.
	// Hunks that only delete lines cover no range.
	Ranges []Range
}

var hunkHeader = regexp.MustCompile(`^@@ -\d+(?:,(\d+))? \+(\d+)(?:,(\d+))? @@`)

// Parse reads a unified diff without path prefixes from r, as produced by
// `git diff --no-prefix`, and returns each file that exists in the new version,
// in the order they appear. Deleted files are omitted.
//
// [ErrBadHunk] is returned if a hunk header is malformed. Any other error is
// returned if r cannot be read.
func Parse(r io.Reader) ([]File, error) {
	p := &parser{}
	scanner := bufio.NewScanner(r)
	scanner.Buffer(nil, 1<<24)
	for scanner.Scan() {
		if err := p.line(scanner.Text()); err != nil {
			return nil, err
		}
	}
	if err := scanner.Err(); err != nil {
		return nil, err
	}
	return p.files, nil
}

type parser struct {
	files []File

	// inFile reports whether a file header has been read for the hunks that
	// follow it, and current is that file, or nil for a deleted file.
	inFile  bool
	current *File

	// oldLeft and newLeft are the lines remaining in the current hunk, which
	// must not be mistaken for headers.
	oldLeft, newLeft int
}

func (p *parser) line(text string) error {
	if p.oldLeft > 0 || p.newLeft > 0 {
		p.content(text)
		return nil
	}
	switch {
	case strings.HasPrefix(text, "diff "):
		p.inFile = false
		p.current = nil
	case strings.HasPrefix(text, "+++ "):
		p.file(text)
	case strings.HasPrefix(text, "@@ "):
		return p.hunk(text)
	}
	return nil
}

func (p *parser) file(text string) {
	p.inFile = true
	path := strings.TrimSuffix(strings.TrimPrefix(text, "+++ "), "\t")
	if path == "/dev/null" {
		p.current = nil
		return
	}
	p.files = append(p.files, File{Path: path})
	p.current = &p.files[len(p.files)-1]
}

func (p *parser) hunk(text string) error {
	match := hunkHeader.FindStringSubmatch(text)
	if match == nil || !p.inFile {
		return fmt.Errorf("parse diff: %w %q", ErrBadHunk, text)
	}
	p.oldLeft = p.count(match[1])
	start, _ := strconv.Atoi(match[2])
	p.newLeft = p.count(match[3])
	if p.current != nil && p.newLeft > 0 {
		p.current.Ranges = append(p.current.Ranges, Range{
			Start: start,
			End:   start + p.newLeft - 1,
		})
	}
	return nil
}

func (p *parser) content(text string) {
	switch {
	case strings.HasPrefix(text, "-"):
		p.oldLeft--
	case strings.HasPrefix(text, "+"):
		p.newLeft--
	case strings.HasPrefix(text, `\`):
	default:
		p.oldLeft--
		p.newLeft--
	}
}

// count returns the line count of a hunk side, which is 1 when omitted.
func (*parser) count(s string) int {
	if s == "" {
		return 1
	}
	n, _ := strconv.Atoi(s)
	return n
}
