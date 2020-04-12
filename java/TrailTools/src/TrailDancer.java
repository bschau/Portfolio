package com.schau.TrailTools;

import java.awt.Color;
import java.awt.Graphics2D;
import java.awt.image.BufferedImage;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;

import java.util.StringTokenizer;

import javax.imageio.ImageIO;

/**
 * TrailDancer.
 */
public class TrailDancer {
	private static TDRunnerCanvas rc=new TDRunnerCanvas();

	/**
	 * Main entry.
	 */
	public static void main(String[] args) {
		Options opt = new Options();
		String spriteBlob = null, backgroundImage = null;
		int dispW = 320, dispH = 480, dispC = 0xffffff;
		int sprW = 16, sprH = 16, sprC = 0x0000ff;
		TDVars v = new TDVars();

		try {
			String[] arr;
			String s;
			
			// Sprite Blob -B
			// Display info -D (x,y,c)
			// Sprite info -G (x,y,c)
			// Background picture -P
			// Anim delay -d
			opt.Parse(args, "B:D:G:P:d:");
			if ((s = opt.Get("B")) != null) {
				spriteBlob = s;
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

			if ((s = opt.Get("d")) != null) {
				v.animationDelay = Integer.parseInt(s);
			}

		} catch (Exception ex) {
			System.err.println("*** " + ex.getMessage());
			System.exit(1);
		}

		try {
			if (opt.GetRestArgsCount() != 1) {
				throw new Exception("Version 1.0\nUsage: TrailDancer [OPTIONS] script");
			}

			loadSprite(spriteBlob, sprW, sprH, sprC);
			loadBackground(backgroundImage, dispW, dispH, dispC);

			loadScript(opt.GetRestArg(0), v);

		} catch (Exception ex) {
			System.err.println("*** " + ex.getMessage());
			System.exit(1);
		}

		rc.setVars(v);
		rc.cos = calcCos(v);
		rc.sin = calcSin(v);

		javax.swing.SwingUtilities.invokeLater(new Runnable() {
			public void run() {
				rc.main();
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
	 * Load the parameter script.
	 *
	 * @param script Script file to load.
	 * @param v Vars.
	 */
	private static void loadScript(String script, TDVars v) throws Exception {
		BufferedReader in = new BufferedReader(new FileReader(script));
		StringTokenizer st;
		int line = 0, tokCnt;
		String s, token;

		while ((s = in.readLine()) != null) {
			line++;

			s = s.trim();
			if ((s.length() == 0) || (s.startsWith("#"))) {
				continue;
			}

			st = new StringTokenizer(s);
			tokCnt = st.countTokens() - 1;
			token = st.nextToken();
			if (token.equals("coscurve")) {
				if (tokCnt != 4) {
					throw new Exception("coscurve on line " + line + " needs 4 arguments: steps amplitude start-angle end-angle");
				}

				v.cosSteps = Integer.parseInt(st.nextToken());
				v.cosAmplitude = Integer.parseInt(st.nextToken());
				v.cosStart = Integer.parseInt(st.nextToken());
				v.cosEnd = Integer.parseInt(st.nextToken());
			} else if (token.equals("sincurve")) {
				if (tokCnt != 4) {
					throw new Exception("sincurve on line " + line + " needs 4 arguments: steps amplitude start-angle end-angle");
				}

				v.sinSteps = Integer.parseInt(st.nextToken());
				v.sinAmplitude = Integer.parseInt(st.nextToken());
				v.sinStart = Integer.parseInt(st.nextToken());
				v.sinEnd = Integer.parseInt(st.nextToken());
			} else if (token.equals("xset")) {
				if (tokCnt != 4) {
					throw new Exception("xset on line " + line + " needs 4 arguments: startX1 speedX1 startX2 speedX2");
				}

				v.xStart1 = Integer.parseInt(st.nextToken());
				v.xSpeed1 = Integer.parseInt(st.nextToken());
				v.xStart2 = Integer.parseInt(st.nextToken());
				v.xSpeed2 = Integer.parseInt(st.nextToken());
			} else if (token.equals("yset")) {
				if (tokCnt != 4) {
					throw new Exception("yset on line " + line + " needs 4 arguments: startY1 speedY1 startY2 speedY2");
				}

				v.yStart1 = Integer.parseInt(st.nextToken());
				v.ySpeed1 = Integer.parseInt(st.nextToken());
				v.yStart2 = Integer.parseInt(st.nextToken());
				v.ySpeed2 = Integer.parseInt(st.nextToken());
			} else if (token.equals("sprites")) {
				if (tokCnt != 5) {
					throw new Exception("sprites on line " + line + " needs 3 arguments: number-of-sprites strideX1 strideX2 strideY1 strideY2");
				}

				v.sprNumber = Integer.parseInt(st.nextToken());
				v.sprStrideX1 = Integer.parseInt(st.nextToken());
				v.sprStrideX2 = Integer.parseInt(st.nextToken());
				v.sprStrideY1 = Integer.parseInt(st.nextToken());
				v.sprStrideY2 = Integer.parseInt(st.nextToken());
			} else {
				throw new Exception("Unknown token '" + token +"' on line " + line);
			}
		}
	}

	/**
	 * Calculate cosine curve.
	 *
	 * @param v TDVars.
	 * @return curve.
	 */
	private static short[] calcCos(TDVars v) {
		short[] curve = new short[v.cosSteps];
		double angle = (double)v.cosStart;
		double inc = ((double)v.cosEnd - (double)v.cosStart) / (double)v.cosSteps;
		double rad = Math.PI / 180;
		int i;

		for (i = 0; i < v.cosSteps; i++) {
			curve[i] = (short)(Math.cos(angle * rad) * v.cosAmplitude);
			angle += inc;
		}

		return curve;
	}

	/**
	 * Calculate sine curve.
	 *
	 * @param v TDVars.
	 * @return curve.
	 */
	private static short[] calcSin(TDVars v) {
		short[] curve = new short[v.sinSteps];
		double angle = (double)v.sinStart;
		double inc = ((double)v.sinEnd - (double)v.sinStart) / (double)v.sinSteps;
		double rad = Math.PI / 180;
		int i;

		for (i = 0; i < v.sinSteps; i++) {
			curve[i] = (short)(Math.sin(angle * rad) * v.sinAmplitude);
			angle += inc;
		}

		return curve;
	}
}
