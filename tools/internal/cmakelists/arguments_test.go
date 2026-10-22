package cmakelists_test

import (
	"testing"

	"github.com/google/go-cmp/cmp"

	"github.com/bitwizeshift/alloy/tools/internal/cmakelists"
)

func TestArguments(t *testing.T) {
	t.Parallel()

	testCases := []struct {
		name string
		src  string
		want []string
	}{
		{
			name: "empty",
			src:  "",
			want: nil,
		}, {
			name: "commands",
			src:  "set(files\n  a.cpp\n  b.hpp\n)\nadd_library(x ${files})\n",
			want: []string{"set", "files", "a.cpp", "b.hpp", "add_library", "x", "${files}"},
		}, {
			name: "line comment",
			src:  "set(files # a.cpp\n  b.hpp)",
			want: []string{"set", "files", "b.hpp"},
		}, {
			name: "line comment at end",
			src:  "set(files b.hpp) # a.cpp",
			want: []string{"set", "files", "b.hpp"},
		}, {
			name: "bracket comment",
			src:  "#[[\n  a.cpp\n]]\nset(files b.hpp)",
			want: []string{"set", "files", "b.hpp"},
		}, {
			name: "bracket comment with level",
			src:  "#[==[ a.cpp ]] c.cpp ]==] b.hpp",
			want: []string{"b.hpp"},
		}, {
			name: "unterminated bracket comment",
			src:  "b.hpp #[[ a.cpp",
			want: []string{"b.hpp"},
		}, {
			name: "line comment starting with bracket",
			src:  "#[ a.cpp\nb.hpp",
			want: []string{"b.hpp"},
		}, {
			name: "quoted",
			src:  `set(files "a b.cpp" "" "say \"hi\"" "c:\\d" "\n")`,
			want: []string{"set", "files", "a b.cpp", "", `say "hi"`, `c:\d`, `\n`},
		}, {
			name: "unterminated quote",
			src:  `set(files "a.cpp`,
			want: []string{"set", "files", "a.cpp"},
		},
	}

	for _, tc := range testCases {
		t.Run(tc.name, func(t *testing.T) {
			t.Parallel()

			// Act
			args := cmakelists.Arguments(tc.src)

			// Assert
			if got, want := args, tc.want; !cmp.Equal(got, want) {
				t.Errorf("Arguments() = %q, want %q", got, want)
			}
		})
	}
}
