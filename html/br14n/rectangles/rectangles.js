const SPRITE_W = 32;
const SPRITE_H = 32;
const SPRITES = 100;

var canvas = document.getElementById("rectangles");
var ctx = canvas.getContext("2d");
var screen_w, screen_h;
var fps = 60;
var start = Date.now();
var frameDuration = 1000 / fps;
var lag = 0;
var sprites = [];
var colors = [ "#f88", "#8f8", "#88f", "#ff8", "#f8f", "#8ff" ];

function random(min, max) {
	return Math.floor(Math.random() * (max - min)) + min;
}

function setWindowSize() {
	screen_w = document.body.clientWidth;
	screen_h = document.body.clientHeight;
	canvas.width = screen_w;
	canvas.height = screen_h;
}

setWindowSize();

window.addEventListener('resize', function () {
	setWindowSize();
});

var setSpeed = function() {
	var speed = 0;
	while (speed === 0) {
		speed = parseInt(random(-3, 3));
	}

	return speed;
}

var rectangle = function() {
	var r = {};
	r.x = random(0, screen_w - SPRITE_W);
	r.y = random(0, screen_h - SPRITE_H);
	r.oldX = 0;
	r.oldY = 0;
	r.speedX = setSpeed();
	r.speedY = setSpeed();
	r.fillStyle = colors[random(0, 6)];
	r.render = function(ctx, lagOffset) {
		var rX = (r.x - r.oldX) * lagOffset + r.oldX;
		var rY = (r.y - r.oldY) * lagOffset + r.oldY;
		ctx.strokeStyle = "black";
		ctx.lineWidth = 1;
		ctx.fillStyle = r.fillStyle;
		ctx.translate(rX + (SPRITE_W / 2), rY + (SPRITE_H / 2));
		ctx.beginPath();
		ctx.rect(-SPRITE_W / 2, -SPRITE_H / 2, SPRITE_W, SPRITE_H);
		ctx.stroke();
		ctx.fill();
		r.oldX = r.x;
		r.oldY = r.y;
	};

	return r;
};

for (var i = 0; i < SPRITES; i++) {
	sprites.push(rectangle());
}

function loop() {
	requestAnimationFrame(loop, canvas);

	var current = Date.now();
	var elapsed = current - start;
	start = current;
	lag += elapsed;

	while (lag >= frameDuration){
		update();
		lag -= frameDuration;
	}

	var lagOffset = lag / frameDuration;
	render(lagOffset);

	actualFps = Math.floor(1000 / elapsed);
	var status = "ms: " + elapsed + "<br />fps: " + actualFps;
	status += "<br />lag: " + Math.floor(lag);
	status += "<br />offset: " + Math.round(lagOffset);
	document.getElementById('header').innerHTML = status;
}

function update() {
	for (var i = 0; i < SPRITES; i++) {
		var sprite = sprites[i];
		sprite.x += sprite.speedX;
		sprite.y += sprite.speedY;

		if (sprite.x < 0) {
			sprite.x = 0;
			sprite.speedX = -sprite.speedX;
		} else if (sprite.x + SPRITE_W > screen_w) {
			sprite.x = screen_w - SPRITE_W;
			sprite.speedX = -sprite.speedX;
		}

		if (sprite.y < 0) {
			sprite.y = 0;
			sprite.speedY = -sprite.speedY;
		} else if (sprite.y + SPRITE_H > screen_h) {
			sprite.y = screen_h - SPRITE_H;
			sprite.speedY = -sprite.speedY;
		}
	}
}

function render(lagOffset) {
	ctx.fillStyle = "rgb(0,0,0)";
	ctx.fillRect(0, 0, screen_w, screen_h);
	for (var i = 0; i < SPRITES; i++) {
		var sprite = sprites[i];
		ctx.save();
		sprite.render(ctx, lagOffset);
		ctx.restore();
	}
}

loop();
