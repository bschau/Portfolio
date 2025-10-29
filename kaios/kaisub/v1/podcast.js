var GENERATION = 1;

var remote = localStorage.getItem('remote');
main(remote);

function main(remote) {
	var buttons = document.getElementsByClassName('select-podcast-button');
	addClickListeners(buttons, Select.show);

	buttons = document.getElementsByClassName('logout-button');
	addClickListeners(buttons, Login.logout);

	if (!!remote) {
		Remote.set(JSON.parse(remote));

		if (shouldLogin()) {
			Login.show();
			return;
		}

		updateLastLogin();
		var episodeJson = localStorage.getItem('episode');
		if (!!episodeJson) {
			var episode = JSON.parse(episodeJson);
			var http = getXhr('/rest/getPodcasts');
			http.onload = () => continuePlaying(http.responseText, episode);
		} else {
			Select.show();
		}
		return;
	}

	Login.show();
}

function addClickListeners(buttons, func) {
	for (var butIdx = 0; butIdx < buttons.length; butIdx++) {
		var button = buttons[butIdx];
		button.addEventListener('click', function () {
			func()
		});
	}
}

function shouldLogin() {
	var lastLogin = localStorage.getItem('lastLogin');
	if (!!lastLogin) {
		var now = new Date().getTime();
		return (lastLogin + 2419200) < now;
	}

	return true;
}

function continuePlaying(json, srcEpisode) {
	var response = JSON.parse(json)["subsonic-response"];
	if (response.status.toLowerCase() === 'failed') {
		Login.showWithError(response.error.message);
		return;
	}

	var channels = response.podcasts.channel;
	if (channels.length == 0) {
		Select.showNoPodcasts();
		return;
	}

	for (var chanIdx = 0; chanIdx < channels.length; chanIdx++) {
		var channel = channels[chanIdx];
		if (typeof channel.episode !== 'undefined') {
			var episodes = channel.episode;

			for (var eps = 0; eps < episodes.length; eps++) {
				var episode = episodes[eps];
				if (episode.status.toLowerCase() !== 'completed') {
					continue;
				}
				if (episode.streamId == srcEpisode.streamId) {
					Player.load(episode);
					return;
				}
			}
		}
	}

	Select.show();
}

function updateLastLogin() {
	localStorage.setItem('lastLogin', new Date().getTime());
}

function showPage(page) {
	var pages = document.getElementsByClassName('page');
	for (var i = 0; i < pages.length; i++) {
		pages[i].style.display = 'none';
	}

	document.getElementById(page).style.display = 'block';
}

function getStandardParameters() {
	var remote = Remote.get();
	var salt = getSalt();
	var token = CryptoJS.MD5(remote.password.concat(salt));
	var std = "?u=" + remote.username;
	std += "&t=" + token;
	std += "&s=" + salt;
	std += "&v=1.15.0&c=kaisub&f=json";
	return std;
}

function getSalt() {
	var s = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	return Array.apply(null, Array(8)).map(function() {
		return s.charAt(Math.floor(Math.random() * s.length));
	}).join('');
}

function getXhr(url) {
	var std = getStandardParameters();
	var url = Remote.get().server + url + std;

	const http = new XMLHttpRequest()
	http.open("GET", url);
	http.send()
	return http;
}