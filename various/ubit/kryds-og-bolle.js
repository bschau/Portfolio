let py = 0
let px = 0
let y = 0
let x = 0
let pr = 0
let pk = 0
let position = 0
let position2 = 0
let position3 = 0
let t = 0
let ty = 0
let tx = 0
let menneskefarve = 200
let microbitfarve = 12
let ramme = 8
let blink = 0
let celler: string[] = []
let menneske = "x"
let microbit = "o"
let iSpil = " "
let status = iSpil
let hvemBegynder = menneske
let markørfarve = 255
let niveau = "l"
celler = [" ", " ", " ", " ", " ", " ", " ", " ", " "]
let foretrukne = [4, 8, 6, 2, 0, 7, 5, 3, 1]
let træk = [[8, 7, 6], [8, 5, 2], [8, 4, 0], [7, 4, 1], [6, 3, 0], [6, 4, 2], [5, 4, 3], [2, 1, 0]]
let uafgjort = "-"

function tegnRamme () {
	for (let k = 0; k < 5; k++) {
		led.plotBrightness(k, 0, ramme)
		led.plotBrightness(k, 4, ramme)
		led.plotBrightness(0, k, ramme)
		led.plotBrightness(4, k, ramme)
	}
}

function tegnCeller () {
	for (let r = 0; r < 3; r++) {
		for (let o = 0; o < 3; o++) {
			position2 = r * 3 + o
			pk = o + 1
			pr = r + 1
			if (celler[position2] == " ") {
				led.unplot(pk, pr)
			} else if (celler[position2] == microbit) {
				led.plotBrightness(pk, pr, microbitfarve)
			} else {
				led.plotBrightness(pk, pr, menneskefarve)
			}
		}
	}
}

input.onButtonPressed(Button.A, function () {
	if (x > 0) {
		x = x - 1
	} else if (y > 0) {
		x = 2
		y = y - 1
	}
})

input.onButtonPressed(Button.B, function () {
	if (x < 2) {
		x = x + 1
	} else if (y < 2) {
		x = 0
		y = y + 1
	}
})

input.onButtonPressed(Button.AB, function () {
	position = y * 3 + x
	if (celler[position] == " ") {
		celler[position] = menneske
		status = findSpilStatus()
		if (status == iSpil) {
			status = microbitPlacerer()
		}
	}
})

function microbitPlacerer() {
	// Kan microbit vinde?
	let celle = findHul(microbit)
	if (celle != -1) {
		celler[celle] = microbit
		return iSpil
	}

	// Kan microbit blokere?
	celle = findHul(menneske)
	if (celle != -1) {
		celler[celle] = microbit
		return iSpil
	}

	// Tag foretrukent felt
	for (let l = 0; l < foretrukne.length; l++) {
		let position4 = foretrukne[l]
		if (celler[position4] == ' ') {
			celler[position4] = microbit
			return iSpil
		}
	}

	return uafgjort
}

function findHul(farve: string) {
	for (let m = 0; m < 8; m++) {
		let tx2 = træk[m][0]
		let ty2 = træk[m][1]
		let tz = træk[m][2]

		if (celler[tx2] == farve &&
			celler[ty2] == farve &&
			celler[tz] == ' ') {
			return tz
		}

		if (celler[tx2] == farve &&
			celler[ty2] == ' ' &&
			celler[tz] == farve) {
			return ty2
		}

		if (celler[tx2] == ' ' &&
			celler[ty2] == farve &&
			celler[tz] == farve) {
			return tx2
		}
	}

	return -1
}

function findSpilStatus() {
	for (let n = 0; n < 8; n++) {
		let tx22 = træk[n][0]
		let ty22 = træk[n][1]
		let tz2 = træk[n][2]

		if (celler[tx22] == microbit &&
			celler[ty22] == microbit &&
			celler[tz2] == microbit) {
			return microbit
		}

		if (celler[tx22] == menneske &&
			celler[ty22] == menneske &&
			celler[tz2] == menneske) {
			return menneske
		}
	}

	return iSpil
}

function spilSlut(ikon: any) {
	if (blink == 0) {
		basic.showIcon(ikon)
		blink = 1
	} else {
		tegnRamme()
		tegnCeller()
		blink = 0
	}
}

if (niveau == "l") {
	for (let index = 0; index < 100; index++) {
		tx = Math.randomRange(0, 8) + 1
		ty = Math.randomRange(0, 8) + 1
		t = foretrukne[tx]
		foretrukne[tx] = foretrukne[ty]
		foretrukne[ty] = t
	}
	t = Math.randomRange(0, 8) + 1
	foretrukne[0] = foretrukne[t]
	foretrukne[t] = 4
} else {
	for (let index = 0; index < 100; index++) {
		tx = Math.randomRange(0, 3) + 1
		ty = Math.randomRange(0, 3) + 1
		t = foretrukne[tx]
		foretrukne[tx] = foretrukne[ty]
		foretrukne[ty] = t
		tx = Math.randomRange(0, 3) + 5
		ty = Math.randomRange(0, 3) + 5
		t = foretrukne[tx]
		foretrukne[tx] = foretrukne[ty]
		foretrukne[ty] = t
	}
}

if (hvemBegynder == microbit) {
	status = microbitPlacerer()
}

tegnRamme()
basic.forever(function () {
if (status == " ") {
	tegnCeller()
	px = x + 1
	py = y + 1
	if (blink == 0) {
		led.plotBrightness(px, py, markørfarve)
		blink = 1
	} else {
		position3 = y * 3 + x
		if (celler[position3] == " ") {
			led.unplot(px, py)
		} else if (celler[position3] == microbit) {
			led.plotBrightness(px, py, microbitfarve)
		} else {
			led.plotBrightness(px, py, menneskefarve)
		}
		blink = 0
	}
	status = findSpilStatus()
} else if (status == menneske) {
	spilSlut(IconNames.Heart)
} else if (status == microbit) {
	spilSlut(IconNames.Sad)
} else {
	spilSlut(IconNames.Duck)
}

basic.pause(400)
})
