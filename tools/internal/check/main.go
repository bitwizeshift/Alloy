package check

import (
	"errors"
	"flag"
	"fmt"
	"os"

	"github.com/bitwizeshift/alloy/tools/internal/repo"
)

// Main runs cf as a command-line tool over the files named in the program
// arguments, which are relative to the repository root.
//
// By default each file is checked, and findings are written to stderr. With
// the -fix flag, each file is fixed in place instead, and the fixed files are
// written to stdout.
//
// Main returns the process exit code: 0 on success, 1 if any finding was
// reported or any file could not be processed, and 2 for invalid usage.
func Main(cf CheckFixer) int {
	flags := flag.NewFlagSet(os.Args[0], flag.ContinueOnError)
	fix := flags.Bool("fix", false, "fix files in place instead of checking them")
	flags.Usage = func() {
		fmt.Fprintf(flags.Output(), "usage: %s [-fix] file...\n", flags.Name())
		flags.PrintDefaults()
	}
	if err := flags.Parse(os.Args[1:]); err != nil {
		return 2
	}

	if err := repo.Chdir(); err != nil {
		fmt.Fprintln(os.Stderr, err)
		return 1
	}

	err := run(cf, *fix, flags.Args())
	switch {
	case errors.Is(err, ErrFindings):
		return 1
	case err != nil:
		fmt.Fprintln(os.Stderr, err)
		return 1
	}
	return 0
}

func run(cf CheckFixer, fix bool, paths []string) error {
	if fix {
		runner := &Runner{FS: OS{}, Out: os.Stdout}
		return runner.Fix(cf, paths...)
	}
	runner := &Runner{FS: OS{}, Out: os.Stderr}
	return runner.Check(cf, paths...)
}
