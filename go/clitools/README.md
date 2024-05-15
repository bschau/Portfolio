# clitools
A collection of (possibly) cross-platform command line tools.

On Linux, MacOS or other Unices these tools can easily be done either by using shell-scripts or may already be natively implemented.

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

## Output
The command output matching filesystem objects prefixed with one of:

| Prefix | Description        |
|--------|--------------------|
|        | Unknown            |
|   f    | Is a regular file  |
|   d    | Is a directory     |
|   l    | Is a symbolic link |
|   p    | Is a pipe          |
|   s    | Is a socket        |


