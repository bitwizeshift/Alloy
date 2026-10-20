package clangtidy

import (
	"encoding/json"

	"github.com/bitwizeshift/alloy/tools/internal/diff"
)

// Command is a clang-tidy invocation over a single file.
type Command struct {
	// BuildDir is the directory that contains compile_commands.json.
	BuildDir string

	// Fix applies the suggested fixes to the file in place.
	Fix bool
}

// Args returns the arguments to clang-tidy that check the file at path, and
// report only the diagnostics on lines within ranges. When ranges is empty,
// diagnostics on every line are reported.
func (c *Command) Args(path string, ranges []diff.Range) []string {
	args := []string{"-p=" + c.BuildDir}
	if c.Fix {
		args = append(args, "--fix")
	}
	if len(ranges) > 0 {
		args = append(args, "--line-filter="+c.lineFilter(path, ranges))
	}
	return append(args, path)
}

type lineFilter struct {
	Name  string   `json:"name"`
	Lines [][2]int `json:"lines"`
}

func (*Command) lineFilter(path string, ranges []diff.Range) string {
	filter := lineFilter{Name: path}
	for _, r := range ranges {
		filter.Lines = append(filter.Lines, [2]int{r.Start, r.End})
	}
	// Marshaling cannot fail for a struct of strings and integers.
	data, _ := json.Marshal([]lineFilter{filter})
	return string(data)
}
