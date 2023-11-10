package dk.schau.Chessboard;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.Paint.Style;
import android.graphics.Rect;

public class PieceSelector {
	private Chessboard _chessboard;
	private Paint _paint;
	private boolean _containsWhiteKing;
	private boolean _containsBlackKing;
	private Rect _dialog;
	private char _pieceSelected;
	private Canvas _canvas;
	private Pieces _pieces;
	private int _whiteCellColor;
	private int _blackCellColor;
	
	public PieceSelector(Context context, Canvas canvas, Pieces pieces, Chessboard chessboard) {
        _whiteCellColor = context.getResources().getColor(R.color.whitecell);
        _blackCellColor = context.getResources().getColor(R.color.blackcell);
        _canvas = canvas;
        _pieces = pieces;
		_chessboard = chessboard;
		_paint = new Paint();
	}
	
	public void setup() {
    	_containsWhiteKing = _chessboard.containsWhiteKing();
    	_containsBlackKing = _chessboard.containsBlackKing();
    	int width = !_containsWhiteKing ? 6 : (!_containsBlackKing ? 6 : 5);
    	width *= ChessboardView.CellSize;
    	width += Dialog.PaddingLeftRight * 2;
    	int height = (2 * ChessboardView.CellSize) + (Dialog.PaddingTopBottom * 2) + Dialog.PaddingLines;
    	int left = (ChessboardView.BoardSize - width) / 2;
    	int top = (ChessboardView.BoardSize - height) / 2;
    	
    	_dialog = new Rect(left, top, left + width, top + height);
    	_pieceSelected = Chessboard.Empty;
	}
	
    public void draw() {
      	_paint.reset();
        _paint.setStyle(Style.FILL);
        _paint.setColor(_whiteCellColor);
        _canvas.drawRect(_dialog, _paint);
        _paint.setStyle(Style.STROKE);
        _paint.setColor(_blackCellColor);
        _paint.setStrokeWidth(1);
        _canvas.drawRect(_dialog, _paint);
        
        int x = _dialog.left + Dialog.PaddingLeftRight;
        int y = _dialog.top + Dialog.PaddingTopBottom;
        if (!_containsWhiteKing) {
        	_pieces.draw(_canvas, Chessboard.WhiteKing, x,  y);
        	x += ChessboardView.CellSize;
        }
        
        _pieces.draw(_canvas, Chessboard.WhiteQueen, x,  y);
        x += ChessboardView.CellSize;
        _pieces.draw(_canvas, Chessboard.WhiteRook, x,  y);
        x += ChessboardView.CellSize;
        _pieces.draw(_canvas, Chessboard.WhiteKnight, x,  y);
        x += ChessboardView.CellSize;
        _pieces.draw(_canvas, Chessboard.WhiteBishop, x,  y);
        x += ChessboardView.CellSize;
        _pieces.draw(_canvas, Chessboard.WhitePawn, x,  y);
        
        x = _dialog.left + Dialog.PaddingLeftRight;
        y += ChessboardView.CellSize + Dialog.PaddingLines;
        if (!_containsBlackKing) {
        	_pieces.draw(_canvas, Chessboard.BlackKing, x,  y);
        	x += ChessboardView.CellSize;
        }
       	_pieces.draw(_canvas, Chessboard.BlackQueen, x,  y);
        x += ChessboardView.CellSize;
       	_pieces.draw(_canvas, Chessboard.BlackRook, x,  y);
        x += ChessboardView.CellSize;
       	_pieces.draw(_canvas, Chessboard.BlackKnight, x,  y);
        x += ChessboardView.CellSize;
       	_pieces.draw(_canvas, Chessboard.BlackBishop, x,  y);
        x += ChessboardView.CellSize;
       	_pieces.draw(_canvas, Chessboard.BlackPawn, x,  y);
    }
    
    public boolean click(float x, float y, float scaleWidth, float scaleHeight) {
    	int left = (int) ((_dialog.left + Dialog.PaddingLeftRight) * scaleWidth);
    	int top = (int) ((_dialog.top + Dialog.PaddingTopBottom) * scaleHeight);
    	
    	y -= ChessboardView.ButtonBarSize;
    	
    	Rect rect = new Rect(left, top,
    			(int) ((_dialog.right - Dialog.PaddingLeftRight) * scaleWidth),
    			(int) ((_dialog.bottom - Dialog.PaddingTopBottom) * scaleHeight)
    			);
    	
    	if (!rect.contains((int) x, (int) y)) {
    		return false;
    	}
    	
    	x -= left;
    	int dstX = (int) (x / (ChessboardView.CellSize * scaleWidth));
    	y -= top;
    	int dstY = (int) (y / ((ChessboardView.CellSize + (Dialog.PaddingLines / 2)) * scaleHeight));
    	
    	if (dstY == 0) {
    		if (_containsWhiteKing) {
    	    	dstX++;
        	}
    		
    		_pieceSelected = _pieces.getWhitePieceByIndex(dstX);
    	} else {
    		if (_containsBlackKing) {
    			dstX++;
    		}
    		_pieceSelected = _pieces.getBlackPieceByIndex(dstX);
    	}
    	
    	return true;
    }
    
    public char getSelectedPiece() {
    	return _pieceSelected;
    }
}
