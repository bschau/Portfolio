set fO=CreateObject("Scripting.FileSystemObject")
set fld=fO.GetFolder(".\")

v=Mid(fld, InStr(fld, "-")+1)

set rf=fO.OpenTextFile("script\setup.in", 1)
setup=rf.ReadAll()
rf.Close()

setupLen=Len(setup)

set tf=fO.CreateTextFile("setup.iss", true)
prev=1
str=""
while prev<setupLen
	nxt=InStr(prev, setup, "@@VERSION@@")
	if nxt = 0 then
		str=Mid(setup, prev, setupLen-prev+1)
		tf.Write(str)
		prev=setupLen
	else
		str=Mid(setup, prev, nxt-prev) & v
		tf.Write(str)
		prev=nxt+11
	end if
wend
tf.Close
