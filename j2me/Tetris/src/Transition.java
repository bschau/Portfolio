import javax.microedition.lcdui.Graphics;
import javax.microedition.lcdui.Image;

public class Transition {
	private static final int FadeLength = 16;
	private static int[] srcImg;
	private static int[] rgb;
	private static int cW, cH, fadeLen;

	/**
	 * Init the transitions handler.
	 *
	 * @param w Screen width.
	 * @param h Screen height.
	 * @exception Exception.
	 */
	public static void init(int w, int h) throws Exception {
		cW = w;
		cH = h;

		rgb = new int[w * h];
		srcImg = new int[w * h];

		if ((rgb == null) || (srcImg == null)) {
			throw new Exception("Out of memory in Transition.init");
		}
	}

	/**
	 * Ready for fade (from White).
	 *
	 * @param src Fade to image.
	 */
	public static void setupFadeFromWhite(Image src) {
		int len = rgb.length, i;

		for (i = 0; i < len; i++) {
			rgb[i] = 0x00ffffff;
		}

		src.getRGB(srcImg, 0, cW, 0, 0, cW, cH);
		fadeLen = 0;
	}

	/**
	 * Run fade handler.
	 */
	public static void fadeFromWhite() {
		int len = srcImg.length, i, r1, r2, r3, r4, r5, r6, s, x;

		for (i = 0; i < len; i++) {
			x = rgb[i];
			s = srcImg[i];

			r1 = x & 0xf00000;
			if (r1 > (s & 0xf00000)) {
				r1 -= 0x100000;
			}

			r2 = x & 0x0f0000;
			if (r2 > (s & 0x0f0000)) {
				r2 -= 0x010000;
			}

			r3 = x & 0x00f000;
			if (r3 > (s & 0x00f000)) {
				r3 -= 0x001000;
			}

			r4 = x & 0x000f00;
			if (r4 > (s & 0x000f00)) {
				r4 -= 0x000100;
			}

			r5 = x & 0x0000f0;
			if (r5 > (s & 0x0000f0)) {
				r5 -= 0x000010;
			}

			r6 = x & 0x00000f;
			if (r6 > (s & 0x00000f)) {
				r6 -= 0x000001;
			}

			rgb[i] = r1 | r2 | r3 | r4 | r5 | r6;
		}
	}

	/**
	 * Ready for fade (to White).
	 *
	 * @param src Fade from image.
	 */
	public static void setupFadeToWhite(Image src) {
		src.getRGB(rgb, 0, cW, 0, 0, cW, cH);

		fadeLen = 0;
	}

	/**
	 * Fade handler.
	 */
	public static void fadeToWhite() {
		int len = srcImg.length, i, r1, r2, r3, r4, r5, r6, s, x;

		for (i = 0; i < len; i++) {
			x = rgb[i];

			r1 = x & 0xf00000;
			if (r1 < 0xf00000) {
				r1 += 0x100000;
			}

			r2 = x & 0x0f0000;
			if (r2 < 0x0f0000) {
				r2 += 0x010000;
			}

			r3 = x & 0x00f000;
			if (r3 < 0x00f000) {
				r3 += 0x001000;
			}

			r4 = x & 0x000f00;
			if (r4 < 0x000f00) {
				r4 += 0x000100;
			}

			r5 = x & 0x0000f0;
			if (r5 < 0x0000f0) {
				r5 += 0x000010;
			}

			r6 = x & 0x00000f;
			if (r6 < 0x00000f) {
				r6 += 0x000001;
			}

			rgb[i] = r1 | r2 | r3 | r4 | r5 | r6;
		}
	}

	/**
	 * Step fade ...
	 *
	 * @param scr Graphics.
	 * @return true if fade is completed, false otherwise.
	 */
	public static boolean step(Graphics scr) {
		boolean ret = false;

		fadeLen++;
		if (fadeLen == FadeLength) {
			ret = true;
		}

		scr.drawRGB(rgb, 0, cW, 0, 0, cW, cH, false);
		return ret;
	}
}
