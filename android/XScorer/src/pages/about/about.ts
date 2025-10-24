import { Component } from '@angular/core';
import { NavController, NavParams } from 'ionic-angular';
import { SingletonService } from '../../services/singleton';

@Component({
	selector: 'page-about',
	templateUrl: 'about.html',
})
export class AboutPage {
	constructor(public navCtrl: NavController, public navParams: NavParams, public singleton: SingletonService) {
	}
}