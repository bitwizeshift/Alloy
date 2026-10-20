package main

import (
	_ "embed"

	"github.com/bitwizeshift/go-cli"

	"github.com/bitwizeshift/alloy/tools/alloy-cli/internal/checkcmd"
	"github.com/bitwizeshift/alloy/tools/alloy-cli/internal/generatecmd"
	"github.com/bitwizeshift/alloy/tools/internal/root"
)

//go:embed alloy-cli.yaml
var specYAML []byte

func main() {
	rooter := &root.CachedRooter{Rooter: root.GitRooter{}}
	cli.FromBytes(specYAML,
		cli.BindBuilder("alloy-cli.check.commit-msg", &checkcmd.CommitMsgBuilder{Rooter: rooter}),
		cli.BindBuilder("alloy-cli.check.cpp-clang-tidy", &checkcmd.ClangTidyBuilder{Rooter: rooter}),
		cli.BindBuilder("alloy-cli.check.header-guard", checkcmd.HeaderGuard(rooter)),
		cli.BindBuilder("alloy-cli.check.license-header", &checkcmd.LicenseHeaderBuilder{Rooter: rooter}),
		cli.BindBuilder("alloy-cli.check.license-year", &checkcmd.LicenseYearBuilder{Rooter: rooter}),
		cli.BindBuilder("alloy-cli.check.trailing-newline", checkcmd.TrailingNewline(rooter)),
		cli.BindBuilder("alloy-cli.check.whitespace", checkcmd.Whitespace(rooter)),
		cli.BindBuilder("alloy-cli.generate.documentation", &generatecmd.DocumentationBuilder{Rooter: rooter}),
	).Execute()
}
