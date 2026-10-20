package whitespace

import (
	"bufio"
	"errors"
	"io"
	"strings"
	"unicode"

	"github.com/bitwizeshift/alloy/tools/internal/check"
)

// Checker reports and removes trailing whitespace.
type Checker struct{}

// Check returns a [check.Finding] for each line read from r that ends with
// whitespace. An error is returned only if r cannot be read.
func (c Checker) Check(_ string, r io.Reader) ([]check.Finding, error) {
	var findings []check.Finding
	err := c.eachLine(r, func(n int, text, _ string) error {
		if c.trim(text) != text {
			findings = append(findings, check.Finding{
				Line:    n,
				Message: "trailing whitespace",
			})
		}
		return nil
	})
	return findings, err
}

// Fix copies r to w with the trailing whitespace removed from every line.
// An error is returned if r cannot be read, or w cannot be written.
func (c Checker) Fix(_ string, r io.Reader, w io.Writer) error {
	return c.eachLine(r, func(_ int, text, ending string) error {
		_, err := io.WriteString(w, c.trim(text)+ending)
		return err
	})
}

var _ check.CheckFixer = Checker{}

func (Checker) trim(text string) string {
	return strings.TrimRightFunc(text, unicode.IsSpace)
}

// eachLine calls fn with the 1-based number, text, and line ending of every
// line in r. The final line has an empty ending if r does not end in a newline.
func (c Checker) eachLine(r io.Reader, fn func(n int, text, ending string) error) error {
	br := bufio.NewReader(r)
	for n := 1; ; n++ {
		line, readErr := br.ReadString('\n')
		if line != "" {
			text, ending := c.split(line)
			if err := fn(n, text, ending); err != nil {
				return err
			}
		}
		if errors.Is(readErr, io.EOF) {
			return nil
		}
		if readErr != nil {
			return readErr
		}
	}
}

func (Checker) split(line string) (text, ending string) {
	for _, ending := range []string{"\r\n", "\n"} {
		if text, ok := strings.CutSuffix(line, ending); ok {
			return text, ending
		}
	}
	return line, ""
}
