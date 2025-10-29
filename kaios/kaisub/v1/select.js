var Select = {
	show: function () {
		var http = getXhr('/rest/ping');
		http.onload = () => Select.pingCallback(http.responseText)
	},

	pingCallback: function (json) {
		var response = JSON.parse(json)["subsonic-response"];
		if (response.status.toLowerCase() === 'failed') {
			Login.showWithError(response.error.message);
			return;
		}

		showPage('fetching');
		var http = getXhr('/rest/getPodcasts');
		http.onload = () => Select.getNewestPodcastCallback(http.responseText);
	},

	getNewestPodcastCallback: function(json) {
		var response = JSON.parse(json)["subsonic-response"];
		if (response.status.toLowerCase() === 'failed') {
			Login.showWithError(response.error.message);
			return;
		}

		if (typeof response.podcasts.channel === 'undefined') {
			Select.showNoPodcasts();
			return;
		}

		var channels = response.podcasts.channel;
		if (channels.length == 0) {
			Select.showNoPodcasts();
			return;
		}

		var html = '';
		for (var chanIdx = 0; chanIdx < channels.length; chanIdx++) {
			var channel = channels[chanIdx];
			if (typeof channel.episode === 'undefined') {
				continue;
			}

			var episodes = channel.episode;
			episodes.sort(Select.sort);

			for (var eps = 0; eps < episodes.length; eps++) {
				var episode = episodes[eps];
				if (episode.status.toLowerCase() !== 'completed') {
					continue;
				}

				var isNew = episode.playCount === 0;
				html += '<button class="episode base-button button-' + (isNew ? 'purple' : 'grey');
				html += '" data-json="' + utoa(JSON.stringify(episode)) + '">';
				html += '<span class="small">';
				if (isNew) {
					html += '<b>';
				} else {
					html += '&#10004;&nbsp;';
				}

				html += new Date(episode.publishDate).toISOString().substr(0, 10);
				html += '&nbsp;' + channel.title + '</span><br />';
				html += episode.title;
				if (isNew) {
					html += '</b>';
				}
				html += '</button>';
			}
		}

		if (html.trim().length < 1) {
			Select.showNoPodcasts();
			return;
		}

		document.getElementById('podcast-episodes').innerHTML = html;
		showPage('select-podcast');
		var elements = document.getElementsByClassName('episode');

		for (var i = 0; i < elements.length; i++) {
			var element = elements[i];
			element.addEventListener('click', function (event) {
				var element = event.currentTarget;
				var json = element.getAttribute('data-json');
				Player.show(json);
			});
		}
	},

	sort: function (lhs, rhs) {
		var lpd = new Date(lhs.publishDate).getTime();
		var rpd = new Date(rhs.publishDate).getTime();

		if (lpd === rpd) {
			var la = lhs.album.toLowerCase();
			var ra = rhs.album.toLowerCase();

			if (la === ra) {
				return 0;
			}

			return la > ra ? 1 : -1;
		}

		return lpd > rpd ? -1 : 1;
	},

	showNoPodcasts: function () {
		showPage('no-podcasts');
	}
}
