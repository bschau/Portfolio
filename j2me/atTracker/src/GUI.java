import java.util.Enumeration;

import javax.microedition.io.file.FileSystemRegistry;

import javax.microedition.lcdui.Alert;
import javax.microedition.lcdui.AlertType;
import javax.microedition.lcdui.Choice;
import javax.microedition.lcdui.ChoiceGroup;
import javax.microedition.lcdui.Command;
import javax.microedition.lcdui.CommandListener;
import javax.microedition.lcdui.DateField;
import javax.microedition.lcdui.Display;
import javax.microedition.lcdui.Displayable;
import javax.microedition.lcdui.Form;
import javax.microedition.lcdui.Graphics;
import javax.microedition.lcdui.Image;
import javax.microedition.lcdui.ImageItem;
import javax.microedition.lcdui.Item;
import javax.microedition.lcdui.ItemStateListener;
import javax.microedition.lcdui.List;
import javax.microedition.lcdui.Spacer;
import javax.microedition.lcdui.StringItem;
import javax.microedition.lcdui.TextField;

public class GUI {
	private static final int DEFLAYOUT= Item.LAYOUT_LEFT | Item.LAYOUT_NEWLINE_AFTER;
	public static final int ANCHOR = Graphics.LEFT | Graphics.TOP;
	public static final int TASKLENGTH = 50;
	public static final int ARROWHEIGHT = 6;
	public static final int ARROWWIDTH = 11;
	private static atTrackerMIDlet mid;
	private static boolean fatalError;
	public static Form fAbout = null, fEdit = null, fTasksList = null;
	public static Form fSummary = null, fBackup = null, fRestore = null;
	public static List lstEditOptions = null, lstMainOptions = null;
	public static List lstTools = null;
	public static Command cmdBack, cmdCancel, cmdOptions, cmdSave;
	public static Command cmdNew, cmdDelete, cmdSummarize, cmdBackup;
	public static Command cmdRestore, cmdPositive, cmdNegative;
	public static ChoiceGroup cgTasks, cgTasksList, cgBRoots, cgRRoots;
	public static TextField tfMon, tfTue, tfWed, tfThu, tfFri, tfSat;
	public static TextField tfSun, tfTask, tfKeywords;
	public static DateField dfStart, dfEnd;
	public static int confState;
	public static Image arrows;

	/**
	 * Initialize GUI.
	 *
	 * @param m atTracker MIDlet.
	 * @exception Exception.
	 */
	public static void init(atTrackerMIDlet m) throws Exception {
		mid = m;
		cmdBack = new Command(Trl.cmdBack, Command.BACK, 0);
		cmdCancel = new Command(Trl.cmdCancel, Command.BACK, 0);
		cmdOptions = new Command(Trl.cmdOptions, Command.BACK, 0);
		cmdSave = new Command(Trl.cmdSave, Command.OK, 0);
		cmdNew = new Command(Trl.cmdNew, Command.OK, 0);
		cmdDelete = new Command(Trl.cmdDelete, Command.OK, 0);
		cmdSummarize = new Command(Trl.cmdSummarize, Command.OK, 0);
		cmdBackup = new Command(Trl.cmdBackup, Command.OK, 0);
		cmdRestore = new Command(Trl.cmdRestore, Command.OK, 0);

		arrows = Image.createImage("/arrows.png");
	}

	/**
	 * Set the displayable as the current view.
	 *
	 * @param d Displayable (Form, List, ...)
	 */
	public static void setDisplayable(Displayable d) {
		Display.getDisplay(mid).setCurrent(d);
	}

	/**
	 * Give focus to item.
	 *
	 * @param i Item (StringItem, TextField, ...)
	 */
	public static void focus(Item i) {
		Display.getDisplay(mid).setCurrentItem(i);
	}

	/**
	 * Popup an alert of type ERROR with the given title and message.
	 * This is a fatal alert - program will halt execution afterwards.
	 *
	 * @param title Title of popup.
	 * @param msg Message.
	 */
	public static void fatal(String title, String msg) {
		popup(title, msg, AlertType.ERROR, true);
	}

	/**
	 * Popup an alert of type ERROR with the given title and message.
	 *
	 * @param title Title of popup.
	 * @param msg Message.
	 */
	public static void popup(String title, String msg) {
		popup(title, msg, AlertType.ERROR, false);
	}

	/**
	 * Popup an alert of type INFO with the given title and message.
	 *
	 * @param title Title of popup.
	 * @param msg Message.
	 */
	public static void info(String title, String msg) {
		popup(title, msg, AlertType.INFO, false);
	}


