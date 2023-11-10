package dk.schau.Chessboard;

public class Mode {
	private static final int InPlayMode = 0;
	private static final int InBuilder = 1;
	private static final int InEraseMode = 2;
	private static final int InPieceSelector = 3;
	private static final int InSetPiece = 4;
	private static final int InWhitePawnPromotion = 5;
	private static final int InBlackPawnPromotion = 6;
	private int _mode;
	
	public Mode() {
		setPlayMode();
	}
	
	public void setPlayMode() {
		_mode = InPlayMode;
	}
	
	public boolean isInPlayMode() {
		return _mode == InPlayMode;
	}
	
	public void setBuilder() {
		_mode = InBuilder;
	}
	
	public boolean isInBuilder() {
		return _mode == InBuilder;
	}
	
	public void setEraseMode() {
		_mode = InEraseMode;
	}
	
	public boolean isInEraseMode() {
		return _mode == InEraseMode;
	}
	
	public void setPieceSelector() {
		_mode = InPieceSelector;
	}
	
	public boolean isInPieceSelector() {
		return _mode == InPieceSelector;
	}
	
	public void setInSetPiece() {
		_mode = InSetPiece;
	}
	
	public boolean isInSetPiece() {
		return _mode == InSetPiece;
	}
	
	public void setInWhitePawnPromotion() {
		_mode = InWhitePawnPromotion;
	}
	
	public boolean isInWhitePawnPromotion() {
		return _mode == InWhitePawnPromotion;
	}
	
	public void setInBlackPawnPromotion() {
		_mode = InBlackPawnPromotion;
	}
	
	public boolean isInBlackPawnPromotion() {
		return _mode == InBlackPawnPromotion;
	}
}
