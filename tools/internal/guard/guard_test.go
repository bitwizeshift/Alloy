package guard_test

import (
	"errors"
	"io"
	"strings"
	"testing"
	"testing/iotest"

	"github.com/google/go-cmp/cmp"
	"github.com/google/go-cmp/cmp/cmpopts"

	"github.com/bitwizeshift/alloy/tools/internal/check"
	"github.com/bitwizeshift/alloy/tools/internal/guard"
)

var roots = []string{"include", "src"}

func TestCheckerName(t *testing.T) {
	t.Parallel()

	testCases := []struct {
		name string
		path string
		want string
	}{
		{
			name: "under include",
			path: "lib/alloy-core/include/alloy/core/math/vector.hpp",
			want: "ALLOY_CORE_MATH_VECTOR_HPP",
		}, {
			name: "under src",
			path: "lib/alloy-render/src/alloy/render/mesh/attribute_packer.hpp",
			want: "ALLOY_RENDER_MESH_ATTRIBUTE_PACKER_HPP",
		}, {
			name: "first root",
			path: "extra/a/include/src/b.hpp",
			want: "SRC_B_HPP",
		}, {
			name: "hyphens",
			path: "extra/sdl2-bindings/include/alloy/extra/sdl2-bindings/api.hpp",
			want: "ALLOY_EXTRA_SDL2_BINDINGS_API_HPP",
		}, {
			name: "no root",
			path: "example/main.hpp",
			want: "EXAMPLE_MAIN_HPP",
		},
	}

	for _, tc := range testCases {
		t.Run(tc.name, func(t *testing.T) {
			t.Parallel()

			// Arrange
			sut := &guard.Checker{Roots: roots}

			// Act
			name := sut.Name(tc.path)

			// Assert
			if got, want := name, tc.want; !cmp.Equal(got, want) {
				t.Errorf("Checker.Name() = %v, want %v", got, want)
			}
		})
	}
}

func TestCheckerCheck(t *testing.T) {
	t.Parallel()

	testErr := errors.New("test error")
	testCases := []struct {
		name    string
		reader  io.Reader
		want    []check.Finding
		wantErr error
	}{
		{
			name: "guarded",
			reader: strings.NewReader(lines(
				"// comment",
				"/* block",
				"   comment */",
				"",
				"#ifndef A_B_HPP",
				"#define A_B_HPP",
				"",
				"#ifdef _MSC_VER",
				"# pragma once",
				"#endif",
				"",
				"int x;",
				"",
				"#endif /* A_B_HPP */",
			)),
			want:    nil,
			wantErr: nil,
		}, {
			name: "wrong name",
			reader: strings.NewReader(lines(
				"#ifndef A_C_HPP",
				"#define A_C_HPP",
				"",
				"#ifdef _MSC_VER",
				"# pragma once",
				"#endif",
				"",
				"#endif /* A_C_HPP */",
			)),
			want: []check.Finding{
				{Line: 1, Message: `include guard must open with "#ifndef A_B_HPP"`},
				{Line: 2, Message: `include guard must continue with "#define A_B_HPP"`},
				{Line: 8, Message: `include guard must close with "#endif /* A_B_HPP */"`},
			},
			wantErr: nil,
		}, {
			name: "include before pragma once",
			reader: strings.NewReader(lines(
				"#ifndef A_B_HPP",
				"#define A_B_HPP",
				"",
				"#include <vector>",
				"#ifdef _MSC_VER",
				"# pragma once",
				"#endif",
				"",
				"#endif /* A_B_HPP */",
			)),
			want: []check.Finding{
				{Line: 4, Message: `#include must come after the "#ifdef _MSC_VER" block`},
			},
			wantErr: nil,
		}, {
			name: "versioned pragma once",
			reader: strings.NewReader(lines(
				"#ifndef A_B_HPP",
				"#define A_B_HPP",
				"",
				"#if defined(_MSC_VER) && (_MSC_VER >= 1200)",
				"# pragma once",
				"#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)",
				"",
				"#endif /* A_B_HPP */",
			)),
			want: []check.Finding{
				{Line: 4, Message: `include guard must be followed by "#ifdef _MSC_VER" holding "# pragma once"`},
			},
			wantErr: nil,
		}, {
			name: "missing pragma once",
			reader: strings.NewReader(lines(
				"#ifndef A_B_HPP",
				"#define A_B_HPP",
				"",
				"int x;",
				"",
				"#endif /* A_B_HPP */",
			)),
			want: []check.Finding{
				{Line: 3, Message: `include guard must be followed by "#ifdef _MSC_VER" holding "# pragma once"`},
			},
			wantErr: nil,
		}, {
			name: "missing closing comment",
			reader: strings.NewReader(lines(
				"#ifndef A_B_HPP",
				"#define A_B_HPP",
				"",
				"#ifdef _MSC_VER",
				"# pragma once",
				"#endif",
				"",
				"#endif",
				"",
			)),
			want: []check.Finding{
				{Line: 8, Message: `include guard must close with "#endif /* A_B_HPP */"`},
			},
			wantErr: nil,
		}, {
			name: "code before guard",
			reader: strings.NewReader(lines(
				"#include <cstddef>",
				"#ifndef A_B_HPP",
				"#define A_B_HPP",
				"",
				"#ifdef _MSC_VER",
				"# pragma once",
				"#endif",
				"",
				"#endif /* A_B_HPP */",
			)),
			want: []check.Finding{
				{Line: 1, Message: `include guard must open with "#ifndef A_B_HPP"`},
				{Line: 2, Message: `include guard must continue with "#define A_B_HPP"`},
				{Line: 3, Message: `include guard must be followed by "#ifdef _MSC_VER" holding "# pragma once"`},
			},
			wantErr: nil,
		}, {
			name:   "empty",
			reader: strings.NewReader(""),
			want: []check.Finding{
				{Line: 0, Message: `include guard must open with "#ifndef A_B_HPP"`},
				{Line: 0, Message: `include guard must continue with "#define A_B_HPP"`},
				{Line: 0, Message: `include guard must be followed by "#ifdef _MSC_VER" holding "# pragma once"`},
				{Line: 0, Message: `include guard must close with "#endif /* A_B_HPP */"`},
			},
			wantErr: nil,
		}, {
			name:    "read error",
			reader:  iotest.ErrReader(testErr),
			want:    nil,
			wantErr: testErr,
		},
	}

	for _, tc := range testCases {
		t.Run(tc.name, func(t *testing.T) {
			t.Parallel()

			// Arrange
			sut := &guard.Checker{Roots: roots}

			// Act
			findings, err := sut.Check("include/a/b.hpp", tc.reader)

			// Assert
			if got, want := err, tc.wantErr; !cmp.Equal(got, want, cmpopts.EquateErrors()) {
				t.Fatalf("Checker.Check() = %v, want %v", got, want)
			}
			if got, want := findings, tc.want; !cmp.Equal(got, want) {
				t.Errorf("Checker.Check() = %v, want %v", got, want)
			}
		})
	}
}

