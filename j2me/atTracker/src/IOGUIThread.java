import java.lang.Thread;

import java.io.DataOutputStream;

import javax.microedition.io.Connector;
import javax.microedition.io.OutputConnection;
import javax.microedition.io.file.FileConnection;

import javax.microedition.lcdui.Canvas;
import javax.microedition.lcdui.Font;
import javax.microedition.lcdui.Graphics;

import javax.microedition.rms.RecordEnumeration;
import javax.microedition.rms.RecordStore;

public abstract class IOGUIThread extends Canvas implements Runnable {
	private final int SPACING = 10;
	private final int GADGETWIDTH = 50;
	private final int GADGETHEIGHT = 20;
	private final int KNOPHEIGHT = GADGETHEIGHT - 4;
	private final int KNOPWIDTH = 20;
	public boolean runThread = false;
	public Thread thread = null;
	private Font defFont = Font.getDefaultFont();
	private int fntH = defFont.getHeight();
	private String title;
	private int cW, cH, topY, animStep, animSpeed;
	protected String firstLine, secondLine;
	protected String error, dstDir, dst;
	protected FileConnection file;
	protected MainCanvas main;

	/**
	 * Initialize the thread.
	 *
	 * @param th Thread type (backup/restore).
	 * @param m MainCanvas.
	 * @param d Device (source/destination).
	 * @param t Title.
	 * @return true if everything is ok, false in case of error.
	 */
	public boolean init(IOGUIThread th, MainCanvas m, String d, String t) {
		main = m;
		if (d.endsWith("/")) {
			d = d.substring(0, d.length()-1);
		}
		dstDir = "file:///" + d + "/atTracker";
		dst = dstDir + "/"+"atTrackerBackup.atb";
		title = t;
		runThread = true;
		cW = getWidth();
		cH = getHeight();
		animStep = 0;
		animSpeed = 2;
		error = null;
		file = null;
		firstLine = null;
		secondLine = null;

		topY = (cH - (2 * fntH) - SPACING - GADGETHEIGHT - SPACING - (2 * fntH)) / 2;
		try {
			thread = new Thread(th);
			thread.start();
		} catch (Exception ex) {
			GUI.popup(Trl.errThreadTitle, Trl.errThreadBody + ": " + ex.getMessage());
			return false;
		}

		return true;
	}

	/**
	 * Stop execution.
	 */
	public void stop() {
		runThread = false;
		if (thread == null) {
			return;
		}

		try {
			if (thread.isAlive()) {
				thread.join();
			}
		} catch (Exception ex) {}

		thread = null;
		closeFile();
	}

	/**
	 * Close the backup file.
	 */
	protected void closeFile() {
		if (file != null) {
			try {
				file.close();
			} catch (Exception ex) {}

			file = null;
		}
	}

	/**
	 * Paint display.
	 *
	 * @param g Graphics.
	 */
	public void paint(Graphics g) {
		int cW = getWidth(), cH = getHeight(), y = topY, w;

		g.setColor(0xffffff);
		g.fillRect(0, 0, cW, cH);
		g.setColor(0x000000);

		w = defFont.stringWidth(title);
		g.drawString(title, (cW - w) / 2, y, GUI.ANCHOR);
		y += fntH;
		w = defFont.stringWidth(Trl.ioPleaseWait);
		g.drawString(Trl.ioPleaseWait, (cW - w) / 2, y, GUI.ANCHOR);
		y += fntH+SPACING;

		int gS = (cW - GADGETWIDTH) / 2;
		g.drawRect(gS, y, GADGETWIDTH, GADGETHEIGHT);
		g.setColor(0x77aaff);

		g.fillRect(gS + 3 + animStep, y + 3, KNOPWIDTH - 1, KNOPHEIGHT - 1);
		g.setColor(0x000000);
		g.drawRect(gS + 2 + animStep, y + 2, KNOPWIDTH, KNOPHEIGHT);

		y += GADGETHEIGHT + SPACING;

		if (firstLine != null) {
			w = defFont.stringWidth(firstLine);
			g.drawString(firstLine, (cW - w) / 2, y, GUI.ANCHOR);
		}

		y += fntH;
		if (secondLine != null) {
			w = defFont.stringWidth(secondLine);
			g.drawString(secondLine, (cW - w) / 2, y, GUI.ANCHOR);
		}
	}

	/**
	 * Animate.
	 */
	protected void animate() {
		repaint();

		animStep += animSpeed;
		if (animStep < 0) {
			animSpeed = 2;
			animStep = animSpeed;
		} else if ((GADGETWIDTH - 4 - animStep - KNOPWIDTH) <= 0) {
			animSpeed = -2;
			animStep = GADGETWIDTH - 2 - KNOPWIDTH + animSpeed;
		}
	}

	/**
	 * Run method - must be implemented elsewhere.
	 */
	abstract public void run();
}
