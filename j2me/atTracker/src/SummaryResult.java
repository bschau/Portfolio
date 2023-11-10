import java.util.Calendar;
import java.util.Date;
import java.util.Vector;

import javax.microedition.lcdui.Canvas;
import javax.microedition.lcdui.Command;
import javax.microedition.lcdui.CommandListener;
import javax.microedition.lcdui.Displayable;
import javax.microedition.lcdui.Font;
import javax.microedition.lcdui.Graphics;

import javax.microedition.rms.RecordEnumeration;

public class SummaryResult extends Canvas implements CommandListener {
	private final int SPACEAFTERPERIOD = 2;
	private final int SPACINGTIMETASK = 6;
	private Font defFont = Font.getDefaultFont();
	private int fntH = defFont.getHeight();
	private StringBuffer sBuf = new StringBuffer();
	private atTrackerMIDlet mid;
	private Vector posWords, negWords;
	private Calendar start, end;
	private SortedList res;
	private String timeInAllTasks;
	private int lstTop, lstLines, cW, cH, maxTimeSize;

	/**
	 * Constructor.
	 */
	public SummaryResult() {}

	/**
	 * Initialize the Summary Result canvas.
	 *
	 * @param m atTrackerMIDlet
	 * @exception Exception.
	 */
	public void init(atTrackerMIDlet m) throws Exception {
		mid = m;

		this.addCommand(GUI.cmdBack);
		this.setCommandListener(this);

		cW = getWidth();
		cH = getHeight();

		lstLines = (cH - (2 * fntH) - SPACEAFTERPERIOD - GUI.ARROWHEIGHT) / fntH;
	}

	/**
	 * Initialize the Summary Result view.
	 *
	 * @exception Exception.
	 */
	public void initView() throws Exception {
		lstTop = 0;

		int resCnt = res.getCount(), w;
		long l, v;

		maxTimeSize = 0;
		for (int idx = 0; idx < resCnt; idx++) {
			sBuf.delete(0, sBuf.length());
			l = res.getTime(idx);
			v = l / 60;
			sBuf.append(Long.toString(v));
			sBuf.append(':');
			v = l % 60;
			if (v < 10) {
				sBuf.append('0');
			}
			sBuf.append(Long.toString(v));
			w = defFont.stringWidth(sBuf.toString());
			if (w > maxTimeSize) {
				maxTimeSize = w;
			}
		}
	}

	/**
	 * Paint display.
	 *
	 * @param g Graphics object.
	 */
	public void paint(Graphics g) {
		int y = 0, x;

		g.setColor(0xffffff);
		g.fillRect(0, 0, cW, cH);
		g.setFont(defFont);

		g.setColor(0x000000);
		g.drawString(timeInAllTasks, 0, y, GUI.ANCHOR);
		y += fntH;
		g.drawLine(0, y + (fntH / 2), cW, y + (fntH / 2));
		y += fntH;

		long l, v;
		String s;
		int idx = lstTop, resCnt = res.getCount(), p;
		for (int i = 0; i < lstLines; i++) {
			if (idx < resCnt) {
				sBuf.delete(0, sBuf.length());
				l = res.getTime(idx);
				v = l / 60;
				sBuf.append(Long.toString(v));
				sBuf.append(':');
				v = l % 60;
				if (v < 10) {
					sBuf.append('0');
				}
				sBuf.append(Long.toString(v));

				s = sBuf.toString();
				p = defFont.stringWidth(s);
				g.drawString(s, maxTimeSize - p, y, GUI.ANCHOR);

				g.drawString(res.getTask(idx), maxTimeSize + SPACINGTIMETASK, y, GUI.ANCHOR);
			}

			y += fntH;
			idx++;
		}

		y += SPACEAFTERPERIOD;

		x = (cW - (2 * GUI.ARROWWIDTH) - 2) / 2;
		if (lstTop > 0) {
			g.drawRegion(GUI.arrows, 0, 0, GUI.ARROWWIDTH, GUI.ARROWHEIGHT, 0, x, y, GUI.ANCHOR);
		} else {
			g.drawRegion(GUI.arrows, GUI.ARROWWIDTH * 2, 0, GUI.ARROWWIDTH, GUI.ARROWHEIGHT, 0, x, y, GUI.ANCHOR);
		}

		x += 2 + GUI.ARROWWIDTH;
		if ((lstTop + lstLines) < resCnt) {
			g.drawRegion(GUI.arrows, GUI.ARROWWIDTH, 0, GUI.ARROWWIDTH, GUI.ARROWHEIGHT, 0, x, y, GUI.ANCHOR);
		} else {
			g.drawRegion(GUI.arrows, GUI.ARROWWIDTH * 3, 0, GUI.ARROWWIDTH, GUI.ARROWHEIGHT, 0, x, y, GUI.ANCHOR);
		}
	}

