let Nedre = 0
let Øvre = 0
let Gæt = 0
input.onButtonPressed(Button.AB, function () {
	Nedre = 1
	Øvre = 20
	Gæt = Math.randomRange(Nedre, Øvre)
	basic.showNumber(Gæt)
})
input.onButtonPressed(Button.A, function () {
	Øvre = Gæt - 1
	Gæt = Math.round((Øvre + Nedre) / 2)
	basic.showNumber(Gæt)
})
input.onButtonPressed(Button.B, function () {
	Nedre = Gæt + 1
	Gæt = Math.round((Øvre + Nedre) / 2)
	basic.showNumber(Gæt)
})
