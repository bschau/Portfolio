package dk.schau.jStar;

/**
 * Vector class.
 */
public class Vector {
	public double x, y;

	/**
	 * Initialize a vector.
	 */
	public Vector() {
		zero();
	}

	/**
	 * Initialize a vector with the given characteristica.
	 *
	 * @param x X.
	 * @param y Y.
	 */
	public Vector(double x, double y) {
		this.x = x;
		this.y = y;
	}

	/**
	 * Zero out a vector.
	 */
	public void zero() {
		x = 0.0;
		y = 0.0;
	}

	/**
	 * Clone the given vector.
	 *
	 * @param v Vector to clone.
	 */
	public void clone(Vector v) {
		x = v.x;
		y = v.y;
	}

	/**
	 * Add the vector to this vector.
	 *
	 * @param v Vector to add.
	 */
	public void add(Vector v) {
		x += v.x;
		y += v.y;
	}

	/**
	 * Subtract the vector from this vector.
	 *
	 * @param v Vector to subtract.
	 */
	public void subtract(Vector v) {
		x -= v.x;
		y -= v.y;
	}

	/**
	 * Multiply this vector with a constant.
	 *
	 * @param val Constant value.
	 */
	public void multiply(double val) {
		x *= val;
		y *= val;
	}

	/**
	 * Divide this vector with a constant.
	 *
	 * @param val Constant value.
	 */
	public void divide(double val) {
		x /= val;
		y /= val;
	}
}
