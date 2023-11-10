import java.util.Vector;

import javax.microedition.lcdui.Alert;
import javax.microedition.lcdui.AlertType;
import javax.microedition.lcdui.Command;
import javax.microedition.lcdui.CommandListener;
import javax.microedition.lcdui.Choice;
import javax.microedition.lcdui.ChoiceGroup;
import javax.microedition.lcdui.Displayable;
import javax.microedition.lcdui.Form;
import javax.microedition.lcdui.Graphics;
import javax.microedition.lcdui.Image;
import javax.microedition.lcdui.ImageItem;
import javax.microedition.lcdui.Item;
import javax.microedition.lcdui.List;
import javax.microedition.lcdui.Spacer;
import javax.microedition.lcdui.StringItem;
import javax.microedition.lcdui.TextBox;
import javax.microedition.lcdui.TextField;

public class GUI {
	private static final int DEFLAYOUT = Item.LAYOUT_LEFT | Item.LAYOUT_NEWLINE_AFTER;
	public static final int ANCHOR = Graphics.LEFT | Graphics.TOP;
	private static PocketChessboardMIDlet mid;
	private static boolean fatalError;
	public static Form fAbout = null, fFinalEdit = null;
	public static List lstMainOpts = null, lstMarks = null;
	public static TextBox tbAddMark = null;
	public static TextField feTurn = null;
	public static ChoiceGroup cgInTurn = null;
	public static Command cmdPositive, cmdNegative;
	public static Command cmdBack, cmdOptions, cmdSave, cmdDone;
	public static int confState;

	/**
	 * Initialize GUI.
	 *
	 * @param m PocketChessboard MIDlet.
	 * @exception Exception.
	 */
	public static void init(PocketChessboardMIDlet m) {
		mid = m;
		cmdBack = new Command(Locale.get("cmdBack"), Command.BACK, 0);
		cmdOptions = new Command(Locale.get("cmdOptions"), Command.BACK, 0);
		cmdSave = new Command(Locale.get("cmdSave"), Command.OK, 0);
		cmdDone = new Command(Locale.get("cmdDone"), Command.OK, 0);
	}

	/**
	 * Set the displayable as the current view.
	 *
	 * @param d Displayable (Form, List, ...)
	 */
	public static void setDisplayable(Displayable d) {
		mid.display.setCurrent(d);
	}

	/**
	 * Give focus to item.
	 *
	 * @param i Item (StringItem, TextField, ...)
	 */
	public static void focus(Item i) {
		mid.display.setCurrentItem(i);
	}

	/**
	 * Popup an alert of type ERROR with the given title and message.
	 * This is a fatal alert - program will halt execution afterwards.
	 *
	 * @param title Title of popup.
	 * @param msg Message.
	 */
	public static void fatal(String title, String msg, CommandListener l) {
		popup(title, msg, AlertType.ERROR, true, l);
	}

	/**
	 * Popup an alert of type ERROR with the given title and message.
	 *
	 * @param title Title of popup.
	 * @param msg Message.
	 */
	public static void popup(String title, String msg, CommandListener l) {
		popup(title, msg, AlertType.ERROR, false, l);
	}

	/**
	 * Popup an alert of the given type and with the given title and
	 * message.
	 *
	 * @param title Title.
	 * @param msg Message.
	 * @param t AlertType.
	 * @param f Fatal error?
	 * @param l CommandListener.
	 */
	private static void popup(String title, String msg, AlertType t, boolean f, CommandListener l) {
		fatalError = f;
		Alert a = new Alert(title, msg, null, t);
		a.setTimeout(Alert.FOREVER);
		a.setCommandListener(l);
		setDisplayable(a);
	}
	
	/**
	 * Return fatalError state.
	 *
	 * @return state.
	 */
	public static boolean isFatal() {
		return fatalError;
	}

	/**
	 * Show an confirmation dialog with two buttons.
	 *
	 * @param t Title.
	 * @param msg Message.
	 * @param pos Positive reply text.
	 * @param neg Negative reply text.
	 * @param s Current state.
	 * @param l CommandListener.
	 */
	public static void confirmation(String t, String msg, String pos, String neg, int s, CommandListener l) {
		Alert a = new Alert(t, msg, null, AlertType.CONFIRMATION);

		a.setTimeout(Alert.FOREVER);
		a.setCommandListener(l);

		cmdPositive = new Command(pos, Command.OK, 0);
		cmdNegative = new Command(neg, Command.EXIT, 0);
		a.addCommand(cmdPositive);
		a.addCommand(cmdNegative);
		confState = s;
		setDisplayable(a);
	}

