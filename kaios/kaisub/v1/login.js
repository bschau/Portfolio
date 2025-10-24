var Login = {
	errorTimeout: false,

	show: function() {
		if (Remote.remote !== false) {
			document.getElementById('username').value = Remote.get().username;
			document.getElementById('server').value = Remote.get().server;
		}
		showPage('login');
		document.getElementById('username').focus();
		document.getElementById('login-button').addEventListener('click', Login.login);
		document.getElementById('subsonic-demo-button').addEventListener('click', Login.setDemoServer);
	},

	showWithError: function(error) {
		Login.show();
		Login.showError(error);
	},

	login: function() {
		try {
			var username = Login.GetValue('username');
			var password = Login.GetValue('password');
			var server = Login.GetValue('server');

			var remote = {
				version: GENERATION,
				username: username,
				password: password,
				server: server
			};

			localStorage.setItem('remote', JSON.stringify(remote));
			updateLastLogin();
			Remote.set(remote);
			Select.show();
		} catch (exception) {
			Login.showError(exception);
		}
	},

	setDemoServer: function() {
		document.getElementById('username').value = "guest";
		document.getElementById('password').value = "guest";
		document.getElementById('server').value = "http://demo.subsonic.org";
		document.getElementById('login-button').click();
	},

	showError: function(text) {
		if (Login.errorTimeout !== false) {
			clearInterval(Login.errorTimeout);
		}

		var errorPage = document.getElementById('login-page-error');
		errorPage.innerHTML = text;
		errorPage.style.display = 'block';

		Login.errorTimeout = setTimeout(Login.clearDialog, 5000);
	},

	GetValue: function(key) {
		var element = document.getElementById(key);
		var value = element.value;
		if (!!value) {
			var trimmed = value.trim();
			if (trimmed.length > 0) {
				return trimmed;
			}
		}

		element.focus();
		throw "You must enter something in the '" + key + "' field.";
	},

	clearDialog: function() {
		clearTimeout(Login.errorTimeout);
		document.getElementById('login-page-error').style.display = 'none';
	},

	logout: function() {
		localStorage.removeItem('lastLogin');
		Login.show();
	}
}
