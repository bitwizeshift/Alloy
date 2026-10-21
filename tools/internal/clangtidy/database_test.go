package clangtidy_test

import (
	"bytes"
	"os"
	"path/filepath"
	"strings"
	"testing"

	"github.com/google/go-cmp/cmp"
	"github.com/google/go-cmp/cmp/cmpopts"

	"github.com/bitwizeshift/alloy/tools/internal/clangtidy"
)

func TestReadDatabase(t *testing.T) {
	t.Parallel()

	testCases := []struct {
		name    string
		input   string
		want    clangtidy.Database
		wantErr error
	}{
		{
			name: "command",
			input: `[{
				"directory": "/repo/build",
				"command": "clang++ -c /repo/a.cpp",
				"file": "/repo/a.cpp",
				"output": "a.cpp.o"
			}]`,
			want: clangtidy.Database{
				{
					Directory: "/repo/build",
					File:      "/repo/a.cpp",
					Command:   "clang++ -c /repo/a.cpp",
					Output:    "a.cpp.o",
				},
			},
			wantErr: nil,
		}, {
			name: "arguments",
			input: `[{
				"directory": "/repo/build",
				"arguments": ["clang++", "-c", "/repo/a.cpp"],
				"file": "/repo/a.cpp"
			}]`,
			want: clangtidy.Database{
				{
					Directory: "/repo/build",
					File:      "/repo/a.cpp",
					Arguments: []string{"clang++", "-c", "/repo/a.cpp"},
				},
			},
			wantErr: nil,
		}, {
			name:    "not a database",
			input:   `{"directory": "/repo/build"}`,
			want:    nil,
			wantErr: clangtidy.ErrDatabase,
		},
	}

	for _, tc := range testCases {
		t.Run(tc.name, func(t *testing.T) {
			t.Parallel()

			// Arrange
			reader := strings.NewReader(tc.input)

			// Act
			db, err := clangtidy.ReadDatabase(reader)

			// Assert
			if got, want := err, tc.wantErr; !cmp.Equal(got, want, cmpopts.EquateErrors()) {
				t.Fatalf("ReadDatabase() = %v, want %v", got, want)
			}
			if got, want := db, tc.want; !cmp.Equal(got, want) {
				t.Errorf("ReadDatabase() = %v, want %v\n%s", got, want, cmp.Diff(want, got))
			}
		})
	}
}

func TestDatabaseConfig(t *testing.T) {
	t.Parallel()

	debugCommand := clangtidy.Entry{
		Directory: "/repo/build",
		File:      "/repo/a.cpp",
		Command:   `clang++ -DCMAKE_INTDIR=\"Debug\" -c /repo/a.cpp`,
	}
	releaseCommand := clangtidy.Entry{
		Directory: "/repo/build",
		File:      "/repo/a.cpp",
		Command:   `clang++ -DCMAKE_INTDIR=\"Release\" -c /repo/a.cpp`,
	}
	debugArguments := clangtidy.Entry{
		Directory: "/repo/build",
		File:      "/repo/b.cpp",
		Arguments: []string{"clang++", `-DCMAKE_INTDIR="Debug"`, "-c", "/repo/b.cpp"},
	}
	releaseArguments := clangtidy.Entry{
		Directory: "/repo/build",
		File:      "/repo/b.cpp",
		Arguments: []string{"clang++", `-DCMAKE_INTDIR="Release"`, "-c", "/repo/b.cpp"},
	}
	singleConfig := clangtidy.Entry{
		Directory: "/repo/build",
		File:      "/repo/c.cpp",
		Command:   "clang++ -c /repo/c.cpp",
	}
	testCases := []struct {
		name   string
		db     clangtidy.Database
		config string
		want   clangtidy.Database
	}{
		{
			name:   "commands",
			db:     clangtidy.Database{debugCommand, releaseCommand},
			config: "Release",
			want:   clangtidy.Database{releaseCommand},
		}, {
			name:   "arguments",
			db:     clangtidy.Database{debugArguments, releaseArguments},
			config: "Debug",
			want:   clangtidy.Database{debugArguments},
		}, {
			name:   "single config",
			db:     clangtidy.Database{singleConfig, debugCommand},
			config: "Release",
			want:   clangtidy.Database{singleConfig},
		}, {
			name:   "unknown config",
			db:     clangtidy.Database{debugCommand, releaseCommand},
			config: "MSan",
			want:   nil,
		},
	}

	for _, tc := range testCases {
		t.Run(tc.name, func(t *testing.T) {
			t.Parallel()

			// Arrange
			sut := tc.db

			// Act
			db := sut.Config(tc.config)

			// Assert
			if got, want := db, tc.want; !cmp.Equal(got, want) {
				t.Errorf("Database.Config() = %v, want %v\n%s", got, want, cmp.Diff(want, got))
			}
		})
	}
}

