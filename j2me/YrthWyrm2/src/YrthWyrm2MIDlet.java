import java.util.Random;
import java.lang.Math;
import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import javax.microedition.lcdui.*;
import javax.microedition.midlet.*;
import javax.microedition.rms.RecordStore;
import javax.microedition.rms.RecordStoreException;
import javax.microedition.lcdui.game.*;
import javax.microedition.media.*;
import javax.microedition.media.control.*;

public class YrthWyrm2MIDlet extends MIDlet implements CommandListener {
	private static final String STORENAME="Preferences";
	private static final int BoomDX[]={ -1, 0, 1, -1, 1, -1, 0, 1 };
	private static final int BoomDY[]={ -1, -1, -1, 0, 0, 1, 1, 1 };

	protected boolean started=false;
	private RecordStore prfRS=null;
	private Form fSorry=null, fInstructions=null;
	private Form fAbout=null, fOptions=null;
	private Canvas cIntro=null;
	private HiScore cHiScore=null;
	private Display display;
	private Command cmdBack, cmdBummer, cmdOptions;
	private Game game;
	private int hiScore, optSpeed, optSound, optTail;
	private List lstOptions;
	private ChoiceGroup cgSpeed, cgSound, cgTail;
	private Image imgStar;
	private int screenH, screenW;

	protected void startApp() {
		if (!started) {
			started=true;
			display=Display.getDisplay(this);

			PreferencesOpen();
			PreferencesGet();

			cmdBack=new Command("Back", Command.BACK, 0);
			cmdOptions=new Command("Options", Command.OK, 0);

			lstOptions=new List("", List.IMPLICIT);
			lstOptions.append("Play", null);
			lstOptions.append("High Score", null);
			lstOptions.append("Options", null);
			lstOptions.append("Instructions", null);
			lstOptions.append("About", null);
			lstOptions.append("Exit", null);
			lstOptions.setCommandListener(this);

			cIntro=new Intro();
			cIntro.addCommand(cmdOptions);
			cIntro.setCommandListener(this);

			cHiScore=new HiScore();
			((Canvas)cHiScore).addCommand(cmdOptions);
			((Canvas)cHiScore).setCommandListener(this);

			try {
				imgStar=Image.createImage("/star.png");
			} catch (Exception ex) {
				imgStar=null;
			}
	
			display.setCurrent(cIntro);
		}
	}

	protected void pauseApp() {
		notifyDestroyed();
	}

	protected void destroyApp(boolean unconditional) {
		if (game!=null) {
			game.Stop();
			game=null;
		}

		if (cIntro!=null) {
			((Intro)cIntro).Silence();
			cIntro=null;
		}

		PreferencesClose();
	}

	private void PreferencesOpen() {
		try {
			prfRS=RecordStore.openRecordStore(STORENAME, true);
		} catch (RecordStoreException rse) {}
	}

	private void PreferencesClose() {
		if (prfRS!=null) {
			PreferencesSet();
			try {
				prfRS.closeRecordStore();
				prfRS=null;
			} catch (RecordStoreException rse) {}
		}
	}

	private void PreferencesGet() {
		if (prfRS!=null) {
			try {
				if (prfRS.getNumRecords()>0) {
					byte[] b=prfRS.getRecord(1);
					DataInputStream d=new DataInputStream(new ByteArrayInputStream(b));
					try {
						hiScore=d.readInt();
						int options=d.readInt();
						optSpeed=(options>>16)&0xff;
						optSound=(options>>8)&0xff;
						optTail=options&0xff;
					} catch (IOException ex) {}
				} else {
					hiScore=0;
					optSpeed=1;
					optSound=1;
					optTail=1;
				}
			} catch (RecordStoreException rse) {}
		}
	}

	private void PreferencesSet() {
		if (prfRS!=null) {
			ByteArrayOutputStream b=new ByteArrayOutputStream();
			DataOutputStream d=new DataOutputStream(b);

			try {
				int options;

				options=(optSpeed<<16)|(optSound<<8)|optTail;
				d.writeInt(hiScore);
				d.writeInt(options);
			} catch (IOException ex) {}

			byte[] a=b.toByteArray();
			try {
				if (prfRS.getNumRecords()>0)
					prfRS.setRecord(1, a, 0, a.length);
				else
					prfRS.addRecord(a, 0, a.length);
			} catch (RecordStoreException rse) {}
		}
	}

	public void commandAction(Command c, Displayable d) {
		if (c==cmdBummer) {
			notifyDestroyed();
		} else if (c==cmdOptions) {
			if (d==cIntro) {
				((Intro)cIntro).Silence();
				cIntro=null;
			}

			display.setCurrent(lstOptions);
		} else if (d==lstOptions) {
			switch (lstOptions.getSelectedIndex()) {
			case 0:
				game=new Game();
				display.setCurrent(game);
				break;
			case 1:
				cHiScore.ShowHiScore();
				display.setCurrent(cHiScore);
				break;
			case 2:
				if (fOptions==null) {
					fOptions=new Form("Options");
					cgSpeed=new ChoiceGroup("Select speed", ChoiceGroup.EXCLUSIVE, new String[] { "Slow", "Normal", "Fast" }, null);
					cgSpeed.setSelectedIndex(optSpeed, true);
					cgSound=new ChoiceGroup("Play sounds?", ChoiceGroup.EXCLUSIVE, new String[] { "No", "Yes" }, null);
					cgSound.setSelectedIndex(optSound, true);
					cgTail=new ChoiceGroup("Size of tail", ChoiceGroup.EXCLUSIVE, new String[] { "Short", "Normal", "Long" }, null);
					cgTail.setSelectedIndex(optTail, true);

					fOptions.append(cgSpeed);
					fOptions.append(cgSound);
					fOptions.append(cgTail);
					fOptions.addCommand(cmdBack);
					fOptions.setCommandListener(this);
				}
				display.setCurrent(fOptions);
				break;
			case 3:
				if (fInstructions==null) {
					fInstructions=new Form("Instructions");
					fInstructions.append("\nSteer the worm through the ever-tightening cave.\n\n");
					fInstructions.append("Avoid the obstacles!\n\nCollect stars to earn bonus points!\n\n");
					fInstructions.append("Press any key to go up, release to go down.\n\n");
					fInstructions.append("One miss and it is over ...\n\n");
					fInstructions.addCommand(cmdOptions);
					fInstructions.setCommandListener(this);
				}
				display.setCurrent(fInstructions);
				break;
			case 4:
				if (fAbout==null) {
					fAbout=new Form("About");
					fAbout.append("YrthWyrm2 2.1\n\n");
					fAbout.append("Written by:\nBrian Schau\n\n");
					fAbout.append("Please visit the website at:\n\n");
					fAbout.append("http://www.schau.com/midlets/yrthwyrm/\n\n");
					fAbout.addCommand(cmdOptions);
					fAbout.setCommandListener(this);
				}
				display.setCurrent(fAbout);
				break;
			default:
				notifyDestroyed();
				break;
			}
		} else if (c==cmdBack) {
			if (fOptions!=null) {
				optSpeed=cgSpeed.getSelectedIndex();
				optSound=cgSound.getSelectedIndex();
				optTail=cgTail.getSelectedIndex();
			}

			display.setCurrent(lstOptions);
		}
	}

	${@src/Intro.java}
	${@src/HiScore.java}
	${@src/Game.java}
}
