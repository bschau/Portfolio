import javax.microedition.rms.RecordComparator;

public class PeriodDataComparator implements RecordComparator {
	/**
	 * Compare two records.
	 *
	 * @param r1 Record #1.
	 * @param r2 Record #2.
	 * @return PRECEDES, FOLLOWS or EQUIVALENT.
	 */
	public int compare(byte[] r1, byte[] r2) {
		int i1 = r1[22], i2 = r2[22];
		String s1 = new String(r1, 23, i1).toLowerCase(), s2 = new String(r2, 23, i2).toLowerCase();

		i1 = s1.compareTo(s2);
		if (i1 < 0) {
			return RecordComparator.PRECEDES;
		} else if (i1 > 0) {
			return RecordComparator.FOLLOWS;
		}

		return RecordComparator.EQUIVALENT;
	}
}
