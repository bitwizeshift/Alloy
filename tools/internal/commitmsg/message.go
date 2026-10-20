package commitmsg

import (
	"bufio"
	"io"
	"regexp"
	"slices"
	"strings"
	"unicode"
)

// Message is a commit message with its comments removed.
type Message struct {
	// Subject is the first line of the message.
	Subject string

	// Lines are the lines that follow the subject, starting with the line
	// that should separate the subject from the body.
	Lines []string

	// Trailers are the trailers of the last paragraph, in order.
	Trailers []Trailer
}

// Trailer is a "Key: value" line at the end of a commit message.
type Trailer struct {
	Key   string
	Value string
}

var trailerLine = regexp.MustCompile(`^([A-Za-z0-9][A-Za-z0-9-]*):\s*(.*)$`)

// Parse reads a commit message from r.
//
// Lines that start with "#" are removed, and a "#" line containing ">8" is a
// scissors line that ends the message. Trailing whitespace is removed from each
// line, as are leading and trailing blank lines. The last paragraph is read as
// trailers if it follows the subject, and every line in it is a trailer.
//
// An error is returned only if r cannot be read.
func Parse(r io.Reader) (*Message, error) {
	var lines []string
	scanner := bufio.NewScanner(r)
	scanner.Buffer(nil, 1<<24)
	for scanner.Scan() {
		line := scanner.Text()
		if strings.HasPrefix(line, "#") {
			if strings.Contains(line, ">8") {
				break
			}
			continue
		}
		lines = append(lines, strings.TrimRightFunc(line, unicode.IsSpace))
	}
	if err := scanner.Err(); err != nil {
		return nil, err
	}

	lines = trimBlank(lines)
	if len(lines) == 0 {
		return &Message{}, nil
	}
	return &Message{
		Subject:  lines[0],
		Lines:    lines[1:],
		Trailers: trailers(lines),
	}, nil
}

func trimBlank(lines []string) []string {
	blank := func(line string) bool { return line == "" }
	start := slices.IndexFunc(lines, func(line string) bool { return !blank(line) })
	if start < 0 {
		return nil
	}
	end := len(lines)
	for blank(lines[end-1]) {
		end--
	}
	return lines[start:end]
}

func trailers(lines []string) []Trailer {
	separator := -1
	for i, line := range lines {
		if line == "" {
			separator = i
		}
	}
	if separator < 0 {
		return nil
	}

	var result []Trailer
	for _, line := range lines[separator+1:] {
		match := trailerLine.FindStringSubmatch(line)
		if match == nil {
			return nil
		}
		result = append(result, Trailer{Key: match[1], Value: match[2]})
	}
	return result
}
