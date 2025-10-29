import { Component } from '@angular/core';
import { AppVersion } from '@ionic-native/app-version';
import { Platform } from 'ionic-angular';
import { StatusBar } from '@ionic-native/status-bar';
import { SplashScreen } from '@ionic-native/splash-screen';
import { PlayersPage } from '../pages/players/players';
import { SingletonService } from '../services/singleton';

@Component({
	templateUrl: 'app.html'
})
export class MyApp {
	rootPage:any = PlayersPage;

	constructor(platform: Platform, statusBar: StatusBar, splashScreen: SplashScreen, singleton: SingletonService, appVersion: AppVersion) {
		platform.ready().then(() => {
			if (platform.is("cordova")) {
				appVersion.getVersionNumber().then((data) => {
					singleton.version = data;
				});
			}
			statusBar.styleDefault();
			splashScreen.hide();
		});
	}
}
