package dk.schau.jStar;

/**
 * Wall class.
 */
public class Wall extends Thing {
	public static final double WALLVSW = 30;
	public static final double WALLVSH = 5;
	public static final double WALLHSW = 5;
	public static final double WALLHSH = 30;

	public static final double WALLVMW = 45;
	public static final double WALLVMH = 7;
	public static final double WALLHMW = 7;
	public static final double WALLHMH = 45;

	public static final double WALLVLW = 60;
	public static final double WALLVLH = 10;
	public static final double WALLHLW = 10;
	public static final double WALLHLH = 60;

	public Vector location, size;

	/**
	 * Initialize a wall.
	 *
	 * @param l Location vector.
	 * @param s Size vector.
	 */
	public Wall(Vector l, Vector s) {
		super(Thing.Type.WALL);

		location = l;
		size = s;
	}

	/**
	 * Hit test.
	 */
	public boolean hitTest(int x, int y) {
		int t1 = (int)location.x, t2 = (int)location.y;

		if ((x >= t1) && (y >= t2)) {
			t1 += (int)size.x;
			t2 += (int)size.y;
			if ((x <= t1) && (y <= t2)) {
				return true;
			}
		}

		return false;
	}
}
