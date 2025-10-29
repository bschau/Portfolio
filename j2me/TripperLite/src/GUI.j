			// build commands
			cmdBack=new Command("#=LCmdBack", Command.BACK, 0);
			cmdOptions=new Command("#=LCmdOptions", Command.BACK, 0);
			cmdSave=new Command("#=LCmdSave", Command.OK, 0);
			cmdDelete=new Command("#=LCmdDelete", Command.OK, 0);

			// About form
			fAbout=new Form("#=LAboutTitle");
			Image logo=Image.createImage("/logo.png");
			ImageItem l=new ImageItem(null, logo, Item.LAYOUT_CENTER, null);
			fAbout.append(l);
			fAbout.append(new Spacer(fAbout.getWidth(), 8));

			StringItem s=new StringItem("#=LAboutVersion", "1.0");
			fAbout.append(s);
			s.setLayout(Item.LAYOUT_CENTER);

			s=new StringItem("URL:", "http://www.schau.com/");
			fAbout.append(s);
			s.setLayout(Item.LAYOUT_CENTER);
			fAbout.addCommand(cmdBack);
			fAbout.setCommandListener(this);

			// Edit form
			fEdit=new Form("#=LEditTitle");
			seChoices=new ChoiceGroup("#=LEditTitle", ChoiceGroup.EXCLUSIVE, new String[] { "#=LEditNew", "#=LEditSent", "#=LEditClosed" }, null);
			seChoices.setLayout(Item.LAYOUT_LEFT|Item.LAYOUT_NEWLINE_AFTER);
			seDate=new DateField("#=LEditDate", DateField.DATE);
			seDate.setLayout(Item.LAYOUT_LEFT|Item.LAYOUT_NEWLINE_AFTER);
			seMileage=new TextField("#=LEditMileage", "", 4, TextField.NUMERIC);
			seMileage.setLayout(Item.LAYOUT_LEFT|Item.LAYOUT_NEWLINE_AFTER);
			sePurpose=new TextField("#=LEditPurpose", "", 200, TextField.ANY);
			sePurpose.setLayout(Item.LAYOUT_LEFT|Item.LAYOUT_NEWLINE_AFTER);

			fEdit.append(seChoices);
			fEdit.append(seDate);
			fEdit.append(seMileage);
			fEdit.append(sePurpose);
			fEdit.addCommand(cmdSave);
			fEdit.addCommand(cmdOptions);
			fEdit.setCommandListener(this);

			// Edit Options
			lstEditOptions=new List("", List.IMPLICIT);
			lstEditOptions.append("#=LEditCancel", null);
			lstEditOptions.append("#=LEditDelete", null);
			lstEditOptions.setCommandListener(this);

			// Clean Up
			fCleanUp=new Form("#=LCleanUpTitle");
			cuChoices=new ChoiceGroup("#=LCleanUpOpt", ChoiceGroup.MULTIPLE, new String[] { "#=LCleanUpNew", "#=LCleanUpSent", "#=LCleanUpClosed" }, null);
			cuChoices.setLayout(Item.LAYOUT_LEFT|Item.LAYOUT_NEWLINE_AFTER);

			cuDate=new DateField("#=LCleanUpDate", DateField.DATE);
			cuDate.setLayout(Item.LAYOUT_LEFT|Item.LAYOUT_NEWLINE_AFTER);
			fCleanUp.append(cuChoices);
			fCleanUp.append(cuDate);
			fCleanUp.addCommand(cmdDelete);
			fCleanUp.addCommand(cmdBack);
			fCleanUp.setCommandListener(this);
