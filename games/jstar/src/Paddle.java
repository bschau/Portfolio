package dk.schau.jStar;

/**
 * Paddle class.
 */
public class Paddle extends Thing {
	public static final int WIDTH = 6;
	enum Type { VSS, VSF, VMS, VMF, VLS, VLF, HSS, HSF, HMS, HMF, HLS, HLF;
		/**
		 * Return the integer value of this type.
		 *
		 * @param t Type.
		 * @return integer or -1 (unknown).
		 */
		public static int toInt(Type t) {
			switch (t) {
				case VSS:
					return 0;
				case VSF:
					return 1;
				case VMS:
					return 2;
				case VMF:
					return 3;
				case VLS:
					return 4;
				case VLF:
					return 5;
				case HSS:
					return 6;
				case HSF:
					return 7;
				case HMS:
					return 8;
				case HMF:
					return 9;
				case HLS:
					return 10;
				case HLF:
					return 11;
			}

			return -1;
		}

		/**
		 * Convert this type to a string.
		 *
		 * @param t Type.
		 * @return string or null (unknown).
		 */
		public static String toString(Type t) {
			switch (t) {
				case VSS:
					return "VSS";
				case VSF:
					return "VSF";
				case VMS:
					return "VMS";
				case VMF:
					return "VMF";
				case VLS:
					return "VLS";
				case VLF:
					return "VLF";
				case HSS:
					return "HSS";
				case HSF:
					return "HSF";
				case HMS:
					return "HMS";
				case HMF:
					return "HMF";
				case HLS:
					return "HLS";
				case HLF:
					return "HLF";
			}

			return null;
		}

		/**
		 * Convert string to enum type.
		 *
		 * @param s String.
		 * @return type or null (unknown).
		 */
		public static Type fromString(String s) {
			if (s.equals("VSS")) {
				return VSS;
			} else if (s.equals("VSF")) {
				return VSF;
			} else if (s.equals("VMS")) {
				return VMS;
			} else if (s.equals("VMF")) {
				return VMF;
			} else if (s.equals("VLS")) {
				return VLS;
			} else if (s.equals("VLF")) {
				return VLF;
			} else if (s.equals("HSS")) {
				return HSS;
			} else if (s.equals("HSF")) {
				return HSF;
			} else if (s.equals("HMS")) {
				return HMS;
			} else if (s.equals("HMF")) {
				return HMF;
			} else if (s.equals("HLS")) {
				return HLS;
			} else if (s.equals("HLF")) {
				return HLF;
			}

			return null;
		}
	};

	public Vector sledge, sledgeSize, sledgeSpeed;
	public Vector location, containerSize;
	public Type type;

	/** */
	private static final double[] epX = { 60, 60, 75, 75, 90, 90, 0, 0, 0, 0, 0, 0 };
	private static final double[] epY = { 0, 0, 0, 0, 0, 0, 60, 60, 75, 75, 90, 90 };
	private static final double[] spdX = { 1, 2, 1, 2, 1, 2, 0, 0, 0, 0, 0, 0 };
	private static final double[] spdY = { 0, 0, 0, 0, 0, 0, 1, 2, 1, 2, 1, 2 };
	private static final int[] szX = { 20, 20, 30, 30, 40, 40, WIDTH, WIDTH, WIDTH, WIDTH, WIDTH, WIDTH };
	private static final int[] szY = { WIDTH, WIDTH, WIDTH, WIDTH, WIDTH, WIDTH, 20, 20, 30, 30, 40, 40 };

	/**
	 * Initialize a paddle.
	 *
	 * @param l Location vector.
	 * @param t Type of paddle.
	 */
	public Paddle(Vector l, Type t) {
		super(Thing.Type.PADDLE);

		type = t;
		location = l;

		int idx = t.toInt(t);
		containerSize = new Vector(epX[idx], epY[idx]);
		sledge = new Vector(location.x, location.y);
		sledgeSize = new Vector(szX[idx], szY[idx]);
		sledgeSpeed = new Vector(spdX[idx], spdY[idx]);
	}

	/**
	 * Hit test.
	 */
	public boolean hitTest(int x, int y) {
		int t1 = (int)location.x, t2 = (int)location.y;

		if ((x >= t1) && (y >= t2)) {
			if ((int)containerSize.x == 0) {
				t1 += WIDTH;
			} else {
				t1 += (int)containerSize.x;
			}

			if ((int)containerSize.y == 0) {
				t2 += WIDTH;
			} else {
				t2 += (int)containerSize.y;
			}

			if ((x <= t1) && (y <= t2)) {
				return true;
			}
		}

		return false;
	}

	/**
	 * Move paddle.
	 */
	public void move() {
		if (sledgeSpeed.x != 0) {
			if ((sledge.x < location.x) || (sledge.x + sledgeSize.x > location.x + containerSize.x)) {
				sledgeSpeed.x = -sledgeSpeed.x;
			}
		} else if (sledgeSpeed.y != 0) {
			if ((sledge.y < location.y) || (sledge.y + sledgeSize.y > location.y + containerSize.y)) {
				sledgeSpeed.y = -sledgeSpeed.y;
			}
		}

		sledge.add(sledgeSpeed);
	}
}
