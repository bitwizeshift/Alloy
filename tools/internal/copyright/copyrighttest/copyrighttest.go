package copyrighttest

import (
	"github.com/bitwizeshift/alloy/tools/internal/copyright"
)

// History returns a [copyright.History] in which every file was modified in
// each of years.
func History(years ...int) copyright.History {
	return history(func(string) ([]int, error) {
		return years, nil
	})
}

// ErrHistory returns a [copyright.History] that fails for every file with err.
func ErrHistory(err error) copyright.History {
	return history(func(string) ([]int, error) {
		return nil, err
	})
}

type history func(path string) ([]int, error)

func (h history) Years(path string) ([]int, error) {
	return h(path)
}
