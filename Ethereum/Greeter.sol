pragma solidity ^0.5.11;

/* Compile with:   solc -o target --bin --abi Greeter.sol */
contract Greeter {
    /* Define variable greeting of the type string */
    string public greeting;
    address payable owner;

    /* This runs when the contract is executed */
    constructor (string memory _greeting) public {
        greeting = _greeting;
	owner = msg.sender;
    }

    function kill() public {
    	if (msg.sender == owner) {
		selfdestruct(owner);
	}
    }

    /* Main function */
    function greet() external view returns (string memory) {
        return greeting;
    }

	function () external payable {
		// to receive ether
	}
}
