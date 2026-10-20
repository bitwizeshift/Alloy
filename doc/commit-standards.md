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

* Must contain a `Change-Category:` and a `Change-Impact:` Git Trailer to help
  categorize the commit. See [Trailers](#trailers) for more information.

* Should contain a `Component:` Git Trailer for each component that the commit
  changes.

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

`Change-Category` is a Git Trailer that identifies the type of change being
performed. Each commit must have exactly one `Change-Category`.

The following categories are supported:

* `feature` - A commit that adds new functionality.

* `fix` - A commit that fixes a bug or regression in the codebase.

* `refactor` - A commit that changes the structure of the code without changing
  its behavior.

* `docs` - A commit that changes only documentation or comments.

* `tests` - A commit that adds or changes only tests.

* `deps` - A commit that adds, removes, or updates a dependency.

* `perf` - A commit that improves performance.

* `security` - A commit that fixes or improves security.

* `chore` - A commit that does maintenance work, such as changes to tooling,
  infrastructure, or build scripts.

### `Change-Impact`

`Change-Impact` is a Git Trailer that identifies how a commit affects users of
the project. Each commit must have exactly one `Change-Impact`. Each impact maps
to a level in semantic versioning.

The following impacts are supported:

* `breaking` - A commit that introduces a change that is not backward
  compatible. This is analogous to a "major" change in semantic versioning.

* `additive` - A commit that adds new functionality in a backward compatible
  way. This is analogous to a "minor" change in semantic versioning.

* `corrective` - A commit that fixes unintended behavior without adding new
  functionality or breaking existing functionality. This is analogous to a
  "patch" change in semantic versioning.

* `none` - A commit that does not affect users of the project. Examples
  include changes to documentation, tests, or tooling.

### `Component`

`Component` is a Git Trailer that identifies which part of the project a commit
changes. A commit may have more than one `Component` trailer, if it changes
more than one component. Each value must be listed only once.

The name of a component is one of the following:

* The name of a library in `lib/`, without the `alloy-` prefix. For example,
  `lib/alloy-core` is `core`, and `lib/alloy-render-gl` is `render-gl`.

* The name of an extra in `extra/`. For example, `extra/logging` is `logging`.

* `tools` - The developer tooling in `tools/`.

## Legacy

When viewing the history of this project, you may see commits following
different standards than are outlined here. The documentation for this practice
is preserved to help understand the initial intent of the commits, but going
forward, all commits should follow the standards outlined above.

### Change Categories

Before the `Change-Impact` trailer was added, the `Change-Category` trailer
identified both the type and the impact of a change. The following categories
were used:

* `trivial` - A commit that makes a trivial change that does not affect the
  codebase in any meaningful way.

* `bugfix` - A commit that fixed a bug or regression in the codebase.

* `improvement` - A commit that improves existing functionality without
  adding new features.

* `breaking-change` - A commit that introduces a change that is not backward
  compatible.

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