	/**
	 * Popup an alert of the given type and with the given title and
	 * message.
	 *
	 * @param title Title.
	 * @param msg Message.
	 * @param t AlertType.
	 * @param f Fatal error?
	 */
	public static void popup(String title, String msg, AlertType t, boolean f) {
		fatalError = f;
		Alert a = new Alert(title, msg, null, t);
		a.setTimeout(Alert.FOREVER);
		a.setCommandListener(mid);
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
	 */
	public static void confirmation(String t, String msg, String pos, String neg, int s) {
		Alert a = new Alert(t, msg, null, AlertType.CONFIRMATION);

		a.setTimeout(Alert.FOREVER);
		a.setCommandListener(mid);

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
			fAbout = new Form(Trl.gAboutTitle);
			try {
				Image logo = Image.createImage("/logo.png");
				fAbout.append(new ImageItem(null, logo, Item.LAYOUT_CENTER, null));
				fAbout.append(new Spacer(fAbout.getWidth(), 8));
			} catch (Exception ex) {}

			StringItem s = new StringItem(Trl.gAboutVersion, Version.VERSION);
			fAbout.append(s);
			s.setLayout(Item.LAYOUT_CENTER);

			s = new StringItem(Trl.gAboutURL+":", "http://www.schau.com/");
			fAbout.append(s);
			s.setLayout(Item.LAYOUT_CENTER);
			fAbout.addCommand(cmdBack);
			fAbout.setCommandListener(l);
		}

		setDisplayable(fAbout);
	}

	/**
	 * Build the Edit form.
	 */
	public static void buildEditForm() {
		if (fEdit == null) {
			fEdit = new Form("");
			cgTasks = new ChoiceGroup(Trl.gEditCG, Choice.POPUP);
			cgTasks.setLayout(DEFLAYOUT);

			tfTask = new TextField(Trl.gEditTask, null, TASKLENGTH, TextField.ANY);
			tfTask.setLayout(DEFLAYOUT);

			tfMon = new TextField(Trl.monl+":", null, 4, TextField.NUMERIC);
			tfMon.setLayout(DEFLAYOUT);
			tfTue = new TextField(Trl.tuel+":", null, 4, TextField.NUMERIC);
			tfTue.setLayout(DEFLAYOUT);
			tfWed = new TextField(Trl.wedl+":", null, 4, TextField.NUMERIC);
			tfWed.setLayout(DEFLAYOUT);
			tfThu = new TextField(Trl.thul+":", null, 4, TextField.NUMERIC);
			tfThu.setLayout(DEFLAYOUT);
			tfFri = new TextField(Trl.fril+":", null, 4, TextField.NUMERIC);
			tfFri.setLayout(DEFLAYOUT);
			tfSat = new TextField(Trl.satl+":", null, 4, TextField.NUMERIC);
			tfSat.setLayout(DEFLAYOUT);
			tfSun = new TextField(Trl.sunl+":", null, 4, TextField.NUMERIC);
			tfSun.setLayout(DEFLAYOUT);

			fEdit.append(cgTasks);
			fEdit.append(tfTask);
			fEdit.append(tfMon);
			fEdit.append(tfTue);
			fEdit.append(tfWed);
			fEdit.append(tfThu);
			fEdit.append(tfFri);
			fEdit.append(tfSat);
			fEdit.append(tfSun);
			fEdit.addCommand(cmdSave);
			fEdit.addCommand(cmdOptions);
			fEdit.setCommandListener(mid);
			fEdit.setItemStateListener(new ItemStateListener() {
				public void itemStateChanged(Item item) {
					if (item instanceof ChoiceGroup) {
						int idx = cgTasks.getSelectedIndex();
						if (idx >- 1) {
							tfTask.setString(cgTasks.getString(idx));
						}
					}
				}
			});
		}
	}

	/**
	 * Show the Edit Options list.
	 */
	public static void showEditOptionsList() {
		if (lstEditOptions == null) {
			lstEditOptions = new List("", List.IMPLICIT);
			lstEditOptions.append(Trl.cmdCancel, null);
			lstEditOptions.append(Trl.cmdDelete, null);
			lstEditOptions.setCommandListener(mid);
		}

		setDisplayable(lstEditOptions);
	}

