#!/bin/bash
f="NeaGox.d64"
echo "Listing d64file"
./d64 l $f
echo "BAM"
./d64 b $f
echo "tslink"
./d64 f $f 1
echo "Type"
./d64 t $f
echo "Extract"
./d64 -n extract_test -o extract_test.t64 x $f 1,2
