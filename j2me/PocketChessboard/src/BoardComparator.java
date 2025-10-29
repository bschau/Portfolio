import javax.microedition.rms.RecordComparator;

public class BoardComparator implements RecordComparator {

	/**
	 * Compare two records.
	 *
	 * @param r1 Record 1.
	 * @param r2 Record 2.
	 * @return PRECEDES, FOLLOWS or EQUIVALENT.
	 */
	public int compare(byte[] r1, byte[] r2) {
		int p1 = 0, p2 = 0;

		p1 = (((((int)r1[0]) << 24) & 0xff000000) |
		      ((((int)r1[1]) << 16) & 0x00ff0000) |
		      ((((int)r1[2]) <<  8) & 0x0000ff00) |
		        ((int)r1[3]         & 0x000000ff));

		p2 = (((((int)r2[0]) << 24) & 0xff000000) |
		      ((((int)r2[1]) << 16) & 0x00ff0000) |
		      ((((int)r2[2]) <<  8) & 0x0000ff00) |
		        ((int)r2[3]         & 0x000000ff));

		if (p1 < p2) {
			return RecordComparator.PRECEDES;
		}
		
		if (p1 > p2) {
			return RecordComparator.FOLLOWS;
		}

		return RecordComparator.EQUIVALENT;
	}
}
