package main

import (
	"flag"
	"os"

	U "codeberg.org/lorentzus/tools/internal/usage"
)

var doc = `Usage: mren v1.0
mren [OPTIONS] prefix [commit]
[OPTIONS]
 -h              Help (this page)
 -d folder       Folder with files to be renamed.

If folder is not given, default to . (current directory).

This program renames all files, except files ending in ~ and .files, in the
selected folder.
The files will be renamed: 

  prefix-number.suffix

You give prefix. The program fill figure out the number and .suffix is the
original files suffix (in lowercase).
F.ex.:

  IMG01231.JPG
  IMG01232.JPG
  IMG01233.JPG

if 'mren Holiday' then these will become:

  IMG01231.JPG -> Holiday-1.jpg
  IMG01232.JPG -> Holiday-2.jpg
  IMG01233.JPG -> Holiday-3.jpg

Run mren without the 'commit' argument to do a dry-run. With 'commit'
changes are persisted.
The 'commit' argument must always be last.
`

func main() {
	help := flag.Bool("h", false, "Help")
	mainFolder := flag.String("d", ".", "Folder with files to be renamed. Default: '.'")
	flag.Parse()

	if *help {
		U.Usage(doc, 0)
	}

	args := flag.Args()
	if len(args) == 0 || len(args) > 2 {
		U.Usage(doc, 1)
	}

	prefix := args[0]
	commit := mustCommit(args)

	Rename(*mainFolder, prefix, commit)

	os.Exit(0)
}

func mustCommit(args []string) bool {
	if len(args) != 2 {
		return false
	}

	if args[1] != "commit" {
		U.Usage(doc, 1)
	}

	return true
}
