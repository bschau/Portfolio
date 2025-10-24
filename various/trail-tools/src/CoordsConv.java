package com.schau.TrailTools;

/**
 * Convert a coordinate pair to a string.
 */
public class CoordsConv {
	private static boolean nullBased = false;

	/**
	 * Set if the conversion needs to be zero based.
	 */
	public static void setNullBased() {
		nullBased = true;
	}

	/**
	 * Convert a coordinate pair to a string.
	 *
	 * @param x X Coordinate.
	 * @param y Y Coordinate.
	 * @return the string.
	 */
	public static String getString(int x, int y) {
		if (!nullBased) {
			x++;
			y++;
		}

		return "" + x + ", " + y;
	}
}
