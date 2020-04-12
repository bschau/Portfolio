all:	 build

build:
	dotnet build

clean:
	-find . -type f -name "*~" -exec rm -f {} \;

distclean:
	-rm -fr bin obj

publish:
	dotnet publish -c release --self-contained --runtime linux-x64 --framework netcoreapp2.2

install:	clean build publish
	rsync -rvz -zz --progress --delete --omit-dir-times bin/release/netcoreapp2.2/linux-x64/publish/ -e "ssh -p $(VPSAPPSPORT)" $(VPSAPPSACCOUNT):/home/bs/VPSApps/
