package dk.schau.jStar;

/**
 * All planets, wormholes, walls etc. must implement this!
 */
public abstract class Thing {
	enum Type {
		PLANET,
		WORMHOLE,
		DEFLECTOR,
		WALL,
		PADDLE;

		/**
		 * Convert this type to a string.
		 *
		 * @param t Type.
		 * @return string or null (unknown).
		 */
		public static String toString(Type t) {
			switch (t) {
				case PLANET:
					return "TypePlanet";
				case WORMHOLE:
					return "TypeWormhole";
				case DEFLECTOR:
					return "TypeDeflector";
				case WALL:
					return "TypeWall";
				case PADDLE:
					return "TypePaddle";
				default:
					break;
			}

			return null;
		}

		/**
		 * Convert string to type.
		 *
		 * @param s String.
		 * @return type or null (unknown).
		 */
		public static Type fromString(String s) {
			if (s.equals("TypePlanet")) {
				return PLANET;
			} else if (s.equals("TypeWormhole")) {
				return WORMHOLE;
			} else if (s.equals("TypeDeflector")) {
				return DEFLECTOR;
			} else if (s.equals("TypeWall")) {
				return WALL;
			} else if (s.equals("TypePaddle")) {
				return PADDLE;
			}

			return null;
		}
	};

	/** */
	private boolean selected = false;
	private Type type;

	/**
	 * Constructor.
	 */
	public Thing(Type t) {
		type = t;
	}

	/**
	 * Select this component.
	 */
	public void select() {
		selected = true;
	}

	/**
	 * Deselect this component.
	 */
	public void deselect() {
		selected = false;
	}

	/**
	 * Is this component selected?
	 *
	 * @return true if selected, false otherwise.
	 */
	public boolean isSelected() {
		return selected;
	}

	/**
	 * Return the type of the thing.
	 *
	 * @return type.
	 */
	public Type getType() {
		return type;
	}

	/**
	 * Hit test on x, y.
	 *
	 * @param x X position.
	 * @param y Y position.
	 * @return true if this object is hit, false otherwise.
	 */
	public abstract boolean hitTest(int x, int y);
}
