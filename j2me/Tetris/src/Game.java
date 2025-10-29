import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;

import java.util.Random;

import javax.microedition.lcdui.Canvas;
import javax.microedition.lcdui.Graphics;
import javax.microedition.lcdui.Image;

import javax.microedition.lcdui.game.GameCanvas;

import javax.microedition.rms.RecordStore;

public class Game {
	private static final String DBNAME = "State";
	private static final int RMFull = 1;
	private static final int RMPartial = 2;
	private static final String MLine1 = "Resume";
	private static final String MLine2 = "Exit Game";
	private static final int TextLeft = ((GUI.TileSize * 10) + 4) + 8;
	private static final int Spacing = FontEngine.FONTH * 3;
	private static final int YTile = FontEngine.FONTH + 4;
	private static final int YLevel = YTile + Spacing + (4 * GUI.TileSize) - FontEngine.FONTH;
	private static final int YScore = YLevel + Spacing;
	private static Random rnd = new Random();
	private static byte[] board = new byte[12*21];
	private static byte[] collLines = new byte[20];
	private static MainCanvas main;
	private static Image imgSide, imgBottom;
	private static boolean paused;
	private static long score;
	private static int level, lines, menuBar, redrawMode;
	private static int tileX, tileY, tileNext, tileCurr, tileSeq;
	private static int cW, cH;
	private static int fallCountdown;
	private static RecordStore rs = null;
	public static boolean canPersist = false, abortGame, gameOver;

	/**
	 * Initialize the Tetris game canvas.
	 *
	 * @param m MainCanvas.
	 */
	public static void init(MainCanvas m) throws Exception {
		main = m;
		cW = main.cW;
		cH = main.cH;
		rnd.setSeed(System.currentTimeMillis());
		imgSide = Image.createImage("/side.png");
		imgBottom = Image.createImage("/bottom.png");
	}

	/**
	 * Setup for a new game.
	 */
	public static void setup() {
		resetBoard();

		tileNext = getNextTile();
		newTile();

		redrawMode = RMFull;

		score = 0;
		lines = 0;
		level = 1;
		paused = false;
		abortGame = false;
		gameOver = false;
		fallCountdown = 1;
		canPersist = true;
	}

	/**
	 * Reset board.
	 */
	private static void resetBoard() {
		int i, j;

		for (i = 0; i < board.length; i++) {
			board[i] = 0;
		}

		j = 0;
		for (i = 0; i < 20; i++) {
			board[j] = 64;
			board[j + 11] = 64;
			j += 12;
		}

		j = 12 * 20;
		for (i = 0; i < 12; i++) {
			board[j++] = 64;
		}
	}

	/**
	 * Set default tile position.
	 */
	private static void setDefaultTilePosition() {
		tileX = 3;
		tileY = -1;
		tileSeq = 0;
	}

	/**
	 * Get next tile.
	 *
	 * @return next tile.
	 */
	private static int getNextTile() {
		int v = rnd.nextInt() % 7;

		if (v < 0) {
			v = -v;
		}

		return v + 1;
	}

	/**
	 * Create a new tile.
	 */
	private static boolean newTile() {
		tileCurr = tileNext;
		tileNext = getNextTile();
		setDefaultTilePosition();

		byte[] td = Tiles.getTileDefinition(tileCurr, tileSeq);
		int idx = 4, bIdx = tileX + 1, j, i;

		for (j = 0; j < 3; j++) {
			for (i = 0; i < 4; i++) {
				if (td[idx] > 0) {
					if (board[bIdx] != 0) {
						return false;
					}
				}

				idx++;
				bIdx++;
			}

			bIdx += 8;
		}
		
		return true;
	}

	/**
	 * Get X adjustment.
	 *
	 * @return adjustment.
	 */
	private static int getAdjustX() {
		return (cW - TextLeft - (6 * FontEngine.FONTW)) / 2;
	}

	/**
	 * Get Y adjustment.
	 *
	 * @return adjustment.
	 */
	private static int getAdjustY() {
		return (cH - (20 * GUI.TileSize) - 2) / 2;
	}

