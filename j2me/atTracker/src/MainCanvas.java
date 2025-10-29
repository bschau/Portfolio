import java.util.Enumeration;
import java.util.Vector;

import javax.microedition.io.Connector;
import javax.microedition.io.file.FileConnection;
import javax.microedition.io.file.FileSystemRegistry;

import javax.microedition.lcdui.Canvas;
import javax.microedition.lcdui.Command;
import javax.microedition.lcdui.CommandListener;
import javax.microedition.lcdui.Displayable;
import javax.microedition.lcdui.Font;
import javax.microedition.lcdui.Graphics;
import javax.microedition.lcdui.Image;

import javax.microedition.rms.RecordComparator;
import javax.microedition.rms.RecordEnumeration;
import javax.microedition.rms.RecordFilter;
import javax.microedition.rms.RecordStore;

public class MainCanvas extends Canvas implements CommandListener {
	private final int LEFTRIGHTWIDTH = 6;
	private final int LEFTRIGHTHEIGHT = 5;
	private final int SPACEAFTERLOGO = 4;
	private final int SPACEAFTERPERIOD = 2;
	private final int SPACEAFTERLINE = 2;
	private final int TIMELINEWIDTH = 52;
	public PeriodPicker pp = new PeriodPicker();
	public BackupThread backupThread = null;
	public RestoreThread restoreThread = null;
	private PeriodDataFilter pdFilt = new PeriodDataFilter();
	private PeriodDataComparator pdComp = new PeriodDataComparator();
	private Font defFont = Font.getDefaultFont();
	private int fntH = defFont.getHeight(), barTop = 0;
	private RecordEnumeration reAll = null;
	private StringBuffer sBuf = new StringBuffer();
	private PeriodPicker previousPeriod = new PeriodPicker();
	protected atTrackerMIDlet mid;
	private int lstLines, lstTop, recCnt, cW, cH;
	private Image logo, halfs;
	private int[] recPtr;
	private StringBuffer[] recBuf;
	private int[] recSize;
	private boolean canShowHalf, show2ndHalf;
	private long pTotal;

	/**
	 * Constructor.
	 */
	public MainCanvas() {}

	/**
	 * Initialize the atTracker main canvas.
	 *
	 * @param m atTrackerMIDlet
	 * @exception Exception.
	 */
	public void init(atTrackerMIDlet m) throws Exception {
		mid = m;

		logo = Image.createImage("/logo.png");
		halfs = Image.createImage("/halfs.png");

		this.addCommand(GUI.cmdOptions);
		this.addCommand(GUI.cmdNew);

		this.setCommandListener(this);

		cW = getWidth();
		cH = getHeight();

		lstLines = (cH - logo.getHeight() - SPACEAFTERLOGO - fntH - SPACEAFTERPERIOD - SPACEAFTERLINE - GUI.ARROWHEIGHT - SPACEAFTERLINE) / (2 * fntH);
		recPtr = new int[lstLines];
		recBuf = new StringBuffer[lstLines];
		recSize = new int[lstLines];
		for (int i = 0; i < lstLines; i++) {
			recPtr[i] = -1;
			recBuf[i] = new StringBuffer(TIMELINEWIDTH);
			recSize[i] = -1;
		}

		lstTop = 0;
	}

	/**
	 * Initialize the main view.
	 *
	 * @exception Exception.
	 */
	public void initView() throws Exception {
		if (reAll != null) {
			reAll = null;
			System.gc();
		}

		pdFilt.setPeriod(pp.getPeriodStart(), pp.getPeriodEnd());
		reAll = mid.dbPeriod.enumerateRecords((RecordFilter)pdFilt, (RecordComparator)pdComp, false);
		recCnt = reAll.numRecords();

		Record r = new Record();
		int ptr;

		pTotal = 0;
		for (int idx = 0; idx < recCnt; idx++) {
			if (reAll.hasNextElement()) {
				ptr = reAll.nextRecordId();
				r.load(mid.dbPeriod, ptr);

				pTotal += r.mon;
				pTotal += r.tue;
				pTotal += r.wed;
				pTotal += r.thu;
				pTotal += r.fri;
				pTotal += r.sat;
				pTotal += r.sun;
			} else {
				break;
			}
		}

		lstTop = 0;
		barTop = 0;
		canShowHalf = false;
		show2ndHalf = false;
		rebuildRecPtr();
	}

