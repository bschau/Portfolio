#!/bin/sh
case $1 in
.1.4.2.1.1.1.1) 	echo "s:Spawned on .1.4.2.1.1.1.1" ;;
.1.4.3.1.1.1.2) 	echo "i:127.0.0.1" ;;
.1.4.3.1.1.1.3)		echo "c:10001" ;;
.1.4.3.1.1.1.4)		echo "g:20002" ;;
.1.4.3.1.1.1.5)		echo "n:30003" ;;
.1.4.3.1.1.1.6)		echo "t:4000000000" ;;
.1.4.3.1.1.1.7)		echo "a:Arbitrary string" ;;
.1.4.3.1.1.1.8)		echo "o:.1.3.6.1.2.1.1.1.0" ;;
.1.4.3.1.1.1.9)		echo "0" ;;
.1.4.3.1.1.1.10)	echo "s:Fixed string" ;;
esac
exit 0
