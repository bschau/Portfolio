package dk.schau.Chessboard;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.Paint.Style;

import java.util.Arrays;

public class Chessboard {
	private static final String Alpha = "ABCDEFGH";
	private static final String Numbers = "87654321";
	public static final char Empty = ' ';
	public static final char WhiteKing = 'K';
	public static final char WhiteQueen = 'Q';
	public static final char WhiteRook = 'R';
	public static final char WhiteKnight = 'N';
	public static final char WhiteBishop = 'B';
	public static final char WhitePawn = 'P';
	public static final char BlackKing = 'k';
	public static final char BlackQueen = 'q';
	public static final char BlackRook = 'r';
	public static final char BlackKnight = 'n';
	public static final char BlackBishop = 'b';
	public static final char BlackPawn = 'p';
	public static final int WhiteToMove = 1;
	public static final int BlackToMove = 2;
    public static final int CastlingNone = 0;
    public static final int CastlingWhiteShort = 1;
    public static final int CastlingWhiteLong = 2;
    public static final int CastlingBlackShort = 3;
    public static final int CastlingBlackLong = 4;

	private static final String[] _omg = new String[] {
		"a UFO",
		"a smiling martian",
		"a dancing cow",
		"Heinz Doofenshmirtz"
	};
	private char[] board = new char[64];
	private Canvas _canvas;
	private Pieces _pieces;
	private Paint _paint;
	private int _blackCellColor;
	private int _blackColor;
	private int inMove;
	private boolean boardEmpty;
	private int lastFrom;
	private int lastTo;
    private char lastFromPiece;
	private char lastToPiece;
	private int lastInMove;
    private int lastCastling;
    private boolean lastEnpassant;

	public Chessboard(Context context, Canvas canvas, Pieces pieces) {
        _blackCellColor = context.getResources().getColor(R.color.blackcell);
        _blackColor = context.getResources().getColor(R.color.black);
		_canvas = canvas;
		_pieces = pieces;
		_paint = new Paint();
		reset();
	}
	
	public String toString() {
		StringBuilder stringBuilder = new StringBuilder(100);

        stringBuilder.append(Arrays.toString(board));
        stringBuilder.append(inMove == WhiteToMove ? "w" : "b");
		return stringBuilder.toString();
	}
	
	public void fromString(String source) {
		clear();

        int index = 0;
		for (; index < 64; index++) {
			board[index] = source.charAt(index);
		}
		
		inMove = source.charAt(index) == 'w' ? WhiteToMove : BlackToMove;
		boardEmpty = isEmpty();
	}
	
	public void reset() {
		clear();
		
		board[0] = BlackRook;
		board[1] = BlackKnight;
		board[2] = BlackBishop;
		board[3] = BlackQueen;
		board[4] = BlackKing;
		board[5] = BlackBishop;
		board[6] = BlackKnight;
		board[7] = BlackRook;
		
		board[56] = WhiteRook;
		board[57] = WhiteKnight;
		board[58] = WhiteBishop;
		board[59] = WhiteQueen;
		board[60] = WhiteKing;
		board[61] = WhiteBishop;
		board[62] = WhiteKnight;
		board[63] = WhiteRook;
		
		for (int x = 0; x < 8; x++) {
			board[8 + x] = BlackPawn;
			board[48 + x] = WhitePawn;
		}
		
		boardEmpty = false;
	}
	
	public void clear() {
		for (int index = 0; index < 64; index++) {
			board[index] = Empty;
		}
		
		inMove = WhiteToMove;
		boardEmpty = true;
		clearUndo();
	}
	
	public void clearUndo() {
		lastFrom = -1;
		lastTo = -1;
        lastCastling = CastlingNone;
        lastEnpassant = false;
	}
	
	public boolean canUndo() {
		return lastCastling != CastlingNone || lastFrom != -1;
	}
	
