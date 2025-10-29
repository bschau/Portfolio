pragma solidity ^0.6.4;

/* Compile with:   solc -o target --bin --abi Doubler.sol */
contract Doubler {
	function execute(int a) public pure returns(int ok) {
		return a + a;
	}
}
