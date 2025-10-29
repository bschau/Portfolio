import java.lang.Thread;

import javax.microedition.lcdui.Alert;
import javax.microedition.lcdui.Command;
import javax.microedition.lcdui.CommandListener;
import javax.microedition.lcdui.Display;
import javax.microedition.lcdui.Displayable;

import javax.microedition.midlet.MIDlet;

import javax.microedition.rms.RecordComparator;
import javax.microedition.rms.RecordEnumeration;
import javax.microedition.rms.RecordStore;
import javax.microedition.rms.RecordStoreNotFoundException;

public class PocketChessboardMIDlet extends MIDlet implements CommandListener {
	private final String DBNAME = "Hakuna Mutata";
	protected MainCanvas main;
	protected Thread thread;
	protected Display display;

	/**
	 * Constructor.
	 */
	public PocketChessboardMIDlet() {}

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
		persist();
	}

	/**
	 * Start app. Open and load preferences and build GUI.
	 */
	public void startApp() {
		display = Display.getDisplay(this);
		Locale.init();

		GUI.init(this);

		try {
			main = new MainCanvas(false);
			main.init(this);

			initPocketChessboard();

			GUI.setDisplayable(main);

			main.forceRepaint();
		} catch (Exception e) {
			GUI.fatal(Locale.get("fatalError"), e.getMessage(), this);
		}
	}

	/**
	 * Load the various databases.
	 */
	private void initPocketChessboard() throws Exception {
		RecordStore rs = null;

		try {
			rs = RecordStore.openRecordStore(DBNAME, false);

			if (rs.getNumRecords() > 0) {
				BoardComparator boardCmp = new BoardComparator();
				RecordEnumeration re = rs.enumerateRecords(null, (RecordComparator)boardCmp, false);
				Board brd = new Board();
				if (brd.unserialize(re.nextRecord())) {
					main.board = brd;
				}

				while (re.hasNextElement()) {
					brd = new Board();
					if (brd.unserialize(re.nextRecord())) {
						main.marks.addElement(brd);
					}
				}
			} else {
				throw new Exception();
			}
		} catch (Exception ex) {
			main.setDefaultBoard();
		} finally {
			if (rs != null) {
				try {
					rs.closeRecordStore();
				} catch (Exception ex) {}
			}
		}
	}

	/**
	 * Persist state.
	 */
	private void persist() {
		RecordStore rs = null;
		boolean valid = false;

		try {
			RecordStore.deleteRecordStore(DBNAME);
		} catch (Exception ex) {}

		try {
			int idx;

			String[] dbs = RecordStore.listRecordStores();
			if (dbs != null) {
				for (idx = 0; idx < dbs.length; idx++) {
					if (dbs[idx].compareTo(DBNAME) == 0) {
						throw new Exception();
					}
				}
			}

			rs = RecordStore.openRecordStore(DBNAME, true);
			int recPos = 1;
			byte[] b = main.board.serialize(recPos);
			if (b != null) {
				rs.addRecord(b, 0, b.length);

				Board brd;
				for (idx = 0; idx < main.marks.size(); idx++) {
					recPos++;
					brd = (Board)main.marks.elementAt(idx);
					b = brd.serialize(recPos);
					if (b != null) {
						rs.addRecord(b, 0, b.length);
					}
				}

				valid = true;
			}
		} catch (Exception ex) {
		} finally {
			if (rs != null) {
				try {
					rs.closeRecordStore();
				} catch (Exception ex) {}
			}

			if (!valid) {
				try {
					RecordStore.deleteRecordStore(DBNAME);
				} catch (Exception ex) {}
			}
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
			if (GUI.isFatal()) {
				exit();
			} else {
				GUI.setDisplayable(main);
			}
		} else if (c == GUI.cmdBack) {
			GUI.setDisplayable(main);
		}
	}
}
