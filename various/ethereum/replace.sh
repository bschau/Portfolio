#!/bin/bash
abi="$(cat target/Doubler.abi)"
bin="$(cat target/Doubler.bin)"
cat doubler.in | sed "s/@ABI@/$abi/" | sed "s/@BIN@/$bin/" > doubler.js
