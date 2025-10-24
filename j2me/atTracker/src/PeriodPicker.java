import java.util.Calendar;
import java.util.Date;

public class PeriodPicker {
	private final String[] months = new String[] { Trl.jan, Trl.feb, Trl.mar, Trl.apr, Trl.may, Trl.jun, Trl.jul, Trl.aug, Trl.sep, Trl.oct, Trl.nov, Trl.dec };
	private StringBuffer sBuf = new StringBuffer();
	private Calendar start, end;

	/**
	 * Initialize a new PeriodPicker.
	 */
	public PeriodPicker() {
		start = Calendar.getInstance();
		end = Calendar.getInstance();

		reset();
	}

	/**
	 * Reset the period to the current date.
	 */
	public void reset() {
		start.setTime(new Date());
		adjustPeriod();
	}

	/**
	 * Set start of period.
	 *
	 * @param s Start date.
	 */
	public void setStartOfPeriod(long s) {
		start.setTime(new Date(s));
		adjustPeriod();
	}

	/**
	 * Set period start/end.
	 */
	private void adjustPeriod() {
		start.set(Calendar.HOUR_OF_DAY, 1);
		start.set(Calendar.MINUTE, 0);
		start.set(Calendar.SECOND, 0);
		start.set(Calendar.MILLISECOND, 0);

		int f = start.get(Calendar.DAY_OF_WEEK);
		if (f > 2) {
			long t = ((Date)start.getTime()).getTime();

			t -= (f - 2) * (60 * 60 * 24) * 1000;
			start.setTime(new Date(t));
		}

		setEnd();
	}

	/**
	 * Set end period (based on start period).
	 */
	private void setEnd() {
		long t = ((Date)start.getTime()).getTime();
		t += Const.PeriodEndStride;
		end.setTime(new Date(t));
	}

	/**
	 * Go to the previous period.
	 */
	public void prev() {
		long t = ((Date)start.getTime()).getTime();
		t -= 7 * 60 * 60 * 24 * 1000;
		start.setTime(new Date(t));

		setEnd();
	}

	/**
	 * Go to the next period.
	 */
	public void next() {
		long t = ((Date)start.getTime()).getTime();
		t += 7 * 60 * 60 * 24 * 1000;
		start.setTime(new Date(t));

		setEnd();
	}

	/**
	 * Return this period as a string.
	 *
	 * @return string.
	 */
	public String toString() {
		int m;

		sBuf.delete(0, sBuf.length());

		sBuf.append(start.get(Calendar.YEAR));
		sBuf.append(' ');
		sBuf.append(start.get(Calendar.DAY_OF_MONTH));
		sBuf.append(' ');
		sBuf.append(months[start.get(Calendar.MONTH)]);

		sBuf.append(" - ");
		sBuf.append(end.get(Calendar.DAY_OF_MONTH));
		sBuf.append(' ');
		sBuf.append(months[end.get(Calendar.MONTH)]);

		return sBuf.toString();
	}

	/**
	 * Return start date in 'raw format'.
	 *
	 * @return period start as a long.
	 */
	public long getPeriodStart() {
		return ((Date)start.getTime()).getTime();
	}

	/**
	 * Return end date in 'raw format'.
	 *
	 * @return period end as a long.
	 */
	public long getPeriodEnd() {
		return ((Date)end.getTime()).getTime();
	}
}
