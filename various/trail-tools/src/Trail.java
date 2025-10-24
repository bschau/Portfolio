package com.schau.TrailTools;

import java.awt.image.BufferedImage;
import java.io.File;
import javax.imageio.ImageIO;

/**
 * The trail engine.
 */
public class Trail {
	/**
	 * Width and height of image trail.
	 */
	public int width, height;
	
	/**
	 * X and Y Coodinates of marker.
	 */
	public int x = -1, y = -1;
	

	/**
	 * Number of bridges seen.
	 */
	public int bridges = 0;

	/**
	 * Are we on a bridge?
	 */
	public boolean inBridge = false;
	
	/**
	 * Did we come from a bridge.
	 */
	public boolean comeFromBridge=false;

	/** */
	private BufferedImage image;
	private int cW, cH;
	private int cBridge, cHidden, cProcessed, cTrail;


	/**
	 * Constructor.
	 *
	 * @param t Trail color. 
	 * @param b Bridge color.
	 * @param h Hidden color.
	 * @param p Processed color.
	 */
	public Trail(int t, int b, int h, int p) {
		cBridge = b;
		System.out.println("Bridge color: " + Integer.toHexString(cBridge));
		cHidden = h;
		System.out.println("Hidden color: " + Integer.toHexString(cHidden));
		cProcessed = p;
		System.out.println("Processed color: " + Integer.toHexString(cProcessed));
		cTrail = t;
		System.out.println("Trail color: " + Integer.toHexString(cTrail));
	}

	/**
	 * Load image with trail.
	 *
	 * @param fileName File to load.
	 * @exception Exception.
	 */
	public void load(String fileName) throws Exception {
		System.out.println("Loading image");

		image = ImageIO.read(new File(fileName));
		height = image.getHeight();
		cH = height - 1;
		width = image.getWidth();
		cW = width - 1;

		System.out.println("| Image dimensions: " + width + ", " + height);
	}

	/**
	 * Get the RGB value at x, y.
	 *
	 * @param x X Coordinate.
	 * @param y Y Coordinate.
	 * @return RGB value.
	 */
	public int getRGB(int x, int y) {
		return image.getRGB(x, y);
	}

	/**
	 * Find the start marker.
	 *
	 * @param cStart Color of start marker.
	 * @exception Exception.
	 */
	public void findStart(int cStart) throws Exception {
		System.out.println("Searching for start marker");

		int i, j;

		for (j = 0; j < height; j++) {
			for (i = 0; i < width; i++) {
				if ((image.getRGB(i, j) & 0xffffff) == cStart) {
					System.out.println("| Found start marker at (" + i + ", " + j + ")");
					x = i;
					y = j;
				}
			}
		}

		if ((x == -1) || (y == -1)) {
			throw new Exception("No start marker");
		}

		clear(x, y);
	}

	/**
	 * "Clear" the point at x, y. Clearing here means set to processed
	 * color.
	 *
	 * @param x X Coordinate.
	 * @param y Y Coordinate.
	 */
	public void clear(int x, int y) {
		image.setRGB(x, y, cProcessed);
	}

	/**
	 * Find the closest connected dot.
	 *
	 * @return true if a point was found, false otherwise (= end of trail).
	 * @exception Exception.
	 */
	public boolean findNext() throws Exception {
		if (inBridge) {
			if (tryBridge(x - 2, y - 2)) {
				return true;
			}

			if (tryBridge(x - 1, y - 2)) {
				return true;
			}

			if (tryBridge(x, y - 2)) {
				return true;
			}

			if (tryBridge(x + 1, y - 2)) {
				return true;
			}

			if (tryBridge(x + 2, y - 2)) {
				return true;
			}

			if (tryBridge(x - 2, y - 1)) {
				return true;
			}

			if (tryBridge(x + 2, y - 1)) {
				return true;
			}

			if (tryBridge(x - 2, y)) {
				return true;
			}

			if (tryBridge(x + 2, y)) {
				return true;
			}

			if (tryBridge(x - 2, y + 1)) {
				return true;
			}

			if (tryBridge(x + 2, y + 1)) {
				return true;
			}

			if (tryBridge(x - 2, y + 2)) {
				return true;
			}

			if (tryBridge(x - 1, y + 2)) {
				return true;
			}

			if (tryBridge(x, y + 2)) {
				return true;
			}

			if (tryBridge(x + 1, y + 2)) {
				return true;
			}

			if (tryBridge(x + 2, y + 2)) {
				return true;
			}

			// Cannot jump that far ..
			throw new Exception("No bridge connector found to " + CoordsConv.getString(x, y));

		}

		if (tryFind(x - 1, y - 1)) {
			return true;
		}

		if (tryFind(x, y - 1)) {
			return true;
		}

		if (tryFind(x + 1, y - 1)) {
			return true;
		}

		if (tryFind(x - 1, y)) {
			return true;
		}

		if (tryFind(x + 1, y)) {
			return true;
		}

		if (tryFind(x - 1, y + 1)) {
			return true;
		}

		if (tryFind(x, y + 1)) {
			return true;
		}

		if (tryFind(x + 1, y + 1)) {
			return true;
		}

		return false;
	}

	/**
	 * Find the nearest dot.
	 *
	 * @param xs X Coordinate.
	 * @param ys Y Coordinate.
	 * @return true if found, false otherwise (= end of trail).
	 */
	private boolean tryFind(int xs, int ys) {
		if ((xs < 0) || (xs > cW) || (ys < 0) || (ys > cH)) {
			return false;
		}

		int c = image.getRGB(xs, ys) & 0xffffff;
		if (c == cTrail) {
			x = xs;
			y = ys;
			clear(x, y);
			inBridge = false;
			comeFromBridge = false;
			return true;
		}

		if ((!comeFromBridge) && (c == cHidden)) {
			x = xs;
			y = ys;
			clear(x, y);
			inBridge = false;
			comeFromBridge = false;
			return true;
		}

		if (c == cBridge) {
			x = xs;
			y = ys;
			clear(x, y);
			inBridge = true;
			comeFromBridge = false;
			System.out.println("Bridge start at " + CoordsConv.getString(x, y));
			return true;
		}

		return false;
	}

	/**
	 * Find nearest dot, possible a bridge.
	 *
	 * @param xs X Coordinate.
	 * @param ys Y Coordinate.
	 * @return true if found, false otherwise (= end of trail).
	 * @exception Exception.
	 */
	private boolean tryBridge(int xs, int ys) throws Exception {
		if ((xs < 0) || (xs > cW) || (ys < 0) || (ys > cH)) {
			return false;
		}

		int c = image.getRGB(xs, ys) & 0xffffff;

		if (c == cBridge) {
			Emit.emit((x + xs) / 2, (y + ys) / 2);
			Emit.emit(xs, ys);

			System.out.println("Bridge end at " + CoordsConv.getString(xs, ys));
			x = xs;
			y = ys;
			clear(x, y);
			inBridge = false;
			comeFromBridge = true;
			bridges++;
			return true;
		}

		return false;
	}

	/**
	 * Write the processed canvas image.
	 *
	 * @param name File name.
	 * @exception Exception.
	 */
	public void writeCanvas(String name) throws Exception {
		System.out.println("Writing processed file image '" + name + "'");
		ImageIO.write(image, "png", new File(name));
	}

	/**
	 * Start trailing ...
	 *
	 * @param cStart Color of start dot.
	 * @exception Exception.
	 */
	public void trail(int cStart) throws Exception {
		boolean more = true;

		findStart(cStart);
		System.out.println("Trailing ...");

		while (more) {
			Emit.emit(x, y);
			more = findNext();
		}
	}
}
