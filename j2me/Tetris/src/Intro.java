import javax.microedition.lcdui.Graphics;
import javax.microedition.lcdui.Image;

public class Intro {
	private static final String SBy = "By Brian Schau";
	private static final String SUrl = "http://www.schau.com";
	private static final String SFire = "Press Fire to play!";
	private static final String SHighScores = "High Scores";

	private static MainCanvas main;
	private static byte[] blkTrans;
	private static byte[][] logo = {
		{ 1, 1, 1, 64, 2, 2, 2, 64, 3, 3, 3, 64, 4, 4, 0, 64, 5, 64, 0, 7, 7 },
		{ 0, 1, 0, 64, 2, 0, 0, 64, 0, 3, 0, 64, 4, 0, 4, 64, 5, 64, 7, 0, 0 },
		{ 0, 1, 0, 64, 2, 2, 0, 64, 0, 3, 0, 64, 4, 4, 0, 64, 5, 64, 0, 7, 0 },
		{ 0, 1, 0, 64, 2, 0, 0, 64, 0, 3, 0, 64, 4, 0, 4, 64, 5, 64, 0, 0, 7 },
		{ 0, 1, 0, 64, 2, 2, 2, 64, 0, 3, 0, 64, 4, 0, 4, 64, 5, 64, 7, 7, 0 } };

	/**
	 * Init Intro.
	 *
	 * @param m MainCanvas.
	 */
	public static void init(MainCanvas m) {
		main = m;
		blkTrans = GUI.getBlkTrans();
	}

	/**
	 * Prepare Intro screen.
	 *
	 * @param g Graphics.
	 * @param canvas Canvas to receive screen.
	 */
	public static void prepare(Graphics g, Image canvas) {
		GUI.clearScreen(g);
		int w = GUI.getBlockTextWidth(logo, 21), x;
		int yPos = (main.cH - (6 * GUI.TileSize) - (12 * FontEngine.FONTH) - 5) / 2;

		GUI.scrambleBlkTrans(blkTrans);
		GUI.drawBlockText(g, logo, 21, 5, (main.cW - w) / 2, yPos, blkTrans);

		yPos += 6 * GUI.TileSize;

		FontEngine.setColors(0xffffff, 0x4455ee);
		GUI.renderCenteredText(g, yPos, SBy);

		yPos += 2 * FontEngine.FONTH;
		FontEngine.setColors(0xffffff, 0x228833);
		GUI.renderCenteredText(g, yPos, SHighScores);
		g.setColor(0x228833);
		yPos += FontEngine.FONTH + 2;
		w = SHighScores.length() * FontEngine.FONTW;
		x = (main.cW - w) / 2;
		g.drawLine(x, yPos, x + w - 1, yPos);
		yPos += 3;

		x = (main.cW / 2) + 6;
		int xR = x - 12;

		FontEngine.setColors(0xffffff, 0x44ee55);
		GUI.renderText(g, x, yPos, Leaderboard.names[0]);
		renderScore(g, xR, yPos, Leaderboard.scores[0]);

		FontEngine.setColors(0xffffff, 0x228833);
		for (int i = 1; i < Leaderboard.SIZE; i++) {
			yPos += FontEngine.FONTH;
			GUI.renderText(g, x, yPos, Leaderboard.names[i]);
			renderScore(g, xR, yPos, Leaderboard.scores[i]);
		}

		yPos += 2 * FontEngine.FONTH;
		FontEngine.setColors(0xffffff, 0x4455ee);
		GUI.renderCenteredText(g, yPos, SUrl);

		yPos += 2 * FontEngine.FONTH;
		FontEngine.setColors(0xffffff, 0xee5544);
		GUI.renderCenteredText(g, yPos, SFire);

		Transition.setupFadeFromWhite(canvas);
	}

	/**
	 * Render a score.
	 *
	 * @param g Graphics.
	 * @param xR Right X coordinate.
	 * @param yPos Y Coordinate.
	 * @param score Score.
	 */
	private static void renderScore(Graphics g, int xR, int yPos, long score) {
		String s = Long.toString(score);

		GUI.renderText(g, xR - (s.length() * FontEngine.FONTW), yPos, s);
	}
}
