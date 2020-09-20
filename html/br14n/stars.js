var STARS = 512;
var SPEED = 0.05;
var PROJECTION = 128.0;
var MAX_DEPTH = 16;
var MAX_WIDTH = 40;
var stars_x = new Array(STARS);
var stars_y = new Array(STARS);
var stars_z = new Array(STARS);
var canvas = document.getElementById("stars");
var ctx = canvas.getContext("2d");
var screen_w, screen_h, middle_w, middle_h;

function random(min, max) {
	return Math.floor(Math.random() * (max - min - 1)) + min;
}

function setWindowSize() {
	screen_w = document.body.clientWidth;
	screen_h = document.body.clientHeight;
	canvas.width = screen_w;
	canvas.height = screen_h;
	middle_w = canvas.width / 2;
	middle_h = canvas.height / 2;
}

setWindowSize();

window.addEventListener('resize', function () {
	setWindowSize();
});

for (var i = 0; i < STARS; i++) {
	stars_x[i] = random(-MAX_WIDTH, MAX_WIDTH);
	stars_y[i] = random(-MAX_WIDTH, MAX_WIDTH);
	stars_z[i] = random(1, MAX_DEPTH);
}

function loop() {
	requestAnimationFrame(loop, canvas);

	ctx.fillStyle = "rgb(0, 0, 0)";
	ctx.fillRect(0, 0, canvas.width, canvas.height);

	for (var i = 0; i < STARS; i++ ) {
		stars_z[i] -= SPEED;

		if (stars_z[i] <= 0) {
			stars_x[i] = random(-MAX_WIDTH, MAX_WIDTH);
			stars_y[i] = random(-MAX_WIDTH, MAX_WIDTH);
			stars_z[i] = MAX_DEPTH;
		}

		var k = PROJECTION / stars_z[i];
		var px = stars_x[i] * k + middle_w;
		var py = stars_y[i] * k + middle_h;

		if (px < 0 || px > screen_w || py < 0 || py > screen_h) {
			continue;
		}

		var size = (1 - stars_z[i] / 16.0) * 5;
		var green = parseInt((0.8 - stars_z[i] / 32.0) * 255);
		var blue = parseInt((0.25 - stars_z[i] / 32.0) * 255);
		ctx.fillStyle = "rgb(0," + green + "," + blue + ")";
		ctx.fillRect(px, py, size, size);
	}
}

loop();
