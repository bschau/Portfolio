var Player = {
	podcast: false,
	timeUpdateCounter: 0,
	player: false,
	pauseControl: false,
	volumeControl: false,
	progressBar: false,
	durationPanel: false,
	skipIndex: 0,
	skipTime: 0,
	skipDirection: 0,

	show: function(episode) {
		var episodeJson = atou(episode);
		localStorage.setItem('episode', episodeJson);
		var podcast = JSON.parse(episodeJson);

		Player.podcast = podcast;
		Player.run(0);
	},

	load: function (episode) {
		Player.podcast = episode;
		var currentTime = localStorage.getItem('currentTime');
		currentTime = !!currentTime ? currentTime : 0;
		currentTime = currentTime == 'undefined' ? 0 : currentTime;
		Player.run(currentTime);
	},

	run: function(currentTime) {
		podcast = Player.podcast;
		document.getElementById('podcast-album').innerHTML = 'Podcast: <i>' + podcast.album + '</i>';
		document.getElementById('podcast-title').innerHTML = podcast.title;
		document.getElementById('podcast-duration').innerHTML = 'Duration: <i>' + Player.getFormattedTime(podcast.duration) + '</i>';

		var std = getStandardParameters();
		var url = Remote.get().server + "/rest/stream" + std + "&id=" + podcast.streamId;

		var html = '<source src="' + url + '" type="' + podcast.contentType + '">';

		Player.player = document.getElementById('podcast-player');
		Player.pauseControl = document.getElementById('play-pause');
		Player.volumeControl = document.getElementById('play-volume');
		Player.progressBar = document.getElementById('player-progress');
		Player.durationPanel = document.getElementById('duration-panel');

		var player = Player.player;
		player.innerHTML = html;
		player.currentTime = parseInt(currentTime);

		player.addEventListener('timeupdate', function (event) {
			Player.onTimeUpdate();
		});

		document.getElementById('play-fr').addEventListener('click', function () {
			Player.forwardRewind(-1);
		});
		document.getElementById('play-ff').addEventListener('click', function () {
			Player.forwardRewind(1);
		});
		document.getElementById('play-pause').addEventListener('click', function () {
			Player.togglePlayPause();
		});
		document.getElementById('play-volume').addEventListener('click', function () {
			Player.toggleVolume();
		});
		document.getElementById('player-progress').addEventListener('click', function (event) {
			Player.progressBarClick(event);
		});
		Player.updateVolumeButton();
		showPage('player');

		Player.updateProgress(currentTime);
	},

	onTimeUpdate: function() {
		var player = Player.player;
		Player.updateProgress(player.currentTime);

		if (player.ended) {
			Player.playEnded();
			return;
		}

		Player.timeUpdateCounter++;
		if (Player.timeUpdateCounter > 10) {
			localStorage.setItem('currentTime', parseInt(player.currentTime));
			Player.timeUpdateCounter = 0;
		}
	},

	togglePlayPause: function () {
		var elem = Player.pauseControl;
		var state = elem.innerHTML.toLowerCase();
		var player = Player.player;

		if (state === 'pause') {
			elem.innerHTML = 'play_arrow';
			player.pause();
			return;
		}

		elem.innerHTML = 'pause';
		player.play();
	},

	toggleVolume: function () {
		var player = Player.player;
		player.muted = !player.muted;
		Player.updateVolumeButton();
	},

	forwardRewind: function (direction) {
		var now = new Date().getTime();
		if (Player.skipDirection !== direction) {
			Player.skipDirection = direction;
			Player.skipIndex = 1;
		} else {
			if (now - Player.skipTime <= 1000) {
				Player.skipIndex++
				if (Player.skipIndex > 3) {
					Player.skipIndex = 3;
				}
			} else {
				Player.skipIndex = 1;
			}
		}
		Player.skipTime = now;

		var time = Player.skipIndex * 60 * Player.skipDirection;
		var newTime = Player.player.currentTime + time;
		if (newTime < 0) {
			newTime = 0;
		} else if (newTime >= Player.player.duration) {
			newTime = Player.player.duration;
		}

		Player.player.currentTime = newTime;
		Player.updateProgress(newTime);
	},

	updateVolumeButton: function () {
		var button = Player.volumeControl;
		var player = Player.player;
		if (player.muted) {
			if (button.classList.contains('purple')) {
				return;
			}

			button.classList.add('purple');
			return;
		}

		button.classList.remove('purple');
	},

	updateProgress: function (currentTime) {
		var progressBar = Player.progressBar;
		progressBar.max = Player.podcast.duration;
		progressBar.value = currentTime;

		var durationPanel = Player.durationPanel;
		durationPanel.innerHTML = '' + Player.getFormattedTime(currentTime);

		var width = durationPanel.clientWidth;
		var position = parseInt(((currentTime * progressBar.offsetWidth) / Player.podcast.duration) - (width / 2));
		if (position < 0) {
			position = 0;
		} else if (position + width > progressBar.offsetWidth) {
			position = progressBar.offsetWidth - width;
		}

		var x = progressBar.offsetLeft + position + 'px';
		durationPanel.style.left = x;
	},

	getFormattedTime: function (time) {
		var output = new Date(1000 * time).toISOString().substr(11, 8);
		return output.startsWith('00:') ? output.substr(3) : output;
	},

	progressBarClick: function (event) {
		var progressBar = Player.progressBar;
		var x = event.clientX - progressBar.offsetLeft;
		var timeWidth = Player.podcast.duration * x;
		var newTime = parseInt(x == 0 ? 0 : timeWidth / progressBar.offsetWidth);
		Player.player.currentTime = newTime;
		Player.updateProgress(newTime);
		localStorage.setItem('currentTime', player.newTime);
	},

	playEnded: function (event) {
		localStorage.removeItem('episode');
		localStorage.removeItem('currentTime');

		showPage('podcast-ended');
	}
}
