package roottest

import (
	"context"

	"github.com/bitwizeshift/alloy/tools/internal/root"
)

// Rooter returns a [root.Rooter] that always reports dir as the root.
func Rooter(dir string) root.Rooter {
	return rooter(func(context.Context) (string, error) {
		return dir, nil
	})
}

// ErrRooter returns a [root.Rooter] that always fails with err.
func ErrRooter(err error) root.Rooter {
	return rooter(func(context.Context) (string, error) {
		return "", err
	})
}

type rooter func(ctx context.Context) (string, error)

func (r rooter) Root(ctx context.Context) (string, error) {
	return r(ctx)
}
