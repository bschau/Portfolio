package dk.schau.Chessboard;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;

public class Pieces {
	private static final char[] _whitePieces = new char[] {
		Chessboard.WhiteKing, Chessboard.WhiteQueen, Chessboard.WhiteRook,
		Chessboard.WhiteKnight, Chessboard.WhiteBishop, Chessboard.WhitePawn
	};
	private static final char[] _blackPieces = new char[] {
		Chessboard.BlackKing, Chessboard.BlackQueen, Chessboard.BlackRook,
		Chessboard.BlackKnight, Chessboard.BlackBishop, Chessboard.BlackPawn
	};
	private Bitmap _blackKingPng;
	private Bitmap _blackQueenPng;
	private Bitmap _blackRookPng;
	private Bitmap _blackKnightPng;
	private Bitmap _blackBishopPng;
	private Bitmap _blackPawnPng;
	private Bitmap _whiteKingPng;
	private Bitmap _whiteQueenPng;
	private Bitmap _whiteRookPng;
	private Bitmap _whiteKnightPng;
	private Bitmap _whiteBishopPng;
	private Bitmap _whitePawnPng;
	private Context _context;
	
	public Pieces(Context context) {
		_context = context;
    	_blackKingPng = getBitmap(R.drawable.bk);
    	_blackQueenPng = getBitmap(R.drawable.bq);
    	_blackRookPng = getBitmap(R.drawable.br);
    	_blackKnightPng = getBitmap(R.drawable.bn);
    	_blackBishopPng = getBitmap(R.drawable.bb);
    	_blackPawnPng = getBitmap(R.drawable.bp);
    	_whiteKingPng = getBitmap(R.drawable.wk);
    	_whiteQueenPng = getBitmap(R.drawable.wq);
    	_whiteRookPng = getBitmap(R.drawable.wr);
    	_whiteKnightPng = getBitmap(R.drawable.wn);
    	_whiteBishopPng = getBitmap(R.drawable.wb);
    	_whitePawnPng = getBitmap(R.drawable.wp);
	}
    
    private Bitmap getBitmap(int id) {
    	return BitmapFactory.decodeResource(_context.getResources(), id);
    }
    
    public void draw(Canvas canvas, char piece, float left, float top) {
       	switch (piece) {
       	case Chessboard.BlackKing:
       		canvas.drawBitmap(_blackKingPng, left, top, null);
       		break;
       	case Chessboard.BlackQueen:
       		canvas.drawBitmap(_blackQueenPng, left, top, null);
       		break;
       	case Chessboard.BlackRook:
       		canvas.drawBitmap(_blackRookPng, left, top, null);
       		break;
       	case Chessboard.BlackKnight:
  			canvas.drawBitmap(_blackKnightPng, left, top, null);
   			break;
   		case Chessboard.BlackBishop:
   			canvas.drawBitmap(_blackBishopPng, left, top, null);
   			break;
   		case Chessboard.BlackPawn:
   			canvas.drawBitmap(_blackPawnPng, left, top, null);
   			break;
   		case Chessboard.WhiteKing:
   			canvas.drawBitmap(_whiteKingPng, left, top, null);
   			break;
   		case Chessboard.WhiteQueen:
   			canvas.drawBitmap(_whiteQueenPng, left, top, null);
  			break;
   		case Chessboard.WhiteRook:
   			canvas.drawBitmap(_whiteRookPng, left, top, null);
   			break;
        case Chessboard.WhiteKnight:
        	canvas.drawBitmap(_whiteKnightPng, left, top, null);
        	break;
        case Chessboard.WhiteBishop:
        	canvas.drawBitmap(_whiteBishopPng, left, top, null);
        	break;
        case Chessboard.WhitePawn:
        	canvas.drawBitmap(_whitePawnPng, left, top, null);
        	break;
        }
    }
    
    public char getWhitePieceByIndex(int index) {
    	return _whitePieces[index];
    }
    
    public char getBlackPieceByIndex(int index) {
    	return _blackPieces[index];
    }
}
