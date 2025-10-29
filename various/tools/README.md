# tools
A collection of (possibly) cross-platform command line tools.

On Linux, MacOS or other Unices these tools can easily be done either by using shell-scripts or may already be natively implemented.

## colconv
Convert from hex / decimal / OpenGL to hex / decimal / OpenGL

### Usage
```
  colconv -h | xxxxxx | r g b | r-gl g-gl b-gl
  [OPTIONS]
   -h              Help (this page)
   xxxxxx          Hex code
   r g b           Red, green and blue components in integer form
   r-gl g-gl b-gl  Red, green and blue components in Open GL form (0.xx)
```

## fnchg
Rename files and possibly folders to lowercase (default) or uppercase in the current directory.

Not that cool case-insensitive filesystems (such as on Windows or MacOS :-)

### Usage
```
  fnchg [OPTIONS]
  [OPTIONS]
   -d              Dry run - show what would be renamed
   -D              Incude dotfiles and folders
   -F              Also rename folders
   -h              Help (this page)
   -q              Quiet mode
   -U              Rename to uppercase
```

## genpwd
A command-line tool to generate possibly pronouncable passwords.

### Usage
```
  genpwd [OPTIONS] [number of passwords]
  [OPTIONS]
   -d digits       Number of digits in passwords
   -h              Help (this page)
   -l length       Length of passwords
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
```

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

## remtilde
A small tool to find and delete VIM backup files.

This is the equivalent of the following alias in .bashrc:
```
  alias remtilde='find . -type f -iname "*~" -exec rm -vf {} \;' 
```

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

This is basically the equivalents to the following .bashrc aliases:

``
  alias wi='find . -iname'
``

### Usage
```
  wi [OPTIONS] term1 term2 ... termX
  [OPTIONS]
   -h              Help (this page)
   -r root-folder  Folder to start search in

  If root-folder is not given, default to . (current directory).
```

### Output
The command output matching filesystem objects prefixed with one of:

| Prefix | Description        |
|--------|--------------------|
|        | Unknown            |
|   f    | Is a regular file  |
|   d    | Is a directory     |
|   l    | Is a symbolic link |
|   p    | Is a pipe          |
|   s    | Is a socket        |

