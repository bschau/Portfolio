import { BrowserModule } from '@angular/platform-browser';
import { ErrorHandler, NgModule } from '@angular/core';
import { IonicApp, IonicErrorHandler, IonicModule } from 'ionic-angular';
import { IonicStorageModule } from '@ionic/storage';
import { SplashScreen } from '@ionic-native/splash-screen';
import { StatusBar } from '@ionic-native/status-bar';
import { MyApp } from './app.component';
import { PlayersPage } from '../pages/players/players';
import { UpdatePage } from '../pages/update/update';
import { ScorePage } from '../pages/score/score';
import { AboutPage } from '../pages/about/about';
import { SingletonService } from '../services/singleton';
import { AppVersion } from '@ionic-native/app-version';

@NgModule({
	declarations: [
		MyApp,
		PlayersPage,
		UpdatePage,
		ScorePage,
		AboutPage
	],
	imports: [
		BrowserModule,
		IonicModule.forRoot(MyApp),
		IonicStorageModule.forRoot()
	],
	bootstrap: [IonicApp],
	entryComponents: [
		MyApp,
		PlayersPage,
		UpdatePage,
		ScorePage,
		AboutPage
	],
	providers: [
		StatusBar,
		SplashScreen,
		{provide: ErrorHandler, useClass: IonicErrorHandler},
		SingletonService,
		AppVersion
	]
})
export class AppModule {}
