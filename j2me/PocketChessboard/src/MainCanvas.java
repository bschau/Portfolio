import java.lang.Thread;

import java.util.Vector;

import javax.microedition.lcdui.game.GameCanvas;
import javax.microedition.lcdui.Canvas;
import javax.microedition.lcdui.Command;
import javax.microedition.lcdui.CommandListener;
import javax.microedition.lcdui.Displayable;
import javax.microedition.lcdui.Font;
import javax.microedition.lcdui.Graphics;
import javax.microedition.lcdui.Image;
import javax.microedition.lcdui.List;

public class MainCanvas extends GameCanvas implements CommandListener {
	private final int GOTOMARK = 1;
	private final int RESETBOARD = 2;
	private final int PieceStartX = 16;
	private final int PieceStartY = 170;
	private final int PieceW = 24;
	private final int PieceH = 24;
	private final int MoveMarkerW = 12;
	private final int MoveMarkerH = 12;
	public Vector marks = new Vector();
	private Font defFont = Font.getDefaultFont();
	private boolean editMode = false;
	private boolean pieceSelected = false;
	private int goToMark = -1;
	private PocketChessboardMIDlet mid;
	private Image imgBoard, imgPieces, imgRcursor, imgGcursor, imgBcursor;
	private int csrX, csrY, srcX, srcY;
	private Thread animThread;
	private Board saveBoard;
	public Board board;

	/**
	 * Constructor.
	 */
	public MainCanvas(boolean suppress) {
		super(suppress);
	}

	/**
	 * Initialize the PocketChessboard main canvas.
	 *
	 * @param m PocketChessboardMIDlet
	 * @exception Exception.
	 */
	public void init(PocketChessboardMIDlet m) throws Exception {
		mid = m;

		imgBoard = Image.createImage("/chessboard.png");
		imgPieces = Image.createImage("/pieces.png");
		imgBcursor = Image.createImage("/bcursor.png");
		imgGcursor = Image.createImage("/gcursor.png");
		imgRcursor = Image.createImage("/rcursor.png");

		resetCursor();

		editMode = false;

		this.addCommand(GUI.cmdOptions);
		this.setCommandListener(this);
	}

	/**
	 * Force repaint.
	 */
	public void forceRepaint() {
		repaint();
		serviceRepaints();
	}

	/**
	 * Set this board as a default board.
	 */
	public void setDefaultBoard() {
		board = new Board();

		Board brd = new Board(board);
		marks.removeAllElements();
		marks.addElement(brd);
	}

	/**
	 * Paint display.
	 *
	 * @param g Graphics object.
	 */
	public void paint(Graphics g) {
		int cW = getWidth(), cH = getHeight();

		g.setClip(0, 0, cW, cH);

		g.setColor(0xffffff);
		g.fillRect(0, 0, cW, cH);

		g.setFont(defFont);
		g.drawImage(imgBoard, 0, 0, GUI.ANCHOR);

		int yPos = PieceStartY, xPos, p, x, y;

		for (y = 0; y < 8; y++) {
			xPos = PieceStartX;
			for (x = 0; x < 8; x++) {
				p = (int)board.getPiece(x, y);
				if (p > 0) {
					p--;
					g.drawRegion(imgPieces, p * PieceW, 0, PieceW, PieceH, 0, xPos, yPos, GUI.ANCHOR);
				}

				xPos += PieceW;
			}
			yPos -= PieceH;
		}
	
		if (editMode) {
			g.drawImage((pieceSelected ? imgGcursor : imgRcursor), (csrX * PieceW) + PieceStartX, PieceStartY - (csrY * PieceH), GUI.ANCHOR);
		} else {
			g.drawImage((pieceSelected ? imgGcursor : imgBcursor), (csrX * PieceW) + PieceStartX, PieceStartY - (csrY * PieceH), GUI.ANCHOR);
		}
	
		yPos = imgBoard.getHeight() + 2;
		if (board.getTurn() == Board.WhiteInTurn) {
			g.drawRegion(imgPieces, (Board.WN - 1) * PieceW, 0, PieceW, PieceH, 0, 0, yPos, GUI.ANCHOR);
		} else {
			g.drawRegion(imgPieces, (Board.BN - 1) * PieceW, 0, PieceW, PieceH, 0, 0, yPos, GUI.ANCHOR);
		}

		String s = Locale.get("mcTurn") + ": " + ((board.getPly() / 2) + 1);
		g.setColor(0x000000);
		yPos += (PieceH - defFont.getHeight()) / 2;
		xPos = PieceW + 4;
		g.drawString(s, xPos, yPos, GUI.ANCHOR);

		xPos += 12 + defFont.stringWidth(s);
		if (editMode) {
			s = Locale.get("mcEditMode");
		} else {
			s = board.getMark();
		}

		if (s != null) {
			g.drawString(s, xPos, yPos, GUI.ANCHOR);
		}
	}

