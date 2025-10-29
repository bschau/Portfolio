package dk.schau.Chessboard;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.Paint.Style;

public class MoveMarker {
	private Canvas _canvas;
	private int _markerColor;
	private Paint _paint;
	private Move _move;
	
	public MoveMarker(Context context, Canvas canvas, Move move) {
		_markerColor = context.getResources().getColor(R.color.boardmarker);
		_canvas = canvas;
		_paint = new Paint();
		_move = move;
	}
	
	public void draw() {
    	if (_move == null || !_move.isFromMoveSet()) {
    		return; 
    	}
    	
    	_paint.reset();
        _paint.setStyle(Style.FILL);
        _paint.setColor(_markerColor);

        int square = _move.getFrom();
        int x = (square % 8) * ChessboardView.CellSize;
        int y = (square / 8) * ChessboardView.CellSize;
        
        _canvas.drawRect(x, y, x + ChessboardView.CellSize, y + ChessboardView.CellSize, _paint);
    }
}
