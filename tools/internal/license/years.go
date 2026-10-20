package license

import (
	"fmt"
	"io"
	"regexp"
	"strconv"
	"strings"

	"github.com/bitwizeshift/alloy/tools/internal/check"
)

var noticePattern = regexp.MustCompile(`Copyright \(c\) ([0-9]{4}(?:[0-9, -]*[0-9])?)`)

// YearChecker reports and fixes copyright notices whose years are not a
// single range ending in the current year, such as "2018-2026".
//
// The range starts at the first year of the notice, so that fixing a notice
// keeps the year in which the work began.
type YearChecker struct {
	// Year is the current year, which every range must end in.
	Year int
}

// Check returns a [check.Finding] for each notice read from r whose years are
// not the expected range, for each notice after the first, and for a file
// without a notice. An error is returned only if r cannot be read.
func (yc *YearChecker) Check(_ string, r io.Reader) ([]check.Finding, error) {
	data, err := io.ReadAll(r)
	if err != nil {
		return nil, err
	}
	content := string(data)

	notices := yc.notices(content)
	if len(notices) == 0 {
		return []check.Finding{{Message: "missing copyright notice"}}, nil
	}

	var findings []check.Finding
	for i, notice := range notices {
		if i > 0 {
			findings = append(findings, check.Finding{
				Line:    notice.line,
				Message: "multiple copyright notices",
			})
		}
		if want := yc.want(notice.years); notice.years != want {
			findings = append(findings, check.Finding{
				Line:    notice.line,
				Message: fmt.Sprintf("copyright years are %q, want %q", notice.years, want),
			})
		}
	}
	return findings, nil
}

// Fix copies r to w with the years of every notice replaced by the expected
// range. An error is returned if r cannot be read, or w cannot be written.
func (yc *YearChecker) Fix(_ string, r io.Reader, w io.Writer) error {
	data, err := io.ReadAll(r)
	if err != nil {
		return err
	}
	content := string(data)

	var fixed strings.Builder
	end := 0
	for _, notice := range yc.notices(content) {
		fixed.WriteString(content[end:notice.start])
		fixed.WriteString(yc.want(notice.years))
		end = notice.end
	}
	fixed.WriteString(content[end:])

	_, err = io.WriteString(w, fixed.String())
	return err
}

var _ check.CheckFixer = (*YearChecker)(nil)

// notice is a copyright notice, where years is content[start:end].
type notice struct {
	line       int
	years      string
	start, end int
}

func (yc *YearChecker) notices(content string) []notice {
	var notices []notice
	for _, match := range noticePattern.FindAllStringSubmatchIndex(content, -1) {
		start, end := match[2], match[3]
		notices = append(notices, notice{
			line:  strings.Count(content[:start], "\n") + 1,
			years: content[start:end],
			start: start,
			end:   end,
		})
	}
	return notices
}

// want returns the range from the first year of years to the current year.
// The pattern guarantees that years starts with four digits.
func (yc *YearChecker) want(years string) string {
	first, _ := strconv.Atoi(years[:4])
	if first == yc.Year {
		return strconv.Itoa(yc.Year)
	}
	return fmt.Sprintf("%d-%d", first, yc.Year)
}
