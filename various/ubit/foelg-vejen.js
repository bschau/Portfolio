let x = 1
let l1 = 2
let l2 = 2
let l3 = 2
let l4 = 2
let l5 = 2
let længde = 0

input.onButtonPressed(Button.A, function () {
	if (x > -1) {
		x = x - 1
	}
})

input.onButtonPressed(Button.B, function () {
	if (x < 4) {
		x = x + 1
	}
})

basic.forever(function () {
	led.unplot(l1, 0)
	led.unplot(l2, 1)
	led.unplot(l3, 2)
	led.unplot(l4, 3)
	led.unplot(0, 4)
	led.unplot(1, 4)
	led.unplot(2, 4)
	led.unplot(3, 4)
	led.unplot(4, 4)
	l5 = l4
	l4 = l3
	l3 = l2
	l2 = l1
	længde = længde + 1
	if (længde == 4) {
		længde = 0
		if (Math.randomRange(1, 10) < 6) {
			if (l1 > 0) {
				l1 = l1 - 1
			}
		} else {
			if (l1 < 4) {
				l1 = l1 + 1
			}
		}
	}
	led.plot(l1, 0)
	led.plot(l2, 1)
	led.plot(l3, 2)
	led.plot(l4, 3)
	led.plot(l5, 4)
	led.plot(x, 4)
	led.plot(x + 1, 4)
	if (l5 < x || l5 > x + 1) {
		basic.clearScreen()
		basic.showIcon(IconNames.Sad)
		basic.pause(5000)
		control.reset()
	}
	basic.pause(200)
})
