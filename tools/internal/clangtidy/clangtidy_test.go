package clangtidy_test

import (
	"testing"

	"github.com/google/go-cmp/cmp"

	"github.com/bitwizeshift/alloy/tools/internal/clangtidy"
	"github.com/bitwizeshift/alloy/tools/internal/diff"
)

func TestCommandArgs(t *testing.T) {
	t.Parallel()

	testCases := []struct {
		name     string
		fix      bool
		buildDir string
		path     string
		ranges   []diff.Range
		want     []string
	}{
		{
			name:     "whole file",
			fix:      false,
			buildDir: "build",
			path:     "src/a.cpp",
			ranges:   nil,
			want:     []string{"-p=build", "src/a.cpp"},
		}, {
			name:     "changed ranges",
			fix:      false,
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
			name:     "fix",
			fix:      true,
			buildDir: "build",
			path:     "src/a.cpp",
			ranges:   []diff.Range{{Start: 3, End: 3}},
			want: []string{
				"-p=build",
				"--fix",
				`--line-filter=[{"name":"src/a.cpp","lines":[[3,3]]}]`,
				"src/a.cpp",
			},
		},
	}

	for _, tc := range testCases {
		t.Run(tc.name, func(t *testing.T) {
			t.Parallel()

			// Arrange
			sut := &clangtidy.Command{Fix: tc.fix}

			// Act
			args := sut.Args(tc.buildDir, tc.path, tc.ranges)

			// Assert
			if got, want := args, tc.want; !cmp.Equal(got, want) {
				t.Errorf("Command.Args() mismatch (-got +want):\n%s", cmp.Diff(got, want))
			}
		})
	}
}