	/**
	 * Show About form.
	 *
	 * @param l CommandListener (of form receiving input).
	 */
	public static void showAboutForm(CommandListener l) {
		if (fAbout == null) {
			fAbout = new Form(Locale.get("gAboutTitle"));
			try {
				Image logo = Image.createImage("/logo.png");
				fAbout.append(new ImageItem(null, logo, Item.LAYOUT_CENTER, null));
				fAbout.append(new Spacer(fAbout.getWidth(), 8));
			} catch (Exception ex) {}

			StringItem s = new StringItem(Locale.get("gAboutVersion"), Version.VERSION);
			fAbout.append(s);
			s.setLayout(Item.LAYOUT_CENTER);

			s = new StringItem(Locale.get("gAboutURL") + ":", "http://www.schau.com/");
			fAbout.append(s);
			s.setLayout(Item.LAYOUT_CENTER);
			fAbout.addCommand(cmdBack);
			fAbout.setCommandListener(l);
		}

		setDisplayable(fAbout);
	}

	/**
	 * Show the main Options list.
	 *
	 * @param l CommandListener (of form receiving input).
	 */
	public static void showMainOptionsList(CommandListener l) {
		if (lstMainOpts == null) {
			lstMainOpts = new List("", List.IMPLICIT);
			lstMainOpts.append(Locale.get("cmdBack"), null);
			lstMainOpts.append(Locale.get("mrkMarks"), null);
			lstMainOpts.append(Locale.get("molEditBoard"), null);
			lstMainOpts.append(Locale.get("molResetBoard"), null);
			lstMainOpts.append(Locale.get("molAbout"), null);
			lstMainOpts.append(Locale.get("molExit"), null);
			lstMainOpts.setCommandListener(l);
		}

		setDisplayable(lstMainOpts);
	}

	/**
	 * Show the Add Mark textbox.
	 *
	 * @param l CommandListener (of form receiving input).
	 * @param t Turn.
	 */
	public static void showAddMarkTextBox(CommandListener l, int t) {
		if (tbAddMark == null) {
			tbAddMark = new TextBox(Locale.get("mrkAddMark"), "", 200, TextField.ANY);
			tbAddMark.addCommand(cmdSave);
			tbAddMark.addCommand(cmdBack);
			tbAddMark.setCommandListener(l);
		}

		tbAddMark.setString(Locale.get("mcTurn") + " " + Integer.toString(t));

		setDisplayable(tbAddMark);
	}

	/**
	 * Show the Marks list.
	 *
	 * @param m Vector with marks.
	 * @param l CommandListener (of form receiving input).
	 */
	public static void showMarksList(Vector m, CommandListener l) {
		if (lstMarks == null) {
			lstMarks = new List("", List.IMPLICIT);
		}

		lstMarks.deleteAll();
		lstMarks.append(Locale.get("cmdBack"), null);
		lstMarks.append(Locale.get("mrkAddMark"), null);

		Board b;
		for (int i = 0; i < m.size(); i++) {
			b = (Board)m.elementAt(i);
			lstMarks.append(b.getMark(), null);
		}

		lstMarks.setCommandListener(l);
		setDisplayable(lstMarks);
	}

	/**
	 * Show the "Finalize Edit" form.
	 *
	 * @param p Ply.
	 * @param l CommandListener (of form receiving input).
	 */
	public static void showFinalEditForm(int p, CommandListener l) {
		if (fFinalEdit == null) {
			fFinalEdit = new Form(Locale.get("feTitle"));

			feTurn = new TextField(Locale.get("feTurn"), null, 3, TextField.NUMERIC);
			feTurn.setLayout(Item.LAYOUT_LEFT);
			cgInTurn = new ChoiceGroup("", ChoiceGroup.EXCLUSIVE);
			cgInTurn.append(Locale.get("feWhite"), null);
			cgInTurn.append(Locale.get("feBlack"), null);
			cgInTurn.setLayout(Item.LAYOUT_LEFT);

			fFinalEdit.append(feTurn);
			fFinalEdit.append(cgInTurn);
			fFinalEdit.addCommand(cmdDone);
			fFinalEdit.setCommandListener(l);
		}

		feTurn.setString(Integer.toString((p / 2) + 1));
		cgInTurn.setSelectedIndex(p % 2, true);

		setDisplayable(fFinalEdit);
		focus(feTurn);
	}
}