	/**
	 * Rebuild the list of record "Pointers".
	 *
	 * @exception Exception.
	 */
	private void rebuildRecPtr() throws Exception {
		int idx = 0;

		reAll.reset();

		while (idx < lstTop) {
			reAll.nextRecordId();
			idx++;
		}

		StringBuffer sBuf;
		Record r = new Record();
		for (idx = 0; idx < lstLines; idx++) {
			if (reAll.hasNextElement()) {
				recPtr[idx] = reAll.nextRecordId();
				r.load(mid.dbPeriod, recPtr[idx]);

				sBuf = recBuf[idx];
				sBuf.delete(0, sBuf.length());
				outputTime(r.mon, sBuf);
				outputTime(r.tue, sBuf);
				outputTime(r.wed, sBuf);
				outputTime(r.thu, sBuf);
				outputTime(r.fri, sBuf);
				outputTime(r.sat, sBuf);
				outputTime(r.sun, sBuf);

				recSize[idx] = defFont.stringWidth(sBuf.toString());
				if (recSize[idx] > cW) {
					canShowHalf = true;
				}
			} else {
				break;
			}
		}
		r = null;

		while (idx < lstLines) {
			recPtr[idx] = -1;
			recBuf[idx].delete(0, recBuf[idx].length());
			idx++;
		}
	}

	/**
	 * Output a time slot.
	 *
	 * @param t Time.
	 * @param sBuf StringBuffer.
	 */
	private void outputTime(int t, StringBuffer sBuf) {
		if (t == 0) {
			sBuf.append('-');
		} else {
			int v = t / 60;

			sBuf.append(Integer.toString(v));
			v = t % 60;
			if (v > 0) {
				sBuf.append(':');
				if (v < 10) {
					sBuf.append('0');
				}
				sBuf.append(Integer.toString(v));
			}
		}

		sBuf.append("  ");
	}

	/**
	 * Paint display.
	 *
	 * @param g Graphics object.
	 */
	public void paint(Graphics g) {
		g.setColor(0xffffff);
		g.fillRect(0, 0, cW, cH);
		g.setFont(defFont);

		g.setColor(0x000000);
		g.drawImage(logo, (cW - logo.getWidth()) / 2, 0, GUI.ANCHOR);

		int y = logo.getHeight() + SPACEAFTERLOGO;
		String p = pp.toString();
		int w = defFont.stringWidth(p), x, pos;
		long h, m;

		x = defFont.charWidth('<') + 2;
		if (barTop == 0) {
			g.setColor(0x8ea1b0);
			g.fillRect(x, y, w, fntH);
		}
		g.setColor(0x000000);
		g.drawString(p, x, y, GUI.ANCHOR);
		g.drawChar('<', 0, y, GUI.ANCHOR);
		g.drawChar('>', x + w + 2, y, GUI.ANCHOR);

		sBuf.delete(0, sBuf.length());
		h = pTotal / 60;
		m = pTotal % 60;
		sBuf.append(Long.toString(h));
		sBuf.append(':');
		if (m < 10) {
			sBuf.append('0');
		}
		sBuf.append(Long.toString(m));

		p = sBuf.toString();
		g.drawString(p, cW - defFont.stringWidth(p), y, GUI.ANCHOR);

		y += fntH + SPACEAFTERPERIOD;
		g.drawLine(0, y, cW, y);
		y+=SPACEAFTERLINE;

		if (recCnt > 0) {
			Record r = new Record();
			int bt = barTop - 1, i;

			if (barTop > 0) {
				g.setColor(0x8ea1b0);
				g.fillRect(0, y + (bt * fntH * 2), cW, (2 * fntH));
			}

			for (i = 0; i < lstLines; i++) {
				if (recPtr[i] == -1) {
					y += (fntH * 2);
					continue;
				}

				try {
					g.setColor((bt == i) ? 0xffffff : 0x000000);
					r.load(mid.dbPeriod, recPtr[i]);

					g.drawString(r.task, 0, y, GUI.ANCHOR);

					y += fntH;
					StringBuffer sBuf = recBuf[i];
					if (show2ndHalf) {
						p = sBuf.toString();
						pos = p.length() / 2;
						g.drawString(p.substring(pos), 0, y, GUI.ANCHOR);
					} else {
						g.drawString(sBuf.toString(), 0, y, GUI.ANCHOR);
					}

					y += fntH;
				} catch (Exception ex) {}
			}

			System.gc();

			y += SPACEAFTERLINE;

			x = (cW - (2 * GUI.ARROWWIDTH) - 2) / 2;
			if (lstTop > 0) {
				g.drawRegion(GUI.arrows, 0, 0, GUI.ARROWWIDTH, GUI.ARROWHEIGHT, 0, x, y, GUI.ANCHOR);
			} else {
				g.drawRegion(GUI.arrows, GUI.ARROWWIDTH * 2, 0, GUI.ARROWWIDTH, GUI.ARROWHEIGHT, 0, x, y, GUI.ANCHOR);
			}

			x += 2 + GUI.ARROWWIDTH;
			if ((lstTop + lstLines) < recCnt) {
				g.drawRegion(GUI.arrows, GUI.ARROWWIDTH, 0, GUI.ARROWWIDTH, GUI.ARROWHEIGHT, 0, x, y, GUI.ANCHOR);
			} else {
				g.drawRegion(GUI.arrows, GUI.ARROWWIDTH * 3, 0, GUI.ARROWWIDTH, GUI.ARROWHEIGHT, 0, x, y, GUI.ANCHOR);
			}

			if (canShowHalf) {
				if (show2ndHalf) {
					g.drawRegion(halfs, 0, 0, LEFTRIGHTWIDTH, LEFTRIGHTHEIGHT, 0, 0, y, GUI.ANCHOR);
				} else {
					g.drawRegion(halfs, LEFTRIGHTWIDTH, 0, LEFTRIGHTWIDTH, LEFTRIGHTHEIGHT, 0, cW-LEFTRIGHTWIDTH, y, GUI.ANCHOR);
				}
			}
		} else {
			g.drawString(Trl.mcNoRecs, (cW - defFont.stringWidth(Trl.mcNoRecs)) / 2, (cH - fntH) / 2, GUI.ANCHOR);
		}
	}

