// From: https://www.c64-wiki.com/wiki/Color
const background = [ "000000", "ffffff", "880000", "aaffee",
		     "cc44cc", "00cc55", "0000aa", "eeee77",
		     "dd8855", "664400", "ff7777", "333333",
		     "777777", "aaff66", "0088ff", "bbbbbb" ];

const text = [ "ffffff", "000000", "ffffff", "000000",
	       "ffffff", "000000", "ffffff", "000000",
	       "000000", "ffffff", "000000", "ffffff",
	       "ffffff", "000000", "ffffff", "000000" ];

function initLdx() {
	const index = Math.floor(Math.random() * 16);
	const pElem = document.getElementById('ldx');
	let hex = index.toString(16);
	hex = hex.length == 1 ? "0" + hex : hex;
	pElem.innerHTML = 'ldx #' + hex + '&nbsp;&nbsp;<br />stx #d020<br />stx #d021';
	const hexColor = '#' + text[index];
	document.body.style.background = '#' + background[index];
	pElem.style.color = hexColor;
	document.getElementById('small1').style.color = hexColor;
}
