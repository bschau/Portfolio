import java.util.Random;

import javax.microedition.lcdui.Canvas;
import javax.microedition.lcdui.Command;
import javax.microedition.lcdui.CommandListener;
import javax.microedition.lcdui.Displayable;
import javax.microedition.lcdui.Graphics;
import javax.microedition.lcdui.Image;

import javax.microedition.lcdui.game.GameCanvas;

public class MainCanvas extends GameCanvas implements CommandListener, Runnable {
	private final String MLine1 = "Resume";
	private final String MLine2 = "Exit Game";
	private final int SCanvasInit = 0;
	private final int SFadeFromWhite = 1;
	private final int SFadeToWhite = 2;
	private final int SPrepareIntro = 3;
	private final int SRunIntro = 4;
	private final int SPrepareGame = 5;
	private final int SLoadedGame = 6;
	private final int SRunGame = 7;
	private final int SPrepareGameOver = 8;
	private final int SRunGameOver = 9;
	private final int ThreadDelayFade = 10;
	private final int ThreadDelayGame = 75;
	private final int ThreadDelayGameOver = 200;
	public boolean runThread = true;
	public int cW, cH;
	public boolean keyUp, keyDown, keyLeft, keyRight, keyFire;

	private TetrisMIDlet mid;
	private Image canvas;
	private Graphics g;
	private int state, nextState;

	/**
	 * Constructor.
	 */
	protected MainCanvas(boolean suppress) {
		super(false);
	}

	/**
	 * Initialize the Tetris main canvas.
	 *
	 * @param m TetrisMIDlet
	 * @exception Exception.
	 */
	public void init(TetrisMIDlet m) throws Exception {
		mid = m;

		setFullScreenMode(true);

		cW = 1;
		cH = 1;

		state = SCanvasInit;
		nextState = SPrepareIntro;

		clearKeyStates();

		GUI.init(this);
	}

	/**
	 * Command Action.
	 */
	public void commandAction(Command c, Displayable d) {
	}

	/**
	 * Keypress handler.
	 *
	 * @param keyCode Key code.
	 */
	protected void keyPressed(int keyCode) {
		int ga = getGameAction(keyCode);

		if ((ga == Canvas.LEFT) ||
		    (keyCode == Canvas.KEY_NUM4)) {
			keyLeft = true;
		} else if ((ga == Canvas.RIGHT) ||
			   (keyCode == Canvas.KEY_NUM6)) {
			keyRight = true;
		} else if ((ga == Canvas.UP) ||
			   (keyCode == Canvas.KEY_NUM2)) {
			keyUp = true;
		} else if ((ga == Canvas.DOWN) ||
			   (keyCode == Canvas.KEY_NUM8)) {
			keyDown = true;
		} else if ((ga == Canvas.FIRE) ||
			   (keyCode == Canvas.KEY_NUM5)) {
			keyFire = true;
		}
	}

	/**
	 * Clear the key states.
	 */
	public void clearKeyStates() {
		keyUp = false;
		keyDown = false;
		keyLeft = false;
		keyRight = false;
		keyFire = false;
	}

	/**
	 * Main handler thread.
	 */
	public void run() {
		Graphics scr = getGraphics();
		long threadDelay = 100, sT, lT;

		while (runThread) {
			sT = System.currentTimeMillis();

			switch (state) {
				case SCanvasInit:
					flushGraphics();

					cW = getWidth();
					cH = getHeight();

					try {
						canvas = Image.createImage(cW, cH);
						g = canvas.getGraphics();

						Transition.init(cW, cH);
						Intro.init(this);
						Game.init(this);
						GameOver.init(this);
					} catch (Exception ex) {
						mid.exit();
						return;
					}

					GUI.clearScreen(scr);

					state = nextState;
					break;

				case SFadeFromWhite:
					Transition.fadeFromWhite();
					if (Transition.step(scr)) {
						state = nextState;
					}
					break;

				case SFadeToWhite:
					Transition.fadeToWhite();
					if (Transition.step(scr)) {
						state = nextState;
					}
					break;

				case SPrepareIntro:
					Intro.prepare(g, canvas);
					nextState = SRunIntro;
					state = SFadeFromWhite;
					threadDelay = ThreadDelayFade;
					break;

				case SRunIntro:
					if (keyFire) {
						Transition.setupFadeToWhite(canvas);
						nextState = SPrepareGame;
						state = SFadeToWhite;
						threadDelay = ThreadDelayFade;
					}
					clearKeyStates();
					break;

				case SPrepareGame:
					Game.setup();
					Game.paintDisplay(g);
					Transition.setupFadeFromWhite(canvas);

					nextState = SRunGame;
					state = SFadeFromWhite;
					break;

				case SLoadedGame:
					Game.paintDisplay(scr);
					state = SRunGame;
					break;

				case SRunGame:
					threadDelay = ThreadDelayGame;
					Game.execute(scr);

					if ((Game.abortGame) || (Game.gameOver)) {
						Game.canPersist = false;
						Game.finalDisplay(g);
						Transition.setupFadeToWhite(canvas);
						nextState = (Game.abortGame ? SPrepareIntro : SPrepareGameOver);
						state = SFadeToWhite;
						threadDelay = ThreadDelayFade;
					}
					break;

				case SPrepareGameOver:
					GameOver.prepare(g, canvas, Game.getScore());
					nextState = SRunGameOver;
					state = SFadeFromWhite;
					clearKeyStates();
					break;

				case SRunGameOver:
					threadDelay = ThreadDelayGameOver;
					if (GameOver.execute(scr)) {
						GameOver.draw(g);
						Transition.setupFadeToWhite(canvas);
						nextState = SPrepareIntro;
						state = SFadeToWhite;
						threadDelay = ThreadDelayFade;
					}
					clearKeyStates();
					break;
			}

			flushGraphics();

			lT = System.currentTimeMillis() - sT;
			if (lT < threadDelay) {
				try {
					Thread.sleep(threadDelay - lT);
				} catch (Exception ex) {}
			}
		}
	}

	/**
	 * Indicate that a game was loaded.
	 */
	public void setLoadedGame() {
		nextState = SLoadedGame;
	}
}
