#!/bin/bash
rm -f lmake/lmake
h="$(pwd)"

if test "$(id -u)" = "0"; then
	prefix=""
	echo "Please specify the installation prefix. The programs are placed in \$prefix/bin"
	echo "and the manual pages are placed in \$prefix/man/man1."
	echo "Good choices are /usr, /usr/local or /opt."
	echo
	while test -z "$prefix"; do
		echo -n "Installation prefix: "
		read prefix
	done
else
	prefix=""
fi

cd lmake/scripts
chmod 755 build.sh
./build.sh
if test -s ../lmake; then
	chmod 755 ../lmake
	cd ${h}/lcdoc
	${h}/lmake/lmake
	cd ${h}/lhd
	${h}/lmake/lmake
	cd ${h}/lme
	${h}/lmake/lmake

	if test "$prefix"; then
		bin="${prefix}/bin"
		for p in lmake lcdoc lhd lme; do
			echo "Installing $p to $bin"
			cp ${h}/${p}/${p} $bin
			strip ${bin}/${p}
			echo "Installing manual page ${p}.1 to ${prefix}/man/man1"
			cp ${h}/man/${p}.1 ${prefix}/man/man1
		done
	else
		echo "Programs will not be installed as you're not root!"
	fi
else
	echo "Cannot continue - new lmake not found" >&2
	exit 1
fi

exit 0
