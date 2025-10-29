package com.schau.TrailTools;

import java.awt.Color;
import java.awt.Graphics2D;
import java.awt.image.BufferedImage;
import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import javax.imageio.ImageIO;

/**
 * TrailRunner.
 */
public class TrailRunner {
	private static TRRunnerCanvas rc=new TRRunnerCanvas();
	private static TRControlCanvas cc=new TRControlCanvas();

	/**
	 * Main entry.
	 */
	public static void main(String[] args) {
		Options opt = new Options();
		String spriteBlob = null, backgroundImage = null;
		boolean showTrail = true;
		int trailColor = 0xff00ff;
		int spriteStride = 16, numberOfSprites = 8;
		int animationDelay = 100, trailMask = 1;
		int spriteSpeed = 1;
		int xOffset = 16, yOffset = 16;
		int dispW = 320, dispH = 480, dispC = 0xffffff;
		int sprW = 16, sprH = 16, sprC = 0x0000ff;

		try {
			String[] arr;
			String s;
			
			// Sprite Blob -B
			// Display info -D (x,y,c)
			// Sprite info -G (x,y,c)
			// Background picture -P
			// Stride between sprites -S
			// Trail Color -T xxxxxx (or none)
			// Animation speed -d
			// Number of sprite -n
			// Sprite speed -s
			// Trailmask -t
			// x offset -x
			// y offset -y
			opt.Parse(args, "B:C:D:G::P:S:d:n:s:t:x:y:");
			if ((s = opt.Get("B")) != null) {
				spriteBlob = s;
			}

			if ((s = opt.Get("C")) != null) {
				if (s.compareTo("none") == 0) {
					showTrail = false;
				} else {
					trailColor = Integer.parseInt(s, 16);
				}
			}

			if ((s = opt.Get("D")) != null) {
				arr = s.split(",");
				if (arr.length != 3) {
					throw new Exception("Display dimension format invalid (w,h,c)");
				}
				dispW = Integer.parseInt(arr[0]);
				dispH = Integer.parseInt(arr[1]);
				dispC = Integer.parseInt(arr[2]);
			}

			if ((s = opt.Get("G")) != null) {
				arr = s.split(",");
				if (arr.length != 3) {
					throw new Exception("Sprite dimension format invalid (w,h,c)");
				}
				sprW = Integer.parseInt(arr[0]);
				sprH = Integer.parseInt(arr[1]);
				sprC = Integer.parseInt(arr[2]);
			}

			if ((s = opt.Get("P")) != null) {
				backgroundImage = s;
			}

			if ((s = opt.Get("S")) != null) {
				spriteStride = Integer.parseInt(s);
			}

			if ((s = opt.Get("d")) != null) {
				animationDelay = Integer.parseInt(s);
			}

			if ((s = opt.Get("n")) != null) {
				numberOfSprites = Integer.parseInt(s);
			}

			if ((s = opt.Get("s")) != null) {
				spriteSpeed = Integer.parseInt(s);
			}

			if ((s = opt.Get("t")) != null) {
				trailMask = Integer.parseInt(s);
			}

			if ((s = opt.Get("x")) != null) {
				xOffset = Integer.parseInt(s);
			}

			if ((s = opt.Get("y")) != null) {
				yOffset = Integer.parseInt(s);
			}
		} catch (Exception ex) {
			System.err.println("*** " + ex.getMessage());
			System.exit(1);
		}

		try {
			if (opt.GetRestArgsCount() != 1) {
				throw new Exception("Version 1.0\nUsage: TrailRunner [OPTIONS] trail");
			}

			rc.trailFile = opt.GetRestArg(0);
			rc.trailMask = trailMask;
			rc.loadTrail();

			loadSprite(spriteBlob, sprW, sprH, sprC);
			loadBackground(backgroundImage, dispW, dispH, dispC);
			
			rc.numberOfSprites = numberOfSprites;
			rc.animationDelay = animationDelay;
			rc.spriteSpeed = spriteSpeed;
			rc.spriteStride = spriteStride;
			rc.xOffset = xOffset;
			rc.yOffset = yOffset;
			rc.showTrail = showTrail;
			rc.trailColor = trailColor;
			rc.paintTrail();
			allocSprites(spriteStride);
		} catch (Exception ex) {
			System.err.println("*** " + ex.getMessage());
			System.exit(1);
		}

		cc.setRunnerCanvas(rc);

		javax.swing.SwingUtilities.invokeLater(new Runnable() {
			public void run() {
				rc.main();
			}
		});

		javax.swing.SwingUtilities.invokeLater(new Runnable() {
			public void run() {
				cc.main();
			}
		});
	}

	/**
	 * Load sprite image.
	 *
	 * @param blob File with sprite data.
	 * @param w Width of sprite (only if blob == null).
	 * @param h Height of sprite (only if blob == null).
	 * @param c Color of sprite (only if blob == null).
	 * @exception Exception.
	 */
	private static void loadSprite(String blob, int w, int h, int c) throws Exception {
		if (blob != null) {
			rc.spriteImage = ImageIO.read(new File(blob));
		} else {
			rc.spriteImage = createImage(w, h, c);
		}
	}

	/**
	 * Load background image.
	 *
	 * @param back File with background data.
	 * @param w Width of sprite (only if blob == null).
	 * @param h Height of sprite (only if blob == null).
	 * @param c Color of sprite (only if blob == null).
	 * @exception Exception.
	 */
	private static void loadBackground(String back, int w, int h, int c) throws Exception {
		BufferedImage img;

		if (back != null) {
			img = ImageIO.read(new File(back));
		} else {
			img = createImage(w, h, c);
		}

		rc.backImg = new BackgroundImage(img);
	}

	/**
	 * Create a solid image of w x h.
	 *
	 * @param w Width of image.
	 * @param h Height of image.
	 * @param c Color of image.
	 * @return BufferedImage.
	 */
	private static BufferedImage createImage(int w, int h, int c) {
		BufferedImage img = new BufferedImage(w, h, BufferedImage.TYPE_INT_RGB);
		Graphics2D g = img.createGraphics();
		Color col = new Color(c);

		g.setBackground(col);
		g.clearRect(0, 0, w, h);

		return img;
	}

	/**
	 * Allocate the sprites array.
	 *
	 * @param stride Gap between sprites.
	 */
	private static void allocSprites(int stride) {
		rc.sprites = new Sprite[rc.numberOfSprites];

		int pos = 0, i;

		for (i = 0; i < rc.numberOfSprites; i++) {
			rc.sprites[i] = new Sprite();
			rc.sprites[i].pos = pos;
			if (i == 0) {
				rc.sprites[0].hidden = false;
			} else {
				rc.sprites[i].hidden = true;
			}

			pos -= stride;
		}
	}
}
