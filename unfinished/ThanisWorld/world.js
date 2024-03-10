var World = {
	I_CLEAR: 0,
	I_MOUNTAIN: 1,
	I_LAKE: 2,
	I_FOREST: 3,
	I_CITY: 4,
	WIDTH: 17,
	HEIGHT: 17,
	TILE_SIZE: 32,
	map_w: 0,
	map_h: 0,
	map: [],
	cities: [],
	player_x: 0,
	player_y: 0,
	canvas: null,
	ctx: null,
	colors: [],
	frame: 1,
	frame_speed: 100,
	left_pressed: false,
	right_pressed: false,
	up_pressed: false,
	down_pressed: false,

	init: function (map_w, map_h, map, cities, player_x, player_y) {
		World.map_w = map_w;
		World.map_h = map_h;
		World.map = map;
		World.cities = cities;
		World.player_x = player_x;
		World.player_y = player_y;
		World.canvas = document.getElementById('playfield');
		World.ctx = World.canvas.getContext('2d');
		World.colors[World.I_CLEAR] = '#ffffff';
		World.colors[World.I_MOUNTAIN] = '#deb887';
		World.colors[World.I_LAKE] = '#0000ff';
		World.colors[World.I_FOREST] = '#00ff00';
		World.colors[World.I_CITY] = '#000000';

		document.addEventListener('keydown', World.keyDownHandler, false);
		document.addEventListener('keyup', World.keyUpHandler, false);
	},

	render: function (timestamp) {
		if ((timestamp - World.frame) < World.frame_speed) {
			window.requestAnimationFrame(World.render);
			return;
		}

		World.frame = timestamp;

		var save_x = World.player_x;
		var save_y = World.player_y;
		if (World.left_pressed) {
			if (World.player_x > 0) {
				World.player_x--;
			}
		}

		if (World.right_pressed) {
			if (World.player_x < World.map_w - 1) {
				World.player_x++;
			}
		}

		if (World.up_pressed) {
			if (World.player_y > 0) {
				World.player_y--;
			}
		}

		if (World.down_pressed) {
			if (World.player_y < World.map_h - 1) {
				World.player_y++;
			}
		}

		if (!World.can_enter()) {
			World.player_x = save_x;
			World.player_y = save_y;
		}

		World.set_frame_speed();

		var plx = World.player_x - Math.floor(World.WIDTH / 2);
		if (plx < 0) {
			plx = 0;
		} else if (plx > (World.map_w - World.WIDTH)) {
			plx = World.map_w - World.WIDTH;
		}

		var ply = World.player_y - Math.floor(World.HEIGHT / 2);
		if (ply < 0) {
			ply = 0;
		} else if (ply > (World.map_h - World.HEIGHT)) {
			ply = World.map_h - World.HEIGHT;
		}

		var ctx = World.ctx;
		var index = (ply * World.map_w) + plx;
		for (var s_y = 0; s_y < World.HEIGHT; s_y++) {
			for (var s_x = 0; s_x < World.WIDTH; s_x++) {
				ctx.fillStyle = World.colors[World.map[index]];
				ctx.fillRect(s_x * World.TILE_SIZE, s_y * World.TILE_SIZE, World.TILE_SIZE, World.TILE_SIZE);
				index++;
			}
			index += World.map_w - World.WIDTH;
		}

		ctx.fillStyle = '#ff00ff';
		var s_x = World.player_x - plx;
		var s_y = World.player_y - ply;
		ctx.fillRect(s_x * World.TILE_SIZE, s_y * World.TILE_SIZE, World.TILE_SIZE, World.TILE_SIZE);

		window.requestAnimationFrame(World.render);
	},

	can_enter: function() {
		var index = (World.player_y * World.map_w) + World.player_x;
		return World.map[index] != World.I_LAKE;
	},

	set_frame_speed: function() {
		var index = (World.player_y * World.map_w) + World.player_x;
		var tile = World.map[index];
		World.frame_speed = tile == World.I_FOREST || tile == World.I_MOUNTAIN ? 200 : 100;
	},

	keyDownHandler: function(event) {
		if (event.keyCode == 39) {
			World.right_pressed = true;
		} else if (event.keyCode == 37) {
			World.left_pressed = true;
		} else if (event.keyCode == 40) {
			World.down_pressed = true;
		} else if (event.keyCode == 38) {
			World.up_pressed = true;
		}
	},

	keyUpHandler: function(event) {
		if (event.keyCode == 39) {
			World.right_pressed = false;
		} else if (event.keyCode == 37) {
			World.left_pressed = false;
		} else if (event.keyCode == 40) {
			World.down_pressed = false;
		} else if (event.keyCode == 38) {
			World.up_pressed = false;
		}
	}
}