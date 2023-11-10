import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;

import java.util.Calendar;

import javax.microedition.rms.RecordStore;

public class Record {
	private final int DAYSHIFT = 13;
	public int sun, mon, tue, wed, thu, fri, sat;
	public String task;
	public long period;

	/**
	 * Constructor.
	 */
	public Record() {
		clear();
	}

	/**
	 * Clear the current record.
	 */
	public void clear() {
		task = "";
		sun = 0;
		mon = 0;
		tue = 0;
		wed = 0;
		thu = 0;
		fri = 0;
		sat = 0;
		period = 0;
	}

	/**
	 * Parse a time entry.
	 *
	 * @param t Time entry.
	 * @return time in minutes.
	 */
	public int parseTime(String t) {
		if (t == null) {
			return 0;
		}

		t = t.trim();

		int len = t.length(), h = 0, m = 0;
		String s;
		try {
			switch (len) {
				case 0:
					return 0;

				case 1:	// hours H
					h = Integer.parseInt(t);
					break;

				case 2:	// hours HH
					h = Integer.parseInt(t);
					break;

				case 3:	// hours + minutes HMM
					s = t.substring(0, 1);
					h = Integer.parseInt(s);
					s = t.substring(1);
					m = Integer.parseInt(s);
					break;

				default: // hours + minutes HHMM
					s = t.substring(0, 2);
					h = Integer.parseInt(s);
					s = t.substring(2);
					m = Integer.parseInt(s);
					break;
			}
		} catch (Exception ex) {}
					
		int x = (h * 60) + m;

		if (x > (24 * 60)) {
			return (24 * 60);
		}

		return x;
	}
	
	/**
	 * Save the record.
	 *
	 * @param rs RecordStore.
	 * @param recIdx Index (or -1 for new record).
	 * @exception Exception.
	 */
	public void save(RecordStore rs, int recIdx) throws Exception {
		if ((sun == 0) && (mon == 0) && (tue == 0) && (wed == 0) &&
		    (thu == 0) && (fri == 0) && (sat == 0)) {
			return;
		}

		byte[] b = pack();

		if (recIdx == -1) {
			rs.addRecord(b, 0, b.length);
		} else {
			rs.setRecord(recIdx, b, 0, b.length);
		}
	}

	/**
	 * Pack record data to a byte[].
	 *
	 * @return byte[].
	 * @exception Exception.
	 */
	private byte[] pack() throws Exception {
		ByteArrayOutputStream baos = null;
		DataOutputStream dos = null;
		byte[] data = null;

		try {
			byte[] b;
			int l;

			baos = new ByteArrayOutputStream();
			dos = new DataOutputStream(baos);

			dos.writeLong(period);
			
			write(dos, Calendar.SUNDAY, sun);
			write(dos, Calendar.MONDAY, mon);
			write(dos, Calendar.TUESDAY, tue);
			write(dos, Calendar.WEDNESDAY, wed);
			write(dos, Calendar.THURSDAY, thu);
			write(dos, Calendar.FRIDAY, fri);
			write(dos, Calendar.SATURDAY, sat);

			if ((task == null) || (task.length() == 0)) {
				write(dos, ".");
			} else {
				write(dos, task);
			}

			data = baos.toByteArray();
		} catch (Exception ex) {
			throw new Exception(Trl.rNoPack);
		} finally {
			if (dos != null) {
				try {
					dos.close();
				} catch (Exception ex) {}
			}

			if (baos != null) {
				try {
					baos.close();
				} catch (Exception ex) {}
			}

			dos = null;
			baos = null;
		}

		return data;
	}

	/**
	 * Write a string to the output stream.
	 *
	 * @param dos Output stream.
	 * @param src Source string.
	 * @exception Exception.
	 */
	private void write(DataOutputStream dos, String src) throws Exception {
		byte[] b = src.getBytes();
		int l = b.length;

		dos.writeByte(l);
		dos.write(b, 0, l);
	}

	/**
	 * Write a day/value combo to the output stream.
	 *
	 * @param dos Output stream.
	 * @param day Day.
	 * @param val Value.
	 * @exception Exception.
	 */
	private void write(DataOutputStream dos, int day, int val) throws Exception {
		val |= (day << DAYSHIFT);
		dos.writeShort(val);
	}

	/**
	 * Load a record from the RecordStore.
	 *
	 * @param rs RecordStore.
	 * @param recIdx Record index.
	 * @exception Exception.
	 */
	public void load(RecordStore rs, int recIdx) throws Exception {
		ByteArrayInputStream bais = null;
		DataInputStream dis = null;

		clear();
		try {
			byte[] b = rs.getRecord(recIdx);
			byte[] str;
			int l, i;
			short s;

			bais = new ByteArrayInputStream(b);
			dis = new DataInputStream(bais);

			period = dis.readLong();

			for (i = 0; i < 7; i++) {
				s = dis.readShort();
				l = s & ((1 << 13) - 1);

				switch ((s >> 13 ) & 7) {
					case Calendar.SUNDAY:
						sun = l;
						break;
					case Calendar.MONDAY:
						mon = l;
						break;
					case Calendar.TUESDAY:
						tue = l;
						break;
					case Calendar.WEDNESDAY:
						wed = l;
						break;
					case Calendar.THURSDAY:
						thu = l;
						break;
					case Calendar.FRIDAY:
						fri = l;
						break;
					case Calendar.SATURDAY:
						sat = l;
						break;
				}
			}

			if (dis.available() > 0) {
				task = read(dis);
			} else {
				task = ".";
			}
		} catch (Exception ex) {
			throw new Exception(Trl.rNoLoad + ": " + ex.getMessage());
		} finally {
			if (dis != null) {
				try {
					dis.close();
				} catch (Exception ex) {}
			}

			if (bais != null) {
				try {
					bais.close();
				} catch (Exception ex) {}
			}

			dis = null;
			bais = null;
		}
	}

	/**
	 * Read a string from the input stream.
	 *
	 * @param dis Input stream.
	 * @return String.
	 * @exception Exception.
	 */
	private String read(DataInputStream dis) throws Exception {
		int l = (int)dis.readByte();
		if (l > 0) {
			byte[] str = new byte[l];

			dis.read(str);
			return new String(str);
		}

		return "";
	}
}
