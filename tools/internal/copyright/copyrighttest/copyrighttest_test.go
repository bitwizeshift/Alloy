package copyrighttest_test

import (
	"errors"
	"testing"

	"github.com/google/go-cmp/cmp"
	"github.com/google/go-cmp/cmp/cmpopts"

	"github.com/bitwizeshift/alloy/tools/internal/copyright"
	"github.com/bitwizeshift/alloy/tools/internal/copyright/copyrighttest"
)

func TestHistory(t *testing.T) {
	t.Parallel()

	testErr := errors.New("test error")
	testCases := []struct {
		name    string
		history copyright.History
		want    []int
		wantErr error
	}{
		{
			name:    "years",
			history: copyrighttest.History(2021, 2023),
			want:    []int{2021, 2023},
			wantErr: nil,
		}, {
			name:    "error",
			history: copyrighttest.ErrHistory(testErr),
			want:    nil,
			wantErr: testErr,
		},
	}

	for _, tc := range testCases {
		t.Run(tc.name, func(t *testing.T) {
			t.Parallel()

			// Arrange
			sut := tc.history

			// Act
			years, err := sut.Years("a.cpp")

			// Assert
			if got, want := err, tc.wantErr; !cmp.Equal(got, want, cmpopts.EquateErrors()) {
				t.Fatalf("History.Years() = %v, want %v", got, want)
			}
			if got, want := years, tc.want; !cmp.Equal(got, want) {
				t.Errorf("History.Years() = %v, want %v", got, want)
			}
		})
	}
}
