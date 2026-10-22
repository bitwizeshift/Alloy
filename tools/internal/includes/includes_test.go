package includes_test

import (
	"errors"
	"io"
	"strings"
	"testing"
	"testing/iotest"

	"github.com/google/go-cmp/cmp"
	"github.com/google/go-cmp/cmp/cmpopts"

	"github.com/bitwizeshift/alloy/tools/internal/check"
	"github.com/bitwizeshift/alloy/tools/internal/includes"
)

const (
	headerPath = "lib/alloy-a/include/alloy/a/x/y.hpp"
	sourcePath = "lib/alloy-a/src/alloy/a/x/y.cpp"
	testPath   = "lib/alloy-a/test/src/alloy/a/x/y.test.cpp"
)

func newChecker() *includes.Checker {
	return &includes.Checker{
		Roots:     []string{"include", "src"},
		Targets:   []string{"test"},
		Libraries: []string{"alloy/a", "alloy/b", "alloy/b/c"},
	}
}

func TestCheckerCheck(t *testing.T) {
	t.Parallel()

	testErr := errors.New("test error")
	testCases := []struct {
		name    string
		path    string
		reader  io.Reader
		want    []check.Finding
		wantErr error
	}{
		{
			name: "ordered",
			path: sourcePath,
			reader: strings.NewReader(lines(
				`#include "alloy/a/x/y.hpp"`,
				``,
				`#include "alloy/a/x/w.hpp"`,
				`#include "alloy/a/x/z/z.hpp"`,
				`#include "detail.hpp"`,
				``,
				`#include "alloy/a/a.hpp"`,
				`#include "alloy/a/v/v.hpp"`,
				``,
				`#include "alloy/b/b.hpp"`,
				`#include "alloy/b/c/c.hpp"`,
				``,
				`#include <SDL3/SDL.h>`,
				``,
				`#include <cstdint>`,
				`#include <vector>`,
				``,
				`int x;`,
			)),
			want:    nil,
			wantErr: nil,
		}, {
			name: "ordered header",
			path: headerPath,
			reader: strings.NewReader(lines(
				`// comment`,
				`/* block`,
				`   comment */`,
				``,
				`#ifndef ALLOY_A_X_Y_HPP`,
				`#define ALLOY_A_X_Y_HPP`,
				``,
				`#ifdef _MSC_VER`,
				`# pragma once`,
				`#endif`,
				``,
				`#include "alloy/a/x/w.hpp"`,
				`#include "alloy/a/x/y.hpp"`,
				``,
				`#include <cstdint>`,
				``,
				`#endif /* ALLOY_A_X_Y_HPP */`,
			)),
			want:    nil,
			wantErr: nil,
		}, {
			name: "conditional between groups",
			path: headerPath,
			reader: strings.NewReader(lines(
				`#include <SDL3/SDL.h>`,
				`#if defined(__APPLE__)`,
				`# include <OpenGL/gl.h>`,
				`#endif`,
				`#include <cstdint>`,
			)),
			want:    nil,
			wantErr: nil,
		}, {
			name: "conditional includes",
			path: headerPath,
			reader: strings.NewReader(lines(
				`#include <cstdint>`,
				``,
				`#if X`,
				`# include <vector>`,
				`# include <array>`,
				`#endif`,
			)),
			want:    nil,
			wantErr: nil,
		}, {
			name: "unsorted",
			path: headerPath,
			reader: strings.NewReader(lines(
				`#include <vector>`,
				`#include <cstdint> // std::uint32_t`,
			)),
			want: []check.Finding{
				{Line: 2, Message: `<cstdint> must be included before <vector>`},
			},
			wantErr: nil,
		}, {
			name: "groups out of order",
			path: headerPath,
			reader: strings.NewReader(lines(
				`#include "alloy/b/b.hpp"`,
				``,
				`#include "alloy/a/a.hpp"`,
			)),
			want: []check.Finding{
				{Line: 3, Message: `"alloy/a/a.hpp" (same library) must be included before "alloy/b/b.hpp" (another library)`},
			},
			wantErr: nil,
		}, {
			name: "own header out of order",
			path: sourcePath,
			reader: strings.NewReader(lines(
				`#include "alloy/a/x/w.hpp"`,
				``,
				`#include "alloy/a/x/y.hpp"`,
			)),
			want: []check.Finding{
				{Line: 3, Message: `"alloy/a/x/y.hpp" (own header) must be included before "alloy/a/x/w.hpp" (same subsystem)`},
			},
			wantErr: nil,
		}, {
			name: "groups not separated",
			path: headerPath,
			reader: strings.NewReader(lines(
				`#include <SDL3/SDL.h>`,
				`#include <cstdint>`,
			)),
			want: []check.Finding{
				{Line: 2, Message: `<cstdint> (standard library) must be separated from <SDL3/SDL.h> (third-party) by a blank line`},
			},
			wantErr: nil,
		}, {
			name: "groups separated twice",
			path: headerPath,
			reader: strings.NewReader(lines(
				`#include <SDL3/SDL.h>`,
				``,
				``,
				`#include <cstdint>`,
			)),
			want: []check.Finding{
				{Line: 4, Message: `<cstdint> (standard library) must be separated from <SDL3/SDL.h> (third-party) by a blank line`},
			},
			wantErr: nil,
		}, {
			name: "group split",
			path: headerPath,
			reader: strings.NewReader(lines(
				`#include <array>`,
				``,
				`#include <vector>`,
			)),
			want: []check.Finding{
				{Line: 3, Message: `<vector> must directly follow <array> (both standard library)`},
			},
			wantErr: nil,
		}, {
			name: "group split by conditional",
			path: headerPath,
			reader: strings.NewReader(lines(
				`#include <array>`,
				`#if X`,
				`# include <SDL3/SDL.h>`,
				`#endif`,
				`#include <vector>`,
			)),
			want: []check.Finding{
				{Line: 5, Message: `<vector> must directly follow <array> (both standard library)`},
			},
			wantErr: nil,
		}, {
			name: "library header with angle brackets",
			path: headerPath,
			reader: strings.NewReader(lines(
				`#include <alloy/b/b.hpp>`,
			)),
			want: []check.Finding{
				{Line: 1, Message: `<alloy/b/b.hpp> must be included as "alloy/b/b.hpp"`},
			},
			wantErr: nil,
		}, {
			name: "library header with quotes from test",
			path: testPath,
			reader: strings.NewReader(lines(
				`#include <alloy/a/x/y.hpp>`,
				``,
				`#include "alloy/a/x/w.hpp"`,
				`#include "test_doubles.hpp"`,
			)),
			want: []check.Finding{
				{Line: 3, Message: `"alloy/a/x/w.hpp" must be included as <alloy/a/x/w.hpp>`},
			},
			wantErr: nil,
		}, {
			name: "relative include escaping subsystem",
			path: sourcePath,
			reader: strings.NewReader(lines(
				`#include "../a.hpp"`,
				`#include "alloy/a/v/v.hpp"`,
			)),
			want:    nil,
			wantErr: nil,
		}, {
			name: "relative include outside library",
			path: "extra/a/test/main.cpp",
			reader: strings.NewReader(lines(
				`#include "helper.hpp"`,
				``,
				`#include <alloy/a/a.hpp>`,
			)),
			want:    nil,
			wantErr: nil,
		}, {
			name: "own name without header extension",
			path: sourcePath,
			reader: strings.NewReader(lines(
				`#include "alloy/a/x/w.hpp"`,
				`#include "alloy/a/x/y.inl"`,
			)),
			want:    nil,
			wantErr: nil,
		}, {
			name:    "read error",
			path:    headerPath,
			reader:  iotest.ErrReader(testErr),
			want:    nil,
			wantErr: testErr,
		},
	}

	for _, tc := range testCases {
		t.Run(tc.name, func(t *testing.T) {
			t.Parallel()

			// Arrange
			sut := newChecker()

			// Act
			findings, err := sut.Check(tc.path, tc.reader)

			// Assert
			if got, want := err, tc.wantErr; !cmp.Equal(got, want, cmpopts.EquateErrors()) {
				t.Fatalf("Checker.Check() = %v, want %v", got, want)
			}
			if got, want := findings, tc.want; !cmp.Equal(got, want, cmpopts.EquateEmpty()) {
				t.Errorf("Checker.Check() = %v, want %v", got, want)
			}
		})
	}
}

