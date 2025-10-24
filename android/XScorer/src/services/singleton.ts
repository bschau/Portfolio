import { Injectable } from '@angular/core';
import { Storage } from '@ionic/storage';
import { Player } from '../app/interfaces/player';

@Injectable()
export class SingletonService {
	public players: Array<Player> = [];
	public phases: Array<string> = [
		"Two sets of three",
		"One set of three + one run of four",
		"One set of four + one run of four",
		"One run of seven",
		"One run of eight",
		"One run of nine",
		"Two sets of four",
		"Seven cards of one color",
		"One set of five + one set of two",
		"One set of five + one set of three"
	];
	public version: string = "Unknown";

	constructor(public storage: Storage) { }

	public sortPlayers() {
		this.players = this.players.sort((l: Player, r: Player) =>
		{
			if (l.phase > r.phase) {
				return -1;
			}

			if (l.phase < r.phase) {
				return 1;
			}

			if (l.score < r.score) {
				return -1;
			}

			if (l.score > r.score) {
				return 1;
			}

			return l.name.localeCompare(r.name);
		});
	}

	public savePlayers() {
		var state = JSON.stringify(this.players);
		this.storage.set("state", state);
	}

	public loadPlayers() {
		this.players = [];
		this.storage.get("state").then((data) => {
			if (data == null) {
				this.players = [];
				return;
			}
			this.players = JSON.parse(data);
			this.sortPlayers();
		});
	}
}