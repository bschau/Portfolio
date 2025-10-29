public class Tiles {
	private static final byte[] O = {
		0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0 };

	private static final byte[][] I = {
		{ 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0 }, 
		{ 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0 } };

	private static final byte[][] S = {
		{ 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0 },
		{ 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0 } };

	private static final byte[][] Z = {
		{ 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0 },
		{ 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0 } };

	private static final byte[][] L = {
		{ 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0 },
		{ 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0 } };

	private static final byte[][] J = {
		{ 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0 },
		{ 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0 },
		{ 0, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0 } };

	private static final byte[][] T = {
		{ 0, 0 ,0, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0 },
		{ 0, 0 ,1, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0 },
		{ 0, 0 ,1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0 ,1, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0 } };

	/**
	 * Get a tile definition.
	 *
	 * @param tid Tile ID.
	 * @param seq Tile sequence.
	 * @return Tile definition or null.
	 */
	public static byte[] getTileDefinition(int tid, int seq) {
		switch (tid) {
			case 1:
				return O;
			case 2:
				return I[seq];
			case 3:
				return S[seq];
			case 4:
				return Z[seq];
			case 5:
				return L[seq];
			case 6:
				return J[seq];
			case 7:
				return T[seq];
		}

		return null;
	}

	/**
	 * Rotate a tile.
	 *
	 * @param tid Tile ID.
	 * @param seq Tile sequence.
	 * @return next tile ID.
	 */
	public static int rotateTile(int tid, int seq) {
		if (tid != 1) {
			if ((tid > 1) && (tid < 5)) {
				if (seq == 0) {
					return 1;
				}
			} else {
				seq++;
				if (seq < 4) {
					return seq;
				}
			}
		}

		return 0;
	}
}
