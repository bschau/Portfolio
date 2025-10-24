import java.lang.Thread;

import javax.microedition.lcdui.Alert;
import javax.microedition.lcdui.Command;
import javax.microedition.lcdui.CommandListener;
import javax.microedition.lcdui.Displayable;
import javax.microedition.lcdui.TextField;

import javax.microedition.midlet.MIDlet;

import javax.microedition.rms.RecordComparator;
import javax.microedition.rms.RecordEnumeration;
import javax.microedition.rms.RecordStore;

public class atTrackerMIDlet extends MIDlet implements CommandListener {
	protected RecordStore dbPeriod = null, dbTasks = null;
	private SummaryResult sumRes = null;
	private int[] taskPtr = new int[1];
	private RecordComparator taskCmp = new TaskComparator();
	private RecordEnumeration taskAll = null;
	private StringBuffer sBuf = new StringBuffer(50);
	private MainCanvas main;
	private int recIdx;
	private int taskCnt;

	/**
	 * Constructor.
	 */
	public atTrackerMIDlet() {}

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
		if (main.backupThread != null) {
			main.backupThread.stop();
		}

		if (main.restoreThread != null) {
			main.restoreThread.stop();
		}

		dbClose(dbPeriod);
		dbClose(dbTasks);
	}

	/**
	 * Start app. Open and load preferences and build GUI.
	 */
	public void startApp() {
		try {
			GUI.init(this);

			dbTasks = dbOpen("Tasks", Trl.dbName1);
			dbPeriod = dbOpen("PeriodData", Trl.dbName2);
			rebuildTasksList();

			main = new MainCanvas();
			main.init(this);

			goToMain();
		} catch (Exception e) {
			GUI.fatal(Trl.errUnrec, e.getMessage());
		}
	}

	/**
	 * Open a record store.
	 *
	 * @param rsName RecordStore name.
	 * @param name Friendly name (used in popup).
	 * @return RecordStore.
	 * @exception Exception.
	 */
	private RecordStore dbOpen(String rsName, String name) throws Exception {
		String[] dbList = RecordStore.listRecordStores();
		RecordStore r = null;
		int cnt = 0, i, x, v;

		if (dbList != null) {
			for (i = 0; i < dbList.length; i++) {
				if (dbList[i].startsWith(rsName)) {
					if ((x = dbList[i].indexOf('_')) == -1) {
						v = 1;
					} else {
						x++;
						v = Integer.parseInt(dbList[i].substring(x));
					}

					if (v > cnt) {
						cnt = v;
					}
				}
			}
		}
	
		try {
			r = RecordStore.openRecordStore(rsName + "_" + cnt, true);
		} catch (Exception e) {
			throw new Exception(Trl.noDB + ": " + name);
		}

		return r;
	}

	/**
	 * Close a recordstore.
	 *
	 * @param rs RecordStore.
	 */
	public void dbClose(RecordStore rs) {
		if (rs != null) {
			try {
				rs.closeRecordStore();
			} catch (Exception e) {}
		}
	}

	/**
	 * Check to see if handset implements JSR75.
	 *
	 * @return true if JSR75 is available, false otherwise (and popup).
	 */
	public boolean hasJSR75() {
		boolean jsr75 = false;

		try {
			Class.forName("javax.microedition.io.file.FileConnection");
			jsr75 = true;
		} catch (Exception ex) {}

		if (jsr75 == false) {
			GUI.popup(Trl.noJSR75title, Trl.noJSR75body);
		}

		return jsr75;
	}

	/**
	 * Rebuild task list.
	 */
	public void rebuildTasksList() throws Exception {
		taskCnt = dbTasks.getNumRecords();
		taskAll = null;

		if (taskCnt > taskPtr.length) {
			taskPtr = null;
			taskPtr = new int[taskCnt];
			System.gc();
		}

		taskAll = dbTasks.enumerateRecords(null, taskCmp, false);
		for (int i = 0; i < taskCnt; i++) {
			taskPtr[i] = taskAll.nextRecordId();
		}
	}

	/**
	 * Show main canvas.
	 */
	public void goToMain() {
		try {
			main.initView();
			GUI.setDisplayable(main);
		} catch (Exception ex) {
			GUI.fatal(Trl.errUnrec, Trl.noMainView + ": " + ex.getMessage());
		}
	}

	/**
	 * Show Tasks List.
	 */
	public void goToTasksList() {
		GUI.buildTasksListForm();

		try {
			GUI.cgTasksList.deleteAll();

			Task t = new Task();
			for (int i = 0; i < taskCnt; i++) {
				t.clear();
				t.load(dbTasks, taskPtr[i]);
				GUI.cgTasksList.append(t.task, null);
			}
		} catch (Exception ex) {
			GUI.fatal(Trl.errUnrec, Trl.noTasksListView + ": " + ex.getMessage());
		}

		GUI.setDisplayable(GUI.fTasksList);
	}

	/**
	 * Show edit form.
	 * 
	 * @param idx Record index (-1 for new record).
	 */
	public void goToEdit(int idx) {
		recIdx = idx;

		GUI.buildEditForm();
		GUI.fEdit.setTitle(main.pp.toString());

		GUI.cgTasks.deleteAll();
		Task t = new Task();
		for (int i = 0; i < taskCnt; i++) {
			try {
				t.clear();
				t.load(dbTasks, taskPtr[i]);
			} catch (Exception ex) {
				t.task = "-- ?? --";
			}
			GUI.cgTasks.append(t.task, null);
		}

		if (recIdx == -1) {
			GUI.tfMon.setString("");
			GUI.tfTue.setString("");
			GUI.tfWed.setString("");
			GUI.tfThu.setString("");
			GUI.tfFri.setString("");
			GUI.tfSat.setString("");
			GUI.tfSun.setString("");
			GUI.tfTask.setString("");
		} else {
			try {
				Record r = new Record();

				r.load(dbPeriod, recIdx);

				setTime(GUI.tfMon, r.mon);
				setTime(GUI.tfTue, r.tue);
				setTime(GUI.tfWed, r.wed);
				setTime(GUI.tfThu, r.thu);
				setTime(GUI.tfFri, r.fri);
				setTime(GUI.tfSat, r.sat);
				setTime(GUI.tfSun, r.sun);
				GUI.tfTask.setString(r.task);
				r = null;
			} catch (Exception ex) {
				GUI.fatal(Trl.noRRecord, ex.getMessage());
				return;
			}
		}
		System.gc();

		GUI.focus(GUI.tfTask);
		GUI.setDisplayable(GUI.fEdit);
	}

	/**
	 * Set a non-zero time value in the entry field.
	 *
	 * @param f TextField.
	 * @param v Value.
	 */
	private void setTime(TextField f, int v) {
		if (v > 0) {
			int t = v / 60;

			sBuf.delete(0, sBuf.length());
			sBuf.append(Integer.toString(t));
			t = v % 60;
			if (t > 0) {
				if (t < 10) {
					sBuf.append('0');
				}

				sBuf.append(Integer.toString(t));
			}

			f.setString(sBuf.toString());
		} else {
			f.setString("");
		}
	}

	/**
	 * Handle command.
	 *
	 * @param c Command.
	 * @param d Current displayable.
	 */
	public void commandAction(Command c, Displayable d) {
		boolean[] picks;

		if (c == Alert.DISMISS_COMMAND) {
			if (GUI.isFatal()) {
				exit();
			} else {
				goToMain();
			}
		} else if (c == GUI.cmdPositive) {
			if (GUI.confState == Const.CStateClearPeriod) {
				main.clearPeriod();
				goToMain();
			} else if (GUI.confState == Const.CStateCopyPeriod) {
				main.copyPeriod();
				goToMain();
			} else if (GUI.confState == Const.CStateRestore) {
				main.restore();
			}
		} else if (c == GUI.cmdNegative) {
			goToMain();
		} else if (c == GUI.cmdBack) {
			goToMain();
		} else if (c == GUI.cmdOptions) {
			GUI.showEditOptionsList();
		} else if (c == GUI.cmdDelete) {
			boolean[] selFlags = new boolean[taskCnt];
			int idx;

			GUI.cgTasksList.getSelectedFlags(selFlags);
			for (idx = 0; idx < taskCnt; idx++) {
				if (selFlags[idx]) {
					try {
						dbTasks.deleteRecord(taskPtr[idx]);
					} catch (Exception ex) {}
				}
			}

			try {
				rebuildTasksList();
				goToMain();
			} catch (Exception ex) {
				GUI.fatal(Trl.noTasksList, ex.getMessage());
			}
		} else if (c == GUI.cmdSave) {
			if (d == GUI.fEdit) {
				Record r = new Record();

				r.period = main.pp.getPeriodStart();
				r.mon = r.parseTime(GUI.tfMon.getString());
				r.tue = r.parseTime(GUI.tfTue.getString());
				r.wed = r.parseTime(GUI.tfWed.getString());
				r.thu = r.parseTime(GUI.tfThu.getString());
				r.fri = r.parseTime(GUI.tfFri.getString());
				r.sat = r.parseTime(GUI.tfSat.getString());
				r.sun = r.parseTime(GUI.tfSun.getString());
				r.task = GUI.tfTask.getString();

				try {
					r.save(dbPeriod, recIdx);
					insertTask(r.task);
					goToMain();
				} catch (Exception ex) {
					GUI.popup(Trl.noWRecord, ex.getMessage());
				}
			}
		} else if (c == GUI.cmdSummarize) {
			try {
				if (sumRes == null) {
					sumRes = new SummaryResult();
					sumRes.init(this);
				}
				sumRes.setConstraints(GUI.tfKeywords.getString(), GUI.dfStart.getDate(), GUI.dfEnd.getDate());
				try {
					sumRes.summarize();
					if (sumRes.getMatchCount() > 0) {
						sumRes.initView();
						GUI.setDisplayable(sumRes);
					} else {
						GUI.popup(Trl.noMatchesTitle, Trl.noMatchesBody);
					}
				} catch (Exception ex) {
					GUI.popup(Trl.noSummarize, ex.getMessage());
				}
			} catch (Exception ex) {
				GUI.popup(Trl.errUnrec, ex.getMessage());
			}
		} else if (d == GUI.lstEditOptions) {
			switch (GUI.lstEditOptions.getSelectedIndex()) {
				case 0:	// Cancel
					goToMain();
					break;
				case 1: // Delete
					if (recIdx > -1) {
						try {
							dbPeriod.deleteRecord(recIdx);
							goToMain();
						} catch (Exception ex) {
							GUI.popup(Trl.noDelete, ex.getMessage());
						}
					}
					goToMain();
					break;
			}
		}
	}

	/**
	 * Insert a task into the database taking care of not adding a task
	 * which is already present. Rebuilds the tasks list.
	 *
	 * @param task Task to add.
	 * @exception Exception.
	 */
	private void insertTask(String task) throws Exception {
		String lowerTask = task.toLowerCase();
		Task t = new Task();

		for (int i = 0; i < taskCnt; i++) {
			t.clear();
			t.load(dbTasks, taskPtr[i]);
			if (t.task.toLowerCase().compareTo(lowerTask) == 0) {
				return;
			}
		}
			
		t.task = task;
		byte[] b = t.pack();
		dbTasks.addRecord(b, 0, b.length);
		rebuildTasksList();
	}
}
