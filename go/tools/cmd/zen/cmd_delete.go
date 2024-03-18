package main

// CmdDelete - delete a given story / stories
func CmdDelete(zen ZenFile, filename string, args []string) {
	if len(args) == 0 {
		Error("Missing argument(s) to 'delete' command")
	}

	for i := 0; i < len(args); i++ {
		index := ZenItemIndexByID(zen, args[i])
		deleteStory(&zen, index)
	}

	ZenFileSave(zen, filename)
}

func deleteStory(zen *ZenFile, index int) {
	if len(zen.Items) == 1 {
		zen.Items = []ZenItem{}
		return
	}

	if index < len(zen.Items)-1 {
		copy(zen.Items[index:], zen.Items[index+1:])
	}
	zen.Items = zen.Items[:len(zen.Items)-1]
}
