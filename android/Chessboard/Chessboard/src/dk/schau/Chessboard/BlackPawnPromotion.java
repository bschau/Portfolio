package dk.schau.Chessboard;

import android.content.Context;
import android.graphics.Canvas;

public class BlackPawnPromotion extends PawnPromotion {
	public BlackPawnPromotion(Context context, Canvas canvas, Pieces pieces) {
		super(context, canvas, pieces, Chessboard.BlackQueen, Chessboard.BlackRook, Chessboard.BlackKnight, Chessboard.BlackBishop);
	}
}
