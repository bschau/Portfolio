const storage = new Storage();
const height = 180;
const startDate = new Date(2023, 0, 9);
const startWeight = 141.6;
const goal = 90;
const dateOptions = { day: "numeric", month: "long", year: "numeric" };

function run() {
	document.getElementById('height').innerHTML = height;
	document.getElementById('startDate').innerHTML = new Intl.DateTimeFormat("da-DK", dateOptions).format(startDate);
	document.getElementById('startWeight').innerHTML = startWeight;
	document.getElementById('goal').innerHTML = goal;

	if (storage.exists('weight') !== false) {
		var weight = storage.get('weight');
		document.getElementById('weight').value = weight;
		ShowResults(weight);
	}

	document.getElementById('calculate').addEventListener('click', () => { calculate() });
}


function calculate() {
	var weightValue = document.getElementById('weight').value.replace(',', '.');
	var weight = parseFloat(weightValue);
	if (isNaN(weight)) {
		console.log("Is not a number: '" + weightValue + "'");
		return;
	}

	storage.set('weight', weight);
	ShowResults(weight);
}

function ShowResults(weight) {
	var bmi = (weight / ((height * height) / 10000)).toFixed(1);
	document.getElementById('bmi').innerHTML = bmi;

	var now = Date.now();
	var diffTime = Math.abs(now - startDate);
	var diffDays = Math.ceil(diffTime / (1000 * 60 * 60 * 24));
	document.getElementById('stretch').innerHTML = diffDays;

	var lossPrDay = (((startWeight - weight) / diffDays) * 1000).toFixed(0);
	document.getElementById('lossPrDay').innerHTML = lossPrDay;

	var daysToDone = (((startWeight - goal) * 1000) / lossPrDay).toFixed(0);
	document.getElementById('daysToDone').innerHTML = daysToDone;
}