	/**
	 * Handle commands etc.
	 *
	 * @param c Current command.
	 * @param d Displayable.
	 */
	public void commandAction(Command c, Displayable d) {
		if (c == GUI.cmdBack) {
			mid.goToMain();
		}
	}

	/**
	 * Handle keypresses.
	 *
	 * @param keyCode Current key press.
	 */
	protected void keyPressed(int keyCode) {
		int ga = getGameAction(keyCode), l = lstLines - 1;

		if ((ga == Canvas.UP) ||
	    	    (keyCode == Canvas.KEY_NUM2)) {
			lstTop -= l;
			if (lstTop < 0) {
				lstTop = 0;
			}
			repaint();
		} else if ((ga == Canvas.DOWN) ||
		   	   (keyCode == Canvas.KEY_NUM8)) {
			int recCnt = res.getCount();

			lstTop += l;
			if ((lstTop + lstLines) > recCnt) {
				lstTop = recCnt - lstLines;
				if (lstTop < 0) {
					lstTop = 0;
				}
			}
			repaint();
		} else if (keyCode == Canvas.KEY_NUM0) {
			lstTop = 0;
			repaint();
		}
	}

	/**
	 * Set constraints form summarizing.
	 *
	 * @param kw Keywords.
	 * @param s Start date.
	 * @param e End date.
	 */
	public void setConstraints(String kw, Date s, Date e) {
		int kwLen = kw.length();
		StringBuffer sBuf = new StringBuffer(kwLen);
		char c;

		if (s == null) {
			start = null;
		} else {
			start = Calendar.getInstance();
			start.setTime(s);
		}

		if (e == null) {
			end = null;
		} else {
			end = Calendar.getInstance();
			end.setTime(e);
		}

		posWords = null;
		negWords = null;
		for (int i = 0; i < kwLen; i++) {
			c = kw.charAt(i);
			if (c > ' ') {
				sBuf.append(c);
			} else {
				addKeyword(sBuf);
				sBuf.delete(0, sBuf.length());
			}
		}

		if (sBuf.length() > 0) {
			addKeyword(sBuf);
		}
	}

	/**
	 * Add a keyword to the negWords vector if the keyword starts with '-'.
	 * Otherwise the keyword is added to the posWords vector.
	 *
	 * @param sBuf StringBuffer with keyword.
	 */
	private void addKeyword(StringBuffer sBuf) {
		if (sBuf.charAt(0) == '-') {
			sBuf.deleteCharAt(0);
			if (sBuf.length() > 0) {
				if (negWords == null) {
					negWords = new Vector();
				}
				negWords.addElement(sBuf.toString().toLowerCase());
			}
		} else {
			if (posWords == null) {
				posWords = new Vector();
			}
			posWords.addElement(sBuf.toString().toLowerCase());
		}
	}

