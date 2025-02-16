# wi
'W'here 'I's file(s).

This is basically the equivalents to the following .bashrc aliases:

``
  alias wi='find . -iname'
``

## Usage
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

