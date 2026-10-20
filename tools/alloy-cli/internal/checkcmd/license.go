package checkcmd

import (
	"context"
	"regexp"
	"time"

	"github.com/bitwizeshift/go-cli"
	"github.com/bitwizeshift/go-cli/arg"

	"github.com/bitwizeshift/alloy/tools/internal/check"
	"github.com/bitwizeshift/alloy/tools/internal/license"
	"github.com/bitwizeshift/alloy/tools/internal/root"
)

// MITHeader is the license header that every MIT licensed C and C++ file
// starts with.
const MITHeader = `// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.
`

// CMakeMITHeader is the license header that every CMake module starts with.
// It holds the same lines as [MITHeader], in a bracket comment.
const CMakeMITHeader = `#[[
  Copyright (c) Matthew Rodusek
  SPDX-License-Identifier: MIT
  See the LICENSE file at the root of this repository for the full text.
]]
`

// PublicDomainHeader is the license header that every C and C++ test and
// benchmark file starts with.
const PublicDomainHeader = `// SPDX-License-Identifier: CC0-1.0
// Any copyright for this file is dedicated to the Public Domain.
// See https://creativecommons.org/publicdomain/zero/1.0/ for the full text.
`

// LicenseRules returns the license headers of the repository's C, C++ and
// CMake files: C and C++ tests and benchmarks are dedicated to the public
// domain, and every other file is MIT licensed.
func LicenseRules() []license.Rule {
	return []license.Rule{
		{
			Path:   regexp.MustCompile(`(^|/)(test|benchmark)/.*\.(h|hpp|c|cpp)$`),
			Header: PublicDomainHeader,
		}, {
			Path:   regexp.MustCompile(`\.(h|hpp|c|cpp)$`),
			Header: MITHeader,
		}, {
			Path:   regexp.MustCompile(`\.cmake$`),
			Header: CMakeMITHeader,
		},
	}
}

// LicenseHeaderBuilder builds a [CheckRunner] that checks that the files named
// on the command line start with their license header from [LicenseRules].
type LicenseHeaderBuilder struct {
	// Rooter reports the repository root that file paths are relative to.
	Rooter root.Rooter

	paths []string
}

// RegisterArgs registers the files to check.
func (lhb *LicenseHeaderBuilder) RegisterArgs(cl *arg.CommandLine) {
	cl.Add(
		arg.Unmatched("file", &lhb.paths,
			arg.Usage("the files to check, relative to the repository root"),
			arg.Required(),
			arg.CompleteFilesMatching("h", "hpp", "c", "cpp", "cmake"),
		),
	)
}

// Build returns a [CheckRunner] over the parsed files, reading them relative to
// the repository root.
//
// An error is returned if the repository root cannot be determined.
func (lhb *LicenseHeaderBuilder) Build(ctx context.Context) (cli.Runner, error) {
	dir, err := lhb.Rooter.Root(ctx)
	if err != nil {
		return nil, err
	}
	return &CheckRunner{
		Checker: &license.HeaderChecker{Rules: LicenseRules()},
		FS:      check.OS{Dir: dir},
		Paths:   lhb.paths,
	}, nil
}

var (
	_ cli.Builder   = (*LicenseHeaderBuilder)(nil)
	_ arg.Registrar = (*LicenseHeaderBuilder)(nil)
)

// LicenseYearBuilder builds a [FileRunner] that checks that the copyright
// years of the LICENSE file at the repository root are a single range ending
// in the current year.
type LicenseYearBuilder struct {
	// Rooter reports the repository root that contains the LICENSE file.
	Rooter root.Rooter

	fix bool
}

// RegisterArgs registers the --fix flag.
func (lyb *LicenseYearBuilder) RegisterArgs(cl *arg.CommandLine) {
	cl.Add(
		arg.Flag("fix", &lyb.fix,
			arg.Usage("fix the LICENSE file in place instead of reporting problems"),
		),
	)
}

// Build returns a [FileRunner] over the LICENSE file at the repository root.
//
// An error is returned if the repository root cannot be determined.
func (lyb *LicenseYearBuilder) Build(ctx context.Context) (cli.Runner, error) {
	dir, err := lyb.Rooter.Root(ctx)
	if err != nil {
		return nil, err
	}
	return &FileRunner{
		Checker: &license.YearChecker{Year: time.Now().Year()},
		FS:      check.OS{Dir: dir},
		Fix:     lyb.fix,
		Paths:   []string{"LICENSE"},
	}, nil
}

var (
	_ cli.Builder   = (*LicenseYearBuilder)(nil)
	_ arg.Registrar = (*LicenseYearBuilder)(nil)
)
