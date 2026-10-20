package commitmsg_test

import (
	"errors"
	"io"
	"strings"
	"testing"
	"testing/iotest"

	"github.com/google/go-cmp/cmp"
	"github.com/google/go-cmp/cmp/cmpopts"

	"github.com/bitwizeshift/alloy/tools/internal/commitmsg"
)

func TestParse(t *testing.T) {
	t.Parallel()

	testErr := errors.New("test error")
	testCases := []struct {
		name    string
		reader  io.Reader
		want    *commitmsg.Message
		wantErr error
	}{
		{
			name:    "empty",
			reader:  strings.NewReader(""),
			want:    &commitmsg.Message{},
			wantErr: nil,
		}, {
			name:    "only comments",
			reader:  strings.NewReader("# Please enter a message\n#\n"),
			want:    &commitmsg.Message{},
			wantErr: nil,
		}, {
			name:    "subject only",
			reader:  strings.NewReader("Add a thing  \n\n"),
			want:    &commitmsg.Message{Subject: "Add a thing"},
			wantErr: nil,
		}, {
			name: "body and trailers",
			reader: strings.NewReader(
				"\nAdd a thing\n\nBody text.\n\n" +
					"Change-Category: feature\nComponent:core\n# comment\n",
			),
			want: &commitmsg.Message{
				Subject: "Add a thing",
				Lines: []string{
					"",
					"Body text.",
					"",
					"Change-Category: feature",
					"Component:core",
				},
				Trailers: []commitmsg.Trailer{
					{Key: "Change-Category", Value: "feature"},
					{Key: "Component", Value: "core"},
				},
			},
			wantErr: nil,
		}, {
			name:   "trailers without body",
			reader: strings.NewReader("Add a thing\n\nChange-Impact: none\n"),
			want: &commitmsg.Message{
				Subject:  "Add a thing",
				Lines:    []string{"", "Change-Impact: none"},
				Trailers: []commitmsg.Trailer{{Key: "Change-Impact", Value: "none"}},
			},
			wantErr: nil,
		}, {
			name:   "last paragraph is not trailers",
			reader: strings.NewReader("Add a thing\n\nChange-Impact: none\nSee the docs.\n"),
			want: &commitmsg.Message{
				Subject: "Add a thing",
				Lines:   []string{"", "Change-Impact: none", "See the docs."},
			},
			wantErr: nil,
		}, {
			name:   "trailer-like subject paragraph",
			reader: strings.NewReader("Fix: a thing\nChange-Impact: none\n"),
			want: &commitmsg.Message{
				Subject: "Fix: a thing",
				Lines:   []string{"Change-Impact: none"},
			},
			wantErr: nil,
		}, {
			name: "scissors",
			reader: strings.NewReader(
				"Add a thing\n\nChange-Impact: none\n" +
					"# ------------------------ >8 ------------------------\n" +
					"diff --git a/x b/x\n",
			),
			want: &commitmsg.Message{
				Subject:  "Add a thing",
				Lines:    []string{"", "Change-Impact: none"},
				Trailers: []commitmsg.Trailer{{Key: "Change-Impact", Value: "none"}},
			},
			wantErr: nil,
		}, {
			name:    "read error",
			reader:  iotest.ErrReader(testErr),
			want:    nil,
			wantErr: testErr,
		},
	}

	for _, tc := range testCases {
		t.Run(tc.name, func(t *testing.T) {
			t.Parallel()

			// Act
			message, err := commitmsg.Parse(tc.reader)

			// Assert
			if got, want := err, tc.wantErr; !cmp.Equal(got, want, cmpopts.EquateErrors()) {
				t.Fatalf("Parse() = %v, want %v", got, want)
			}
			if got, want := message, tc.want; !cmp.Equal(got, want, cmpopts.EquateEmpty()) {
				t.Errorf("Parse() mismatch (-got +want):\n%s", cmp.Diff(got, want, cmpopts.EquateEmpty()))
			}
		})
	}
}
