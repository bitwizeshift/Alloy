package checkcmd_test

import (
	"context"
	"errors"
	"regexp"
	"strings"
	"testing"
	"time"

	"github.com/google/go-cmp/cmp"
	"github.com/google/go-cmp/cmp/cmpopts"

	"github.com/bitwizeshift/go-cli"
	"github.com/bitwizeshift/go-cli/arg"
	"github.com/bitwizeshift/go-cli/arg/argtest"

	"github.com/bitwizeshift/alloy/tools/alloy-cli/internal/checkcmd"
	"github.com/bitwizeshift/alloy/tools/internal/check"
	"github.com/bitwizeshift/alloy/tools/internal/license"
	"github.com/bitwizeshift/alloy/tools/internal/root/roottest"
)

func TestLicenseRules(t *testing.T) {
	t.Parallel()

	testCases := []struct {
		name    string
		path    string
		content string
		want    []check.Finding
	}{
		{
			name:    "source with MIT header",
			path:    "lib/alloy-core/src/alloy/core/assert.cpp",
			content: checkcmd.MITHeader + "\nint x;\n",
			want:    nil,
		}, {
			name:    "header with MIT header",
			path:    "lib/alloy-core/include/alloy/core/assert.hpp",
			content: checkcmd.MITHeader,
			want:    nil,
		}, {
			name:    "source with public domain header",
			path:    "lib/alloy-core/src/alloy/core/assert.cpp",
			content: checkcmd.PublicDomainHeader,
			want:    []check.Finding{{Line: 1, Message: "missing license header"}},
		}, {
			name:    "test with public domain header",
			path:    "lib/alloy-core/test/src/alloy/core/assert.test.cpp",
			content: checkcmd.PublicDomainHeader,
			want:    nil,
		}, {
			name:    "benchmark with public domain header",
			path:    "lib/alloy-core/benchmark/src/main.cpp",
			content: checkcmd.PublicDomainHeader,
			want:    nil,
		}, {
			name:    "test with MIT header",
			path:    "lib/alloy-core/test/src/alloy/core/assert.test.cpp",
			content: checkcmd.MITHeader,
			want:    []check.Finding{{Line: 1, Message: "missing license header"}},
		}, {
			name:    "other file",
			path:    "README.md",
			content: "# Alloy\n",
			want:    nil,
		},
	}

	for _, tc := range testCases {
		t.Run(tc.name, func(t *testing.T) {
			t.Parallel()

			// Arrange
			sut := &license.HeaderChecker{Rules: checkcmd.LicenseRules()}
			r := strings.NewReader(tc.content)

			// Act
			findings, err := sut.Check(tc.path, r)

			// Assert
			if got, want := err, error(nil); !cmp.Equal(got, want, cmpopts.EquateErrors()) {
				t.Fatalf("HeaderChecker.Check() = %v, want %v", got, want)
			}
			if got, want := findings, tc.want; !cmp.Equal(got, want) {
				t.Errorf("HeaderChecker.Check() = %v, want %v", got, want)
			}
		})
	}
}

func TestLicenseHeaderBuilderRegisterArgs(t *testing.T) {
	t.Parallel()

	// Arrange
	cl := argtest.NewCommandLine()
	sut := &checkcmd.LicenseHeaderBuilder{Rooter: roottest.Rooter("/repo")}

	// Act
	arg.Register(cl, sut)

	// Assert
	flags := argtest.LongFlags(cl)
	unmatched := argtest.GetUnmatched(cl)
	if got, want := flags, []string(nil); !cmp.Equal(got, want, cmpopts.EquateEmpty()) {
		t.Errorf("LicenseHeaderBuilder.RegisterArgs() flags = %v, want %v", got, want)
	}
	wantUnmatched := &argtest.Unmatched{Name: "file", Required: true}
	if got, want := unmatched, wantUnmatched; !cmp.Equal(got, want, cmpopts.IgnoreFields(argtest.Unmatched{}, "Usage")) {
		t.Errorf("LicenseHeaderBuilder.RegisterArgs() unmatched = %v, want %v", got, want)
	}
}

