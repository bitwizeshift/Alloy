package repo_test

import (
	"os"
	"path/filepath"
	"testing"

	"github.com/google/go-cmp/cmp"

	"github.com/bitwizeshift/alloy/tools/internal/repo"
)

func repoRoot(t *testing.T) string {
	t.Helper()
	root, err := filepath.Abs(filepath.Join("..", "..", ".."))
	if err != nil {
		t.Fatalf("filepath.Abs() = %v", err)
	}
	resolved, err := filepath.EvalSymlinks(root)
	if err != nil {
		t.Fatalf("filepath.EvalSymlinks() = %v", err)
	}
	return resolved
}

func workingDir(t *testing.T) string {
	t.Helper()
	wd, err := os.Getwd()
	if err != nil {
		t.Fatalf("os.Getwd() = %v", err)
	}
	resolved, err := filepath.EvalSymlinks(wd)
	if err != nil {
		t.Fatalf("filepath.EvalSymlinks() = %v", err)
	}
	return resolved
}

func TestRoot_InsideWorkingTree_ReturnsTopLevel(t *testing.T) {
	t.Parallel()

	// Arrange
	want := repoRoot(t)

	// Act
	root, err := repo.Root()

	// Assert
	if err != nil {
		t.Fatalf("Root() = %v, want nil", err)
	}
	if got, want := root, want; !cmp.Equal(got, want) {
		t.Errorf("Root() = %q, want %q", got, want)
	}
}

func TestChdir_InsideWorkingTree_EntersTopLevel(t *testing.T) {
	// Arrange
	want := repoRoot(t)
	t.Chdir(".")

	// Act
	err := repo.Chdir()

	// Assert
	if err != nil {
		t.Fatalf("Chdir() = %v, want nil", err)
	}
	wd := workingDir(t)
	if got, want := wd, want; !cmp.Equal(got, want) {
		t.Errorf("Chdir() working directory = %q, want %q", got, want)
	}
}