	/**
	 * Summarize using the given constraints.
	 *
	 * @exception Exception.
	 */
	public void summarize() throws Exception {
		long lS = Long.MIN_VALUE, lE = Long.MAX_VALUE, allTime = 0;

		if (start != null) {
			start.set(Calendar.HOUR_OF_DAY, 1);
			start.set(Calendar.MINUTE, 0);
			start.set(Calendar.SECOND, 0);
			start.set(Calendar.MILLISECOND, 0);
			lS = ((Date)start.getTime()).getTime();
		} else {
			lS = Long.MIN_VALUE;
		}

		if (end != null) {
			end.set(Calendar.HOUR_OF_DAY, 1);
			end.set(Calendar.MINUTE, 0);
			end.set(Calendar.SECOND, 0);
			end.set(Calendar.MILLISECOND, 0);
			lE = ((Date)end.getTime()).getTime();

			if (lE < lS) {
				long t = lE;

				lE = lS;
				lS = t;
			}

			lE += (23 * 60) - 1;	// Go to end of the day ..
		} else {
			lE = Long.MAX_VALUE;
		}

		res = new SortedList();

		System.gc();

		Record r = new Record();
		int oneDay = (24 * 60 * 60) * 1000, time;
		String t;
		long l, p;

		RecordEnumeration re = mid.dbPeriod.enumerateRecords(null, null, false);

		while (re.hasNextElement()) {
			r.load(mid.dbPeriod, re.nextRecordId());

			if (((r.period + Const.PeriodEndStride) < lS) || (r.period > lE)) {
				continue;
			}

			t = r.task.toLowerCase();
			if (negWords != null) {
				if (matchNegKeyword(t)) {
					continue;
				}
			}

			if (posWords != null) {
				if (!matchPosKeyword(t)) {
					continue;
				}
			}

			l = r.period;
			time = 0;
			if ((lS <= l) && (l <= lE)) {
				time += r.mon;
			}
			l += oneDay;
			if ((lS <= l) && (l <= lE)) {
				time += r.tue;
			}
			l += oneDay;
			if ((lS <= l) && (l <= lE)) {
				time += r.wed;
			}
			l += oneDay;
			if ((lS <= l) && (l <= lE)) {
				time += r.thu;
			}
			l += oneDay;
			if ((lS <= l) && (l <= lE)) {
				time += r.fri;
			}
			l += oneDay;
			if ((lS <= l) && (l <= lE)) {
				time += r.sat;
			}
			l += oneDay;
			if ((lS <= l) && (l <= lE)) {
				time += r.sun;
			}

			res.add(r.task, time);
			allTime += time;
		}

		sBuf.delete(0, sBuf.length());
		sBuf.append(Trl.srTime);
		sBuf.append(": ");

		long v = allTime / 60;
		sBuf.append(Long.toString(v));
		v = allTime % 60;
		sBuf.append(':');
		if (v < 10) {
			sBuf.append('0');
		}

		sBuf.append(Long.toString(v));
		timeInAllTasks = sBuf.toString();
	}

	/**
	 * See if any of the keywords on the negative list can be found in
	 * the supplied source.
	 *
	 * @param src Source.
	 * @return true if a match was found, false otherwise.
	 */
	private boolean matchNegKeyword(String src) {
		int vLen = negWords.size(), i;

		for (i = 0; i < vLen; i++) {
			if (src.indexOf((String)negWords.elementAt(i)) > -1) {
				return true;
			}
		}

		return false;
	}

	/**
	 * See if all of the keywords on the positive list can be found in
	 * the supplied source.
	 *
	 * @param src Source.
	 * @return true if all words matched, false otherwise.
	 */
	private boolean matchPosKeyword(String src) {
		int vLen = posWords.size(), i;

		for (i = 0; i < vLen; i++) {
			if (src.indexOf((String)posWords.elementAt(i)) == -1) {
				return false;
			}
		}

		return true;
	}

	/**
	 * Return number of results.
	 *
	 * @param count.
	 */
	public int getMatchCount() {
		return res.getCount();
	}
}
