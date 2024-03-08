package com.schau.TrailTools;

import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.awt.Frame;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.Image;
import java.awt.image.BufferedImage;
import java.awt.Insets;
import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.lang.Thread;
import java.util.ArrayList;
import javax.swing.JFrame;
import javax.swing.JPanel;
import javax.swing.UIManager;

/**
 * The TrailDancer canvas.
 */
public class TDRunnerCanvas extends Frame implements Runnable {
	/**
	 * Curves
	 */
	public short[] cos, sin;

	/**
	 * Sprite image.
	 */
	public BufferedImage spriteImage;

	/**
	 * Background image.
	 */
	public BackgroundImage backImg;

	/** */
	private TDVars vars;
	private Thread animThread;
	private Graphics offScreen;
	private Image offScreenImage;
	private Insets ins;
	private int x1p = 0, x2p = 0, y1p = 0, y2p = 0;


	/**
	 * Set the variables block.
	 *
	 * @param v TDVars.
	 */
	public void setVars(TDVars v) {
		vars = v;
	}

	/**
	 * Build the GUI elements.
	 */
	private void buildGUI() {
		setTitle("TrailDancer");
		pack();
		ins = getInsets();

		setSize(backImg.width + ins.left + ins.right, backImg.height + ins.top + ins.bottom);
		setLocationRelativeTo(null);
		setVisible(true);
	}

	/**
	 * Main entry for the TrailRunner.
	 */
	public void main() {
		try {
			UIManager.setLookAndFeel(UIManager.getSystemLookAndFeelClassName());
		} catch (Exception ex) {}

		buildGUI();

		x1p = vars.xStart1;
		x2p = vars.xStart2;
		y1p = vars.yStart1;
		y2p = vars.yStart2;

		animThread = new Thread(this);
		animThread.start();

		this.addWindowListener(new WindowAdapter() {
			public void windowClosing(WindowEvent e) {
				System.exit(0);
			}
		});
	}

	/**
	 * Thread main.
	 */
	public void run() {
		long time = System.currentTimeMillis();
		long animationDelay = 250;
		int cL = cos.length, sL = sin.length;

		while (true) {
			x1p = addStep(x1p, vars.xSpeed1, cL);
			x2p = addStep(x2p, vars.xSpeed2, cL);
			y1p = addStep(y1p, vars.ySpeed1, sL);
			y2p = addStep(y2p, vars.ySpeed2, sL);

			repaint();
			try {
				time += vars.animationDelay;
				Thread.sleep(Math.max(0, time - System.currentTimeMillis()));
			} catch (InterruptedException ex) {
				System.out.println(ex.getMessage());
			}
		}
	}

	/**
	 * Redraw canvas.
	 *
	 * @param g Graphics.
	 */
	public void update(Graphics g) {
		if (offScreen == null) {
			// Hohum, get me a double buffered display ...
			offScreenImage = createImage(backImg.width, backImg.height);
			offScreen = offScreenImage.getGraphics();
		}

		backImg.drawCurrentImage(offScreen);

		int p1x = x1p, p2x = x2p, p1y = y1p, p2y = y2p, x, y;
		int cL = cos.length, sL = sin.length;
		int sW = 160 - (spriteImage.getWidth() / 2);
		int sH = 240 - (spriteImage.getHeight() / 2);

		for (int i = 0; i < vars.sprNumber; i++) {
			x = cos[p1x];
			if (vars.xSpeed2 > 0) {
				x += cos[p2x];
				x /= 2;
			}

			y = sin[p1y];
			if (vars.ySpeed2 > 0) {
				y += sin[p2y];
				y /= 2;
			}


			((Graphics2D)offScreen).drawImage(spriteImage, null, x + sW, y + sH);

			p1x = addStep(p1x, vars.sprStrideX1, cL);
			p2x = addStep(p2x, vars.sprStrideX2, cL);
			p1y = addStep(p1y, vars.sprStrideY1, sL);
			p2y = addStep(p2y, vars.sprStrideY2, sL);
		}

		g.drawImage(offScreenImage, ins.left, ins.top, this);
	}

	/**
	 * Add one step to the current value.
	 *
	 * @param v Current value.
	 * @param s Step.
	 * @param m Max.
	 */
	private static int addStep(int v, int s, int m) {
		v += s;
		if (v < 0) {
			return m + v;
		} else if (v >= m) {
			return v - m;
		}

		return v;
	}

	/**
	 * Redraw canvas.
	 *
	 * @param g Graphics.
	 */
	public void paint(Graphics g) {
		update(g);
	}
}
