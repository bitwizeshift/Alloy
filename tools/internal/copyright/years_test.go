package copyright_test

import (
	"errors"
	"io"
	"strings"
	"testing"
	"testing/iotest"

	"github.com/google/go-cmp/cmp"
	"github.com/google/go-cmp/cmp/cmpopts"

	"github.com/bitwizeshift/alloy/tools/internal/copyright"
)

func TestParseYears(t *testing.T) {
	t.Parallel()

	testErr := errors.New("test error")
	testCases := []struct {
		name    string
		reader  io.Reader
		want    []int
		wantErr error
	}{
		{
			name:    "empty",
			reader:  strings.NewReader(""),
			want:    nil,
			wantErr: nil,
		}, {
			name:    "one year per line",
			reader:  strings.NewReader("2023\n2021\n2021\n"),
			want:    []int{2023, 2021, 2021},
			wantErr: nil,
		}, {
			name:    "blank lines",
			reader:  strings.NewReader("\n2023\n\n  \n2021"),
			want:    []int{2023, 2021},
			wantErr: nil,
		}, {
			name:    "not a year",
			reader:  strings.NewReader("2023\nnope\n"),
			want:    nil,
			wantErr: copyright.ErrBadYear,
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
			years, err := copyright.ParseYears(tc.reader)

			// Assert
			if got, want := err, tc.wantErr; !cmp.Equal(got, want, cmpopts.EquateErrors()) {
				t.Fatalf("ParseYears() = %v, want %v", got, want)
			}
			if got, want := years, tc.want; !cmp.Equal(got, want) {
				t.Errorf("ParseYears() = %v, want %v", got, want)
			}
		})
	}
}

func TestFormat(t *testing.T) {
	t.Parallel()

	testCases := []struct {
		name  string
		years []int
		want  string
	}{
		{
			name:  "empty",
			years: nil,
			want:  "",
		}, {
			name:  "single year",
			years: []int{2023},
			want:  "2023",
		}, {
			name:  "consecutive years",
			years: []int{2021, 2022, 2023},
			want:  "2021-2023",
		}, {
			name:  "disjoint years",
			years: []int{2019, 2021, 2023},
			want:  "2019, 2021, 2023",
		}, {
			name:  "unsorted with duplicates",
			years: []int{2023, 2019, 2020, 2023, 2021, 2019},
			want:  "2019-2021, 2023",
		},
	}

	for _, tc := range testCases {
		t.Run(tc.name, func(t *testing.T) {
			t.Parallel()

			// Act
			years := copyright.Format(tc.years)

			// Assert
			if got, want := years, tc.want; !cmp.Equal(got, want) {
				t.Errorf("Format() = %q, want %q", got, want)
			}
		})
	}
}
