import javax.microedition.rms.RecordComparator;

public class TaskComparator implements RecordComparator {
	/**
	 * Compare two task records.
	 *
	 * @param r1 Record 1.
	 * @param r2 Record 2.
	 * @return PRECEDES, FOLLOWS or EQUIVALENT.
	 */
	public int compare(byte[] r1, byte[] r2) {
		int i1 = r1[0], i2 = r2[0];
		String s1 = new String(r1, 1, i1), s2 = new String(r2, 1, i2);

		i1 = s1.toLowerCase().compareTo(s2.toLowerCase());
		if (i1 < 0) {
			return RecordComparator.PRECEDES;
		} else if (i1 > 0) {
			return RecordComparator.FOLLOWS;
		}

		return RecordComparator.EQUIVALENT;
	}
}
