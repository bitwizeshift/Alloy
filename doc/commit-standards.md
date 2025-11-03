# Commit Standards

The **Alloy** project has some some requirements on how git commit
messages are formed.

**General:**

* Commits should be small, granular, and easy to follow and revert. Ideally,
  the same SOC practices that would be applied to software development should
  be applied to commits; each commit identifies a separtion of concerns.

**Commit titles:**

* must be in imperitive, present-tense

* must be no longer than 50 characters

* must summarize the change being performed

**Commit messages:**

* Must not exceed 72-character wide

  * Exceptions are made if a message contains a link or other figure that
    exceeds the 72 character rule by nature

* Must indicate the rationale for the change, what was changed, and why

  * In general, more details are always better to help identify the cause of
    changes in a repository

* Should contain a `Change-Category:` Git Trailer to help categorize the commit.
  See [Trailers](#trailers) for more information.

## Trailers

### `Fixes`

The `Fixes` Git Trailer is used to indicate that a commit resolves an issue
tracked in the project's issue tracker.

This borrows from GitHub's automatic issue closing syntax. The format is as
follows:

```text
Fixes: #<issue-number>
```

### `Change-Category`

`Change-Category` is a Git Trailer that may be added to commit messages to help
categorize the type of change being performed.

The following categories are supported:

* `trivial` - A commit that makes a trivial change that does not affect the
  codebase in any meaningful way. Examples include changes to documentation,
  comments, infrastructure, or other non-functional changes.

* `bugfix` - A commit that fixed a bug or regression in the codebase. Commits
  documented with this category should only be fixing unintended behaviors
  without adding new functionality or breaking existing functionality. This is
  analogous to a "patch" change in semantic versioning.

* `improvement` - A commit that improves existing functionality without
  adding new features. This is analogous to a "minor" change in semantic
  versioning.

* `breaking-change` - A commit that introduces a change that is not backward
  compatible. This is analogous to a "major" change in semantic versioning.

## Legacy

When viewing the history of this project, you may see commits following
different standards than are outlined here. The documentation for this practice
is preserved to help understand the initial intent of the commits, but going
forward, all commits should follow the standards outlined above.

### Emoji Keys

Emojis were used to prefix commit titles in order to simplify categorization
of git log messages.

The table below identifies which prefixes should be used for the respective
change:

| Emoji | Reason                                                              |
|---|-------------------------------------------------------------------------|
| 🔖 | Version Tag                                                            |
| 📖 | Documentation / Textual Changes                                        |
| 📇 | Metadata (README, LICENSE, repo docs, etc)                             |
| 🚦 | Continuous Integration                                                 |
| ✨ | New Feature                                                             |
| ✏ | Rename                                                                  |
| 🔨 | Refactor                                                               |
| ⚠ | Deprecation                                                             |
| 🗑️ | Removal                                                               |
| 🎨 | Cosmetic                                                               |
| 🩹 | Bug fix                                                                |
| 🧹 | Code Cleanup (includes moving types/files around)                      |
| ⏱ | Tuning / Performance                                                    |
| 🎯 | Testing (unit, benchmark, integration, etc)                            |
| 🔧 | Tooling                                                                |
| 🔐 | Security                                                               |
| ♿ | Accessibility                                                           |
| 🌐 | Localization / Internationalization                                    |
| 🚧 | WIP                                                                    |
