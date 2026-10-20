package copyright_test

import (
	"errors"
	"io"
	"regexp"
	"strings"
	"testing"
	"testing/iotest"

	"github.com/google/go-cmp/cmp"
	"github.com/google/go-cmp/cmp/cmpopts"

	"github.com/bitwizeshift/alloy/tools/internal/check"
	"github.com/bitwizeshift/alloy/tools/internal/copyright"
	"github.com/bitwizeshift/alloy/tools/internal/copyright/copyrighttest"
)

var pattern = regexp.MustCompile(`Copyright \(c\) (.*) Example All rights reserved\.`)

func TestCheckerCheck(t *testing.T) {
	t.Parallel()

	testErr := errors.New("test error")
	testCases := []struct {
		name    string
		history copyright.History
		year    int
		reader  io.Reader
		want    []check.Finding
		wantErr error
	}{
		{
			name:    "no notice",
			history: copyrighttest.History(2021),
			year:    2023,
			reader:  strings.NewReader("int x;\n"),
			want:    nil,
			wantErr: nil,
		}, {
			name:    "current notice",
			history: copyrighttest.History(2021, 2022),
			year:    2023,
			reader:  strings.NewReader("// Copyright (c) 2021-2023 Example All rights reserved.\n"),
			want:    nil,
			wantErr: nil,
		}, {
			name:    "untracked file",
			history: copyrighttest.History(),
			year:    2023,
			reader:  strings.NewReader("// Copyright (c) 2023 Example All rights reserved.\n"),
			want:    nil,
			wantErr: nil,
		}, {
			name:    "outdated notice",
			history: copyrighttest.History(2019, 2021),
			year:    2023,
			reader:  strings.NewReader("int x;\n// Copyright (c) 2019 Example All rights reserved.\n"),
			want: []check.Finding{
				{Line: 2, Message: `copyright years are "2019", want "2019, 2021, 2023"`},
			},
			wantErr: nil,
		}, {
			name:    "multiple notices",
			history: copyrighttest.History(2023),
			year:    2023,
			reader: strings.NewReader(
				"// Copyright (c) 2023 Example All rights reserved.\n" +
					"// Copyright (c) 2022 Example All rights reserved.\n",
			),
			want: []check.Finding{
				{Line: 2, Message: "multiple copyright notices"},
				{Line: 2, Message: `copyright years are "2022", want "2023"`},
			},
			wantErr: nil,
		}, {
			name:    "history error",
			history: copyrighttest.ErrHistory(testErr),
			year:    2023,
			reader:  strings.NewReader("int x;\n"),
			want:    nil,
			wantErr: testErr,
		}, {
			name:    "read error",
			history: copyrighttest.History(2023),
			year:    2023,
			reader:  iotest.ErrReader(testErr),
			want:    nil,
			wantErr: testErr,
		},
	}

	for _, tc := range testCases {
		t.Run(tc.name, func(t *testing.T) {
			t.Parallel()

			// Arrange
			sut := &copyright.Checker{
				Pattern: pattern,
				History: tc.history,
				Year:    tc.year,
			}

			// Act
			findings, err := sut.Check("a.cpp", tc.reader)

			// Assert
			if got, want := err, tc.wantErr; !cmp.Equal(got, want, cmpopts.EquateErrors()) {
				t.Fatalf("Checker.Check() = %v, want %v", got, want)
			}
			if got, want := findings, tc.want; !cmp.Equal(got, want) {
				t.Errorf("Checker.Check() = %v, want %v", got, want)
			}
		})
	}
}

func TestCheckerFix(t *testing.T) {
	t.Parallel()

	testErr := errors.New("test error")
	testCases := []struct {
		name    string
		history copyright.History
		year    int
		reader  io.Reader
		want    string
		wantErr error
	}{
		{
			name:    "no notice",
			history: copyrighttest.History(2021),
			year:    2023,
			reader:  strings.NewReader("int x;\n"),
			want:    "int x;\n",
			wantErr: nil,
		}, {
			name:    "outdated notices",
			history: copyrighttest.History(2019, 2021),
			year:    2023,
			reader: strings.NewReader(
				"// Copyright (c) 2019 Example All rights reserved.\n" +
					"int x;\n" +
					"// Copyright (c) 2022 Example All rights reserved.\n",
			),
			want: "// Copyright (c) 2019, 2021, 2023 Example All rights reserved.\n" +
				"int x;\n" +
				"// Copyright (c) 2019, 2021, 2023 Example All rights reserved.\n",
			wantErr: nil,
		}, {
			name:    "history error",
			history: copyrighttest.ErrHistory(testErr),
			year:    2023,
			reader:  strings.NewReader("int x;\n"),
			want:    "",
			wantErr: testErr,
		},
	}

	for _, tc := range testCases {
		t.Run(tc.name, func(t *testing.T) {
			t.Parallel()

			// Arrange
			sut := &copyright.Checker{
				Pattern: pattern,
				History: tc.history,
				Year:    tc.year,
			}
			var w strings.Builder

			// Act
			err := sut.Fix("a.cpp", tc.reader, &w)

			// Assert
			if got, want := err, tc.wantErr; !cmp.Equal(got, want, cmpopts.EquateErrors()) {
				t.Fatalf("Checker.Fix() = %v, want %v", got, want)
			}
			if got, want := w.String(), tc.want; !cmp.Equal(got, want) {
				t.Errorf("Checker.Fix() output = %q, want %q", got, want)
			}
		})
	}
}
