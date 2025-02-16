# remtilde
A small tool to find and delete VIM backup files.

This is the equivalent of the following alias in .bashrc:
```
  alias remtilde='find . -type f -iname "*~" -exec rm -vf {} \;' 
```

## Usage
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
