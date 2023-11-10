CC	=	mcs
CFLAGS	=	-warn:4 -out:$(DST) -target:exe -reference:System.Windows.Forms -reference:System.Drawing -reference:System.Data -resource:fAbout.resources,iSecurViewer.fAbout.resources -resource:fMain.resources,iSecurViewer.fMain.resources -resource:fPassword.resources,iSecurViewer.fPassword.resources -resource:fShow.resources,iSecurViewer.fShow.resources -win32icon:App.ico
SRCS	=	AES.cs \
		AssemblyInfo.cs \
		CategoryMgr.cs \
		Config.cs \
		fAbout.cs \
		fMain.cs \
		fPassword.cs \
		fShow.cs \
		PDB.cs \
		Record.cs
RES1	=	fAbout.resources
RES2	=	fMain.resources
RES3	=	fPassword.resources
RES4	=	fShow.resources

$(DST):	$(RES1) $(RES2) $(RES3) $(RES4) $(SRCS)
	$(CC) $(CFLAGS) $(SRCS)

$(RES1):	fAbout.resx
	resgen $<

$(RES2):	fMain.resx
	resgen $<

$(RES3):	fPassword.resx
	resgen $<

$(RES4):	fShow.resx
	resgen $<

clean:
	rm -f $(DST) *~ *.resources
