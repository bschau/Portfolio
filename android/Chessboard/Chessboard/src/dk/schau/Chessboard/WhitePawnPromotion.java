package dk.schau.Chessboard;

import android.content.Context;
import android.graphics.Canvas;

public class WhitePawnPromotion extends PawnPromotion {
	public WhitePawnPromotion(Context context, Canvas canvas, Pieces pieces) {
		super(context, canvas, pieces, Chessboard.WhiteQueen, Chessboard.WhiteRook, Chessboard.WhiteKnight, Chessboard.WhiteBishop);
	}
}
