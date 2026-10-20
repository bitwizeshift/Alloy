package copyright

import (
	"bufio"
	"errors"
	"fmt"
	"io"
	"slices"
	"strconv"
	"strings"
)

// ErrBadYear is returned by [ParseYears] when a line is not a year.
var ErrBadYear = errors.New("invalid year")

// ParseYears reads one year per line from r, ignoring blank lines.
//
// [ErrBadYear] is returned if a line is not an integer. Any other error is
// returned if r cannot be read.
func ParseYears(r io.Reader) ([]int, error) {
	var years []int
	scanner := bufio.NewScanner(r)
	for scanner.Scan() {
		line := strings.TrimSpace(scanner.Text())
		if line == "" {
			continue
		}
		year, err := strconv.Atoi(line)
		if err != nil {
			return nil, fmt.Errorf("parse years: %w %q", ErrBadYear, line)
		}
		years = append(years, year)
	}
	if err := scanner.Err(); err != nil {
		return nil, err
	}
	return years, nil
}

// Format returns years as a comma-separated list, in ascending order, where
// consecutive years are joined into an inclusive range. For example, the years
// 2023, 2019, 2020, and 2021 are formatted as "2019-2021, 2023".
//
// Duplicate years are ignored, and an empty list is formatted as "".
func Format(years []int) string {
	sorted := slices.Clone(years)
	slices.Sort(sorted)
	sorted = slices.Compact(sorted)

	var ranges []string
	for len(sorted) > 0 {
		n := 1
		for n < len(sorted) && sorted[n] == sorted[n-1]+1 {
			n++
		}
		ranges = append(ranges, formatRange(sorted[0], sorted[n-1]))
		sorted = sorted[n:]
	}
	return strings.Join(ranges, ", ")
}

func formatRange(first, last int) string {
	if first == last {
		return strconv.Itoa(first)
	}
	return fmt.Sprintf("%d-%d", first, last)
}