	/**
	 * Handle commands etc.
	 *
	 * @param c Current command.
	 * @param d Displayable.
	 */
	public void commandAction(Command c, Displayable d) {
		if (c == GUI.cmdOptions) {
			GUI.showMainOptionsList(this);
		} else if (c == GUI.cmdNew) {
			mid.goToEdit(-1);
		} else if ((c == GUI.cmdBack) || (c == GUI.cmdCancel)) {
			mid.goToMain();
		} else if (c == GUI.cmdBackup) {
			int idx = GUI.cgBRoots.getSelectedIndex();

			if (idx > -1) {
				if (backupThread == null) {
					backupThread=new BackupThread();
				}

				if (backupThread.init(backupThread, this, GUI.cgBRoots.getString(idx), Trl.mcBackingUp)) {
					GUI.setDisplayable(backupThread);
				}
			} else {
				GUI.popup(Trl.errNoRootTitle, Trl.errNoWRootBody);
			}
		} else if (c == GUI.cmdRestore) {
			GUI.confirmation(Trl.okToRestoreTitle, Trl.okToRestoreBody, Trl.Yes, Trl.No, Const.CStateRestore);
		} else if (d == GUI.lstMainOptions) {
			switch (GUI.lstMainOptions.getSelectedIndex()) {
				case 0:		// Back
					mid.goToMain();
					break;
				case 1:		// Copy previous period
					previousPeriod.setStartOfPeriod(pp.getPeriodStart());
					previousPeriod.prev();

					sBuf.delete(0, sBuf.length());
					sBuf.append(Trl.okToCopyPeriod1);
					sBuf.append(": '");
					sBuf.append(previousPeriod.toString());
					sBuf.append("' ");
					sBuf.append(Trl.okToCopyPeriod2);
					sBuf.append(" (");
					sBuf.append(pp.toString());
					sBuf.append(")?");
					GUI.confirmation(Trl.okToCopyPeriodTitle, sBuf.toString(), Trl.Yes, Trl.No, Const.CStateCopyPeriod);
					break;
				case 2:		// Clear period
					sBuf.delete(0, sBuf.length());
					sBuf.append(Trl.okToClearPeriodBody);
					sBuf.append(" '");
					sBuf.append(pp.toString());
					sBuf.append("'?");
					GUI.confirmation(Trl.okToClearPeriodTitle, sBuf.toString(), Trl.Yes, Trl.No, Const.CStateClearPeriod);
					break;
				case 3:		// Tools
					GUI.showToolsList(this);
					break;
				case 4:		// About
					GUI.showAboutForm(this);
					break;
				default:	// Exit
					mid.exit();
					break;
			}
		} else if (d == GUI.lstTools) {
			switch (GUI.lstTools.getSelectedIndex()) {
				case 0:		// Back
					mid.goToMain();
					break;
				case 1:		// Tasks List
					mid.goToTasksList();
					break;
				case 2:		// Summary
					GUI.showSummaryForm();
					break;
				case 3:		// Backup
					backupDB();
					break;
				default:	// Restore
					restoreDB();
					break;
			}
		}
	}

