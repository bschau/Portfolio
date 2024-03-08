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
 * The TrailRunner canvas.
 */
public class TRRunnerCanvas extends Frame implements Runnable {
	/**
	 * The list of Coordinates.
	 */
	public ArrayList<Coord> coords = null;

	/**
	 * Variables to be manipulated by the Control Canvas.
	 */
	public int numberOfSprites, animationDelay, spriteSpeed, spriteStride;

	/**
	 * Offset of sprites etc.
	 */
	public int xOffset, yOffset, trailMask, trailColor;

	/**
	 * File with trail.
	 */
	public String trailFile;

	/**
	 * Should trail be shown?
	 */
	public boolean showTrail;

	/**
	 * Sprite image.
	 */
	public BufferedImage spriteImage;

	/**
	 * Background image.
	 */
	public BackgroundImage backImg;

	/**
	 * Array with sprites.
	 */
	public Sprite[] sprites;

	/** */
	private Thread animThread;
	private Graphics offScreen;
	private Image offScreenImage;
	private Insets ins;

	/**
	 * Build the GUI elements.
	 */
	private void buildGUI() {
		setTitle("TrailRunner");
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
		Sprite spr;
		int i, pos;
		Coord c;

		while (true) {
			if (coords != null) {
				for (i = 0; i < numberOfSprites; i++) {
					spr = sprites[i];
					spr.pos += spriteSpeed;
					if (spr.pos >= coords.size()) {
						spr.pos = 0;
					} else if (spr.pos < 0) {
						spr.hidden = true;
					} else {
						spr.hidden = false;
						pos = spr.pos;
						c = coords.get(pos);
						spr.x = c.x - xOffset;
						spr.y = c.y - yOffset;
					}
				}
			}

			repaint();
			try {
				time += animationDelay;
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

		Sprite spr;
		for (int i = numberOfSprites - 1; i >- 1; i--) {
			spr = sprites[i];
			if (!spr.hidden) {
				((Graphics2D)offScreen).drawImage(spriteImage, null, spr.x, spr.y);
			}
		}
		g.drawImage(offScreenImage, ins.left, ins.top, this);
	}

	/**
	 * Redraw canvas.
	 *
	 * @param g Graphics.
	 */
	public void paint(Graphics g) {
		update(g);
	}

	/**
	 * Load a trail from the file.
	 *
	 * @exception Exception.
	 */
	public void loadTrail() throws Exception {
		coords = null;
		BufferedReader in = new BufferedReader(new FileReader(trailFile));
		String s = in.readLine();
		int m = trailMask, line = 1;
		String[] arr;
		Coord c;
		ArrayList<Coord> crd = new ArrayList<Coord>();

		while ((s = in.readLine()) != null) {
			line++;
			if (--m > 0) {
				continue;
			}

			m = trailMask;

			arr = s.split("\\s");
			if (arr.length < 2) {
				throw new Exception((line - 1) + ": malformed");
			}

			c = new Coord();
			c.x = Integer.parseInt(arr[0]);
			c.y = Integer.parseInt(arr[1]);
			crd.add(c);
		}

		coords = crd;
	}

	/**
	 * Paint the trail on the image.
	 */
	public void paintTrail() {
		if (showTrail) {
			BufferedImage img = backImg.getImage();
			Coord c;
			int x, y, i;

			for (i = 0; i < coords.size(); i++) {
				c = (Coord)coords.get(i);
				x = c.x - xOffset;
				y = c.y - yOffset;
				if ((x >= 0) && (x < backImg.width) &&
				    (y >= 0) && (y < backImg.height)) {
					img.setRGB(x, y, trailColor);
				}
			}

			backImg.setCurrentImage(img);
		}
	}

	/**
	 * Set sprite stride.
	 *
	 * @param stride Stride between sprites.
	 */
	public void setStride(int stride) {
		spriteStride = stride;

		int pos = sprites[0].pos, i;

		for (i = 1; i < numberOfSprites; i++) {
			pos -= stride;
			sprites[i].pos = pos;

			if (sprites[i].pos < 0) {
				sprites[i].hidden = true;
			} else {
				sprites[i].hidden = false;
			}
		}
	}
}
