package cmakelists

import (
	"strings"
)

// Arguments returns the arguments of every command in src, in order, without
// the quotes of quoted arguments. Comments are skipped, and the escape
// sequences of quoted arguments are kept as written, except for \" and \\.
func Arguments(src string) []string {
	l := &lexer{src: src}
	return l.arguments()
}

// lexer splits the source of a CMakeLists.txt into arguments.
type lexer struct {
	src  string
	pos  int
	args []string
}

func (l *lexer) arguments() []string {
	for l.pos < len(l.src) {
		switch c := l.src[l.pos]; {
		case c == '#':
			l.skipComment()
		case c == '"':
			l.quoted()
		case l.isSeparator(c):
			l.pos++
		default:
			l.unquoted()
		}
	}
	return l.args
}

// skipComment skips the comment at pos: a bracket comment through its closing
// bracket, or a line comment through the end of its line.
func (l *lexer) skipComment() {
	l.pos++
	if closing, ok := l.bracketClose(); ok {
		end := strings.Index(l.src[l.pos:], closing)
		if end < 0 {
			l.pos = len(l.src)
			return
		}
		l.pos += end + len(closing)
		return
	}
	end := strings.IndexByte(l.src[l.pos:], '\n')
	if end < 0 {
		l.pos = len(l.src)
		return
	}
	l.pos += end
}

// bracketClose returns the closing bracket of the opening bracket at pos, such
// as "]==]" for "[==[", and reports whether there is one.
func (l *lexer) bracketClose() (string, bool) {
	rest, ok := strings.CutPrefix(l.src[l.pos:], "[")
	if !ok {
		return "", false
	}
	after := strings.TrimLeft(rest, "=")
	if !strings.HasPrefix(after, "[") {
		return "", false
	}
	level := len(rest) - len(after)
	return "]" + strings.Repeat("=", level) + "]", true
}

// quoted appends the quoted argument at pos, through its closing quote.
func (l *lexer) quoted() {
	var arg strings.Builder
	l.pos++
	for l.pos < len(l.src) {
		c := l.src[l.pos]
		switch {
		case c == '"':
			l.pos++
			l.args = append(l.args, arg.String())
			return
		case c == '\\' && l.pos+1 < len(l.src) && strings.ContainsRune(`"\`, rune(l.src[l.pos+1])):
			arg.WriteByte(l.src[l.pos+1])
			l.pos += 2
		default:
			arg.WriteByte(c)
			l.pos++
		}
	}
	l.args = append(l.args, arg.String())
}

// unquoted appends the unquoted argument at pos, through the last character
// before a separator.
func (l *lexer) unquoted() {
	start := l.pos
	for l.pos < len(l.src) && !l.isSeparator(l.src[l.pos]) {
		l.pos++
	}
	l.args = append(l.args, l.src[start:l.pos])
}

// isSeparator reports whether c separates arguments.
func (*lexer) isSeparator(c byte) bool {
	return strings.ContainsRune(" \t\r\n()", rune(c))
}
