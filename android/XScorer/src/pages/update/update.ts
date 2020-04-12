import { Component } from '@angular/core';
import { NavController, NavParams, Platform, ModalController } from 'ionic-angular';
import { Player } from '../../app/interfaces/player';
import { SingletonService } from '../../services/singleton';
import { ScorePage } from '../score/score';

@Component({
	selector: 'page-update',
	templateUrl: 'update.html',
})
export class UpdatePage {
	player: Player;
	phase: number;
	score: number;
	points: number;

	constructor(public navCtrl: NavController, public navParams: NavParams, public platform: Platform, public modalCtrl: ModalController, public singleton: SingletonService) {
		this.player = this.navParams.get('player');
		this.phase = this.player.phase;
		this.score = this.player.score;
		this.points = 0;
	}

	ionViewWillLeave() {
		let player = this.player;
		player.phase = parseInt('' + player.phase);
		player.score += parseInt(''+ this.points);
		if (player.phase != this.phase || player.score != this.score) {
			player.accounted_for = 1;
		}
		this.singleton.savePlayers();
	}

	presentScore() {
		let score = this.modalCtrl.create(ScorePage);
		score.present();
	}
}