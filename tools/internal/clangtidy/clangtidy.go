package clangtidy

import (
	"context"
	"encoding/json"
	"os/exec"

	"github.com/bitwizeshift/alloy/tools/internal/diff"
)

// Command is a clang-tidy invocation over a single file.
type Command struct {
	// Dir is the directory clang-tidy is run in, which relative paths are
	// resolved against. The working directory is used when Dir is empty.
	Dir string

	// Fix applies the suggested fixes to the file in place.
	Fix bool
}

// Args returns the arguments to clang-tidy that check the file at path with
// the compilation database in buildDir, and report only the diagnostics on
// lines within ranges. When ranges is empty, diagnostics on every line are
// reported.
func (c *Command) Args(buildDir, path string, ranges []diff.Range) []string {
	args := []string{"-p=" + buildDir}
	if c.Fix {
		args = append(args, "--fix")
	}
	if len(ranges) > 0 {
		args = append(args, "--line-filter="+c.lineFilter(path, ranges))
	}
	return append(args, path)
}

// Tidy runs clang-tidy over the file at path with the arguments from
// [Command.Args], and returns its combined output.
//
// An error is returned if clang-tidy cannot be run, or if it reports a failure,
// such as a diagnostic promoted to an error.
func (c *Command) Tidy(ctx context.Context, buildDir, path string, ranges []diff.Range) ([]byte, error) {
	cmd := exec.CommandContext(ctx, "clang-tidy", c.Args(buildDir, path, ranges)...)
	cmd.Dir = c.Dir
	return cmd.CombinedOutput()
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
