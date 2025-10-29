package dk.schau.Chessboard;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.Paint.Style;

public class ButtonBar {
	private Bitmap _switchTurnPng;
	private Bitmap _builderPng;
	private Bitmap _playPng;
	private Bitmap _deletePng;
	private Bitmap _addPng;
	private Bitmap _undoPng;
	private int _whiteCellColor;
	private ChessboardView _chessboardView;
	private Context _context;
	private Pieces _pieces;
	private Chessboard _chessboard;
	private PieceSelector _pieceSelector;
	private Paint _paint;
	
	public ButtonBar(ChessboardView chessboardView, Context context, Pieces pieces, Chessboard chessboard, PieceSelector pieceSelector) {
		_chessboardView = chessboardView;
		_context = context;
        _whiteCellColor = context.getResources().getColor(R.color.whitecell);
		_pieces = pieces;
		_chessboard = chessboard;
		_pieceSelector = pieceSelector;
    	_switchTurnPng = getBitmap(R.drawable.switchturn);
    	_builderPng = getBitmap(R.drawable.builder);
    	_playPng = getBitmap(R.drawable.play);
    	_deletePng = getBitmap(R.drawable.delete);
    	_addPng = getBitmap(R.drawable.add);
    	_undoPng = getBitmap(R.drawable.undo);
    	_paint = new Paint();
	}
	
	private Bitmap getBitmap(int id) {
    	return BitmapFactory.decodeResource(_context.getResources(), id);
	}
    
    public void draw(Canvas canvas, Mode mode) {
    	if (mode.isInPlayMode()) {
    		drawPlayModeBar(canvas);
    	} else if (mode.isInBlackPawnPromotion() || mode.isInWhitePawnPromotion()) {
    		// On purpose ... do nothing!
    	} else {
    		drawBuilderBar(canvas, mode);
    		if (mode.isInEraseMode()) {
    			highLightButton(canvas, 2);
    		}
    	}
    }
    
    private void drawPlayModeBar(Canvas canvas) {
    	canvas.drawBitmap(_switchTurnPng, 0, 0, null);
    	canvas.drawBitmap(_builderPng, 2 * ChessboardView.ButtonSize, 0, null);
    	if (_chessboard.canUndo()) {
    		canvas.drawBitmap(_undoPng, 4 * ChessboardView.ButtonSize, 0, null);
    	}
    }
    
    private void drawBuilderBar(Canvas canvas, Mode mode) {
    	canvas.drawBitmap(_playPng, 0, 0, null);
    	canvas.drawBitmap(_deletePng, 2 * ChessboardView.ButtonSize, 0, null);
    	canvas.drawBitmap(_addPng, 4 * ChessboardView.ButtonSize, 0, null);
    	if (mode.isInSetPiece()) {
    		drawPieceToSet(canvas);
    	}
    }
    
    private void drawPieceToSet(Canvas canvas) {
      	_paint.reset();
        _paint.setStyle(Style.FILL);
        _paint.setColor(_whiteCellColor);
        canvas.drawRect(5 * ChessboardView.ButtonSize, 0, (6 * ChessboardView.ButtonSize) - 1, ChessboardView.ButtonSize - 1, _paint);
      	_pieces.draw(canvas, _pieceSelector.getSelectedPiece(), 5 * ChessboardView.ButtonSize, 0);
    }
    
    private void highLightButton(Canvas canvas, int offset) {
    	_paint.reset();
    	_paint.setStyle(Style.STROKE);
        _paint.setColor(_whiteCellColor);
        
        offset *= ChessboardView.ButtonSize;
        
        canvas.drawRect(offset + 1, 1, offset + ChessboardView.ButtonSize - 2, ChessboardView.ButtonSize - 2, _paint);
    }
	
	public void click(float x, float y) {
		if ((int) (y / ChessboardView.ButtonBarSize) > 0) {
			return;
		}
		
		int buttonIndex = (int) (x / ChessboardView.ButtonSize);
		Mode mode = _chessboardView.getMode();
		if (mode.isInPlayMode()) {
			handlePlayModeButton(buttonIndex);
		} else if (mode.isInBuilder() || mode.isInSetPiece() || mode.isInEraseMode()) {
			handleBuilderButton(buttonIndex);
		}
	}
	
	private void handlePlayModeButton(int buttonIndex) {
		switch (buttonIndex) {
		case 0:
			_chessboard.switchMove();
			_chessboardView.modeSwitch();
			break;
		case 2:
			_chessboardView.goToBuilder();
			break;
		case 4:
			if (_chessboard.canUndo()) {
				_chessboard.undo();
				_chessboardView.modeSwitch();
			}
			break;
		}
	}
	
	private void handleBuilderButton(int buttonIndex) {
		switch (buttonIndex) {
		case 0:
			_chessboardView.goToPlayMode();
			break;
		case 2:
			if (_chessboardView.getMode().isInEraseMode()) {
				_chessboardView.goToBuilder();
			} else {
				_chessboardView.goToEraseMode();
			}
			break;
		case 4:
			_chessboardView.goToPieceSelector();
			break;
		}
	}
}
