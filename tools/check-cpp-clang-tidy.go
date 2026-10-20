//go:build ignore

// Command check-cpp-clang-tidy runs clang-tidy over the C and C++ files given
// as arguments, reporting only the diagnostics on lines near uncommitted
// changes. With -fix, the suggested fixes are applied in place.
//
// Usage:
//
//	go -C tools run ./check-cpp-clang-tidy.go [-fix] [-p dir] [-j n] file...
//
// File paths are relative to the repository root. Diagnostics are limited to
// the lines of each file that differ from HEAD, plus five lines of context. A
// file without such changes is checked in full.
package main

import (
	"context"
	"errors"
	"flag"
	"fmt"
	"io/fs"
	"os"
	"path/filepath"
	"runtime"
	"sync"

	"github.com/bitwizeshift/alloy/tools/internal/clangtidy"
	"github.com/bitwizeshift/alloy/tools/internal/diff"
	"github.com/bitwizeshift/alloy/tools/internal/repo"
)

const contextLines = 5

func main() {
	os.Exit(run())
}

func run() int {
	flag.Usage = func() {
		fmt.Fprintf(flag.CommandLine.Output(), "usage: %s [-fix] [-p dir] [-j n] file...\n", os.Args[0])
		flag.PrintDefaults()
	}
	buildDir := flag.String("p", "build", "the build directory containing compile_commands.json")
	fix := flag.Bool("fix", false, "apply suggested fixes in place")
	jobs := flag.Int("j", runtime.NumCPU(), "the number of files to check at once")
	flag.Parse()

	if err := repo.Chdir(); err != nil {
		fmt.Fprintln(os.Stderr, err)
		return 1
	}
	warnMissingDatabase(*buildDir)

	ranges, err := changedRanges(flag.Args())
	if err != nil {
		fmt.Fprintln(os.Stderr, err)
		return 1
	}

	cmd := &clangtidy.Command{BuildDir: *buildDir, Fix: *fix}
	results := tidy(cmd, ranges, flag.Args(), max(*jobs, 1))

	code := 0
	for _, result := range results {
		if result.err != nil {
			os.Stderr.Write(result.output)
			fmt.Fprintf(os.Stderr, "%s: %v\n", result.path, result.err)
			code = 1
		}
	}
	return code
}

func warnMissingDatabase(buildDir string) {
	database := filepath.Join(buildDir, "compile_commands.json")
	if _, err := os.Stat(database); errors.Is(err, fs.ErrNotExist) {
		fmt.Fprintf(os.Stderr, "warning: compilation database not found at %s\n", database)
	}
}

// changedRanges returns the lines of each path that differ from HEAD.
func changedRanges(paths []string) (map[string][]diff.Range, error) {
	git := diff.Git{Context: contextLines}
	files, err := git.Changes(context.Background(), paths...)
	if err != nil {
		return nil, err
	}

	ranges := make(map[string][]diff.Range, len(files))
	for _, file := range files {
		ranges[file.Path] = file.Ranges
	}
	return ranges, nil
}

type result struct {
	path   string
	output []byte
	err    error
}

// tidy runs clang-tidy over each of paths with up to jobs running at once, and
// returns the results in the same order as paths.
func tidy(cmd *clangtidy.Command, ranges map[string][]diff.Range, paths []string, jobs int) []result {
	results := make([]result, len(paths))
	limit := make(chan struct{}, jobs)
	var wg sync.WaitGroup
	for i, path := range paths {
		wg.Go(func() {
			limit <- struct{}{}
			defer func() { <-limit }()

			output, err := cmd.Tidy(context.Background(), path, ranges[path])
			results[i] = result{path: path, output: output, err: err}
		})
	}
	wg.Wait()
	return results
}
