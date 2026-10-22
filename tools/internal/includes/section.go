package includes

import (
	"regexp"
	"slices"
	"strings"
)

var (
	includePattern = regexp.MustCompile(`^\s*#\s*include\s*([<"])([^>"]*)[>"](.*)$`)
	ifPattern      = regexp.MustCompile(`^\s*#\s*if`)
	endifPattern   = regexp.MustCompile(`^\s*#\s*endif`)
	ifndefPattern  = regexp.MustCompile(`^\s*#\s*ifndef\s+(\w+)`)
	definePattern  = regexp.MustCompile(`^\s*#\s*define\s+(\w+)`)
)

// directive is an #include line.
type directive struct {
	// line is the index of the line of the directive.
	line int

	// path is the path of the included header, as written.
	path string

	// angled reports whether path is delimited by angle brackets rather than
	// quotes.
	angled bool

	// prefix is the text before the opening delimiter, and suffix the text
	// after the closing one.
	prefix, suffix string
}

func (d *directive) String() string {
	return d.spelling(d.angled)
}

// spelling returns the path of the directive, delimited with angle brackets if
// angled, or else with quotes.
func (d *directive) spelling(angled bool) string {
	if angled {
		return "<" + d.path + ">"
	}
	return `"` + d.path + `"`
}

// text returns the line of the directive, with its path delimited as by
// [directive.spelling].
func (d *directive) text(angled bool) string {
	return d.prefix + d.spelling(angled) + d.suffix
}

// item is a part of the include section: either an unconditional #include, or
// a conditional block along with the comments directly above it.
type item struct {
	// start and end delimit the lines of the item.
	start, end int

	// include is the directive of an unconditional #include, and is nil for a
	// conditional block.
	include *directive

	// includes reports whether the item is, or contains, an #include.
	includes bool
}

// section is the run of lines that holds the includes at the top of a file.
type section struct {
	// start and end delimit the lines of the section.
	start, end int

	items []item

	// comments reports whether a comment that is not part of a conditional
	// block lies between the items.
	comments bool
}

// directives returns the unconditional #include directives of the section.
func (s *section) directives() []*directive {
	var directives []*directive
	for _, it := range s.items {
		if it.include != nil {
			directives = append(directives, it.include)
		}
	}
	return directives
}

// hasConditional reports whether a conditional block lies between the lines at
// index start and end.
func (s *section) hasConditional(start, end int) bool {
	for _, it := range s.items {
		if it.include == nil && it.start >= start && it.end <= end {
			return true
		}
	}
	return false
}

// parseSection returns the include section of a file made of lines. The
// section starts at the first #include outside of any conditional other than
// the include guard, and runs through the includes, blank lines, comments and
// conditional blocks after it, up to the last of them that includes a header.
// A file without such an #include has an empty section.
func parseSection(lines []string) *section {
	base := guardDepth(lines)
	depth := 0
	for i, line := range lines {
		if depth == base {
			if d := parseDirective(i, line); d != nil {
				return scanSection(lines, i)
			}
		}
		depth += depthChange(line)
	}
	return &section{}
}

// scanSection returns the include section that starts with the #include at
// index start.
func scanSection(lines []string, start int) *section {
	s := &section{start: start}
	var loose []int
	comment := -1
	for i := start; i < len(lines); {
		text := strings.TrimSpace(lines[i])
		if text == "" || isComment(text) {
			switch {
			case text != "" && comment < 0:
				comment = i
			case text == "" && comment >= 0:
				loose = append(loose, comment)
				comment = -1
			}
			i++
			continue
		}
		if d := parseDirective(i, lines[i]); d != nil {
			if comment >= 0 {
				loose = append(loose, comment)
			}
			s.items = append(s.items, item{start: i, end: i + 1, include: d, includes: true})
			comment = -1
			i++
			continue
		}
		if !ifPattern.MatchString(lines[i]) {
			break
		}
		block := item{start: i, end: matchingEndif(lines, i)}
		block.includes = hasDirective(lines[block.start:block.end])
		if comment >= 0 {
			block.start = comment
		}
		s.items = append(s.items, block)
		comment = -1
		i = block.end
	}
	s.trim()
	for _, line := range loose {
		s.comments = s.comments || line < s.end
	}
	return s
}

// trim drops the items after the last one that includes a header.
func (s *section) trim() {
	last := 0
	for i, it := range s.items {
		if it.includes {
			last = i
		}
	}
	s.items = s.items[:last+1]
	s.end = s.items[last].end
}

// guardDepth returns 1 if lines open with an include guard, so that the
// includes of the file are one conditional deep, or else 0.
func guardDepth(lines []string) int {
	i := skipComments(lines)
	if i+1 >= len(lines) {
		return 0
	}
	ifndef := ifndefPattern.FindStringSubmatch(lines[i])
	define := definePattern.FindStringSubmatch(lines[i+1])
	if ifndef == nil || define == nil || define[1] != ifndef[1] {
		return 0
	}
	return 1
}

// skipComments returns the index of the first line that is neither blank nor
// part of a comment.
func skipComments(lines []string) int {
	inBlock := false
	for i, line := range lines {
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
	return len(lines)
}

// matchingEndif returns the index of the line after the #endif that closes the
// conditional opened at index start, or the number of lines if it is never
// closed.
func matchingEndif(lines []string, start int) int {
	depth := 0
	for i := start; i < len(lines); i++ {
		depth += depthChange(lines[i])
		if depth == 0 {
			return i + 1
		}
	}
	return len(lines)
}

func depthChange(line string) int {
	switch {
	case ifPattern.MatchString(line):
		return 1
	case endifPattern.MatchString(line):
		return -1
	}
	return 0
}

func hasDirective(lines []string) bool {
	return slices.ContainsFunc(lines, includePattern.MatchString)
}

// isComment reports whether the trimmed line text is a comment that starts and
// ends on the same line.
func isComment(text string) bool {
	return strings.HasPrefix(text, "//") ||
		strings.HasPrefix(text, "/*") && strings.HasSuffix(text, "*/")
}

func parseDirective(i int, line string) *directive {
	match := includePattern.FindStringSubmatchIndex(line)
	if match == nil {
		return nil
	}
	return &directive{
		line:   i,
		path:   line[match[4]:match[5]],
		angled: line[match[2]:match[3]] == "<",
		prefix: line[:match[2]],
		suffix: line[match[6]:match[7]],
	}
}
