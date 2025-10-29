package main

import (
	"flag"
	"os"

	U "codeberg.org/lorentzus/tools/internal/usage"
)

var doc = `Usage: fnchg 1.0
fnchg [OPTIONS]
[OPTIONS]
 -d              Dry run - show what would be renamed
 -D              Include dot-files and folders
 -F              Also rename folders
 -h              Help (this page)
 -q              Quiet mode
 -U              Rename to uppercase
`

func main() {
	dryrun := flag.Bool("d", false, "Dry run - show what will be renamed")
	dotFiles := flag.Bool("D", false, "Include dotfiles and folder")
	help := flag.Bool("h", false, "Help")
	quiet := flag.Bool("q", false, "Quiet mode")
	folders := flag.Bool("F", false, "Also rename folders")
	upper := flag.Bool("U", false, "Rename to uppercase")
	flag.Parse()

	if *help {
		U.Usage(doc, 0)
	}

	DryRun = *dryrun
	Quiet = *quiet
	Folders = *folders
	Upper = *upper
	DotFiles = *dotFiles

	FnChg()

	os.Exit(0)
}