	public void undo() {
		if (!canUndo()) {
			return;
		}

        switch (lastCastling) {
            case CastlingWhiteShort:
                setPiece(60, WhiteKing);
                setPiece(61, Empty);
                setPiece(62, Empty);
                setPiece(63, WhiteRook);
                break;

            case CastlingWhiteLong:
                setPiece(56, WhiteRook);
                setPiece(58, Empty);
                setPiece(59, Empty);
                setPiece(60, WhiteKing);
                break;

            case CastlingBlackShort:
                setPiece(4, BlackKing);
                setPiece(5, Empty);
                setPiece(6, Empty);
                setPiece(7, BlackRook);
                break;

            case CastlingBlackLong:
                setPiece(0, BlackRook);
                setPiece(2, Empty);
                setPiece(3, Empty);
                setPiece(4, BlackKing);
                break;

            default:
                if (lastEnpassant) {
                    if (getPiece(lastTo) == WhitePawn) {
                        setPiece(lastFrom, WhitePawn);
                        if ((lastFrom - 9) == lastTo) {
                            setPiece(lastFrom - 1, BlackPawn);
                        } else {
                            setPiece(lastFrom + 1, BlackPawn);
                        }
                    } else {
                        setPiece(lastFrom, BlackPawn);
                        if ((lastFrom + 7) == lastTo) {
                            setPiece(lastFrom - 1, WhitePawn);
                        } else {
                            setPiece(lastFrom + 1, WhitePawn);
                        }
                    }
                    setPiece(lastTo, Empty);
                } else {
                    setPiece(lastFrom, lastFromPiece);
                    setPiece(lastTo, lastToPiece);
                }
                break;
        }

		inMove = lastInMove;
		clearUndo();
	}
	
	public boolean isEmptySquare(int square) {
		return board[square] == Empty;
	}
	
	public boolean isWhitePiece(int square) {
		switch (board[square]) {
		case WhiteKing:
		case WhiteQueen:
		case WhiteRook:
		case WhiteKnight:
		case WhiteBishop:
		case WhitePawn:
			return true;
		}
		
		return false;
	}
	
	public boolean isBlackPiece(int square) {
		switch (board[square]) {
		case BlackKing:
		case BlackQueen:
		case BlackRook:
		case BlackKnight:
		case BlackBishop:
		case BlackPawn:
			return true;
		}
		
		return false;
	}
	
	public char getPiece(int square) {
		return board[square];
	}
	
	public void setPiece(int square, char piece) {
		board[square] = piece;
	}
	
	public void move(int from, int to) {
		lastFrom = from;
		lastTo = to;
		lastFromPiece = getPiece(from);
		lastToPiece = getPiece(to);
		lastInMove = inMove;
        lastCastling = CastlingNone;
        lastEnpassant = false;
		
		board[to] = board[from];
		board[from] = Empty;
		
		boardEmpty = isEmpty();
		switchMove();
	}
	
	public boolean isValidMove(int from, int to) {
		if (isEmptySquare(from)) {
			return false;
		}
		
		boolean emptyDestination = isEmptySquare(to);

		if (isWhitePiece(from) && (isBlackPiece(to) || emptyDestination)) {
			return true;
		}

		if (isBlackPiece(from) && (isWhitePiece(to) || emptyDestination)) {
			return true;
		}

		return false;
	}
	
	public boolean isWhitePly() {
		return inMove == WhiteToMove;
	}
	
	public boolean isBlackPly() {
		return inMove == BlackToMove;
	}
	
	public void switchMove() {
		inMove = (inMove == WhiteToMove) ? BlackToMove : WhiteToMove;
	}
	
	public void erase(int square) {
		board[square] = Empty;
		clearUndo();
		boardEmpty = isEmpty();
	}
	
	private boolean isEmpty() {
		for (int index = 0; index < 64; index++) {
			if (board[index] != Empty) {
				return false;
			}
		}
		
		return true;
	}
	
	public boolean isBoardEmpty() {
		return boardEmpty;
	}
	
	public boolean containsWhiteKing() {
		return contains(WhiteKing);
	}
	
	public boolean containsBlackKing() {
		return contains(BlackKing);
	}
	
	private boolean contains(char piece) {
		for (int index = 0; index < 64; index++) {
			if (board[index] == piece) {
				return true;
			}
		}
		
		return false;
	}

