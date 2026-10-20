package commitmsg_test

import (
	"strings"
	"testing"

	"github.com/google/go-cmp/cmp"
	"github.com/google/go-cmp/cmp/cmpopts"

	"github.com/bitwizeshift/alloy/tools/internal/commitmsg"
)

func policy() *commitmsg.Policy {
	return &commitmsg.Policy{
		SubjectWidth: 10,
		BodyWidth:    8,
		Categories:   []string{"feature", "fix"},
		Impacts:      []string{"additive", "none"},
		Components:   []string{"core", "tools"},
	}
}

func trailers(values ...string) []commitmsg.Trailer {
	var result []commitmsg.Trailer
	for _, value := range values {
		key, value, _ := strings.Cut(value, ": ")
		result = append(result, commitmsg.Trailer{Key: key, Value: value})
	}
	return result
}

func TestPolicyValidate(t *testing.T) {
	t.Parallel()

	testCases := []struct {
		name    string
		message *commitmsg.Message
		want    []error
	}{
		{
			name: "valid",
			message: &commitmsg.Message{
				Subject: "Add thing",
				Lines:   []string{"", "Body.", "A long line.", ""},
				Trailers: trailers(
					"Change-Category: fix",
					"change-impact: none",
					"Component: core",
					"Component: tools",
				),
			},
			want: nil,
		}, {
			name: "subject at limit",
			message: &commitmsg.Message{
				Subject:  "Ádd things",
				Trailers: trailers("Change-Category: fix", "Change-Impact: none"),
			},
			want: nil,
		}, {
			name: "empty subject",
			message: &commitmsg.Message{
				Trailers: trailers("Change-Category: fix", "Change-Impact: none"),
			},
			want: []error{commitmsg.ErrEmptySubject},
		}, {
			name: "subject too long",
			message: &commitmsg.Message{
				Subject:  "Add a thing",
				Trailers: trailers("Change-Category: fix", "Change-Impact: none"),
			},
			want: []error{commitmsg.ErrSubjectTooLong},
		}, {
			name: "no blank line",
			message: &commitmsg.Message{
				Subject:  "Add thing",
				Lines:    []string{"Body.", "", "Change-Category: fix", "Change-Impact: none"},
				Trailers: trailers("Change-Category: fix", "Change-Impact: none"),
			},
			want: []error{commitmsg.ErrNoBlankLine, commitmsg.ErrBodyTooWide},
		}, {
			name: "half of body too wide",
			message: &commitmsg.Message{
				Subject:  "Add thing",
				Lines:    []string{"", "Body.", "A long line."},
				Trailers: trailers("Change-Category: fix", "Change-Impact: none"),
			},
			want: []error{commitmsg.ErrBodyTooWide},
		}, {
			name: "missing trailers",
			message: &commitmsg.Message{
				Subject: "Add thing",
			},
			want: []error{commitmsg.ErrMissingTrailer, commitmsg.ErrMissingTrailer},
		}, {
			name: "repeated trailers",
			message: &commitmsg.Message{
				Subject: "Add thing",
				Trailers: trailers(
					"Change-Category: fix",
					"Change-Category: fix",
					"Change-Impact: none",
					"Component: core",
					"Component: core",
				),
			},
			want: []error{commitmsg.ErrRepeatedTrailer, commitmsg.ErrRepeatedTrailer},
		}, {
			name: "invalid trailers",
			message: &commitmsg.Message{
				Subject: "Add thing",
				Trailers: trailers(
					"Change-Category: bugfix",
					"Change-Impact: corrective",
					"Component: render",
				),
			},
			want: []error{
				commitmsg.ErrInvalidTrailer,
				commitmsg.ErrInvalidTrailer,
				commitmsg.ErrInvalidTrailer,
			},
		},
	}

	for _, tc := range testCases {
		t.Run(tc.name, func(t *testing.T) {
			t.Parallel()

			// Arrange
			sut := policy()

			// Act
			errs := sut.Validate(tc.message)

			// Assert
			if got, want := errs, tc.want; !cmp.Equal(got, want, cmpopts.EquateErrors()) {
				t.Errorf("Policy.Validate() = %v, want %v", got, want)
			}
		})
	}
}
