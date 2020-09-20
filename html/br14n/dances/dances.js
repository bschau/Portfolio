var dots = document.getElementById('dots').getBoundingClientRect();
var x_adjust = dots.left + window.scrollX;
var y_adjust = dots.top + window.scrollY;
var cos_tab = [];
var sin_tab = [];
var amplitude = 200;
var table_length = 512;
var step = (2 * Math.PI) / table_length;
var point = 0;

for (var i = 0; i < table_length; i++) {
	cos_tab[i] = ((Math.cos(point) * amplitude) + amplitude);
	sin_tab[i] = ((Math.sin(point) * amplitude) + amplitude);
	point += step;
}

var divs = [];
for (var i = 0; i < 8; i++) {
	divs[i] = document.getElementById('s' + i);
}

var controls = []
controls["c_speed_1"] = 3;
controls["c_speed_2"] = -1;
controls["c_stride"] = 20;
controls["s_speed_1"] = -1;
controls["s_speed_2"] = 2;
controls["s_stride"] = 40;
controls["c_index_1"] = 32;
controls["c_index_2"] = 36;
controls["s_index_1"] = 100;
controls["s_index_2"] = 200;
document.getElementById("c_index_1").value = controls["c_index_1"];
document.getElementById("c_index_2").value = controls["c_index_2"];
document.getElementById("s_index_1").value = controls["s_index_1"];
document.getElementById("s_index_2").value = controls["s_index_2"];

window.requestAnimationFrame(animation);

function bound_add(value, add) {
	value += add;
	if (value > table_length - 1) {
		return value - table_length;
	}

	if (value < 0) {
		return table_length + value;
	}

	return value;
}

function animation(timestamp) {
	let c_index = bound_add(controls["c_index_1"], controls["c_index_2"]);
	var s_index = bound_add(controls["s_index_1"], controls["s_index_2"]);
	for (var i = 0; i < 8; i++) {
		divs[i].style.left = cos_tab[c_index] + x_adjust + 'px';
		divs[i].style.top = sin_tab[s_index] + y_adjust + 'px';
		c_index = bound_add(c_index, controls["c_stride"]);
		s_index = bound_add(s_index, controls["s_stride"]);
	}
	controls["c_index_1"] = bound_add(controls["c_index_1"], controls["c_speed_1"]);
	controls["c_index_2"] = bound_add(controls["c_index_2"], controls["c_speed_2"]);
	controls["s_index_1"] = bound_add(controls["s_index_1"], controls["s_speed_1"]);
	controls["s_index_2"] = bound_add(controls["s_index_2"], controls["s_speed_2"]);
	window.requestAnimationFrame(animation);
}

var nodes = document.querySelectorAll("input[type=range]");
for (var i = 0; i < nodes.length; i++) {
	nodes[i].onchange = function() {
		const id = this.id;
		controls[id] = parseInt(this.value) - parseInt(this.dataset.adjust);
	}
}

nodes = document.querySelectorAll("input[type=numeric]");
for (var i = 0; i < nodes.length; i++) {
	nodes[i].onkeyup = function(event) {
		if (event.keyCode != 13) {
			return;
		}

		event.preventDefault();
		inputLostFocus(this);
	}
	nodes[i].addEventListener("blur", blur, true);
}

function blur() {
	inputLostFocus(this);
}

function inputLostFocus(element) {
	const id = element.id;
	controls[id] = parseInt(element.value);
}