	/**
	 * Handle keypresses.
	 *
	 * @param keyCode Current key press.
	 */
	protected void keyPressed(int keyCode) {
		int ga = getGameAction(keyCode);

		if (barTop == 0) {
			if ((ga == Canvas.LEFT) ||
			    (keyCode == Canvas.KEY_NUM4)) {
				pp.prev();
				try {
					initView();
				} catch (Exception ex) {}
				repaint();
			} else if ((ga == Canvas.RIGHT) ||
				   (keyCode == Canvas.KEY_NUM6)) {
				pp.next();
				try {
					initView();
				} catch (Exception ex) {}
				repaint();
			} else if ((ga == Canvas.FIRE) ||
				   (keyCode == Canvas.KEY_NUM5)) {
				pp.reset();
				try {
					initView();
				} catch (Exception ex) {}
				repaint();
			} else if ((ga == Canvas.DOWN) ||
				   (keyCode == Canvas.KEY_NUM8)) {
					if (recCnt > 0) {
						barTop = 1;
						repaint();
				}
			}
		} else {
			if ((ga == Canvas.UP) ||
		    	    (keyCode == Canvas.KEY_NUM2)) {
				if (barTop > 1) {
					barTop--;
					repaint();
				} else if (barTop == 1) {
					if (lstTop > 0) {
						lstTop--;
						reworkList();
					} else {
						barTop = 0;
						reworkList();
					}
				}
			} else if ((ga == Canvas.DOWN) ||
			   	   (keyCode == Canvas.KEY_NUM8)) {
				int bt = barTop - 1;
				if (recCnt <= lstLines) {
					if (bt < (recCnt - 1)) {
						barTop++;
						repaint();
					}
				} else {
					if (bt < (lstLines - 1)) {
						barTop++;
						repaint();
					} else if ((lstTop + lstLines) < recCnt) {
						lstTop++;
						reworkList();
					}
				}
			} else if ((ga == Canvas.LEFT) ||
				   (keyCode == Canvas.KEY_NUM4)) {
				if ((canShowHalf) && (show2ndHalf)) {
					show2ndHalf = false;
					repaint();
				}
			} else if ((ga == Canvas.RIGHT) ||
				   (keyCode == Canvas.KEY_NUM6)) {
				if ((canShowHalf) && (!show2ndHalf)) {
					show2ndHalf = true;
					repaint();
				}
			} else if ((ga == Canvas.FIRE) ||
				   (keyCode == Canvas.KEY_NUM5)) {
				mid.goToEdit(recPtr[barTop - 1]);
			} else if (keyCode == Canvas.KEY_NUM0) {
				barTop = 0;
				lstTop = 0;
				reworkList();
			}
		}
	}

	/**
	 * Rework and repaint the main table.
	 */
	private void reworkList() {
		try {
			rebuildRecPtr();
			repaint();
		} catch (Exception ex) {
			GUI.fatal(Trl.errNoRebuildList, ex.getMessage());
		}
	}

	/**
	 * Copy the previous period to this period.
	 */
	public void copyPeriod() {
		pdFilt.setPeriod(previousPeriod.getPeriodStart(), previousPeriod.getPeriodEnd());

		try {
			RecordEnumeration re = mid.dbPeriod.enumerateRecords((RecordFilter)pdFilt, null, false);
			int cnt = re.numRecords(), i, idx;
			Record r = new Record();
			long p = pp.getPeriodStart();

			for (i = 0; i < cnt; i++) {
				idx = re.nextRecordId();
				r.load(mid.dbPeriod, idx);
				r.period = p;
				r.save(mid.dbPeriod, -1);
			}
		} catch (Exception ex) {}
	}

	/**
	 * Clear the current period.
	 */
	public void clearPeriod() {
		pdFilt.setPeriod(pp.getPeriodStart(), pp.getPeriodEnd());

		try {
			RecordEnumeration re = mid.dbPeriod.enumerateRecords((RecordFilter)pdFilt, null, false);
			int cnt = re.numRecords(), i, idx;

			for (i = 0; i < cnt; i++) {
				idx = re.nextRecordId();
				mid.dbPeriod.deleteRecord(idx);

			}
		} catch (Exception ex) {}
	}

