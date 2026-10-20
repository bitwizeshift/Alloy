// Package stream writes text verbatim to the output streams of a go-cli
// application.
//
// The streams of a go-cli application interpret richtext markup, so text that
// the application did not author, such as the output of a subprocess, would
// be restyled or rejected if it happened to contain a tag like "[/fg]". Such
// text is written through a verbatim view of the stream instead:
//
//	out := stream.Verbatim(cli.ErrStream(ctx))
//	out.Write(output)
package stream
