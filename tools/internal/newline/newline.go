package newline

import (
	"io"

	"github.com/bitwizeshift/alloy/tools/internal/check"
)

// Checker reports and fixes content that does not end with a newline.
type Checker struct{}

// Check returns a single [check.Finding] for the whole file if r is not empty
// and does not end with a newline. An error is returned only if r cannot be
// read.
func (c Checker) Check(_ string, r io.Reader) ([]check.Finding, error) {
	ok, err := c.endsWithNewline(r, io.Discard)
	if err != nil || ok {
		return nil, err
	}
	return []check.Finding{{Message: "missing trailing newline"}}, nil
}

// Fix copies r to w, and appends a newline if r is not empty and does not end
// with one. An error is returned if r cannot be read, or w cannot be written.
func (c Checker) Fix(_ string, r io.Reader, w io.Writer) error {
	ok, err := c.endsWithNewline(r, w)
	if err != nil || ok {
		return err
	}
	_, err = io.WriteString(w, "\n")
	return err
}

var _ check.CheckFixer = Checker{}

// endsWithNewline copies r to w, and reports whether r was empty or ended with
// a newline.
func (Checker) endsWithNewline(r io.Reader, w io.Writer) (bool, error) {
	last := &lastByte{w: w}
	if _, err := io.Copy(last, r); err != nil {
		return false, err
	}
	return !last.seen || last.b == '\n', nil
}

type lastByte struct {
	w    io.Writer
	b    byte
	seen bool
}

func (lb *lastByte) Write(p []byte) (int, error) {
	if len(p) > 0 {
		lb.b = p[len(p)-1]
		lb.seen = true
	}
	return lb.w.Write(p)
}