	/**
	 * Paint display.
	 *
	 * @param g Graphics object.
	 */
	public static void paintDisplay(Graphics g) {
		int aX = getAdjustX(), aY = getAdjustY(), yPos, xPos;

		if (redrawMode == RMFull) {
			GUI.clearScreen(g);

			yPos = aY;
			xPos = aX;

			g.drawImage(imgSide, xPos, yPos, GUI.ANCHOR);
			g.drawImage(imgSide, (GUI.TileSize * 10) + 2 + xPos, yPos, GUI.ANCHOR);

			yPos += (GUI.TileSize * 20);
			g.drawImage(imgBottom, xPos, yPos, GUI.ANCHOR);

			FontEngine.setColors(0xffffff, 0x882233);

			yPos = aY;
			xPos += TextLeft;

			FontEngine.renderText(g, xPos, yPos, cW, cH, "Next:");
			yPos += Spacing + (4 * GUI.TileSize);
			FontEngine.renderText(g, xPos, yPos, cW, cH, "Level:");
			yPos += Spacing;
			FontEngine.renderText(g, xPos, yPos, cW, cH, "Lines:");
			yPos += Spacing;
			FontEngine.renderText(g, xPos, yPos, cW, cH, "Score:");

			drawBoard(g);
			redrawMode = RMPartial;
		}

		FontEngine.setColors(0xffffff, 0xdd4455);
		yPos = YLevel + FontEngine.FONTH + aY;
		xPos = aX + TextLeft + 4;

		FontEngine.renderText(g, xPos, yPos, cW, cH, Integer.toString(level));
		yPos += Spacing;
		FontEngine.renderText(g, xPos, yPos, cW, cH, Integer.toString(lines));
		yPos += Spacing;
		FontEngine.renderText(g, xPos, yPos, cW, cH, Long.toString(score));

		g.setColor(0xffffff);
		yPos = aY + YTile;
		g.fillRect(xPos - 8, yPos, GUI.TileSize * 4, GUI.TileSize * 4);

		drawTile(g, tileNext, 0, xPos - 8, yPos, false);
		if (!gameOver) {
			drawTile(g, tileCurr, tileSeq, getTileX(), getTileY(), true);
		}

		if (paused) {
			int boxW = (MLine2.length() * FontEngine.FONTW) + 6;
			int boxH = (2 * FontEngine.FONTH) + 4;
			int x = (cW - boxW) / 2, y = (cH - boxH) / 2;

			g.setColor(0xffffff);
			g.fillRect(x, y, boxW, boxH);
			g.setColor(0x223388);
			g.drawRect(x, y, boxW, boxH);

			x += 3;
			y += 2;

			g.setColor(0xaaaaaa);
			g.fillRect(x, y + (menuBar * FontEngine.FONTH), FontEngine.FONTW * MLine2.length(), FontEngine.FONTH);

			if (menuBar == 0) {
				FontEngine.setColors(0xaaaaaa, 0x4455ee);
			} else {
				FontEngine.setColors(0xffffff, 0x4455ee);
			}
			FontEngine.renderText(g, x, y, cW, cH, MLine1);

			y += FontEngine.FONTH;
			if (menuBar == 1) {
				FontEngine.setColors(0xaaaaaa, 0x4455ee);
			} else {
				FontEngine.setColors(0xffffff, 0x4455ee);
			}
			FontEngine.renderText(g, x, y, cW, cH, MLine2);
		}
	}

	/**
	 * Draw final display (to offscreen buffer).
	 *
	 * @param g Graphics.
	 */
	public static void finalDisplay(Graphics g) {
		redrawMode = RMFull;
		paintDisplay(g);
	}

	/**
	 * Draws the entire board - a biggie.
	 *
	 * @param g Graphics.
	 */
	private static void drawBoard(Graphics g) {
		int bIdx = 1, y, x, j, i, t;

		y = getAdjustY();
		g.setColor(0xffffff);
		g.fillRect(getAdjustX() + 2, y, 10 * GUI.TileSize, 20 * GUI.TileSize);

		for (j = 0; j <20; j++) {
			x = getAdjustX() + 2;
			for (i = 0; i < 10; i++) {
				if (board[bIdx] > 0) {
					g.drawRegion(GUI.imgPieces, board[bIdx] * GUI.TileSize, 0, GUI.TileSize, GUI.TileSize, 0, x, y, GUI.ANCHOR);
				}

				bIdx++;
				x += GUI.TileSize;
			}
			bIdx += 2;
			y += GUI.TileSize;
		}
	}

	/**
	 * Return the left position of this tile.
	 *
	 * @return x position.
	 */
	private static int getTileX() {
		return getAdjustX() + 2 + (tileX * GUI.TileSize);
	}

	/**
	 * Return the top position of this tile.
	 *
	 * @return y position.
	 */
	private static int getTileY() {
		return getAdjustY() + (tileY == -1 ? 0 : tileY * GUI.TileSize);
	}

	/**
	 * Draw a tile on x, y.
	 *
	 * @param g Graphics.
	 * @param t Tile.
	 * @param a Animation.
	 * @param x X position.
	 * @param y Y position.
	 * @param adjust Adjust for tileY position?
	 */
	private static void drawTile(Graphics g, int t, int a, int x, int y, boolean adjust) {
		byte[] td = Tiles.getTileDefinition(t, a);
		int sX = x, maxY = 4, idx = 0, i, j;

		if ((adjust) && (tileY == -1)) {
			maxY = 3;
			idx = 4;
		}

		t *= GUI.TileSize;
		for (j = 0; j < maxY; j++) {
			x = sX;
			for (i = 0; i < 4; i++) {
				if (td[idx] > 0) {
					g.drawRegion(GUI.imgPieces, t, 0, GUI.TileSize, GUI.TileSize, 0, x, y, GUI.ANCHOR);
				}
				
				x += GUI.TileSize;
				idx++;
			}
			y += GUI.TileSize;
		}
	}

