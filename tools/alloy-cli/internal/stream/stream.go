package stream

import (
	"io"

	"github.com/bitwizeshift/go-cli/richtext"
)

// Verbatim returns a writer that writes to the same destination as w without
// interpreting richtext markup. If w does not interpret markup, w is returned.
func Verbatim(w io.Writer) io.Writer {
	if rw, ok := w.(*richtext.Writer); ok {
		return rw.Writer()
	}
	return w
}
