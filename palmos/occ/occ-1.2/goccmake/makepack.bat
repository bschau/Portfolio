mkdir goccmake
copy goccmake.exe goccmake
copy large.bmp goccmake
copy small.bmp goccmake
copy INSTALL goccmake
del GOCCMAKE.ZIP
del goccmake.zip
pkzip -P goccmake.zip goccmake/*.*
move GOCCMAKE.ZIP goccmake.zip
