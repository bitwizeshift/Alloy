package clangtidy

import (
	"encoding/json"
	"errors"
	"fmt"
	"io"
	"io/fs"
	"os"
	"path/filepath"
	"strings"
)

// ErrDatabase is returned when a compilation database cannot be decoded.
var ErrDatabase = errors.New("invalid compilation database")

// DatabaseFile is the name of the compilation database within a build
// directory.
const DatabaseFile = "compile_commands.json"

// Entry is the compilation of a single translation unit in a [Database].
type Entry struct {
	Directory string   `json:"directory"`
	File      string   `json:"file"`
	Command   string   `json:"command,omitempty"`
	Arguments []string `json:"arguments,omitempty"`
	Output    string   `json:"output,omitempty"`
}

// config returns the configuration that the entry is compiled for, which
// multi-config CMake generators define as CMAKE_INTDIR in every command.
func (e *Entry) config() (string, bool) {
	args := e.Arguments
	if len(args) == 0 {
		args = strings.Fields(e.Command)
	}
	for _, arg := range args {
		if value, ok := strings.CutPrefix(arg, "-DCMAKE_INTDIR="); ok {
			return strings.Trim(value, `\"`), true
		}
	}
	return "", false
}

// Database is a compilation database, as read from a compile_commands.json.
type Database []Entry

// ReadDatabase reads a [Database] from the JSON in r.
//
// [ErrDatabase] is returned if r does not contain a compilation database.
func ReadDatabase(r io.Reader) (Database, error) {
	var db Database
	if err := json.NewDecoder(r).Decode(&db); err != nil {
		return nil, fmt.Errorf("%w: %w", ErrDatabase, err)
	}
	return db, nil
}

// Config returns the entries of the database that are compiled for config.
// Entries that are not compiled for any specific configuration, such as those
// of a single-config generator, are always kept.
func (db Database) Config(config string) Database {
	var result Database
	for _, entry := range db {
		if c, ok := entry.config(); !ok || c == config {
			result = append(result, entry)
		}
	}
	return result
}

// Covers reports whether the database compiles any file in the project
// directory of path, which is relative to dir. The project directory is the
// first two directories of path, such as lib/alloy-core or
// extra/sdl2-bindings, which hold the sources and headers of a library.
// Headers have no entries of their own, so a header is covered by the sources
// compiled beside it.
func (db Database) Covers(dir, path string) bool {
	project := projectDir(filepath.ToSlash(path))
	for _, entry := range db {
		file := entry.File
		if !filepath.IsAbs(file) {
			file = filepath.Join(entry.Directory, file)
		}
		rel, err := filepath.Rel(dir, file)
		if err != nil {
			continue
		}
		if project == "" || strings.HasPrefix(filepath.ToSlash(rel), project+"/") {
			return true
		}
	}
	return false
}

func projectDir(path string) string {
	parts := strings.Split(path, "/")
	parts = parts[:len(parts)-1]
	return strings.Join(parts[:min(len(parts), 2)], "/")
}

// Write writes the database to w as JSON.
//
// An error is returned if w cannot be written to.
func (db Database) Write(w io.Writer) error {
	encoder := json.NewEncoder(w)
	encoder.SetIndent("", "  ")
	return encoder.Encode(db)
}

// ConfigDatabase writes the entries of a single configuration from the
// compilation database of a build directory to a new compilation database.
//
// Multi-config generators write an entry for every configuration, and
// clang-tidy checks a file once for each of its entries.
type ConfigDatabase struct {
	// Dir is the directory that a relative BuildDir is resolved against.
	Dir string

	// BuildDir is the directory that contains compile_commands.json.
	BuildDir string

	// Config is the configuration whose entries are kept.
	Config string

	db    Database
	found bool
}

// WriteDatabase writes the entries of Config to a compile_commands.json in a
// new temporary directory, and returns that directory with a function that
// removes it.
//
// When BuildDir has no compile_commands.json, BuildDir itself is returned with
// a function that does nothing, so that clang-tidy reports the missing
// database as it would otherwise.
//
// [ErrDatabase] is returned if the compilation database cannot be decoded. Any
// other error is returned if it cannot be read, or if the new one cannot be
// written.
func (cd *ConfigDatabase) WriteDatabase() (string, func() error, error) {
	buildDir := cd.buildDir()
	db, err := cd.read(filepath.Join(buildDir, DatabaseFile))
	if errors.Is(err, fs.ErrNotExist) {
		return buildDir, func() error { return nil }, nil
	}
	if err != nil {
		return "", nil, err
	}
	cd.db, cd.found = db.Config(cd.Config), true

	dir, err := os.MkdirTemp("", "alloy-clang-tidy-")
	if err != nil {
		return "", nil, err
	}
	remove := func() error { return os.RemoveAll(dir) }
	if err := cd.write(filepath.Join(dir, DatabaseFile), cd.db); err != nil {
		return "", nil, errors.Join(err, remove())
	}
	return dir, remove, nil
}

// Covers reports whether the database last written by
// [ConfigDatabase.WriteDatabase] covers the file at path, relative to Dir, as
// described by [Database.Covers]. Every file is covered when no database was
// found, so that clang-tidy reports the missing database for each of them.
func (cd *ConfigDatabase) Covers(path string) bool {
	if !cd.found {
		return true
	}
	dir, err := filepath.Abs(cd.Dir)
	if err != nil {
		return true
	}
	return cd.db.Covers(dir, path)
}

func (cd *ConfigDatabase) buildDir() string {
	if filepath.IsAbs(cd.BuildDir) {
		return cd.BuildDir
	}
	return filepath.Join(cd.Dir, cd.BuildDir)
}

func (*ConfigDatabase) read(path string) (Database, error) {
	file, err := os.Open(path)
	if err != nil {
		return nil, err
	}
	defer file.Close()
	return ReadDatabase(file)
}

func (*ConfigDatabase) write(path string, db Database) error {
	file, err := os.Create(path)
	if err != nil {
		return err
	}
	return errors.Join(db.Write(file), file.Close())
}
