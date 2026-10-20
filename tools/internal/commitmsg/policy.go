package commitmsg

import (
	"errors"
	"fmt"
	"slices"
	"strings"
	"unicode/utf8"
)

var (
	// ErrEmptySubject is reported when a message has no subject.
	ErrEmptySubject = errors.New("subject is empty")

	// ErrSubjectTooLong is reported when the subject is wider than
	// [Policy.SubjectWidth].
	ErrSubjectTooLong = errors.New("subject is too long")

	// ErrNoBlankLine is reported when the subject is not followed by a blank
	// line.
	ErrNoBlankLine = errors.New("subject must be followed by a blank line")

	// ErrBodyTooWide is reported when at least half of the lines that follow
	// the subject are wider than [Policy.BodyWidth].
	ErrBodyTooWide = errors.New("too many lines are too wide")

	// ErrMissingTrailer is reported when a required trailer is absent.
	ErrMissingTrailer = errors.New("missing trailer")

	// ErrRepeatedTrailer is reported when a trailer appears more often than
	// allowed, or a repeatable trailer repeats a value.
	ErrRepeatedTrailer = errors.New("repeated trailer")

	// ErrInvalidTrailer is reported when a trailer's value is not one of its
	// allowed values.
	ErrInvalidTrailer = errors.New("invalid trailer value")
)

// Trailer keys that a [Policy] validates.
const (
	CategoryKey  = "Change-Category"
	ImpactKey    = "Change-Impact"
	ComponentKey = "Component"
)

// Policy is the set of rules that a commit message must satisfy.
type Policy struct {
	// SubjectWidth is the maximum number of characters in the subject.
	SubjectWidth int

	// BodyWidth is the number of characters that most lines of the body must
	// not exceed. Some lines may exceed it, such as those containing links,
	// so long as they are not at least half of the body.
	BodyWidth int

	// Categories are the allowed values of the required, unique
	// Change-Category trailer.
	Categories []string

	// Impacts are the allowed values of the required, unique Change-Impact
	// trailer.
	Impacts []string

	// Components are the allowed values of the optional, repeatable Component
	// trailer.
	Components []string
}

// Validate returns an error for each rule of p that m violates, or nil if m
// satisfies every rule. Each error wraps one of [ErrEmptySubject],
// [ErrSubjectTooLong], [ErrNoBlankLine], [ErrBodyTooWide],
// [ErrMissingTrailer], [ErrRepeatedTrailer], or [ErrInvalidTrailer].
//
// Trailer keys are matched case-insensitively.
func (p *Policy) Validate(m *Message) []error {
	var errs []error
	errs = append(errs, p.validateSubject(m)...)
	errs = append(errs, p.validateBody(m)...)
	errs = append(errs, p.validateRequired(m, CategoryKey, p.Categories)...)
	errs = append(errs, p.validateRequired(m, ImpactKey, p.Impacts)...)
	errs = append(errs, p.validateComponents(m)...)
	return errs
}

func (p *Policy) validateSubject(m *Message) []error {
	if m.Subject == "" {
		return []error{ErrEmptySubject}
	}
	if width := utf8.RuneCountInString(m.Subject); width > p.SubjectWidth {
		return []error{fmt.Errorf("%w: %d characters, want at most %d", ErrSubjectTooLong, width, p.SubjectWidth)}
	}
	return nil
}

func (p *Policy) validateBody(m *Message) []error {
	if len(m.Lines) == 0 {
		return nil
	}

	var errs []error
	if m.Lines[0] != "" {
		errs = append(errs, ErrNoBlankLine)
	}

	body := m.Lines[1:]
	narrow := 0
	for _, line := range body {
		if utf8.RuneCountInString(line) <= p.BodyWidth {
			narrow++
		}
	}
	if len(body) > 0 && 2*narrow <= len(body) {
		errs = append(errs, fmt.Errorf("%w: more than %d characters on %d of %d lines", ErrBodyTooWide, p.BodyWidth, len(body)-narrow, len(body)))
	}
	return errs
}

func (p *Policy) validateRequired(m *Message, key string, allowed []string) []error {
	values := p.values(m, key)
	if len(values) == 0 {
		return []error{fmt.Errorf("%w %s: want one of %s", ErrMissingTrailer, key, p.list(allowed))}
	}

	var errs []error
	if len(values) > 1 {
		errs = append(errs, fmt.Errorf("%w %s: found %d, want exactly one", ErrRepeatedTrailer, key, len(values)))
	}
	for _, value := range values {
		if !slices.Contains(allowed, value) {
			errs = append(errs, p.invalid(key, value, allowed))
		}
	}
	return errs
}

func (p *Policy) validateComponents(m *Message) []error {
	var errs []error
	var seen []string
	for _, value := range p.values(m, ComponentKey) {
		switch {
		case !slices.Contains(p.Components, value):
			errs = append(errs, p.invalid(ComponentKey, value, p.Components))
		case slices.Contains(seen, value):
			errs = append(errs, fmt.Errorf("%w %s: %q appears more than once", ErrRepeatedTrailer, ComponentKey, value))
		default:
			seen = append(seen, value)
		}
	}
	return errs
}

func (*Policy) values(m *Message, key string) []string {
	var values []string
	for _, trailer := range m.Trailers {
		if strings.EqualFold(trailer.Key, key) {
			values = append(values, trailer.Value)
		}
	}
	return values
}

func (p *Policy) invalid(key, value string, allowed []string) error {
	return fmt.Errorf("%w %s: %q, want one of %s", ErrInvalidTrailer, key, value, p.list(allowed))
}

func (*Policy) list(values []string) string {
	return "`" + strings.Join(values, "`, `") + "`"
}
