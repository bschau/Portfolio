#!/bin/ksh

#################################################################################
#	check
#
#	Is argument non-null?
#
function check
{
	if test $1 -ne 0; then
		exit 1
	fi
}

#################################################################################
#	'main'
#
if test "$(whoami)" = "root"; then
	if test $# -eq 5; then
		if test ! -d "$2"; then
			mkdir -p $2
		fi

		bin=$1
		dst=$2/$1
		mode=$3
		user=$4
		group=$5
		
		cp $bin $dst
		check $?
		type="$(file $dst)"
		case "$type" in
			*COFF*)
				strip $dst
				check $?
				;;
		esac

		chmod $mode $dst
		check $?
		chown $user $dst
		check $?
		chgrp $group $dst
		check $?
		ret=0
	else
		echo "Usage: $0 binary destination mode user group"
		ret=1
	fi
else
	echo "You must be root!"
	ret=1
fi

exit $ret
