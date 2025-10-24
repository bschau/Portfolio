package dk.schau.jStar;

/**
 * Wormhole class.
 */
public class Wormhole extends Thing {
	public static final double WormholeConstant = 0.05;
	public static final double WHMASS = 4000.0;
	public static final double WHSIZE = 16.0;
	public Vector midLoc = new Vector();
	public Vector location;
	public double mass, size;

	/**
	 * Initialize a wormhole.
	 *
	 * @param l Vector.
	 * @param m Worm hole mass.
	 * @param s Worm hole size.
	 */
	public Wormhole(Vector l, double m, double s) {
		super(Thing.Type.WORMHOLE);

		location = l;
		mass = m;
		size = s;

		midLoc.x = location.x + (size / 2.0);
		midLoc.y = location.y + (size / 2.0);
	}

	/**
	 * Find mid point of wormhole.
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
		Vector v = midLocation();
		double pX = midLoc.x - (double)x;
		double pY = midLoc.y - (double)y;
		double l = Math.sqrt(Math.abs((pX * pX) + (pY * pY)));

		if (l > (size / 2.0)) {
			return false;
		}
		
		return true;
	}
}