func TestDatabaseWrite(t *testing.T) {
	t.Parallel()

	// Arrange
	var buffer bytes.Buffer
	sut := clangtidy.Database{
		{
			Directory: "/repo/build",
			File:      "/repo/a.cpp",
			Command:   "clang++ -c /repo/a.cpp",
		},
	}

	// Act
	err := sut.Write(&buffer)

	// Assert
	if got, want := err, error(nil); !cmp.Equal(got, want, cmpopts.EquateErrors()) {
		t.Fatalf("Database.Write() = %v, want %v", got, want)
	}
	want := `[
  {
    "directory": "/repo/build",
    "file": "/repo/a.cpp",
    "command": "clang++ -c /repo/a.cpp"
  }
]
`
	if got, want := buffer.String(), want; !cmp.Equal(got, want) {
		t.Errorf("Database.Write() = %q, want %q", got, want)
	}
}

func TestConfigDatabaseWriteDatabase(t *testing.T) {
	t.Parallel()

	// Arrange
	sut := &clangtidy.ConfigDatabase{
		Dir:      "testdata",
		BuildDir: "multi-config",
		Config:   "Release",
	}

	// Act
	dir, remove, err := sut.WriteDatabase()

	// Assert
	if got, want := err, error(nil); !cmp.Equal(got, want, cmpopts.EquateErrors()) {
		t.Fatalf("ConfigDatabase.WriteDatabase() = %v, want %v", got, want)
	}
	t.Cleanup(func() { _ = remove() })
	db := readDatabase(t, filepath.Join(dir, clangtidy.DatabaseFile))
	want := clangtidy.Database{
		{
			Directory: "/repo/build",
			File:      "/repo/a.cpp",
			Command:   `/usr/bin/clang++ -DCMAKE_INTDIR=\"Release\" -O3 -o a.dir/Release/a.cpp.o -c /repo/a.cpp`,
			Output:    "a.dir/Release/a.cpp.o",
		},
	}
	if got, want := db, want; !cmp.Equal(got, want) {
		t.Errorf("ConfigDatabase.WriteDatabase() database = %v, want %v\n%s", got, want, cmp.Diff(want, got))
	}
}

func TestConfigDatabaseWriteDatabase_Removed_DeletesDirectory(t *testing.T) {
	t.Parallel()

	// Arrange
	sut := &clangtidy.ConfigDatabase{
		Dir:      "testdata",
		BuildDir: "multi-config",
		Config:   "Debug",
	}
	dir, remove := writeDatabase(t, sut)

	// Act
	err := remove()

	// Assert
	if got, want := err, error(nil); !cmp.Equal(got, want, cmpopts.EquateErrors()) {
		t.Fatalf("ConfigDatabase.WriteDatabase() remove = %v, want %v", got, want)
	}
	_, statErr := os.Stat(dir)
	if got, want := statErr, os.ErrNotExist; !cmp.Equal(got, want, cmpopts.EquateErrors()) {
		t.Errorf("os.Stat() = %v, want %v", got, want)
	}
}

func TestConfigDatabaseWriteDatabase_MissingDatabase_ReturnsBuildDir(t *testing.T) {
	t.Parallel()

	// Arrange
	sut := &clangtidy.ConfigDatabase{
		Dir:      "testdata",
		BuildDir: "missing",
		Config:   "Debug",
	}

	// Act
	dir, remove, err := sut.WriteDatabase()

	// Assert
	if got, want := err, error(nil); !cmp.Equal(got, want, cmpopts.EquateErrors()) {
		t.Fatalf("ConfigDatabase.WriteDatabase() = %v, want %v", got, want)
	}
	if got, want := dir, filepath.Join("testdata", "missing"); !cmp.Equal(got, want) {
		t.Errorf("ConfigDatabase.WriteDatabase() dir = %q, want %q", got, want)
	}
	removeErr := remove()
	if got, want := removeErr, error(nil); !cmp.Equal(got, want, cmpopts.EquateErrors()) {
		t.Errorf("ConfigDatabase.WriteDatabase() remove = %v, want %v", got, want)
	}
}

func TestConfigDatabaseWriteDatabase_InvalidDatabase_ReturnsError(t *testing.T) {
	t.Parallel()

	// Arrange
	sut := &clangtidy.ConfigDatabase{
		Dir:      "testdata",
		BuildDir: "invalid",
		Config:   "Debug",
	}

	// Act
	_, _, err := sut.WriteDatabase()

	// Assert
	if got, want := err, clangtidy.ErrDatabase; !cmp.Equal(got, want, cmpopts.EquateErrors()) {
		t.Fatalf("ConfigDatabase.WriteDatabase() = %v, want %v", got, want)
	}
}

// writeDatabase writes the database of cd, and returns its directory with the
// function that removes it.
func writeDatabase(t *testing.T, cd *clangtidy.ConfigDatabase) (string, func() error) {
	t.Helper()
	dir, remove, err := cd.WriteDatabase()
	if err != nil {
		t.Fatalf("ConfigDatabase.WriteDatabase() = %v", err)
	}
	return dir, remove
}

// readDatabase reads the compilation database at path.
func readDatabase(t *testing.T, path string) clangtidy.Database {
	t.Helper()
	file, err := os.Open(path)
	if err != nil {
		t.Fatalf("os.Open() = %v", err)
	}
	defer file.Close()
	db, err := clangtidy.ReadDatabase(file)
	if err != nil {
		t.Fatalf("ReadDatabase() = %v", err)
	}
	return db
}
