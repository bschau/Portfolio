function set_result(html) {
	var element = document.getElementById('result');
	element.innerHTML = html;
	element.hidden = html.length == 0;
}

function clear_result() {
	set_result('');
	document.getElementById('count').value = 10;
	document.getElementById('length').value = 10;
	document.getElementById('digits').value = 2;
}

function get_passwords(count, length, digits) {
	length -= digits;
	var passwords = [];
	var consonants = 'bcdfghjklmnprstvwxyz';
	var c_len = consonants.length;
	var vocals = 'aeiou';
	var v_len = vocals.length;
	var numbers = '0123456789';
	var n_len = numbers.length;

	for (; count > 0; count--) {
		var password = '';
		var alphas = parseInt(length / 2);
		for (var i = 0; i < alphas; i++) {
			password += consonants.charAt(Math.random() * c_len);
			password += vocals.charAt(Math.random() * v_len);
		}

		if (length % 2 == 1) {
			password += vocals.charAt(Math.random() * v_len);
		}

		for (var i = 0; i < digits; i++) {
			password += numbers.charAt(Math.random() * n_len);
		}

		passwords.push(password);
	}

	return passwords;
}

function generate() {
	try {
		var count = parseInt(document.getElementById('count').value);
		if (isNaN(count) || count < 1 || count > 100) {
			throw "Passwords must be between 1 and 100";
		}
		
		var length = parseInt(document.getElementById('length').value);
		if (isNaN(length) || length < 1 || length > 50) {
			throw "Length must be between 1 and 50";
		}

		var digits = parseInt(document.getElementById('digits').value);
		if (isNaN(digits) || digits < 0 || digits > length) {
			throw "Digits must be between 0 and " + length;
		}

		var passwords = get_passwords(count, length, digits);
		var html = passwords.join('<br />');
		set_result(html);
	} catch (err) {
		alert(err);
	}
}

clear_result('');
