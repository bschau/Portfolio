package dk.schau.jStar;

/**
 * Planet class.
 */
public class Planet extends Thing {
	public static final double PlanetConstant = 0.1;
	public static final double PSMASS = 1500.0;
	public static final double PSSIZE = 7.0;
	public static final double PMMASS = 2500.0;
	public static final double PMSIZE = 11.0;
	public static final double PLMASS = 5000.0;
	public static final double PLSIZE = 16.0;
	public Vector midLoc = new Vector();
	public Vector location;
	public double mass, size;

	/**
	 * Initialize a planet.
	 *
	 * @param l Location vector.
	 * @param m Planet mass.
	 * @param s Planet size.
	 */
	public Planet(Vector l, double m, double s) {
		super(Thing.Type.PLANET);

		location = l;
		mass = m;
		size = s;

		midLoc.x = location.x + (size / 2.0);
		midLoc.y = location.y + (size / 2.0);
	}

	/**
	 * Find mid point of planet.
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

	/**
	 * Grow planet.
	 */
	public void grow() {
		int s = (int)size;

		if (s == (int)PSSIZE) {
			mass = PMMASS;
			size = PMSIZE;
		} else if (s == (int)PMSIZE) {
			mass = PLSIZE;
			size = PLSIZE;
		}

		midLoc.x = location.x + (size / 2.0);
		midLoc.y = location.y + (size / 2.0);
	}
}
