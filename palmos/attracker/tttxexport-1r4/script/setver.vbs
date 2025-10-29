set fO=CreateObject("Scripting.FileSystemObject")
set fld=fO.GetFolder(".\")

v=Mid(fld, InStr(fld, "-")+1)

set tf=fO.CreateTextFile("src\version.h", true)
tf.WriteLine("#ifndef VERSION_H")
tf.WriteLine("#define VERSION_H")
tf.WriteLine("#define VERSION " & chr(34) & v & chr(34))
tf.WriteLine("#endif")
tf.Close