	/**
	 * Clear the current tile.
	 *
	 * @param g Graphics.
	 */
	private static void clearTile(Graphics g) {
		byte[] td = Tiles.getTileDefinition(tileCurr, tileSeq);
		int maxY = 4, idx = 0, sX = getTileX(), y = getTileY(), x, i, j;

		if (tileY == -1) {
			maxY = 3;
			idx = 4;
		}

		g.setColor(0xffffff);
		for (j = 0; j < maxY; j++) {
			x = sX;
			for (i = 0; i < 4; i++) {
				if (td[idx++] > 0) {
					g.fillRect(x, y, GUI.TileSize, GUI.TileSize);
				}

				x += GUI.TileSize;
			}

			y += GUI.TileSize;
		}
	}

	/**
	 * Try to move/rotate the current tile to these new dimensions.
	 *
	 * @param x New X position.
	 * @param y New Y position.
	 * @param s New sequence.
	 * @return true if piece can move/rotate, false otherwise.
	 */
	private static boolean tryMove(int x, int y, int s) {
		byte[] b = Tiles.getTileDefinition(tileCurr, s);
		int pH = 4, idx = 0, bIdx = (y * 12) + x + 1;

		if (tileY == -1) {
			pH = 3;
			idx = 4;
			bIdx += 12;
		}

		for (y = 0; y < pH; y++) {
			for (x = 0; x < 4; x++) {
				if (b[idx] > 0) {
					if (board[bIdx] != 0) {
						return false;
					}
				}
				idx++;
				bIdx++;
			}

			bIdx += 8;
		}

		return true;
	}

	/**
	 * Move the tile down.
	 *
	 * @param g Graphics.
	 */
	private static void moveTileDown(Graphics g) {
		if (fallCountdown > 0) {
			fallCountdown--;
		} else {
			fallCountdown = getFallCountdown();
			if (tryMove(tileX, tileY + 1, tileSeq)) {
				tileY++;
			} else {
				finalizeTile(g);
			}
		}
	}

	/**
	 * Get next countdown tick.
	 *
	 * @return tick.
	 */
	private static int getFallCountdown() {
		return 10 - level;
	}

	/**
	 * Finalize tile in this position.
	 *
	 * @param g Graphics.
	 */
	private static void finalizeTile(Graphics g) {
		byte[] pd = Tiles.getTileDefinition(tileCurr, tileSeq);
		int idx = 0, hY = 4;
		int bIdx = (tileY * 12) + tileX + 1, i, j, t;
	
		if (tileY == -1) {
			hY = 3;
			idx = 4;
			bIdx += 12;
		}

		for (j = 0; j < hY; j++) {
			for (i = 0; i < 4; i++) {
				if (pd[idx] > 0) {
					board[bIdx] = (byte)tileCurr;
				}

				idx++;
				bIdx++;
			}

			bIdx += 8;
		}

		score += (long)level;
		collapse();

		drawBoard(g);

		i = tileCurr;
		j = tileNext;

		if (!newTile()) {
			tileCurr = i;
			tileNext = j;
			gameOver = true;
		}
	}

	/**
	 * Collapse construction and add to score.
	 */
	private static void collapse() {
		int addToScore = 0, pt = 1, sIdx = 1, y, x, idx, dIdx;

		for (y = 0; y < 20; y++) {
			collLines[y] = 1;
		}

		for (y = 0; y < 20; y++) {
			idx = sIdx;
			for (x = 0; x < 10; x++) {
				if (board[idx++] == 0) {
					collLines[y] = 0;
					break;
				}
			}

			if (collLines[y] > 0) {
				addToScore += pt;
				pt += pt;
			}

			sIdx += 12;
		}

		if (addToScore > 0) {
			score += (long)(addToScore * level);
		}

		sIdx = -1;
		for (y = 0; y < 20; y++) {
			if (collLines[y] == 1) {
				sIdx = y;
				break;
			}
		}

		if (sIdx == -1) {
			return;
		}

		dIdx = -1;
		for (y = 19; y >= sIdx; y--) {
			if (collLines[y] == 1) {
				dIdx = y;
				break;
			}
		}

		lines += ((dIdx - sIdx) + 1);

		sIdx --;
		y = sIdx;

		sIdx *= 12;
		sIdx++;

		dIdx *= 12;
		dIdx++;
		for (; y > -1; y--) {
			for (x = 0; x < 10; x++) {
				board[dIdx++] = board[sIdx++];
			}

			dIdx -= 22;
			sIdx -= 22;
		}

		while (dIdx >= 1) {
			for (x = 0; x < 10; x++) {
				board[dIdx++] = 0;
			}

			dIdx -= 22;
		}
	}

