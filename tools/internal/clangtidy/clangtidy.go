package clangtidy

import (
	"context"
	"encoding/json"
	"errors"
	"fmt"
	"os"
	"os/exec"
	"path/filepath"
	"strconv"
	"sync"

	"github.com/bitwizeshift/alloy/tools/internal/diff"
)

// Command is a clang-tidy invocation over a single file.
//
// A Command is safe for concurrent use, and must not be copied after its first
// use.
type Command struct {
	// Dir is the directory clang-tidy is run in, which relative paths are
	// resolved against. The working directory is used when Dir is empty.
	Dir string

	// Fix exports the suggested fixes of each file, for [Command.ApplyFixes]
	// to apply once every file has been checked.
	Fix bool

	// Checks is the glob of checks that clang-tidy runs, in place of the
	// checks enabled by the .clang-tidy configuration. The configured checks
	// are run when Checks is empty.
	Checks string

	// FormatStyle is the clang-format style that the code around applied fixes
	// is formatted with, or "none" to apply them as they are. The style of the
	// .clang-format file of each fixed file is used when FormatStyle is empty.
	FormatStyle string

	mu    sync.Mutex
	fixes string
	count int
}

// Args returns the arguments to clang-tidy that check the file at path with
// the compilation database in buildDir, and report only the diagnostics on
// lines within ranges. When ranges is empty, diagnostics on every line are
// reported. When fixes is not empty, the suggested fixes are exported to the
// file fixes.
func (c *Command) Args(buildDir, path string, ranges []diff.Range, fixes string) []string {
	args := []string{"-p=" + buildDir}
	if c.Checks != "" {
		args = append(args, "--checks="+c.Checks)
	}
	if fixes != "" {
		args = append(args, "--export-fixes="+fixes)
	}
	if len(ranges) > 0 {
		args = append(args, "--line-filter="+c.lineFilter(path, ranges))
	}
	return append(args, path)
}

// Tidy runs clang-tidy over the file at path with the arguments from
// [Command.Args], and returns its combined output. With Fix, the suggested
// fixes are exported rather than applied, so that the files checked after it
// are not checked against a partly fixed tree.
//
// An error is returned if clang-tidy cannot be run, or if it reports a failure,
// such as a diagnostic promoted to an error.
func (c *Command) Tidy(ctx context.Context, buildDir, path string, ranges []diff.Range) ([]byte, error) {
	fixes, err := c.fixesFile()
	if err != nil {
		return nil, err
	}
	cmd := exec.CommandContext(ctx, "clang-tidy", c.Args(buildDir, path, ranges, fixes)...)
	cmd.Dir = c.Dir
	return cmd.CombinedOutput()
}

// ApplyArgs returns the arguments to clang-apply-replacements that apply the
// fixes exported to dir, formatted with FormatStyle. Insertions at the same
// location, such as two includes added to the same file, are all applied, as
// they are when clang-tidy applies its own fixes.
func (c *Command) ApplyArgs(dir string) []string {
	args := []string{"--ignore-insert-conflict"}
	switch c.FormatStyle {
	case "none":
	case "":
		args = append(args, "--format", "--style=file")
	default:
		args = append(args, "--format", "--style="+c.FormatStyle)
	}
	return append(args, dir)
}

// ApplyFixes applies the fixes that [Command.Tidy] has exported since the last
// call, with the arguments from [Command.ApplyArgs], and then removes them.
// Nothing is done when no fixes were exported.
//
// An error is returned if clang-apply-replacements cannot be run or fails, or
// if the exported fixes cannot be removed.
func (c *Command) ApplyFixes(ctx context.Context) error {
	c.mu.Lock()
	dir := c.fixes
	c.fixes, c.count = "", 0
	c.mu.Unlock()
	if dir == "" {
		return nil
	}
	cmd := exec.CommandContext(ctx, "clang-apply-replacements", c.ApplyArgs(dir)...)
	cmd.Dir = c.Dir
	var err error
	if output, applyErr := cmd.CombinedOutput(); applyErr != nil {
		err = fmt.Errorf("clang-apply-replacements: %w: %s", applyErr, output)
	}
	return errors.Join(err, os.RemoveAll(dir))
}

// fixesFile returns a new file to export the fixes of a single clang-tidy run
// to, or an empty string without Fix.
func (c *Command) fixesFile() (string, error) {
	if !c.Fix {
		return "", nil
	}
	c.mu.Lock()
	defer c.mu.Unlock()
	if c.fixes == "" {
		dir, err := os.MkdirTemp("", "alloy-clang-tidy-fixes-")
		if err != nil {
			return "", err
		}
		c.fixes = dir
	}
	c.count++
	return filepath.Join(c.fixes, strconv.Itoa(c.count)+".yaml"), nil
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
