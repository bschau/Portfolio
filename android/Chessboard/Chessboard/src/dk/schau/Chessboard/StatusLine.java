package dk.schau.Chessboard;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.Paint.Style;

public class StatusLine {
	private Context _context;
	private Chessboard _chessboard;
	private PieceSelector _pieceSelector;
	private int _cornerWidth;
	private int _cornerHeight;
	private int _whiteColor;
	private Paint _paint;
	
	public StatusLine(Context context, Chessboard chessboard, PieceSelector pieceSelector, int cornerWidth, int cornerHeight) {
		_context = context;
		_chessboard = chessboard;
		_pieceSelector = pieceSelector;
		_cornerWidth = cornerWidth;
		_cornerHeight = cornerHeight;
		_whiteColor = context.getResources().getColor(R.color.white);
		
		_paint = new Paint();
	}

    public void draw(Canvas canvas, Mode mode, int width, int height) {
    	_paint.reset();
        _paint.setStyle(Style.FILL);
        _paint.setColor(_whiteColor);
        
        String statusLine = "";
        
        if (mode.isInPlayMode()) {
        	if (!_chessboard.isBoardEmpty()) {
        		String inMove = _chessboard.isWhitePly() ? _context.getString(R.string.white) : _context.getString(R.string.black);
        		statusLine = String.format(_context.getString(R.string.status_line_inplaymode), inMove);
        	}
        } else if (mode.isInBuilder()) {
        	statusLine = _context.getString(R.string.status_line_inbuilder);
        } else if (mode.isInEraseMode()) {
        	statusLine = _context.getString(R.string.status_line_deleting);
        } else if (mode.isInPieceSelector()) {
        	statusLine = _context.getString(R.string.status_line_selectpiecetoplace);
        } else if (mode.isInSetPiece()) {
        	statusLine = String.format(_context.getString(R.string.status_line_place), _chessboard.pieceToString(_pieceSelector.getSelectedPiece()));
        } else if (mode.isInWhitePawnPromotion()) {
        	statusLine = _context.getString(R.string.status_line_whitepawnpromotion);
        } else if (mode.isInBlackPawnPromotion()) {
        	statusLine = _context.getString(R.string.status_line_blackpawnpromotion);
        }
        
        float textSize = _cornerHeight - 4;
        if (textSize < 6) {
        	textSize = 6;
        }
        _paint.setTextSize(textSize);
        _paint.setAntiAlias(true);
        int length = (int) _paint.measureText(statusLine);
        
        canvas.drawText(statusLine, (width - _cornerWidth - length) / 2, height - 2, _paint);
    }
}
