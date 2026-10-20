package license_test

import (
	"errors"
	"io"
	"strings"
	"testing"
	"testing/iotest"

	"github.com/google/go-cmp/cmp"
	"github.com/google/go-cmp/cmp/cmpopts"

	"github.com/bitwizeshift/alloy/tools/internal/check"
	"github.com/bitwizeshift/alloy/tools/internal/license"
)

func TestYearCheckerCheck(t *testing.T) {
	t.Parallel()

	testErr := errors.New("test error")
	testCases := []struct {
		name    string
		year    int
		reader  io.Reader
		want    []check.Finding
		wantErr error
	}{
		{
			name:    "current range",
			year:    2026,
			reader:  strings.NewReader("MIT\n\nCopyright (c) 2018-2026 Example\n"),
			want:    nil,
			wantErr: nil,
		}, {
			name:    "current single year",
			year:    2026,
			reader:  strings.NewReader("Copyright (c) 2026 Example\n"),
			want:    nil,
			wantErr: nil,
		}, {
			name:   "outdated range",
			year:   2026,
			reader: strings.NewReader("MIT\n\nCopyright (c) 2018-2022 Example\n"),
			want: []check.Finding{
				{Line: 3, Message: `copyright years are "2018-2022", want "2018-2026"`},
			},
			wantErr: nil,
		}, {
			name:   "outdated single year",
			year:   2026,
			reader: strings.NewReader("Copyright (c) 2018 Example\n"),
			want: []check.Finding{
				{Line: 1, Message: `copyright years are "2018", want "2018-2026"`},
			},
			wantErr: nil,
		}, {
			name:   "gaps",
			year:   2026,
			reader: strings.NewReader("Copyright (c) 2018-2020, 2026 Example\n"),
			want: []check.Finding{
				{Line: 1, Message: `copyright years are "2018-2020, 2026", want "2018-2026"`},
			},
			wantErr: nil,
		}, {
			name: "multiple notices",
			year: 2026,
			reader: strings.NewReader(
				"Copyright (c) 2018-2026 Example\n" +
					"Copyright (c) 2020-2026 Other\n",
			),
			want: []check.Finding{
				{Line: 2, Message: "multiple copyright notices"},
			},
			wantErr: nil,
		}, {
			name:    "no notice",
			year:    2026,
			reader:  strings.NewReader("MIT\n"),
			want:    []check.Finding{{Line: 0, Message: "missing copyright notice"}},
			wantErr: nil,
		}, {
			name:    "read error",
			year:    2026,
			reader:  iotest.ErrReader(testErr),
			want:    nil,
			wantErr: testErr,
		},
	}

	for _, tc := range testCases {
		t.Run(tc.name, func(t *testing.T) {
			t.Parallel()

			// Arrange
			sut := &license.YearChecker{Year: tc.year}

			// Act
			findings, err := sut.Check("LICENSE", tc.reader)

			// Assert
			if got, want := err, tc.wantErr; !cmp.Equal(got, want, cmpopts.EquateErrors()) {
				t.Fatalf("YearChecker.Check() = %v, want %v", got, want)
			}
			if got, want := findings, tc.want; !cmp.Equal(got, want) {
				t.Errorf("YearChecker.Check() = %v, want %v", got, want)
			}
		})
	}
}

func TestYearCheckerFix(t *testing.T) {
	t.Parallel()

	testErr := errors.New("test error")
	testCases := []struct {
		name    string
		year    int
		reader  io.Reader
		want    string
		wantErr error
	}{
		{
			name:    "current range",
			year:    2026,
			reader:  strings.NewReader("Copyright (c) 2018-2026 Example\n"),
			want:    "Copyright (c) 2018-2026 Example\n",
			wantErr: nil,
		}, {
			name:    "outdated range",
			year:    2026,
			reader:  strings.NewReader("MIT\n\nCopyright (c) 2018-2022 Example\n\nText\n"),
			want:    "MIT\n\nCopyright (c) 2018-2026 Example\n\nText\n",
			wantErr: nil,
		}, {
			name:    "gaps",
			year:    2026,
			reader:  strings.NewReader("Copyright (c) 2018-2020, 2024 Example\n"),
			want:    "Copyright (c) 2018-2026 Example\n",
			wantErr: nil,
		}, {
			name:    "no notice",
			year:    2026,
			reader:  strings.NewReader("MIT\n"),
			want:    "MIT\n",
			wantErr: nil,
		}, {
			name:    "read error",
			year:    2026,
			reader:  iotest.ErrReader(testErr),
			want:    "",
			wantErr: testErr,
		},
	}

	for _, tc := range testCases {
		t.Run(tc.name, func(t *testing.T) {
			t.Parallel()

			// Arrange
			sut := &license.YearChecker{Year: tc.year}
			var w strings.Builder

			// Act
			err := sut.Fix("LICENSE", tc.reader, &w)

			// Assert
			if got, want := err, tc.wantErr; !cmp.Equal(got, want, cmpopts.EquateErrors()) {
				t.Fatalf("YearChecker.Fix() = %v, want %v", got, want)
			}
			if got, want := w.String(), tc.want; !cmp.Equal(got, want) {
				t.Errorf("YearChecker.Fix() output = %q, want %q", got, want)
			}
		})
	}
}
