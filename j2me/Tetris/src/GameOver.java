import javax.microedition.lcdui.Graphics;
import javax.microedition.lcdui.Image;

public class GameOver {
	private static MainCanvas main;
	private static byte[][] game = {
		{ 0, 1, 1, 64, 0, 4, 0, 64, 5, 0, 5, 64, 7, 7, 7 },
		{ 1, 0, 0, 64, 4, 0, 4, 64, 5, 5, 5, 64, 7, 0 ,0 },
		{ 1, 0, 1, 64, 4, 4, 4, 64, 5, 0, 5, 64, 7, 7, 0 },
		{ 1, 0, 1, 64, 4, 0, 4, 64, 5, 0, 5, 64, 7, 0, 0 },
		{ 0, 1, 1, 64, 4, 0, 4, 64, 5, 0, 5, 64, 7, 7, 7 } };

	private static byte[][] over = {
		{ 0, 7, 0, 64, 5, 0, 5, 64, 4, 4, 4, 64, 1, 1, 0 },
		{ 7, 0, 7, 64, 5, 0, 5, 64, 4, 0, 0, 64, 1, 0, 1 },
		{ 7, 0, 7, 64, 5, 0, 5, 64, 4, 4, 0, 64, 1, 1, 0 },
		{ 7, 0, 7, 64, 5, 0, 5, 64, 4, 0, 0, 64, 1, 0, 1 },
		{ 0, 7, 0, 64, 0, 5, 0, 64, 4, 4, 4, 64, 1, 0, 1 } };

	private static String SYouScored = "You scored: ";
	private static String SPressFire = "Press Fire to continue";
	private static String STheBest = "You're the best!";
	private static String SAmongTheBest = "You're one of the best ...";
	private static String NAMES = ".abcdefghijklmnopqrstuvwxyz";
	private static StringBuffer sBuf = new StringBuffer();
	private static char[] saveName = new char[3];
	private static char[] name = new char[3];
	private static int position, lIdx, nIdx, namePos;
	private static boolean showCursor;
	private static long score;
	private static byte[] blkTrans1;
	private static byte[] blkTrans2;

	/**
	 * Initialize Game Over screen.
	 *
	 * @param m MainCanvas.
	 */
	public static void init(MainCanvas m) {
		main = m;
		saveName[0] = '.';
		saveName[1] = '.';
		saveName[2] = '.';
		blkTrans1 = GUI.getBlkTrans();
		blkTrans2 = GUI.getBlkTrans();
	}

	/**
	 * Prepare the Game Over screen.
	 *
	 * @param g Graphics.
	 * @param canvas Canvas to receive screen.
	 * @param s Score.
	 */
	public static void prepare(Graphics g, Image canvas, long s) {
		score = s;
		position = Leaderboard.qualifyPosition(score);
		GUI.scrambleBlkTrans(blkTrans1);
		GUI.scrambleBlkTrans(blkTrans2);
		draw(g);
		Transition.setupFadeFromWhite(canvas);
	}

	/**
	 * Draw High Score screen.
	 *
	 * @param g Graphics.
	 */
	public static void draw(Graphics g) {
		int w = (main.cW - GUI.getBlockTextWidth(game, 15)) / 2, dispH = (11 * GUI.TileSize) + (4 * FontEngine.FONTH);
		int yPos = (main.cH - dispH) / 2;

		GUI.clearScreen(g);
		GUI.drawBlockText(g, game, 15, 5, w, yPos, blkTrans1);
		yPos += (6 * GUI.TileSize);
		GUI.drawBlockText(g, over, 15, 5, w, yPos, blkTrans2);

		yPos += (5 * GUI.TileSize) + FontEngine.FONTH;

		if (position > -1) {
			FontEngine.setColors(0xffffff, 0x4455ee);
			GUI.renderCenteredText(g, yPos, (position == 0) ? STheBest : SAmongTheBest);

			yPos += 2 * FontEngine.FONTH;
			namePos = yPos;

			FontEngine.setColors(0xffffff, 0x223388);
			String s = Long.toString(score);

			GUI.renderText(g, (main.cW / 2) - 6 - (s.length() * FontEngine.FONTW), yPos, s);
			nIdx = 0;
			name[0] = saveName[0];
			name[1] = saveName[1];
			name[2] = saveName[2];

			pickupLetterIndex(name[0]);

			showCursor = false;
			outputName(g);
		} else {
			FontEngine.setColors(0xffffff, 0x223388);
			GUI.renderCenteredText(g, yPos, SYouScored + Long.toString(score));

			yPos += 2 * FontEngine.FONTH;

			FontEngine.setColors(0xffffff, 0xee5544);
			GUI.renderCenteredText(g, yPos, SPressFire);
		}
	}

	/**
	 * Run the High Score screen.
	 *
	 * @param g Graphics.
	 */
	public static boolean execute(Graphics g) {
		if (position > -1) {
			boolean ret = false;

			showCursor = !showCursor;
			if (main.keyUp) {
				lIdx++;
				if (lIdx >= NAMES.length()) {
					lIdx = 0;
				}
			} else if (main.keyDown) {
				lIdx--;
				if (lIdx < 0) {
					lIdx = NAMES.length() - 1;
				}
			}

			name[nIdx] = NAMES.charAt(lIdx);

			if (main.keyFire) {
				nIdx++;
				if (nIdx > 2) {
					saveName[0] = name[0];
					saveName[1] = name[1];
					saveName[2] = name[2];

					sBuf.delete(0, sBuf.length());
					sBuf.append(name);
					Leaderboard.setName(position, sBuf.toString());
					ret = true;
				} else {
					pickupLetterIndex(name[nIdx]);
				}
			}

			outputName(g);
			return ret;
		}

		if (main.keyFire) {
			return true;
		}

		return false;
	}

	/**
	 * Draw the name.
	 *
	 * @param g Graphics.
	 */
	private static void outputName(Graphics g) {
		int x = (main.cW / 2) + 6, w = FontEngine.FONTW + (FontEngine.FONTW / 2), i;

		g.setColor(0xffffff);
		g.fillRect(x - 2 , namePos - 1, (w * 3) + 4, FontEngine.FONTH + 4);

		FontEngine.setColors(0xffffff, 0x223388);
		for (i = 0; i < 3; i++) {
			FontEngine.renderChar(g, x, namePos, name[i]);
			x += w;
		}

		if (showCursor) {
			g.setColor(0x112266);
			g.drawRoundRect(((main.cW / 2) + 6) + (nIdx * w) - 2, namePos - 1, FontEngine.FONTW + 2, FontEngine.FONTH + 2, 2, 2);
		}
	}

	/**
	 * Get index of letter.
	 *
	 * @param c Letter.
	 */
	private static void pickupLetterIndex(char c) {
		for (int i = 0; i < NAMES.length(); i++) {
			if (NAMES.charAt(i) == c) {
				lIdx = i;
				return;
			}
		}

		lIdx = 0;
	}
}