	/**
	 * Handle commands etc.
	 *
	 * @param c Current command.
	 * @param d Displayable.
	 */
	public void commandAction(Command c, Displayable d) {
		if (c == GUI.cmdOptions) {
			GUI.showMainOptionsList(this);
		} else if ((c == GUI.cmdBack) || (c == GUI.cmdNegative)) {
			GUI.setDisplayable(mid.main);
		} else if (c == GUI.cmdDone) {
			if (editMode) {
				setEditMode(false);
				GUI.showFinalEditForm(board.getPly(), this);
			} else if (d == GUI.fFinalEdit) {
				int p = Integer.parseInt(GUI.feTurn.getString()) - 1;

				p <<= 1;
				if (GUI.cgInTurn.getSelectedIndex() == 1) {
					p++;
				}

				board.setPly(p);
				GUI.setDisplayable(mid.main);
			}
		} else if (c == GUI.cmdPositive) {
			if (GUI.confState == GOTOMARK) {
				if (goToMark > -1) {
					board = (Board)marks.elementAt(goToMark);

					for (int i = marks.size() - 1; i > goToMark; i--) {
						marks.removeElementAt(i);
					}

					if (goToMark > 0) {
						marks.removeElementAt(goToMark);
					}

					goToMark = -1;
				}
			} else if (GUI.confState == RESETBOARD) {
				marks.removeAllElements();
				board = new Board();
				marks.addElement(board);
			}
			GUI.setDisplayable(mid.main);
		} else if (c == GUI.cmdSave) {
			StringBuffer sBuf = new StringBuffer();
			String s = GUI.tbAddMark.getString();

			if (s != null) {
				s.replace('\r', ' ');
				s.replace('\n', ' ');
			}

			sBuf.append(s);
			sBuf.append(" (");
			sBuf.append(Integer.toString((board.getPly() / 2) + 1));
			sBuf.append('/');
			sBuf.append((board.getTurn() == Board.WhiteInTurn ? Locale.get("constW") : Locale.get("constB")));
			sBuf.append(')');

			board.setMark(sBuf.toString());
			Board brd = new Board(board);
			marks.addElement(brd);

			GUI.setDisplayable(mid.main);
		} else if (d == GUI.lstMainOpts) {
			switch (GUI.lstMainOpts.getSelectedIndex()) {
				case 0:		// Back
					GUI.setDisplayable(mid.main);
					break;
				case 1:		// Marks
					GUI.showMarksList(marks, this);
					break;
				case 2:		// Edit board
					setEditMode(true);
					GUI.setDisplayable(mid.main);
					break;
				case 3:		// Reset board
					GUI.confirmation(Locale.get("cResetBoardTitle"), Locale.get("cResetBoardBody"), Locale.get("cYes"), Locale.get("cNo"), RESETBOARD, this);
					break;
				case 4:		// About
					GUI.showAboutForm(this);
					break;
				default:	// Exit
					mid.exit();
					break;
			}
		} else if (d == GUI.lstMarks) {
			goToMark = GUI.lstMarks.getSelectedIndex();
			if (goToMark <= 0) {
				GUI.setDisplayable(mid.main);
			} else if (goToMark == 1) {
				GUI.showAddMarkTextBox(this, (board.getPly() / 2) + 1);
			} else {
				goToMark -= 2;
		
				GUI.confirmation(Locale.get("cGoToMarkTitle"), Locale.get("cGoToMarkBody"), Locale.get("cYes"), Locale.get("cNo"), GOTOMARK, this);
			}
		}
	}

