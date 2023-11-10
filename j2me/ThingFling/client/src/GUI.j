			// build commands
			cmdFling=new Command("#=LCmdFling", Command.OK, 0);
			cmdSave=new Command("#=LCmdSave", Command.OK, 0);
			cmdBack=new Command("#=LCmdBack", Command.BACK, 0);
			cmdOptions=new Command("#=LCmdOptions", Command.BACK, 0);
			cmdCancel=new Command("#=LCmdCancel", Command.BACK, 0);

			// left button list
			lstOptions=new List("", List.IMPLICIT);
			lstOptions.append("#=LListBack", null);
			lstOptions.append("#=LListMobFace", null);
			lstOptions.append("#=LListPrefs", null);
			lstOptions.append("#=LListAbout", null);
			lstOptions.append("#=LListExit", null);
			lstOptions.setCommandListener(this);

			// main form
			flingTxt=new TextField("#=LMainText", "", 300, TextField.PLAIN);
			flingTxt.setLayout(TextField.LAYOUT_NEWLINE_AFTER|TextField.LAYOUT_CENTER);
			fMain=new Form("Thing Fling");
			fMain.append(getLogo());
			fMain.append(new Spacer(fMain.getWidth(), 8));
			fMain.append(flingTxt);

			fMain.addCommand(cmdOptions);
			fMain.addCommand(cmdFling);
			fMain.setCommandListener(this);

			// preferences form
			prfUID=new TextField("UID", "", 16, TextField.NUMERIC);
			prfPin=new TextField("Pin", "", 16, TextField.PASSWORD);

			fPrefs=new Form("#=LPrefsTitle");
			fPrefs.append(getLogo());
			fPrefs.append(new Spacer(fPrefs.getWidth(), 8));
			fPrefs.append("#=(LPrefsText):");
			fPrefs.append(new Spacer(fPrefs.getWidth(), 8));
			fPrefs.append(prfUID);
			fPrefs.append(prfPin);
			fPrefs.addCommand(cmdCancel);
			fPrefs.addCommand(cmdSave);
			fPrefs.setCommandListener(this);

			// HTTP form
			httpTxt=new StringItem("", "", StringItem.PLAIN);
			httpTxt.setLayout(StringItem.LAYOUT_NEWLINE_BEFORE|StringItem.LAYOUT_CENTER);
			httpGauge=new Gauge("", false, 3, 0);
			httpGauge.setLayout(Gauge.LAYOUT_CENTER);
			fHTTP=new Form("#=LHttpTitle");
			fHTTP.append(getLogo());
			fHTTP.append(new Spacer(fHTTP.getWidth(), 8));
			fHTTP.append(httpTxt);
			fHTTP.append(new Spacer(fHTTP.getWidth(), 8));
			fHTTP.append(httpGauge);
			fHTTP.addCommand(cmdCancel);
			fHTTP.setCommandListener(this);

			// about form
			fAbout=new Form("#=LAboutTitle");
			fAbout.append(getLogo());
			fAbout.append(new Spacer(fAbout.getWidth(), 8));
			fAbout.append(new StringItem("Version:", "1.0"));
			fAbout.append(new StringItem("URL:", "http://www.schau.com/"));
			fAbout.addCommand(cmdBack);
			fAbout.setCommandListener(this);

			// get pin form
			tPin=new TextField("#=LGetPinText", "", 16, TextField.PASSWORD);
			fGetPin=new Form("#=LGetPinTitle");
			fGetPin.append(getLogo());
			fGetPin.append(new Spacer(fGetPin.getWidth(), 8));
			fGetPin.append(tPin);
			fGetPin.addCommand(cmdBack);
			fGetPin.addCommand(cmdFling);
			fGetPin.setCommandListener(this);
