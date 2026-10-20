package license

import (
	"io"
	"regexp"
	"strings"

	"github.com/bitwizeshift/alloy/tools/internal/check"
)

// Rule selects the license header of the files whose path matches Path.
type Rule struct {
	// Path matches the paths of the files that the rule applies to.
	Path *regexp.Regexp

	// Header is the exact text that matching files must start with.
	Header string
}

// HeaderChecker reports files that do not start with their license header.
type HeaderChecker struct {
	// Rules select the header of each file. The first rule whose Path matches
	// a file applies to it, and a file that matches no rule is not checked.
	Rules []Rule
}

// Check returns a single [check.Finding] on the first line if the content read
// from r does not start with the header selected for path. An error is
// returned only if r cannot be read.
func (hc *HeaderChecker) Check(path string, r io.Reader) ([]check.Finding, error) {
	rule, ok := hc.rule(path)
	if !ok {
		return nil, nil
	}
	data, err := io.ReadAll(r)
	if err != nil {
		return nil, err
	}
	if strings.HasPrefix(string(data), rule.Header) {
		return nil, nil
	}
	return []check.Finding{{Line: 1, Message: "missing license header"}}, nil
}

var _ check.Checker = (*HeaderChecker)(nil)

func (hc *HeaderChecker) rule(path string) (Rule, bool) {
	for _, rule := range hc.Rules {
		if rule.Path.MatchString(path) {
			return rule, true
		}
	}
	return Rule{}, false
}