func TestLicenseHeaderBuilderBuild(t *testing.T) {
	t.Parallel()

	testErr := errors.New("test error")
	testCases := []struct {
		name    string
		builder *checkcmd.LicenseHeaderBuilder
		args    []string
		want    cli.Runner
		wantErr error
	}{
		{
			name:    "files",
			builder: &checkcmd.LicenseHeaderBuilder{Rooter: roottest.Rooter("/repo")},
			args:    []string{"a.cpp", "b.hpp"},
			want: &checkcmd.CheckRunner{
				Checker: &license.HeaderChecker{Rules: checkcmd.LicenseRules()},
				FS:      check.OS{Dir: "/repo"},
				Paths:   []string{"a.cpp", "b.hpp"},
			},
			wantErr: nil,
		}, {
			name:    "rooter error",
			builder: &checkcmd.LicenseHeaderBuilder{Rooter: roottest.ErrRooter(testErr)},
			args:    []string{"a.cpp"},
			want:    nil,
			wantErr: testErr,
		},
	}

	for _, tc := range testCases {
		t.Run(tc.name, func(t *testing.T) {
			t.Parallel()

			// Arrange
			ctx := context.Background()
			sut := tc.builder
			parseArgs(t, sut, tc.args...)

			// Act
			runner, err := sut.Build(ctx)

			// Assert
			if got, want := err, tc.wantErr; !cmp.Equal(got, want, cmpopts.EquateErrors()) {
				t.Fatalf("LicenseHeaderBuilder.Build() = %v, want %v", got, want)
			}
			if got, want := runner, tc.want; !cmp.Equal(got, want, compareRegexps()) {
				t.Errorf("LicenseHeaderBuilder.Build() = %v, want %v", got, want)
			}
		})
	}
}

func TestLicenseYearBuilderRegisterArgs(t *testing.T) {
	t.Parallel()

	// Arrange
	cl := argtest.NewCommandLine()
	sut := &checkcmd.LicenseYearBuilder{Rooter: roottest.Rooter("/repo")}

	// Act
	arg.Register(cl, sut)

	// Assert
	flags := argtest.LongFlags(cl)
	unmatched := argtest.GetUnmatched(cl)
	if got, want := flags, []string{"fix"}; !cmp.Equal(got, want) {
		t.Errorf("LicenseYearBuilder.RegisterArgs() flags = %v, want %v", got, want)
	}
	if got, want := unmatched, (*argtest.Unmatched)(nil); !cmp.Equal(got, want) {
		t.Errorf("LicenseYearBuilder.RegisterArgs() unmatched = %v, want %v", got, want)
	}
}

func TestLicenseYearBuilderBuild(t *testing.T) {
	t.Parallel()

	testErr := errors.New("test error")
	testCases := []struct {
		name    string
		builder *checkcmd.LicenseYearBuilder
		args    []string
		want    cli.Runner
		wantErr error
	}{
		{
			name:    "check",
			builder: &checkcmd.LicenseYearBuilder{Rooter: roottest.Rooter("/repo")},
			args:    nil,
			want: &checkcmd.FileRunner{
				Checker: &license.YearChecker{Year: time.Now().Year()},
				FS:      check.OS{Dir: "/repo"},
				Fix:     false,
				Paths:   []string{"LICENSE"},
			},
			wantErr: nil,
		}, {
			name:    "fix",
			builder: &checkcmd.LicenseYearBuilder{Rooter: roottest.Rooter("/repo")},
			args:    []string{"--fix"},
			want: &checkcmd.FileRunner{
				Checker: &license.YearChecker{Year: time.Now().Year()},
				FS:      check.OS{Dir: "/repo"},
				Fix:     true,
				Paths:   []string{"LICENSE"},
			},
			wantErr: nil,
		}, {
			name:    "rooter error",
			builder: &checkcmd.LicenseYearBuilder{Rooter: roottest.ErrRooter(testErr)},
			args:    nil,
			want:    nil,
			wantErr: testErr,
		},
	}

	for _, tc := range testCases {
		t.Run(tc.name, func(t *testing.T) {
			t.Parallel()

			// Arrange
			ctx := context.Background()
			sut := tc.builder
			parseArgs(t, sut, tc.args...)

			// Act
			runner, err := sut.Build(ctx)

			// Assert
			if got, want := err, tc.wantErr; !cmp.Equal(got, want, cmpopts.EquateErrors()) {
				t.Fatalf("LicenseYearBuilder.Build() = %v, want %v", got, want)
			}
			if got, want := runner, tc.want; !cmp.Equal(got, want) {
				t.Errorf("LicenseYearBuilder.Build() = %v, want %v", got, want)
			}
		})
	}
}

func compareRegexps() cmp.Option {
	return cmp.Comparer(func(lhs, rhs *regexp.Regexp) bool {
		return lhs.String() == rhs.String()
	})
}