	/**
	 * Handle keypresses.
	 */
	private static void handleKeys(Graphics g) {
		if (paused) {
			if (main.keyUp) {
				if (menuBar > 0) {
					menuBar--;
				}
			}

			if (main.keyDown) {
				if (menuBar < 1) {
					menuBar++;
				}
			}

			if (main.keyFire) {
				if (menuBar == 0) {
					redrawMode = RMFull;
					paused = false;
				} else {
					abortGame = true;
				}
			}
		} else {
			if (main.keyLeft) {
				if (tryMove(tileX - 1, tileY, tileSeq)) {
					tileX--;
				}
			}

			if (main.keyRight) {
				if (tryMove(tileX + 1, tileY, tileSeq)) {
					tileX++;
				}
			}

			if (main.keyUp) {
				int seq = Tiles.rotateTile(tileCurr, tileSeq);

				if (tryMove(tileX, tileY, seq)) {
					tileSeq = seq;
				}
			}

			if (main.keyDown) {
				dropTile(g);
			}

			if (main.keyFire) {
				menuBar = 0;
				paused = true;
			}
		}

		main.clearKeyStates();
	}

	/**
	 * Drop piece.
	 */
	private static void dropTile(Graphics g) {
		int i;

		for (i = 0; i < 20; i++) {
			if (!tryMove(tileX, tileY + 1, tileSeq)) {
				break;
			}

			tileY++;
		}

		finalizeTile(g);
	}

	/**
	 * Execute one game frame.
	 */
	public static void execute(Graphics g) {
		if (!paused) {
			clearTile(g);
		}

		handleKeys(g);
		if (!paused) {
			moveTileDown(g);
		}

		level = lines / 10;
		if (level > 10) {
			level = 10;
		} else if (level < 1) {
			level = 1;
		}

		paintDisplay(g);
	}

	/**
	 * Get current level.
	 *
	 * @return current level.
	 */
	public static int getLevel() {
		return level;
	}

	/**
	 * Get score.
	 *
	 * @return score.
	 */
	public static long getScore() {
		return score;
	}

	/**
	 * Persist the game state.
	 */
	public static void persist() {
		if (rs != null) {
			ByteArrayOutputStream baos = null;
			DataOutputStream dos = null;

			try {
				baos = new ByteArrayOutputStream();
				dos = new DataOutputStream(baos);

				if (canPersist) {
					int i;

					dos.writeInt(1);	// DB Version

					dos.writeInt(tileNext);
					dos.writeInt(tileCurr);
					dos.writeInt(tileSeq);
					dos.writeInt(tileX);
					dos.writeInt(tileY);
					dos.writeInt(level);
					dos.writeInt(lines);
					dos.writeLong(score);
					for (i = 0; i < board.length; i++) {
						dos.writeByte(board[i]);
					}

					for (i = 0; i < collLines.length; i++) {
						dos.writeByte(collLines[i]);
					}
				} else {
					dos.writeInt(0);
				}

				byte[] data = baos.toByteArray();

				if (rs.getNumRecords() > 0) {
					rs.setRecord(1, data, 0, data.length);
				} else {
					rs.addRecord(data, 0, data.length);
				}
			} catch (Exception ex) {
			} finally {
				if (rs != null) {
					try {
						rs.closeRecordStore();
					} catch (Exception ex) {}
				}
			}
		}
	}

	/**
	 * Loaded persisted state.
	 *
	 * @return true if loaded, false otherwise.
	 */
	public static boolean load() {
		try {
			rs = RecordStore.openRecordStore(DBNAME, true);

			if (rs.getNumRecords() > 0) {
				byte[] b = rs.getRecord(1);
				ByteArrayInputStream bais = new ByteArrayInputStream(b);
				DataInputStream dis = new DataInputStream(bais);

				int i = dis.readInt();
				if (i > 0) {
					tileNext = dis.readInt();
					tileCurr = dis.readInt();
					tileSeq = dis.readInt();
					tileX = dis.readInt();
					tileY = dis.readInt();
					level = dis.readInt();
					lines = dis.readInt();
					score = dis.readLong();
					for (i = 0; i < board.length; i++) {
						board[i] = dis.readByte();
					}

					for (i = 0; i < collLines.length; i++) {
						collLines[i] = dis.readByte();
					}

					redrawMode = RMFull;
					paused = false;
					abortGame = false;
					gameOver = false;
					fallCountdown = 1;
					canPersist = true;
					return true;
				}
			}
		} catch (Exception ex) {}

		return false;
	}
}
