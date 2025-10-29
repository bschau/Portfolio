/*
vim: ts=2 sw=2 et
*/
import javax.microedition.lcdui.*;
import javax.microedition.midlet.*;

public class Juggle extends MIDlet implements CommandListener {
	protected boolean started=false;
	private Display display;
	private Command cmdOptions, cmdBack, cmdBummer;
	private Canvas cIntro, cHiScore, cEndGame;
	private GameEngine eGame;
	private List lstOptions;
	private Form fInstructions, fDifficulty, fSorry, fAbout;
	private ChoiceGroup cgDiffChoices;
	public int scoreA, scoreB, difficulty;

	protected void startApp() {
		if (!started) {
			started=true;
			display=Display.getDisplay(this);
		  Init();
		}
	}

	public void Init() {
		Preferences p=new Preferences();
		p.GetPreferences();
		p.Close();

		scoreA=p.GetScoreA();
		scoreB=p.GetScoreB();
		difficulty=p.GetDifficulty();

		cmdOptions=new Command("Options", Command.OK, 0);
		cmdBack=new Command("Back", Command.BACK, 0);

		lstOptions=new List("", List.IMPLICIT);
		lstOptions.append("Play", null);
		lstOptions.append("High Scores", null);
		lstOptions.append("Difficulty", null);
		lstOptions.append("Instructions", null);
		lstOptions.append("About", null);
		lstOptions.append("Exit", null);
		
		lstOptions.setCommandListener(this);

		cIntro=new IntroCanvas();
		cIntro.addCommand(cmdOptions);
		cIntro.setCommandListener(this);

		cHiScore=new HiScoreCanvas();
		cHiScore.addCommand(cmdOptions);
		cHiScore.setCommandListener(this);
		((HiScoreCanvas)cHiScore).SetParent(this);

		cEndGame=new EndGameCanvas();
		cEndGame.addCommand(cmdOptions);
		cEndGame.setCommandListener(this);
		((EndGameCanvas)cEndGame).SetDisplay(display);
		((EndGameCanvas)cEndGame).SetParent(this);

		eGame=new GameEngine();
		
		fInstructions=new Form("Instructions");
		fInstructions.append("Keep all the balls in air!\r\n");
		fInstructions.append("Use left side keys to make arms go left.\r\n");
		fInstructions.append("Use right side keys to make arms go right.\r\n\r\n");
		fInstructions.append("Drop a ball and it is over ...\r\n");
		fInstructions.addCommand(cmdOptions);
		fInstructions.setCommandListener(this);

		fDifficulty=new Form("Difficulty");
		cgDiffChoices=new ChoiceGroup("Game is ...", ChoiceGroup.EXCLUSIVE, new String[] { "Easy", "Hard" }, null);
		cgDiffChoices.setSelectedIndex(difficulty, true);
		fDifficulty.append(cgDiffChoices);
		fDifficulty.addCommand(cmdBack);
		fDifficulty.setCommandListener(this);

		fAbout=new Form("About");
		fAbout.append("Juggle 2.0\r\n\r\n");
		fAbout.append("Written by:\r\n  Brian Schau\r\n\r\nOriginal concept:\r\n  Nintendo\r\n\r\n");
		fAbout.append("Please visit my website at:\r\n\r\n");
		fAbout.append("http://www.schau.com/\r\n\r\n");
		fAbout.addCommand(cmdOptions);
		fAbout.setCommandListener(this);

		display.setCurrent(cIntro);
	}

	protected void pauseApp() {
		destroyApp(true);
	}

	protected void destroyApp(boolean unconditional) {
		if (eGame!=null) {
			eGame.Destroy();

			UpdatePreferences();
		}
	}

	protected void UpdatePreferences() {
		Preferences p=new Preferences();
		p.UpdatePreferences(scoreA, scoreB, difficulty);
		p.Close();
	}
	
	public void commandAction(Command c, Displayable d) {
		if (c==cmdOptions) {
			display.setCurrent(lstOptions);
		} else if (c==cmdBack) {
			difficulty=cgDiffChoices.getSelectedIndex();
			display.setCurrent(lstOptions);
		} else if (d==lstOptions) {
			int idx=lstOptions.getSelectedIndex();

			switch (lstOptions.getSelectedIndex()) {
			case 0:
				StartGame();
				break;
			case 1:
				display.setCurrent(cHiScore);
				break;
			case 2:
				display.setCurrent(fDifficulty);
				break;
			case 3:
				display.setCurrent(fInstructions);
				break;
			case 4:
				display.setCurrent(fAbout);
				break;
			default:
				notifyDestroyed();
				break;
			}
		} else if (c==cmdBummer) {
			notifyDestroyed();
		}
	}

	public void StartGame() {
		eGame.Init(difficulty, cEndGame);
		display.setCurrent(eGame);
	}

	public void StopGame() {
		display.setCurrent(lstOptions);
	}
}
