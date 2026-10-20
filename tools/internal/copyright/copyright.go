package copyright

import (
	"fmt"
	"io"
	"regexp"
	"slices"
	"strings"

	"github.com/bitwizeshift/alloy/tools/internal/check"
)

// Checker reports and fixes copyright notices whose years are out of date.
type Checker struct {
	// Pattern matches a copyright notice within a single line. Its first
	// capture group must match the years of the notice.
	Pattern *regexp.Regexp

	// History provides the years in which each file was modified.
	History History

	// Year is the current year, which is always expected in a notice.
	Year int
}

// Check returns a [check.Finding] for each notice read from r whose years are
// not the expected years for path, and for each notice after the first.
// A file without a notice has no findings.
//
// An error is returned if r cannot be read, or the history of path cannot be
// read.
func (c *Checker) Check(path string, r io.Reader) ([]check.Finding, error) {
	content, want, err := c.read(path, r)
	if err != nil {
		return nil, err
	}

	var findings []check.Finding
	for i, notice := range c.notices(content) {
		if i > 0 {
			findings = append(findings, check.Finding{
				Line:    notice.line,
				Message: "multiple copyright notices",
			})
		}
		if notice.years != want {
			findings = append(findings, check.Finding{
				Line:    notice.line,
				Message: fmt.Sprintf("copyright years are %q, want %q", notice.years, want),
			})
		}
	}
	return findings, nil
}

// Fix copies r to w with the years of every notice replaced by the expected
// years for path.
//
// An error is returned if r cannot be read, the history of path cannot be
// read, or w cannot be written.
func (c *Checker) Fix(path string, r io.Reader, w io.Writer) error {
	content, want, err := c.read(path, r)
	if err != nil {
		return err
	}

	var fixed strings.Builder
	end := 0
	for _, notice := range c.notices(content) {
		fixed.WriteString(content[end:notice.start])
		fixed.WriteString(want)
		end = notice.end
	}
	fixed.WriteString(content[end:])

	_, err = io.WriteString(w, fixed.String())
	return err
}

var _ check.CheckFixer = (*Checker)(nil)

func (c *Checker) read(path string, r io.Reader) (content, want string, err error) {
	data, err := io.ReadAll(r)
	if err != nil {
		return "", "", err
	}
	years, err := c.History.Years(path)
	if err != nil {
		return "", "", err
	}
	return string(data), Format(append(slices.Clone(years), c.Year)), nil
}

// notice is a copyright notice, where years is content[start:end].
type notice struct {
	line       int
	years      string
	start, end int
}

func (c *Checker) notices(content string) []notice {
	var notices []notice
	for _, match := range c.Pattern.FindAllStringSubmatchIndex(content, -1) {
		start, end := match[2], match[3]
		notices = append(notices, notice{
			line:  strings.Count(content[:start], "\n") + 1,
			years: content[start:end],
			start: start,
			end:   end,
		})
	}
	return notices
}
