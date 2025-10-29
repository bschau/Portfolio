import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;

import javax.microedition.rms.RecordStore;

public class Leaderboard {
	private static final String DBNAME = "Leaderboard";
	public static final int SIZE = 5;
	private static RecordStore rs = null;
	public static long[] scores = new long[SIZE];
	public static String[] names = new String[SIZE];

	/**
	 * Init Leaderboard.
	 */
	public static void init() {
		try {
			rs = RecordStore.openRecordStore(DBNAME, true);

			if (rs.getNumRecords() > 0) {
				ByteArrayInputStream bais = null;
				DataInputStream dis = null;

				try {
					byte[] b = rs.getRecord(1);
					bais = new ByteArrayInputStream(b);
					dis = new DataInputStream(bais);

					for (int i = 0; i < SIZE; i++) {
						scores[i] = dis.readLong();
						names[i] = dis.readUTF();
					}

					return;
				} catch (Exception ex) {}
			} 
		} catch (Exception ex) {
			if (rs != null) {
				try {
					rs.closeRecordStore();
				} catch (Exception e) {}

				rs = null;
			}
		}

		scores[0] = 1000;
		names[0] = "bsc";
		scores[1] = 500;
		names[1] = "bsc";
		scores[2] = 200;
		names[2] = "bsc";
		scores[3] = 100;
		names[3] = "bsc";
		scores[4] = 50;
		names[4] = "bsc";
	}

	/**
	 * Persist Leaderboard.
	 */
	public static void persist() {
		if (rs != null) {
			ByteArrayOutputStream baos = null;
			DataOutputStream dos = null;

			try {
				baos = new ByteArrayOutputStream();
				dos = new DataOutputStream(baos);

				for (int i = 0; i < SIZE; i++) {
					dos.writeLong(scores[i]);
					dos.writeUTF(names[i]);
				}

				byte[] data = baos.toByteArray();

				if (rs.getNumRecords() > 0) {
					rs.setRecord(1, data, 0, data.length);
				} else {
					rs.addRecord(data, 0, data.length);
				}
			} catch (Exception ex) {
			} finally {
				try {
					rs.closeRecordStore();
				} catch (Exception ex) {}

				rs = null;
			}
		}
	}

	/**
	 * Find position of score in Leaderboard.
	 *
	 * @param score Score.
	 * @return position or -1 if not qualified.
	 */
	public static int qualifyPosition(long score) {
		for (int i = 0; i < SIZE; i++) {
			if (scores[i] < score) {
				int e;
				for (int j = SIZE - 1; j > i; ) {
					e = j - 1;
					scores[j] = scores[e];
					names[j] = names[e];
					j = e;
				}

				scores[i] = score;
				names[i] = "???";
				return i;
			}
		}

		return -1;
	}

	/**
	 * Set name of position X.
	 *
	 * @param idx Index of name.
	 * @param n Name.
	 */
	public static void setName(int idx, String n) {
		names[idx] = n;
	}
}
