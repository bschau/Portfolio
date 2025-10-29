#!/bin/sh
v="$(basename `pwd` | cut -f 2 -d '-')"
{
	cat <<EOF
#ifndef VERSION_H
#define VERSION_H
#define VERSION "$v"
#endif
EOF
} > src/version.h
exit 0
