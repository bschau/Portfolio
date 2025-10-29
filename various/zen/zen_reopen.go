package main

// CmdReopen - repopen a given story / stories
func CmdReopen(zen ZenFile, filename string, args []string) {
	if len(args) == 0 {
		Error("Missing argument(s) to 'reopen' command")
	}

	for i := 0; i < len(args); i++ {
		index := ZenItemIndexByID(zen, args[i])
		if zen.Items[index].Open {
			Warn("Story already open:", zen.Items[index].ID)
			continue
		}

		zen.Items[index].Open = true
		zen.Items[index].Modified = Now()
		zen.Items[index].Closed = ""
	}

	ZenFileSave(zen, filename)
}
