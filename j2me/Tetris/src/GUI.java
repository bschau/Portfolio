import java.util.Random;

import javax.microedition.lcdui.Graphics;
import javax.microedition.lcdui.Image;

public class GUI {
	public static final int ANCHOR = Graphics.LEFT | Graphics.TOP;
	public static final int TileSize = 12;
	private static MainCanvas main;
	private static Random rnd = new Random();
	public static Image imgPieces;

	/**
	 * Initializes GUI.
	 *
	 * @param m MainCanvas.
	 * @exception Exception.
	 */
	public static void init(MainCanvas m) throws Exception {
		main = m;
		imgPieces = Image.createImage("/pieces.png");
	}

	/**
	 * Sets the color of this graphics to white.
	 *
	 * @param g Graphics.
	 */
	public static void clearScreen(Graphics g) {
		g.setColor(0xffffff);
		g.fillRect(0, 0, main.cW, main.cH);
	}

	/**
	 * Get pixel width of a block text.
	 *
	 * @param b Block text.
	 * @param cnt Number of elements pr. line.
	 * @return pixel width.
	 */
	public static int getBlockTextWidth(byte[][] b, int cnt) {
		int w = 0, idx;

		for (idx = 0; idx < cnt; idx++) {
			if (b[0][idx] == 64) {
				w += (TileSize / 2);
			} else {
				w += TileSize;
			}
		}

		return w;
	}

	/**
	 * Get Block trans.
	 *
	 * @return block trans.
	 */
	public static byte[] getBlkTrans() {
		byte[] blkTrans = new byte[8];

		for (int i = 0; i < blkTrans.length; i++) {
			blkTrans[i] = (byte)i;
		}

		return blkTrans;
	}

	/**
	 * Scramble the blkTrans table.
	 *
	 * @param blkTrans Block trans.
	 */
	public static void scrambleBlkTrans(byte[] blkTrans) {
		int len = blkTrans.length, v;
		byte b;

		for (int i = 1; i < len; i++) {
			v = rnd.nextInt() % (len - 1);
			if (v < 0) {
				v = -v;
			}
			v++;

			b = blkTrans[i];
			blkTrans[i] = blkTrans[v];
			blkTrans[v] = b;
		}
	}

	/**
	 * Draw the block text.
	 *
	 * @param g Graphics.
	 * @param b Block text.
	 * @param w Width of block text.
	 * @param h Height of block text.
	 * @param x X coordinate.
	 * @param y Y coordinate.
	 * @param blkTrans Block trans.
	 */
	public static void drawBlockText(Graphics g, byte[][] b, int w, int h, int x, int y, byte[] blkTrans) {
		int sX = x, j, i, t;

		for (j = 0; j < h; j++) {
			x = sX;
			for (i = 0; i < w; i++) {
				t = b[j][i];

				if (t == 64) {
					x += (TileSize / 2);
				} else if (t == 0) {
					x += TileSize;
				} else {
					g.drawRegion(imgPieces, blkTrans[t] * TileSize, 0, TileSize, TileSize, 0, x, y, ANCHOR);
					x += TileSize;
				}
			}
			y += TileSize;
		}
	}

	/**
	 * Render centered text.
	 *
	 * @param g Graphics.
	 * @param y Y coordinate.
	 * @param txt Text to render.
	 */
	public static void renderCenteredText(Graphics g, int y, String txt) {
		renderText(g, (main.cW - (FontEngine.FONTW * txt.length())) / 2, y, txt);
	}

	/**
	 * Render text.
	 *
	 * @param g Graphics.
	 * @param x X coordinate.
	 * @param y Y coordinate.
	 * @param txt Text to render.
	 */
	public static void renderText(Graphics g, int x, int y, String txt) {
		FontEngine.renderText(g, x, y, main.cW, main.cH, txt);
	}
}
