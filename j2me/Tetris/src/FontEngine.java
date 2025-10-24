import javax.microedition.lcdui.Graphics;
import javax.microedition.lcdui.Image;
import javax.microedition.lcdui.game.Sprite;

public class FontEngine {
	public static final int FONTW = 8;
	public static final int FONTH = 16;
	private static final int CHARS = 96;
	private static final int SCANLINE = (CHARS * FONTW);
	private static final int TABW = FONTW * 4;
	private static int[] fontData;
	private static int[] rgb;

	/**
	 * Initialize the Font Engine.
	 *
	 * @exception Exception.
	 */
	public static void init() throws Exception {
		try {
			Image img = Image.createImage("/font.png");
			int i, d;

			fontData = new int[SCANLINE * FONTH];
			rgb = new int[SCANLINE * FONTH];
			img.getRGB(fontData, 0, SCANLINE, 0, 0, SCANLINE, FONTH);
			for (i = 0; i < (SCANLINE * FONTH); i++) {
				d = fontData[i] & 0xffffff;
				if (d > 0) {
					d = 0xffffff;
				}
				fontData[i] = d;
			}
			img = null;
		} catch (Exception e) {
			throw new Exception("Failed to open game font.");
		}
	}

	/**
	 * Set font colors.
	 *
	 * @param back Background color.
	 * @param fore Foreground color.
	 */
	public static void setColors(int back, int fore) {
		for (int i = 0; i < (SCANLINE * FONTH); i++) {
			if (fontData[i] == 0) {
				rgb[i] = fore;
			} else {
				rgb[i] = back;
			}
		}
	}

	/**
	 * Render text into image.
	 *
	 * @param dst Destination image.
	 * @param x X coordinate.
	 * @param y Y coordinate.
	 * @param txt Text to render.
	 */
	public static void renderText(Image dst, int x, int y, String txt) {
		renderText(dst.getGraphics(), x, y, dst.getWidth(), dst.getHeight(), txt);
	}

	/**
	 * Render text into Graphics.
	 *
	 * @param g Graphics.
	 * @param x X coordinate.
	 * @param y Y coordinate.
	 * @param w Width of Graphics.
	 * @param h Height of Graphics.
	 * @param txt Text to render.
	 */
	public static void renderText(Graphics g, int x, int y, int w, int h, String txt) {
		int txtLen = txt.length(), i, j;
		char c;

		w -= FONTW;
		h -= FONTH;
		for (i = 0; i < txtLen; i++) {
			if (x > w) {
				y += FONTH;
				x = 0;
			}

			if (y > h) {
				return;
			}

			c = txt.charAt(i);
			switch (c) {
				case '\r':
					x = 0;
					break;
				case '\n':
					y += FONTH;
					break;
				case '\t':
					x += TABW;
					break;
				default:
					j = ((int)c) - (int)' ';
					g.drawRGB(rgb, j * FONTW, SCANLINE, x, y, FONTW, FONTH, false);
					x += FONTW;
					break;
			}
		}
	}

	/**
	 * Render char into Graphics.
	 *
	 * @param g Graphics.
	 * @param x X coordinate.
	 * @param y Y coordinate.
	 * @param c Character.
	 */
	public static void renderChar(Graphics g, int x, int y, char c) {
		g.drawRGB(rgb, (((int)c) - (int)' ') * FONTW, SCANLINE, x, y, FONTW, FONTH, false);
	}

	/**
	 * Find centered leftmost position of a set of strings.
	 *
	 * @param items Strings to measure.
	 * @param w Width of bounding box.
	 * @return position.
	 */
	public static int findLeftPosition(String[] items, int w) {
		int len = 0, l, i;

		for (i = 0; i < items.length; i++) {
			l = items[i].length();
			if (l > len) {
				len = l;
			}
		}

		return (w - (len * FONTW)) / 2;
	}

	/**
	 * Find centered leftmost position of string.
	 *
	 * @param item String to measure.
	 * @param w Width of bounding box.
	 * @return position.
	 */
	public static int findLeftPosition(String item, int w) {
		return ((w - (item.length() * FONTW)) / 2);
	}
}
