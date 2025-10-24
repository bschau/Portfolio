package dk.schau.jStar;

/**
 * Deflector class.
 */
public class Deflector extends Thing {
	public static final double DeflectorConstant = 0.2;
	public static final double DSMASS = 2000.0;
	public static final double DSSIZE = 7.0;
	public static final double DMMASS = 3000.0;
	public static final double DMSIZE = 11.0;
	public static final double DLMASS = 4000.0;
	public static final double DLSIZE = 16.0;
	public Vector midLoc = new Vector();
	public Vector location;
	public double mass, size;

	/**
	 * Initialize a deflector.
	 *
	 * @param l Location vector.
	 * @param m Mass.
	 * @param s Size.
	 */
	public Deflector(Vector l, double m, double s) {
		super(Thing.Type.DEFLECTOR);

		location = l;
		mass = m;
		size = s;

		midLoc.x = location.x + (size / 2.0);
		midLoc.y = location.y + (size / 2.0);
	}

	/**
	 * Find mid point of deflector.
	 *
	 * @return mid point.
	 */
	public Vector midLocation() {
		return midLoc;
	}

	/**
	 * Hit test.
	 */
	public boolean hitTest(int x, int y) {
		double pX = midLoc.x - (double)x;
		double pY = midLoc.y - (double)y;
		double l = Math.abs(Math.sqrt((pX * pX) + (pY * pY)));

		if (l > (size / 2.0)) {
			return false;
		}

		return true;
	}
}
