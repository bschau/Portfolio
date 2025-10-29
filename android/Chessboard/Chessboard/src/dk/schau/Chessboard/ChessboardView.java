package dk.schau.Chessboard;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Matrix;
import android.util.AttributeSet;
import android.view.View;

public class ChessboardView extends View {
	public static final int CellSize = 32;
	public static final int BoardSize = CellSize * 8;
	public static final int ButtonBarSize = 32;
	public static final int ButtonSize = 32;
	public static final int LabelsSize = 12;
	private Matrix _matrix = new Matrix();
	private int _cornerWidth;
	private int _cornerHeight;
	private Chessboard _chessboard;
	private Move _move;
	private Mode _mode;
    private MoveMarker _moveMarker;
	private Bitmap _bitmap;
    private PieceSelector _pieceSelector;
	private WhitePawnPromotion _whitePawnPromotion;
	private BlackPawnPromotion _blackPawnPromotion;
	private ButtonBar _buttonBar;
	private StatusLine _statusLine;
	
    public ChessboardView(Context context) {
        super(context);
        
        initView(context);
    }

    public ChessboardView(Context context, AttributeSet attributes) {
        super(context, attributes);
        
        initView(context);
    }
    
    private void initView(Context context) {
    	Bitmap corner = getBitmap(null, R.drawable.corner);
    	_cornerWidth = corner.getWidth();
    	_cornerHeight = corner.getHeight();
    	_bitmap = Bitmap.createBitmap(BoardSize, BoardSize, Bitmap.Config.RGB_565);
        Canvas canvas = new Canvas(_bitmap);
    	_move = new Move();
    	_move.reset();
    	_moveMarker = new MoveMarker(context, canvas, _move);

        Pieces pieces = new Pieces(context);
    	_chessboard = new Chessboard(context, canvas, pieces);
    	
    	_pieceSelector = new PieceSelector(context, canvas, pieces, _chessboard);
    	_whitePawnPromotion = new WhitePawnPromotion(context, canvas, pieces);
    	_blackPawnPromotion = new BlackPawnPromotion(context, canvas, pieces);
    	
    	_buttonBar = new ButtonBar(this, context, pieces, _chessboard, _pieceSelector);
    	_statusLine = new StatusLine(context, _chessboard, _pieceSelector, _cornerWidth, _cornerHeight);
    	
    	_mode = new Mode();
    }
    
    private Bitmap getBitmap(Bitmap bitmap, int id) {
    	if (bitmap != null) {
    		return bitmap;
    	}
    	
    	return BitmapFactory.decodeResource(getResources(), id);
    }
    
    public Chessboard getChessboard() {
    	return _chessboard;
    }
    
    public Move getMove() {
    	return _move;
    }
    
    public Mode getMode() {
    	return _mode;
    }
    
    public PieceSelector getPieceSelector() {
    	return _pieceSelector;
    }
    
    @Override
    protected void onDraw(Canvas canvas) {
    	_chessboard.drawBoard();
    	_moveMarker.draw();
    	_chessboard.drawPieces();
    	if (_mode.isInPieceSelector()) {
    		_pieceSelector.draw();
        } else if (_mode.isInWhitePawnPromotion()) {
        	_whitePawnPromotion.draw();
        } else if (_mode.isInBlackPawnPromotion()) {
        	_blackPawnPromotion.draw();
    	}
    	transferBoardToView(canvas);
    	_buttonBar.draw(canvas, _mode);
    	_statusLine.draw(canvas, _mode, getWidth(), getHeight());
    }
    
    private void transferBoardToView(Canvas canvas) {
        canvas.drawRGB(0, 0, 0);
        _matrix.reset();
        _matrix.postScale(getScaleWidth(), getScaleHeight());
         
        canvas.save();
        canvas.translate(0, ButtonBarSize);
        canvas.drawBitmap(_bitmap, _matrix, null);
        canvas.restore();
    }
    
    public float getScaleWidth() {
    	float width = getWidth() - _cornerWidth;
    	
    	return width / BoardSize;
    }
    
    public float getScaleHeight() {
    	float height = getHeight() - _cornerHeight - ButtonBarSize;
    	
    	return height / BoardSize;
    }
    
    public void goToBuilder() {
    	_mode.setBuilder();
    	modeSwitch();
    }
    
    public void goToPlayMode() {
    	_mode.setPlayMode();
    	modeSwitch();
    }
    
    public void goToEraseMode() {
    	_mode.setEraseMode();
    	modeSwitch();
    }
    
    public void goToPieceSelector() {
    	_pieceSelector.setup();
    	_mode.setPieceSelector();
    	modeSwitch();
    }
    
    public void goToSetPiece() {
    	_mode.setInSetPiece();
    	modeSwitch();
    }

    public void goToWhitePawnPromotion(int square) {
    	_whitePawnPromotion.setup(square);
    	_chessboard.clearUndo();
    	_mode.setInWhitePawnPromotion();
    	modeSwitch();
    }
    
    public void goToBlackPawnPromotion(int square) {
    	_blackPawnPromotion.setup(square);
    	_chessboard.clearUndo();
    	_mode.setInBlackPawnPromotion();
    	modeSwitch();
    }
    
    public void modeSwitch() {
    	_move.reset();
    	invalidate();
    }
    
    public void pieceSelectorClick(float x, float y) {
    	if (!_pieceSelector.click(x, y, getScaleWidth(), getScaleHeight())) {
    		goToBuilder();
    		return;
    	}
    	
    	goToSetPiece();
    }
    
    public void pawnPromotionClick(float x, float y) {
    	PawnPromotion pawnPromotion;
    	
    	if (_mode.isInBlackPawnPromotion()) {
    		pawnPromotion = _blackPawnPromotion;
    	} else {
    		pawnPromotion = _whitePawnPromotion;
    	}
    	
    	if (!pawnPromotion.click(x, y, getScaleWidth(), getScaleHeight())) {
    		return;
    	}
    	
    	_chessboard.setPiece(pawnPromotion.getSquare(), pawnPromotion.getSelectedPiece());
    	goToPlayMode();
    }
    
    public void buttonBarClick(float x, float y) {
    	_buttonBar.click(x, y);
    }
}