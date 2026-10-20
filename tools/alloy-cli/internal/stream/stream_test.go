package stream_test

import (
	"io"
	"strings"
	"testing"

	"github.com/google/go-cmp/cmp"
	"github.com/google/go-cmp/cmp/cmpopts"

	"github.com/bitwizeshift/go-cli/richtext"

	"github.com/bitwizeshift/alloy/tools/alloy-cli/internal/stream"
)

const markup = "[fg:red]text[/fg] [/fg]\n"

func TestVerbatim_RichtextWriter_WritesMarkupVerbatim(t *testing.T) {
	t.Parallel()

	// Arrange
	var out strings.Builder
	sut := stream.Verbatim(richtext.NewWriter(&out, richtext.DefaultTheme))

	// Act
	_, err := io.WriteString(sut, markup)

	// Assert
	if got, want := err, error(nil); !cmp.Equal(got, want, cmpopts.EquateErrors()) {
		t.Fatalf("Verbatim().Write() = %v, want %v", got, want)
	}
	if got, want := out.String(), markup; !cmp.Equal(got, want) {
		t.Errorf("Verbatim().Write() output = %q, want %q", got, want)
	}
}

func TestVerbatim_PlainWriter_WritesUnchanged(t *testing.T) {
	t.Parallel()

	// Arrange
	var out strings.Builder
	sut := stream.Verbatim(&out)

	// Act
	_, err := io.WriteString(sut, markup)

	// Assert
	if got, want := err, error(nil); !cmp.Equal(got, want, cmpopts.EquateErrors()) {
		t.Fatalf("Verbatim().Write() = %v, want %v", got, want)
	}
	if got, want := out.String(), markup; !cmp.Equal(got, want) {
		t.Errorf("Verbatim().Write() output = %q, want %q", got, want)
	}
}
