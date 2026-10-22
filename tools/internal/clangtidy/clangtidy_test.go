package clangtidy_test

import (
	"context"
	"testing"

	"github.com/google/go-cmp/cmp"
	"github.com/google/go-cmp/cmp/cmpopts"

	"github.com/bitwizeshift/alloy/tools/internal/clangtidy"
	"github.com/bitwizeshift/alloy/tools/internal/diff"
)

func TestCommandArgs(t *testing.T) {
	t.Parallel()

	testCases := []struct {
		name     string
		checks   string
		buildDir string
		path     string
		ranges   []diff.Range
		fixes    string
		want     []string
	}{
		{
			name:     "whole file",
			checks:   "",
			buildDir: "build",
			path:     "src/a.cpp",
			ranges:   nil,
			want:     []string{"-p=build", "src/a.cpp"},
		}, {
			name:     "changed ranges",
			checks:   "",
			buildDir: "build",
			path:     "src/a.cpp",
			ranges: []diff.Range{
				{Start: 1, End: 4},
				{Start: 21, End: 22},
			},
			want: []string{
				"-p=build",
				`--line-filter=[{"name":"src/a.cpp","lines":[[1,4],[21,22]]}]`,
				"src/a.cpp",
			},
		}, {
			name:     "fixes",
			checks:   "",
			buildDir: "build",
			path:     "src/a.cpp",
			ranges:   []diff.Range{{Start: 3, End: 3}},
			fixes:    "fixes/1.yaml",
			want: []string{
				"-p=build",
				"--export-fixes=fixes/1.yaml",
				`--line-filter=[{"name":"src/a.cpp","lines":[[3,3]]}]`,
				"src/a.cpp",
			},
		}, {
			name:     "checks",
			checks:   "-*,misc-include-cleaner",
			buildDir: "build",
			path:     "src/a.cpp",
			ranges:   nil,
			fixes:    "",
			want: []string{
				"-p=build",
				"--checks=-*,misc-include-cleaner",
				"src/a.cpp",
			},
		},
	}

	for _, tc := range testCases {
		t.Run(tc.name, func(t *testing.T) {
			t.Parallel()

			// Arrange
			sut := &clangtidy.Command{Checks: tc.checks}

			// Act
			args := sut.Args(tc.buildDir, tc.path, tc.ranges, tc.fixes)

			// Assert
			if got, want := args, tc.want; !cmp.Equal(got, want) {
				t.Errorf("Command.Args() mismatch (-got +want):\n%s", cmp.Diff(got, want))
			}
		})
	}
}

func TestCommandApplyArgs(t *testing.T) {
	t.Parallel()

	testCases := []struct {
		name  string
		style string
		want  []string
	}{
		{
			name:  "configured style",
			style: "",
			want:  []string{"--ignore-insert-conflict", "--format", "--style=file", "fixes"},
		}, {
			name:  "no formatting",
			style: "none",
			want:  []string{"--ignore-insert-conflict", "fixes"},
		}, {
			name:  "named style",
			style: "llvm",
			want:  []string{"--ignore-insert-conflict", "--format", "--style=llvm", "fixes"},
		},
	}

	for _, tc := range testCases {
		t.Run(tc.name, func(t *testing.T) {
			t.Parallel()

			// Arrange
			sut := &clangtidy.Command{FormatStyle: tc.style}

			// Act
			args := sut.ApplyArgs("fixes")

			// Assert
			if got, want := args, tc.want; !cmp.Equal(got, want) {
				t.Errorf("Command.ApplyArgs() mismatch (-got +want):\n%s", cmp.Diff(got, want))
			}
		})
	}
}

func TestCommandApplyFixes_NoFixes_DoesNothing(t *testing.T) {
	t.Parallel()

	// Arrange
	ctx := context.Background()
	sut := &clangtidy.Command{Fix: true}

	// Act
	err := sut.ApplyFixes(ctx)

	// Assert
	if got, want := err, error(nil); !cmp.Equal(got, want, cmpopts.EquateErrors()) {
		t.Fatalf("Command.ApplyFixes() = %v, want %v", got, want)
	}
}
