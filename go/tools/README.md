# tools
A handful of command-line tools in Go.
Not all have been documented here.


## colconv
Convert from hex / decimail / OpenGL to hex / decimail / OpenGL

### Usage
```
  colconv -h | xxxxxx | r g b | r-gl g-gl b-gl
  [OPTIONS]
   -h              Help (this page)
   xxxxxx          Hex code
   r g b           Red, green and blue components in integer form
   r-gl g-gl b-gl  Red, green and blue components in Open GL form (0.xx)
```

## hxd
A small hex-dumper.

### Usage
```
  hxd [OPTIONS] file [start [end]]
  [OPTIONS]
   -h              Help (this page)
```

## imgsize
A tool to reveal the size of a graphics file.

### Usage
```
  imgsize [OPTIONS] [image-file1 image-file2 ... image-fileX]
  [OPTIONS]
   -h              Help (this page)
```

## mren
Multiple rename.

### Usage
```
  mren [OPTIONS] prefix [commit]
  [OPTIONS]
    -h              Help (this page)
    -d folder       Folder with files to be renamed.

  If folder is not given, default to . (current directory).

  This program renames all files, except files ending in ~ and .files, in the selected folder.
  The files will be renamed: 

    prefix-number.suffix

  You give prefix. The program fill figure out the number and .suffix is the original files suffix (in lowercase).
  F.ex.:

    IMG01231.JPG
    IMG01232.JPG
    IMG01233.JPG

  if 'mren Holiday' then these will become:

    IMG01231.JPG -> Holiday-1.jpg
    IMG01232.JPG -> Holiday-2.jpg
    IMG01233.JPG -> Holiday-3.jpg

  Run mren without the 'commit' argument to do a dry-run. With 'commit' changes are persisted.
  The 'commit' argument must always be last.
```

## procpwds
A command-line tool to generate pronouncable passwords.

### Usage
```
  procpwds [OPTIONS] [number of passwords]
  [OPTIONS]
   -d digits       Number of digits in passwords
   -h              Help (this page)
   -l length       Length of passwords
```

## remtilde
A small tool to find and delete VIM backup files.

### Usage
```
  remtilde [OPTIONS] [path1 path2 ... pathX]
  [OPTIONS]
   -d              Dry run - show what would be deleted
   -h              Help (this page)
   -i              Ignore dot-files (.rc, .something, ...)
   -t              Trace files
   -u              Ignore underscore-files (\_rc, \_something, ...)
   -v              Verbose/Debug output

  If paths are not given, default to . (current directory).
```

## sauk
A minimalist cli webserver for serving static content.

If invoked without argument sauk binds to port 8080 and serves files from
the current directory.

### Usage
```
  sauk [OPTIONS]
  [OPTIONS]
   -D              Enable debugging
   -d path         Document root
   -h              Help (this page)
   -p num          Listen on port number
```

## tolower
Rename files and folders to lowercase. Not that cool on Windows :-)

### Usage
```
  tolower [OPTIONS]
  [OPTIONS]
   -d              Dry run - show what would be renamed
   -F              Also rename folders
   -h              Help (this page)
   -q              Quiet mode
   -U              Rename to uppercase
```

## waves
Create sin and cos tables.

### Usage
```
  waves [OPTIONS] amplitude
  [OPTIONS]
   -0              Origo is amplitude
   -e length       Entries pr. line, usually defaults to 256
   -h help         This page
   -o type         Output type (c)
   -t length       Table length, defaults to 256
```

## wi
'W'here 'I's file(s).

### Usage
```
  wi [OPTIONS] term1 term2 ... termX
  [OPTIONS]
   -h              Help (this page)
   -r root-folder  Folder to start search in

  If root-folder is not given, default to . (current directory).
```

## zen
A small "project management" (ahem) tool.

### Usage
```
  zen [OPTIONS] [master] COMMAND [ARGUMENTS]

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
```
