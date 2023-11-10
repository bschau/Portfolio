import { Component } from '@angular/core';
import { NavController, NavParams, ItemSliding } from 'ionic-angular';
import { AlertController } from 'ionic-angular';
import { Player } from '../../app/interfaces/player';
import { SingletonService } from '../../services/singleton';
import { UpdatePage } from '../update/update';
import { AboutPage } from '../about/about';

@Component({
	selector: 'page-players',
	templateUrl: 'players.html',
})
export class PlayersPage {
	constructor(public navCtrl: NavController, public navParams: NavParams, private alertCtrl: AlertController, public singleton: SingletonService) {
		singleton.loadPlayers();
	}

	ionViewDidLoad() {
		this.singleton.loadPlayers();
	}

	ionViewWillEnter() {
		this.singleton.sortPlayers();
		setTimeout(() => {
			for (let player of this.singleton.players) {
				if (player.accounted_for == 0) {
					return;
				}
			}

			for (let player of this.singleton.players) {
				player.accounted_for = 0;
			}
		}, 750)
	}

	addPlayer() {
		let alert = this.alertCtrl.create({
			title: 'Add player',
			inputs: [ { name: 'name', placeholder: 'Name of player',  } ],
			buttons: [
				{ text: 'Cancel', role: 'cancel' },
				{ text: 'Add', handler: data => {
						this.singleton.players.push({ name: data.name, score: 0, phase: 0, accounted_for: 0 });
						this.singleton.savePlayers();
					}
				}
			]
		});
		alert.present();
	}

	removePlayer(slidingItem: ItemSliding, player: Player) {
		let index = this.singleton.players.indexOf(player);
		if (index > -1) {
			let prompt = this.alertCtrl.create({
				title: 'Remove player?',
				message: 'Is it OK to remove player: ' + player.name + '?',
				buttons: [
					{ text: 'Cancel', role: 'cancel', handler: data => {
							slidingItem.close();
					}},
					{ text: 'Remove', handler: data => {
							this.singleton.players.splice(index, 1);
							this.singleton.savePlayers();
						}
					}
				]
			});
			prompt.present();
		}
	}

	renamePlayer(slidingItem: ItemSliding, player: Player) {
		let alert = this.alertCtrl.create({
			title: 'Rename player',
			inputs: [ { name: 'name', placeholder: player.name, value: player.name } ],
			buttons: [
				{ text: 'Cancel', role: 'cancel', handler: data => { slidingItem.close(); } },
				{ text: 'Rename', handler: data => {
						player.name = data.name;
						this.singleton.savePlayers();
						slidingItem.close();
					}
				}
			]
		});
		alert.present();
	}

	newGame() {
		if (this.singleton.players.length == 0) {
			return;
		}

		let confirm = this.alertCtrl.create({
			title: 'Start a new game?',
			message: 'Do you want to start a new game? This will erase all players. If you want to keep players, select the Reset scores menu-item instead.',
			buttons: [
				{ text: 'Yes', handler: () => {
					this.singleton.players = [];
					this.singleton.savePlayers();
				 }},
				{ text: 'No', role: 'cancel' }
			]
		});
		confirm.present();
	}

	resetScores() {
		if (this.singleton.players.length == 0) {
			return;
		}

		var sum = 0;
		var index = this.singleton.players.length;
		while (index--) {
			sum += this.singleton.players[index].score;
		}

		if (sum == 0) {
			return;
		}

		let confirm = this.alertCtrl.create({
			title: 'Reset scores?',
			message: 'Do you want to reset the scores for the players?',
			buttons: [
				{ text: 'Yes', handler: () => {
					var index = this.singleton.players.length;
					while (index--) {
						this.singleton.players[index].score = 0;
						this.singleton.players[index].phase = 0;
						this.singleton.players[index].accounted_for = 0;
					}
					this.singleton.savePlayers();
					this.singleton.sortPlayers();
				}},
				{ text: 'No', role: 'cancel' }
			]
		});
		confirm.present();
	}

	updatePlayer(player: Player) {
		this.navCtrl.push(UpdatePage, { player: player });
	}

	about() {
		this.navCtrl.push(AboutPage);
	}
}
