#!/bin/bash
cd $HOME
rm -fr .schau
if test -d .schau; then
	echo "$HOME/.schau already exists" >&2
	exit 1
fi

out="$HOME/init.$$"
pwd="$HOME/pwd.$$"
trap "rm -f $out $pwd" 0

password="abc123"
echo "$password" > "$pwd"

echo "Creating account ..."
geth --datadir .schau --password "$pwd" account new | tee "$out"
set -- $(grep "Public address of the key:" "$out")
account="${6:2}"
echo "$account" >$HOME/account-key-address

{
	cat <<EOF
{
	"config": {
		"chainId": 242,
		"homesteadBlock": 0,
		"eip150Block": 0,
		"eip155Block": 0,
		"eip158Block": 0,
		"byzantiumBlock": 0,
		"constantinopleBlock": 0
	},
	"difficulty": "0x20000",
	"gasLimit": "0x8000000",
	"alloc": {
		"$account": {
			"balance": "20000000000000000000"
		}
	}
}
EOF
} > $out

echo "Initializing"
geth --datadir .schau init $out

{
	cat <<EOF
personal.unlockAccount("$account", "$password", 0);
loadScript("/home/bs/Workspace/Ethereum/doubler.js");
miner.start();
EOF
} > load

exit 0