    public boolean tryWhiteCastling(int from, int to) {
        if (from == 60 && getPiece(from) == WhiteKing) {
            if (to == 58 && getPiece(56) == WhiteRook && isEmptySquare(57) && isEmptySquare(58) && isEmptySquare(59)) {
                setPiece(58, WhiteKing);
                setPiece(59, WhiteRook);
                setPiece(60, Empty);
                setPiece(56, Empty);
                setPiece(57, Empty);
                handleCastling(CastlingWhiteLong);
                return true;
            }

            if (to == 62 && getPiece(63) == WhiteRook && isEmptySquare(61) && isEmptySquare(62)) {
                setPiece(62, WhiteKing);
                setPiece(61, WhiteRook);
                setPiece(60, Empty);
                setPiece(63, Empty);
                handleCastling(CastlingWhiteShort);
                return true;
            }
        }

        return false;
    }

    public boolean tryBlackCastling(int from, int to) {
        if (from == 4 && getPiece(from) == BlackKing) {
            if (to == 2 && getPiece(0) == BlackRook && isEmptySquare(1) && isEmptySquare(2) && isEmptySquare(3)) {
                setPiece(2, BlackKing);
                setPiece(3, BlackRook);
                setPiece(0, Empty);
                setPiece(1, Empty);
                setPiece(4, Empty);
                handleCastling(CastlingBlackLong);
                return true;
            }

            if (to == 6 && getPiece(7) == BlackRook && isEmptySquare(5) && isEmptySquare(6)) {
                setPiece(6, BlackKing);
                setPiece(5, BlackRook);
                setPiece(4, Empty);
                setPiece(7, Empty);
                handleCastling(CastlingBlackShort);
                return true;
            }
        }

        return false;
    }

    private void handleCastling(int castlingType) {
        clearUndo();
        lastCastling = castlingType;
        lastInMove = inMove;
        switchMove();
    }

    public boolean tryWhiteEnpassant(int from, int to) {
        if (to < 16 || to > 23 || from < 24 || from > 31 || getPiece(from) != WhitePawn) {
            return false;
        }

        if ((from - 9) == to && getPiece(from - 1) == BlackPawn) {
            setPiece(from - 9, WhitePawn);
            setPiece(from - 1, Empty);
            handleEnpassant(from, to);
            return true;
        }

        if ((from - 7) == to && getPiece(from + 1) == BlackPawn) {
            setPiece(from - 7, WhitePawn);
            setPiece(from + 1, Empty);
            handleEnpassant(from, to);
            return true;
        }

        return false;
    }

    public boolean tryBlackEnpassant(int from, int to) {
        if (to < 40 || to > 47 || from < 32 || from > 39 || getPiece(from) != BlackPawn) {
            return false;
        }

        if ((from + 9) == to && getPiece(from + 1) == WhitePawn) {
            setPiece(from + 9, BlackPawn);
            setPiece(from + 1, Empty);
            handleEnpassant(from, to);
            return true;
        }

        if ((from + 7) == to && getPiece(from - 1) == WhitePawn) {
            setPiece(from + 7, BlackPawn);
            setPiece(from - 1, Empty);
            handleEnpassant(from, to);
            return true;
        }

        return false;
    }

    private void handleEnpassant(int from, int to) {
        setPiece(from, Empty);
        lastFrom = from;
        lastTo = to;
        lastEnpassant = true;

        switchMove();
    }

	public String pieceToString(char piece) {
		switch (piece) {
		case WhiteKing:
			return "white king";
		case WhiteQueen:
			return "white queen";
		case WhiteRook:
			return "white rook";
		case WhiteKnight:
			return "white knight";
		case WhiteBishop:
			return "white bishop";
		case WhitePawn:
			return "white pawn";
		case BlackKing:
			return "black king";
		case BlackQueen:
			return "black queen";
		case BlackRook:
			return "black rook";
		case BlackKnight:
			return "black knight";
		case BlackBishop:
			return "black bishop";
		case BlackPawn:
			return "black pawn";
		}
		
		int index = (int) System.currentTimeMillis() % 4;
		return _omg[index];
	}
	
