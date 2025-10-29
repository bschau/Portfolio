import javax.microedition.rms.RecordComparator;

public class TripComparator implements RecordComparator {
	private final int STARTOFPURPOSE=13;

	/**
	 * Compare two records.
	 *
	 * @param r1 Record #1.
	 * @param r2 Record #2.
	 * @return PRECEDS, FOLLOWS or EQUIVALENT.
	 */
	public int compare(byte[] r1, byte[] r2) {
		long l1, l2;
		String s1, s2;
		int i1, i2;

		if (r1[0]<r2[0]) {
			return RecordComparator.PRECEDES;
		} else if (r1[0]>r2[0]) {
			return RecordComparator.FOLLOWS;
		}

		l1=(((long)r1[1]<<56L)&0xff00000000000000L)|
		   (((long)r1[2]<<48L)&0x00ff000000000000L)|
		   (((long)r1[3]<<40L)&0x0000ff0000000000L)|
		   (((long)r1[4]<<32L)&0x000000ff00000000L)|
		   (((long)r1[5]<<24L)&0x00000000ff000000L)|
		   (((long)r1[6]<<16L)&0x0000000000ff0000L)|
		   (((long)r1[7]<<8L) &0x000000000000ff00L)|
		    ((long)r1[8]      &0x00000000000000ffL);

		l2=(((long)r2[1]<<56L)&0xff00000000000000L)|
		   (((long)r2[2]<<48L)&0x00ff000000000000L)|
		   (((long)r2[3]<<40L)&0x0000ff0000000000L)|
		   (((long)r2[4]<<32L)&0x000000ff00000000L)|
		   (((long)r2[5]<<24L)&0x00000000ff000000L)|
		   (((long)r2[6]<<16L)&0x0000000000ff0000L)|
		   (((long)r2[7]<<8L) &0x000000000000ff00L)|
		    ((long)r2[8]      &0x00000000000000ffL);

		if (l1<l2) {		// l1 is older than l2
			return RecordComparator.FOLLOWS;
		} else if (l1>l2) {	// l1 is newer than l2
			return RecordComparator.PRECEDES;
		}

		s1=new String(r1, STARTOFPURPOSE, r1.length-STARTOFPURPOSE);
		s2=new String(r2, STARTOFPURPOSE, r2.length-STARTOFPURPOSE);
		i1=s1.compareTo(s2);
		if (i1<0) {
			return RecordComparator.PRECEDES;
		} else if (i1>0) {
			return RecordComparator.FOLLOWS;
		}

		i1=((r1[9]<<24)&0xff000000)|
		   ((r1[10]<<16)&0x00ff0000)|
		   ((r1[11]<<8)&0x0000ff00)|
		   ((r1[12])&0x000000ff);
		i2=((r2[9]<<24)&0xff000000)|
		   ((r2[10]<<16)&0x00ff0000)|
		   ((r2[11]<<8)&0x0000ff00)|
		   ((r2[12])&0x000000ff);

		if (i1>i2) {
			return RecordComparator.PRECEDES;
		} else if (i1<i2) {
			return RecordComparator.FOLLOWS;
		}

		return RecordComparator.EQUIVALENT;
	}
}