func TestCheckerFix(t *testing.T) {
	t.Parallel()

	testErr := errors.New("test error")
	guarded := lines(
		"// comment",
		"",
		"#ifndef A_B_HPP",
		"#define A_B_HPP",
		"",
		"#ifdef _MSC_VER",
		"# pragma once",
		"#endif",
		"",
		"int x;",
		"",
		"#endif /* A_B_HPP */",
		"",
	)
	testCases := []struct {
		name    string
		reader  io.Reader
		want    string
		wantErr error
	}{
		{
			name:    "guarded",
			reader:  strings.NewReader(guarded),
			want:    guarded,
			wantErr: nil,
		}, {
			name: "wrong name",
			reader: strings.NewReader(lines(
				"// comment",
				"",
				"#ifndef A_C_HPP",
				"#define A_C_HPP",
				"",
				"#ifdef _MSC_VER",
				"# pragma once",
				"#endif",
				"",
				"int x;",
				"",
				"#endif /* A_C_HPP */",
				"",
			)),
			want:    guarded,
			wantErr: nil,
		}, {
			name: "include before pragma once",
			reader: strings.NewReader(lines(
				"// comment",
				"",
				"#ifndef A_B_HPP",
				"#define A_B_HPP",
				"",
				"#include <vector>",
				"",
				"#include <array>",
				"#ifdef _MSC_VER",
				"# pragma once",
				"#endif",
				"",
				"int x;",
				"",
				"#endif /* A_B_HPP */",
				"",
			)),
			want: lines(
				"// comment",
				"",
				"#ifndef A_B_HPP",
				"#define A_B_HPP",
				"",
				"#ifdef _MSC_VER",
				"# pragma once",
				"#endif",
				"",
				"#include <vector>",
				"#include <array>",
				"int x;",
				"",
				"#endif /* A_B_HPP */",
				"",
			),
			wantErr: nil,
		}, {
			name: "versioned pragma once",
			reader: strings.NewReader(lines(
				"// comment",
				"",
				"#ifndef A_B_HPP",
				"#define A_B_HPP",
				"",
				"#if defined(_MSC_VER) && (_MSC_VER >= 1200)",
				"#  pragma once",
				"#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)",
				"",
				"int x;",
				"",
				"#endif /* A_B_HPP */",
				"",
			)),
			want:    guarded,
			wantErr: nil,
		}, {
			name: "missing pragma once",
			reader: strings.NewReader(lines(
				"// comment",
				"",
				"#ifndef A_B_HPP",
				"#define A_B_HPP",
				"",
				"int x;",
				"",
				"#endif /* A_B_HPP */",
				"",
			)),
			want:    guarded,
			wantErr: nil,
		}, {
			name: "missing pragma once and blank line",
			reader: strings.NewReader(lines(
				"// comment",
				"",
				"#ifndef A_B_HPP",
				"#define A_B_HPP",
				"int x;",
				"",
				"#endif /* A_B_HPP */",
				"",
			)),
			want:    guarded,
			wantErr: nil,
		}, {
			name: "missing closing comment",
			reader: strings.NewReader(lines(
				"// comment",
				"",
				"#ifndef A_B_HPP",
				"#define A_B_HPP",
				"",
				"#ifdef _MSC_VER",
				"# pragma once",
				"#endif",
				"",
				"int x;",
				"",
				"#endif",
				"",
			)),
			want:    guarded,
			wantErr: nil,
		}, {
			name:    "unguarded",
			reader:  strings.NewReader(lines("#pragma once", "int x;", "")),
			want:    lines("#pragma once", "int x;", ""),
			wantErr: nil,
		}, {
			name:    "read error",
			reader:  iotest.ErrReader(testErr),
			want:    "",
			wantErr: testErr,
		},
	}

	for _, tc := range testCases {
		t.Run(tc.name, func(t *testing.T) {
			t.Parallel()

			// Arrange
			sut := &guard.Checker{Roots: roots}
			var w strings.Builder

			// Act
			err := sut.Fix("include/a/b.hpp", tc.reader, &w)

			// Assert
			if got, want := err, tc.wantErr; !cmp.Equal(got, want, cmpopts.EquateErrors()) {
				t.Fatalf("Checker.Fix() = %v, want %v", got, want)
			}
			if got, want := w.String(), tc.want; !cmp.Equal(got, want) {
				t.Errorf("Checker.Fix() output = %q, want %q", got, want)
			}
		})
	}
}

func lines(lines ...string) string {
	return strings.Join(lines, "\n")
}
