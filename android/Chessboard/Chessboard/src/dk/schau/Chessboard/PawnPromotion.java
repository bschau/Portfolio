package dk.schau.Chessboard;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.Paint.Style;
import android.graphics.Rect;

public abstract class PawnPromotion {
	private char _queen;
	private char _rook;
	private char _knight;
	private char _bishop;
	private Canvas _canvas;
	private Paint _paint;
	private Pieces _pieces;
	private int _blackCellColor;
	private int _whiteCellColor;
	private Rect _dialog;
	private char _pieceSelected;
	private int _square;

	protected PawnPromotion(Context context, Canvas canvas, Pieces pieces, char queen, char rook, char knight, char bishop) {
		_blackCellColor = context.getResources().getColor(R.color.blackcell);
		_whiteCellColor = context.getResources().getColor(R.color.whitecell);
		
		_canvas = canvas;
		_pieces = pieces;
		_queen = queen;
		_rook = rook;
		_knight = knight;
		_bishop = bishop;
		_paint = new Paint();
	}
	
	public void setup(int square) {
    	_square = square;

    	int width = 4 * ChessboardView.CellSize;
    	width += Dialog.PaddingLeftRight * 2;
    	int height = ChessboardView.CellSize + (Dialog.PaddingTopBottom * 2);
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
        _pieces.draw(_canvas, _queen, x, y);
        x += ChessboardView.CellSize;
        _pieces.draw(_canvas, _rook, x, y);
        x += ChessboardView.CellSize;
        _pieces.draw(_canvas, _knight, x, y);
        x += ChessboardView.CellSize;
        _pieces.draw(_canvas, _bishop, x, y);
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
    	
    	y -= top;
    	y /= ChessboardView.CellSize * scaleHeight;
    	
    	if ((int) y != 0) {
    		return false;
    	}
    	
    	x -= left;
    	x /= ChessboardView.CellSize * scaleWidth;
    	
    	switch ((int) x) {
    	case 0:
    		_pieceSelected = _queen;
    		break;
    	case 1:
    		_pieceSelected = _rook;
    		break;
    	case 2:
    		_pieceSelected = _knight;
    		break;
    	case 3:
    		_pieceSelected = _bishop;
    		break;
    	default:
    		return false;
    	}
    	
    	return true;
    }
    
    public int getSquare() {
    	return _square;
    }
    
	public char getSelectedPiece() {
		return _pieceSelected;
	}
}
