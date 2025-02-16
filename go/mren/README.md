# mren
Multiple rename.

## Usage
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