func TestCheckerFix(t *testing.T) {
	t.Parallel()

	testErr := errors.New("test error")
	ordered := lines(
		`#include "alloy/a/x/y.hpp"`,
		``,
		`#include "alloy/a/a.hpp"`,
		``,
		`#include <cstdint> // std::uint32_t`,
		`#include <vector>  // std::vector`,
		``,
		`int x;`,
	)
	commented := lines(
		`// Vectors`,
		`#include <vector>`,
		``,
		`// Integers`,
		`#include <cstdint>`,
		``,
		`int x;`,
	)
	testCases := []struct {
		name    string
		path    string
		reader  io.Reader
		want    string
		wantErr error
	}{
		{
			name:    "ordered",
			path:    sourcePath,
			reader:  strings.NewReader(ordered),
			want:    ordered,
			wantErr: nil,
		}, {
			name: "unordered",
			path: sourcePath,
			reader: strings.NewReader(lines(
				`#include <vector> // std::vector`,
				`#include "alloy/a/a.hpp"`,
				``,
				``,
				`#include <cstdint> // std::uint32_t`,
				`#include "alloy/a/x/y.hpp"`,
				``,
				`int x;`,
			)),
			want:    ordered,
			wantErr: nil,
		}, {
			name: "wrong quotes",
			path: testPath,
			reader: strings.NewReader(lines(
				`#include "alloy/a/x/y.hpp" // subject`,
				``,
				`#include "test_doubles.hpp"`,
			)),
			want: lines(
				`#include <alloy/a/x/y.hpp> // subject`,
				``,
				`#include "test_doubles.hpp"`,
			),
			wantErr: nil,
		}, {
			name: "conditional",
			path: headerPath,
			reader: strings.NewReader(lines(
				`#include <vector>`,
				`#include <SDL3/SDL.h>`,
				`// OpenGL`,
				`#if defined(__APPLE__)`,
				`# include <OpenGL/gl.h>`,
				`#endif`,
				`#include <array>`,
				``,
				`#if X`,
				`# define Y`,
				`#endif`,
			)),
			want: lines(
				`#include <SDL3/SDL.h>`,
				``,
				`#include <array>`,
				`#include <vector>`,
				``,
				`// OpenGL`,
				`#if defined(__APPLE__)`,
				`# include <OpenGL/gl.h>`,
				`#endif`,
				``,
				`#if X`,
				`# define Y`,
				`#endif`,
			),
			wantErr: nil,
		}, {
			name:    "comment between includes",
			path:    headerPath,
			reader:  strings.NewReader(commented),
			want:    commented,
			wantErr: nil,
		}, {
			name:    "no includes",
			path:    headerPath,
			reader:  strings.NewReader(lines(`int x;`)),
			want:    lines(`int x;`),
			wantErr: nil,
		}, {
			name:    "read error",
			path:    headerPath,
			reader:  iotest.ErrReader(testErr),
			want:    "",
			wantErr: testErr,
		},
	}

	for _, tc := range testCases {
		t.Run(tc.name, func(t *testing.T) {
			t.Parallel()

			// Arrange
			sut := newChecker()
			var w strings.Builder

			// Act
			err := sut.Fix(tc.path, tc.reader, &w)

			// Assert
			if got, want := err, tc.wantErr; !cmp.Equal(got, want, cmpopts.EquateErrors()) {
				t.Fatalf("Checker.Fix() = %v, want %v", got, want)
			}
			if got, want := w.String(), tc.want; !cmp.Equal(got, want) {
				t.Errorf("Checker.Fix() output mismatch (-want +got):\n%s", cmp.Diff(want, got))
			}
		})
	}
}

func lines(lines ...string) string {
	return strings.Join(lines, "\n") + "\n"
}
