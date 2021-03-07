package main

import (
	"fmt"
	"io"
	"os"
)

// Usage - write usage and exit with exit code
func Usage(exitCode int) {
	s := getStream(exitCode)
	doc := `zen v1.1
Usage: zen [OPTIONS] [master] COMMAND [ARGUMENTS]

[OPTIONS]
 -f zen-file     Path to .zen-file
 -h              Help (this page)
 -v              Verbose/Debug output
 -s string       Separator string when adding words
 -w width        Width of display to the 'list brief' command

[COMMAND]
 add (a)         Adds the following words as a story. Words are separated by
                 a space. A word starting with @ is treated as a path to a
                 file of which the content is included.
                 Examples:
                     zen add this is a small story
                     zen add @story.txt
 close (cl)      Close story / stories.
                 Examples:
                     zen close 1
                     zen close 1 2 3 4
 count (c)       Show count of open stories. Specify the 'all' keyword to
                 show the count of all stories.
                 Examples:
                     zen count
                     zen count all
 count0 (c0)     As the count command, but omit the trailing newline.
                 Useful in scripts.
 delete (d)      Delete story / stories by id.
                 Examples:
                     zen delete 1
                     zen delete 1 2 3 4
 edit (e)        Edit story by ID.
                 Examples:
                     zen edit 1 change this
                     zen edit 1 @story.txt
 help            This page.
 init            Initialize new .zen file here or, if the 'master' keyword,
                 initialize the master .zen file.
 list (l)        List all open stories. Specify the 'all' keyword to show
                 all stories. Specify the 'brief' keyword to get a brief or
                 terse output.
 reopen (r)      Reopen previously closed story / stories.
                 Examples:
                     zen reopen 1
                     zen reopen 1 2 3 4
 status (s)      Show status of story / stories.
                 Examples:
                     zen show 1
                     zen show 1 2 3 4
 view (v)        View story / stories.
                 Examples:
                     zen view 1
                     zen view 1 2 3 4
`
	fmt.Fprint(s, doc)
	os.Exit(exitCode)
}

func getStream(exitCode int) io.Writer {
	if exitCode != 0 {
		return os.Stderr
	}

	return os.Stdout
}
