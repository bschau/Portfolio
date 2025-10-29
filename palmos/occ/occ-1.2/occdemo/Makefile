all:	demo

clean:
	rm -f *.prc *~ *.zip

distclean:	clean

demo:
	../occmake/occmake -e -n "Demo Exe" -l large.bmp -s small.bmp demo-e www.schau.com/palm/occ/demo.php?type=exe
	../occmake/occmake -n "Demo" -l large.bmp -s small.bmp demo www.schau.com/palm/occ/demo.php?type=norm

zip:
	zip demo.zip Demo.prc
