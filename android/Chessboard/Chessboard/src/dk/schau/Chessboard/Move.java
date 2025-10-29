package dk.schau.Chessboard;

public class Move {
	private static final int Nothing = 0;
	private static final int SetTo = 1;
	private static final int BothSet = 2;
	private int from;
	private int to;
	private int moveStatus = Nothing;
	
	public void reset() {
		moveStatus = Nothing;
	}
	
	public boolean isFirstMove() {
		return moveStatus == Nothing;
	}
	
	public boolean isValidMove() {
		return moveStatus == BothSet;
	}
	
	public boolean isFromMoveSet() {
		return moveStatus == SetTo;
	}
	
	public int getFrom() {
		return from;
	}
	
	public int getTo() {
		return to;
	}

	public void add(int x, int y) {
		if (x < 0 || x > 7 || y < 0 || y > 7) {
			return;
		}
		
		if (moveStatus == SetTo) {
            int tmp = convert(x, y);
			if (from == tmp) {
				moveStatus = Nothing;
				return;
			}

            to = tmp;
			moveStatus = BothSet;
			return;
		}

        from = convert(x, y);
		moveStatus = SetTo;
	}

    public int convert(int x, int y) {
        return (y * 8) + x;
    }
}
