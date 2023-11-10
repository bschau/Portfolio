#include "Include.j"
import java.util.Date;
import javax.microedition.rms.RecordFilter;

public class DeleterFilter implements RecordFilter {
	public long date;
	public int billState;

	/**
	 * Setup the DeleterFilter.
	 *
	 * @param dt Date (or -1 if date filter should be skipped).
	 * @param bs Bill State mask.
	 */
	public DeleterFilter(long dt, int bs) {
		date=dt;
		billState=bs;
	}

	/**
	 * Match the current candidate.
	 *
	 * @param c Candidate.
	 * @return true if candidate matches, false otherwise.
	 */
	public boolean matches(byte[] c) {
		if (date!=-1) {
			long l=(((long)c[1]<<56L)&0xff00000000000000L)|
		   	       (((long)c[2]<<48L)&0x00ff000000000000L)|
		   	       (((long)c[3]<<40L)&0x0000ff0000000000L)|
		   	       (((long)c[4]<<32L)&0x000000ff00000000L)|
		   	       (((long)c[5]<<24L)&0x00000000ff000000L)|
		   	       (((long)c[6]<<16L)&0x0000000000ff0000L)|
		   	       (((long)c[7]<<8L) &0x000000000000ff00L)|
		   	        ((long)c[8]      &0x00000000000000ffL);

			if (l>=date) {
				return false;
			}
			
			if (billState==0) {
				return true;
			}
		}

		int bs=c[0]&billState;
		if ((bs&#=BSClosed)==#=BSClosed) {
			return true;
		}

		if ((bs&#=BSSent)==#=BSSent) {
			return true;
		}

		if ((bs&#=BSNew)==#=BSNew) {
			return true;
		}

		return false;
	}
}