	/**
	 * Handle keypresses.
	 *
	 * @param keyCode Current key press.
	 */
	protected void keyPressed(int keyCode) {
		int ga = getGameAction(keyCode);

		if ((ga == Canvas.LEFT) ||
		    (keyCode == Canvas.KEY_NUM4)) {
			if ((editMode) && (pieceSelected)) {
				byte b = board.getPiece(csrX, csrY);

				if (b == Board.WP) {
					board.setPiece(csrX, csrY, (byte)Board.WK);
				} else if (b == Board.BP) {
					board.setPiece(csrX, csrY, (byte)Board.BK);
				} else {
					b++;
					board.setPiece(csrX, csrY, b);
				}
			} else {
				if (csrX > 0) {
					csrX--;
				} else {
					csrX = 7;
				}
			}
		} else if ((ga == Canvas.RIGHT) ||
			   (keyCode == Canvas.KEY_NUM6)) {
			if ((editMode) && (pieceSelected)) {
				byte b = board.getPiece(csrX, csrY);

				if (b == Board.WK) {
					board.setPiece(csrX, csrY, (byte)Board.WP);
				} else if (b == Board.BK) {
					board.setPiece(csrX, csrY, (byte)Board.BP);
				} else {
					b--;
					board.setPiece(csrX, csrY, b);
				}
			} else {
				if (csrX < 7) {
					csrX++;
				} else {
					csrX = 0;
				}
			}
		} else if ((ga == Canvas.UP) ||
			   (keyCode == Canvas.KEY_NUM2)) {
			if ((editMode) && (pieceSelected)) {
				flipPiece();
			} else {
				if (csrY < 7) {
					csrY++;
				} else {
					csrY = 0;
				}
			}
		} else if ((ga == Canvas.DOWN) ||
			   (keyCode == Canvas.KEY_NUM8)) {
			if ((editMode) && (pieceSelected)) {;
				flipPiece();
			} else {
				if (csrY > 0) {
					csrY--;
				} else {
					csrY = 7;
				}
			}
		} else if (keyCode == Canvas.KEY_NUM0) {
			if (editMode) {
				board.clear(csrX, csrY);
			} else {
				resetCursor();
			}
		} else if ((ga == Canvas.FIRE) ||
			   (keyCode == Canvas.KEY_NUM5)) {
			if (editMode) {
				if (pieceSelected) {
					pieceSelected = false;
				} else {
					pieceSelected = true;
				}
			} else {
				if (pieceSelected) {
					pieceSelected = false;

					if ((srcX != csrX) || (srcY != csrY)) {
						board.move(srcX, srcY, csrX, csrY);
						board.bumpPly();
					}
				} else {
					byte b = board.getPiece(csrX, csrY);
					int t = board.getTurn();

					if (((b >= Board.WK) && (b <= Board.WP) && (t == Board.WhiteInTurn)) ||
					    ((b >= Board.BK) && (b <= Board.BP) && (t == Board.BlackInTurn))) {
						srcX = csrX;
						srcY = csrY;
						pieceSelected = true;
					}
				}
			}
		} else if (keyCode == Canvas.KEY_NUM1) {
			if (editMode) {
				setEditMode(false);
			} else {
				setEditMode(true);
			}
		}
	       
		if (editMode) {
			if (keyCode == Canvas.KEY_NUM7) {
				board.setPiece(csrX, csrY, saveBoard.getPiece(csrX, csrY));
			}
		} else {
			if (keyCode == Canvas.KEY_NUM3) {
				GUI.showAddMarkTextBox(this, (board.getPly() / 2) + 1);
			} else if (keyCode == Canvas.KEY_NUM9) {
				GUI.showMarksList(marks, this);
			}
		}

		repaint();
	}

	/**
	 * Flip the piece - ie. make a white piece black and vice versa.
	 */
	private void flipPiece() {
		byte b = board.getPiece(csrX, csrY);

		if (b >= Board.BK) {
			b -= 6;
		} else {
			b += 6;
		}
		board.setPiece(csrX, csrY, b);
	}

	/**
	 * Reset cursor to the "center" of the board.
	 */
	private void resetCursor() {
		csrX = 3;
		csrY = 3;
	}

	/**
	 * Set/unset edit mode.
	 *
	 * @param mode Edit (true), non-edit (false).
	 */
	private void setEditMode(boolean mode) {
		if (mode) {
			addCommand(GUI.cmdDone);
			saveBoard = new Board(board);
		} else {
			removeCommand(GUI.cmdDone);
			saveBoard = null;
			GUI.showFinalEditForm(board.getPly(), this);
		}

		pieceSelected = false;
		editMode = mode;
	}
}