	/**
	 * Build the Tasks List form.
	 */
	public static void buildTasksListForm() {
		if (fTasksList == null) {
			fTasksList = new Form(Trl.gTasksListTitle);
			cgTasksList = new ChoiceGroup(null, ChoiceGroup.MULTIPLE);
			cgTasksList.setLayout(DEFLAYOUT);
			fTasksList.append(cgTasksList);
			fTasksList.addCommand(cmdDelete);
			fTasksList.addCommand(cmdBack);
			fTasksList.setCommandListener(mid);
		}
	}

	/**
	 * Show the Summary form.
	 */
	public static void showSummaryForm() {
		if (fSummary == null) {
			fSummary = new Form(Trl.gSummaryTitle);
			tfKeywords = new TextField(Trl.gSummaryKeywords, null, TASKLENGTH, TextField.ANY);
			tfKeywords.setLayout(DEFLAYOUT);
			dfStart = new DateField(Trl.gSummaryStart+":", DateField.DATE);
			dfStart.setLayout(DEFLAYOUT);
			dfEnd = new DateField(Trl.gSummaryEnd+":", DateField.DATE);
			dfEnd.setLayout(DEFLAYOUT);
			fSummary.append(tfKeywords);
			fSummary.append(dfStart);
			fSummary.append(dfEnd);
			fSummary.addCommand(cmdBack);
			fSummary.addCommand(cmdSummarize);
			fSummary.setCommandListener(mid);
		}

		focus(tfKeywords);
		setDisplayable(fSummary);
	}

	/**
	 * Show the Main Options list.
	 *
	 * @param l CommandListener (of form receiving input).
	 */
	public static void showMainOptionsList(CommandListener l) {
		if (lstMainOptions == null) {
			lstMainOptions = new List("", List.IMPLICIT);
			lstMainOptions.append(Trl.cmdBack, null);
			lstMainOptions.append(Trl.gMainOptsCopy, null);
			lstMainOptions.append(Trl.gMainOptsClear, null);
			lstMainOptions.append(Trl.gMainOptsTools, null);
			lstMainOptions.append(Trl.gMainOptsAbout, null);
			lstMainOptions.append(Trl.gMainOptsExit, null);
			lstMainOptions.setCommandListener(l);
		}

		setDisplayable(lstMainOptions);
	}

	/**
	 * Show the Tools List.
	 *
	 * @param l CommandListener (of form receiving input).
	 */
	public static void showToolsList(CommandListener l) {
		if (lstTools == null) {
			lstTools = new List("", List.IMPLICIT);
			lstTools.append(Trl.cmdBack, null);
			lstTools.append(Trl.gToolsTasks, null);
			lstTools.append(Trl.gToolsSummary, null);
			lstTools.append(Trl.gToolsBackup, null);
			lstTools.append(Trl.gToolsRestore, null);
			lstTools.setCommandListener(l);
		}

		setDisplayable(lstTools);
	}

	/**
	 * Show the Backup form.
	 *
	 * @param l CommandListener (of form receiving input).
	 */
	public static void showBackupForm(CommandListener l) {
		if (fBackup == null) {
			try {
				Enumeration e = FileSystemRegistry.listRoots();

				fBackup = new Form(Trl.gBackupTitle);

				cgBRoots = new ChoiceGroup(Trl.gBackupTo+":", Choice.EXCLUSIVE);
	
				int cnt = 0;
				String s;
				while (e.hasMoreElements()) {
					s = (String)e.nextElement();
					if (!s.endsWith(":/")) {
						cgBRoots.append(s, null);
						cnt++;
					}
				}

				if (cnt == 0) {
					throw new Exception();
				}

				cgBRoots.setLayout(DEFLAYOUT);
				fBackup.append(cgBRoots);

				fBackup.addCommand(cmdBackup);
				fBackup.addCommand(cmdCancel);
				fBackup.setCommandListener(l);
			} catch (Exception ex) {
				fBackup = null;
				popup(Trl.noBackupTitle, Trl.noBackupBody);
				return;
			}
		}

		setDisplayable(fBackup);
	}

	/**
	 * Build the Restore form.
	 *
	 * @param l CommandListener (of form receiving input).
	 */
	public static void buildRestoreForm(CommandListener l) {
		if (fRestore == null) {
			fRestore = new Form(Trl.gRestoreTitle);

			cgRRoots = new ChoiceGroup(Trl.gRestoreFrom, Choice.EXCLUSIVE);
			cgRRoots.setLayout(DEFLAYOUT);
			fRestore.append(cgRRoots);

			fRestore.addCommand(cmdRestore);
			fRestore.addCommand(cmdCancel);
			fRestore.setCommandListener(l);
		}
	}
}
