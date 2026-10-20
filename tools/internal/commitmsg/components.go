package commitmsg

import (
	"fmt"
	"io/fs"
	"slices"
	"strings"
)

// Components returns the valid Component trailer values for the repository
// rooted at fsys, in sorted order. These are the name of each library under
// "lib" without its "alloy-" prefix, the name of each extra under "extra", and
// "tools".
//
// An error is returned if either directory cannot be read.
func Components(fsys fs.FS) ([]string, error) {
	components := []string{"tools"}

	libraries, err := fs.ReadDir(fsys, "lib")
	if err != nil {
		return nil, fmt.Errorf("components: %w", err)
	}
	for _, entry := range libraries {
		if name, ok := strings.CutPrefix(entry.Name(), "alloy-"); ok && entry.IsDir() {
			components = append(components, name)
		}
	}

	extras, err := fs.ReadDir(fsys, "extra")
	if err != nil {
		return nil, fmt.Errorf("components: %w", err)
	}
	for _, entry := range extras {
		if entry.IsDir() {
			components = append(components, entry.Name())
		}
	}

	slices.Sort(components)
	return slices.Compact(components), nil
}