	public boolean pawnPromotionWhite(int dst) {
        return dst >= 0 && dst <= 7 && getPiece(dst) == WhitePawn;

    }
	
	public boolean pawnPromotionBlack(int dst) {
        return dst >= 56 && dst <= 63 && getPiece(dst) == BlackPawn;

    }
	
    public void drawBoard() {
      	_paint.reset();
        _paint.setStyle(Style.FILL);
        _paint.setColor(_blackCellColor);
        
        _canvas.drawRGB(0xdd, 0xdd, 0xdd);
        
        for (int pair = 0; pair < 4; pair++) {
        	int even = (pair * 2) * ChessboardView.CellSize;
        	int odd = even + ChessboardView.CellSize;
        	
        	_canvas.drawRect(odd, 0, odd + ChessboardView.CellSize, ChessboardView.CellSize, _paint);
        	_canvas.drawRect(even, ChessboardView.CellSize, even + ChessboardView.CellSize, ChessboardView.CellSize + ChessboardView.CellSize, _paint);
        	_canvas.drawRect(odd, (2 * ChessboardView.CellSize), odd + ChessboardView.CellSize, (2 * ChessboardView.CellSize) + ChessboardView.CellSize, _paint);
        	_canvas.drawRect(even, (3 * ChessboardView.CellSize), even + ChessboardView.CellSize, (3 * ChessboardView.CellSize) + ChessboardView.CellSize, _paint);
        	_canvas.drawRect(odd, (4 * ChessboardView.CellSize), odd + ChessboardView.CellSize, (4 * ChessboardView.CellSize) + ChessboardView.CellSize, _paint);
        	_canvas.drawRect(even, (5 * ChessboardView.CellSize), even + ChessboardView.CellSize, (5 * ChessboardView.CellSize) + ChessboardView.CellSize, _paint);
        	_canvas.drawRect(odd, (6 * ChessboardView.CellSize), odd + ChessboardView.CellSize, (6 * ChessboardView.CellSize) + ChessboardView.CellSize, _paint);
        	_canvas.drawRect(even, (7 * ChessboardView.CellSize), even + ChessboardView.CellSize, (7 * ChessboardView.CellSize) + ChessboardView.CellSize, _paint);
        }
        
        drawLabels();
    }
    
    private void drawLabels() {
      	_paint.reset();
        _paint.setStyle(Style.FILL_AND_STROKE);
        _paint.setColor(_blackColor);
        _paint.setTextSize(ChessboardView.LabelsSize);
        _paint.setAntiAlias(true);
        
    	drawAlpha();
    	drawNumbers();
    }
    
    private void drawAlpha() {
    	int x = 0;
    	for (int alphaIndex = 0; alphaIndex < Alpha.length(); alphaIndex++) {
    		int width = (int)_paint.measureText(Alpha, alphaIndex, alphaIndex + 1);
    		int y = (8 * ChessboardView.CellSize) - 2;
    	
    		_canvas.drawText(Alpha, alphaIndex, alphaIndex + 1, x + (ChessboardView.CellSize - width - 2), y, _paint);
    		x += ChessboardView.CellSize;
    	}
    }
    
    private void drawNumbers() {
    	int y = 2 + ChessboardView.LabelsSize;
    	for (int numberIndex = 0; numberIndex < Numbers.length(); numberIndex++) {
    		_canvas.drawText(Numbers, numberIndex, numberIndex + 1, 2, y, _paint);
    		y += ChessboardView.CellSize;
    	}
    }
    
    public void drawPieces() {
        int index = 0;

        for (int y = 0; y < 8; y++) {
       		float top = (y * ChessboardView.CellSize) + 2;
        	for (int x = 0; x < 8; x++) {
        		char piece = getPiece(index);
        		float left = (x * ChessboardView.CellSize) + 2;
        		
        		_pieces.draw(_canvas, piece, left, top);
                index++;
        	}
        }
    }
}