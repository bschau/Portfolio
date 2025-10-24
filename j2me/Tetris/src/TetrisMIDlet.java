import java.lang.Thread;

import javax.microedition.lcdui.Alert;
import javax.microedition.lcdui.AlertType;
import javax.microedition.lcdui.Command;
import javax.microedition.lcdui.CommandListener;
import javax.microedition.lcdui.Display;
import javax.microedition.lcdui.Displayable;

import javax.microedition.midlet.MIDlet;

import javax.microedition.rms.RecordEnumeration;
import javax.microedition.rms.RecordStore;
import javax.microedition.rms.RecordStoreNotFoundException;

public class TetrisMIDlet extends MIDlet implements CommandListener {
	private final String DBNAME = "HiScores";
	protected MainCanvas main;
	protected Thread thread;

	/**
	 * Constructor.
	 */
	public TetrisMIDlet() {}

	/**
	 * Pause app.
	 */
	public void pauseApp() {}

	/**
	 * Exit application.
	 */
	public void exit() {
		destroyApp(false);
		notifyDestroyed();
	}

	/**
	 * Destroy app.
	 *
	 * @param unconditional unused.
	 */
	public void destroyApp(boolean unconditional) {
		main.runThread = false;
		try {
			thread.join();
		} catch (Exception ex) {}

		thread = null;
		Game.persist();
		Leaderboard.persist();
	}

	/**
	 * Start app. Open and load preferences and build GUI.
	 */
	public void startApp() {
		try {
			FontEngine.init();
			Leaderboard.init();

			main = new MainCanvas(true);
			main.init(this);
			if (Game.load()) {
				main.setLoadedGame();
			}
			thread = new Thread(main);
			thread.start();

			Display.getDisplay(this).setCurrent(main);
		} catch (Exception e) {
			Alert a = new Alert("Failed to start Tetris", e.getMessage(), null, AlertType.ERROR);
			a.setTimeout(Alert.FOREVER);
			a.setCommandListener(this);
			Display.getDisplay(this).setCurrent(a);
		}
	}

	/**
	 * Handle command.
	 *
	 * @param c Command.
	 * @param d Current displayable.
	 */
	public void commandAction(Command c, Displayable d) {
		if (c == Alert.DISMISS_COMMAND) {
			exit();
		}
	}

	/**
	 * Return thread object.
	 *
	 * @return thread object.
	 */
	public Thread getThread() {
		return thread;
	}
}
