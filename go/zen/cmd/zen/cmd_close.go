package main

// CmdClose - close a given story / stories
func CmdClose(zen ZenFile, filename string, args []string) {
	if len(args) == 2 {
		Error("Missing argument(s) to 'close' command")
	}

	for i := 0; i < len(args); i++ {
		index := ZenItemIndexByID(zen, args[i])
		if !zen.Items[index].Open {
			Warn("Story already closed:", zen.Items[index].ID)
			continue
		}

		now := Now()
		zen.Items[index].Open = false
		zen.Items[index].Modified = now
		zen.Items[index].Closed = now
	}

	ZenFileSave(zen, filename)
}