	/**
	 * Show the Backup form.
	 */
	private void backupDB() {
		if (mid.hasJSR75()) {
			GUI.showBackupForm(this);
		}
	}

	/**
	 * Show the Restore form.
	 */
	private void restoreDB() {
		if (mid.hasJSR75()) {
			Vector v = new Vector();
			FileConnection in = null;
			Enumeration e = null;
			int cnt = 0;
			String dev, s;

			try {
				e = FileSystemRegistry.listRoots();
			} catch (Exception ex) {}

			if (e != null) {
				while (e.hasMoreElements()) {
					dev = (String)e.nextElement();
					if (!dev.endsWith(":/")) {
						if (dev.endsWith("/")) {
							dev = dev.substring(0, dev.length() - 1);
						}
						s = "file:///" + dev + "/atTracker/atTrackerBackup.atb";

						try {
							in = (FileConnection)Connector.open(s);
							if (in.exists()) {
								v.addElement(dev);
								cnt++;
							}
						} catch (Exception ex) {
						} finally {
							if (in != null) {
								try {
									in.close();
								} catch (Exception ex) {}
								in = null;
							}
						}
					}
				}
			}

			if (cnt > 0) {
				GUI.buildRestoreForm(this);
				GUI.cgRRoots.deleteAll();

				for (int i = 0; i < cnt; i++) {
					GUI.cgRRoots.append((String)v.elementAt(i), null);
				}

				GUI.setDisplayable(GUI.fRestore);
			} else {
				GUI.popup(Trl.errNoBackupTitle, Trl.errNoBackupBody);
			}
		}
	}

	/**
	 * Start restore thread.
	 */
	public void restore() {
		int idx = GUI.cgRRoots.getSelectedIndex();

		if (idx >- 1) {
			if (restoreThread == null) {
				restoreThread = new RestoreThread();
			}

			restoreThread.rsPeriod = null;
			restoreThread.rsTasks = null;
			try {
				restoreThread.rsPeriod = openUniqueDB("PeriodData");
				restoreThread.rsTasks = openUniqueDB("Tasks");

				if (restoreThread.init(restoreThread, this, GUI.cgRRoots.getString(idx), Trl.mcRestoring)) {
					GUI.setDisplayable(restoreThread);
				}
			} catch (Exception ex) {
				closeAndDeleteDB(restoreThread.rsPeriod);
				closeAndDeleteDB(restoreThread.rsTasks);
				GUI.popup(Trl.errRestoreTitle, Trl.errRestoreBody+": "+restoreThread.error);
			}
		} else {
			GUI.popup(Trl.errNoRootTitle, Trl.errNoRRootBody);
		}
	}

	/**
	 * Handler called when the restore is done.
	 */
	public void restoreDone() {
		if (restoreThread.error != null) {
			closeAndDeleteDB(restoreThread.rsPeriod);
			closeAndDeleteDB(restoreThread.rsTasks);
			GUI.popup(Trl.errRestoreTitle, Trl.errRestoreBody + ": " + restoreThread.error);
		} else {
			closeAndDeleteDB(mid.dbPeriod);
			closeAndDeleteDB(mid.dbTasks);
			mid.dbPeriod = restoreThread.rsPeriod;
			mid.dbTasks = restoreThread.rsTasks;
			try {
				mid.rebuildTasksList();
				GUI.info(Trl.btrtDone, Trl.rtDone);
			} catch (Exception ex) {
				GUI.fatal(Trl.noTasksList, ex.getMessage());
			}
		}
	}

	/**
	 * Create and open a new database. Database name: ${prefix}_${cnt}.
	 *
	 * @param prefix Prefix of database.
	 * @return RecordStore.
	 * @exception Exception.
	 */
	private RecordStore openUniqueDB(String prefix) throws Exception {
		String[] dbList = RecordStore.listRecordStores();
		int cnt = 0, i, x, v;

		if (dbList != null) {
			for (i = 0; i < dbList.length; i++) {
				if (dbList[i].startsWith(prefix)) {
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

			cnt++;
		}

		return RecordStore.openRecordStore(prefix + "_" + cnt, true);
	}

	/**
	 * Close and delete a RecordStore.
	 *
	 * @param rs RecordStore.
	 */
	private void closeAndDeleteDB(RecordStore rs) {
		if (rs != null) {
			try {
				String n = rs.getName();
				mid.dbClose(rs);
				RecordStore.deleteRecordStore(n);
			} catch (Exception ex) {}
		}
	}
}
