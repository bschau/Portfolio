all:	clean compile

clean:
	rm -fr target
	rm -f doubler.js
	rm -f *~

compile:
	solc -o target --bin --abi Doubler.sol
	./replace.sh
