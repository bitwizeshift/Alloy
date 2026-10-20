// Package checkcmd implements "alloy-cli check", which verifies that files and
// commit messages follow the project's standards.
//
// File paths given to a subcommand are relative to the repository root.
// Subcommands that can repair what they find do so when given --fix.
package checkcmd
