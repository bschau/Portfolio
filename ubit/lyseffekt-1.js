const koordinater = [
	[-5, 0], [-4, 0], [-3, 0], [-2, 0], [-1, 0],
	[0, 0], [1, 0], [2, 0], [3, 0], [4, 0],
	[4, 1], [4, 2], [4, 3], [4, 4], [3, 4],
	[2, 4], [1, 4], [0, 4], [0, 3], [0, 2],
	[0, 1], [1, 1], [2, 1], [3, 1], [3, 2],
	[3, 3], [2, 3], [1, 3], [1, 2], [2, 2]
]
const prikker = [0, 1, 2, 3, 4];

basic.forever(function () {
	let hale = prikker[0];
	let p = koordinater[hale];
	if (p[0] > -1) {
		led.unplot(p[0], p[1]);
	}

	let farve = 55;
	for (let i = 0; i < prikker.length; i++) {
		prikker[i]++;
		if (prikker[i] >= koordinater.length) {
			prikker[i] = 0;
		}

		p = koordinater[prikker[i]]
		if (p[0] > -1) {
			led.plotBrightness(p[0], p[1], farve);
		}

		farve += 50;
	}
	basic.pause(100);
})
