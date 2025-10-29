#!/bin/bash
DATADIR="--datadir .schau"
cd /home/bs
if test "$1" = "new"; then
	if test -d .schau; then
		echo ".schau already exists, cannot create new account" >&2
		exit 1
	fi

	geth $DATADIR account new
	exit 0
fi

geth --lightkdf --identity schaupi --rpc --rpcport 8080 --rpccorsdomain "*" $DATADIR --port 30303 --nodiscover --rpcapi "db,eth,net,web3" --networkid 242 --nousb --allow-insecure-unlock console   #--verbosity 4
