import javax.microedition.rms.RecordFilter;

public class PeriodDataFilter implements RecordFilter {
	private long start, end;

	/**
	 * Set period.
	 *
	 * @param s Start.
	 * @parem e End.
	 */
	public void setPeriod(long s, long e) {
		start=s;
		end=e;
	}

	/**
	 * Filter based on period.
	 *
	 * @param r Record.
	 * @return true if record matches, false otherwise.
	 */
	public boolean matches(byte[] r) {
		long l;

		l = (((long)r[0] << 56L) & 0xff00000000000000L)|
		    (((long)r[1] << 48L) & 0x00ff000000000000L)|
		    (((long)r[2] << 40L) & 0x0000ff0000000000L)|
		    (((long)r[3] << 32L) & 0x000000ff00000000L)|
		    (((long)r[4] << 24L) & 0x00000000ff000000L)|
		    (((long)r[5] << 16L) & 0x0000000000ff0000L)|
		    (((long)r[6] << 8L)  & 0x000000000000ff00L)|
		     ((long)r[7]         & 0x00000000000000ffL);

		if ((l >= start) && (l <= end)) {
			return true;
		}

		return false;
	}
}
