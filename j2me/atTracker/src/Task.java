import java.io.ByteArrayOutputStream;
import java.io.DataOutputStream;

import javax.microedition.rms.RecordStore;

public class Task {
	public String task;

	/**
	 * Constructor.
	 */
	public Task() {
		clear();
	}

	/**
	 * Clear task.
	 */
	public void clear() {
		task = "";
	}

	/**
	 * Pack this task record.
	 *
	 * @return byte[] with task data.
	 * @exception Exception.
	 */
	public byte[] pack() throws Exception {
		byte[] data = null;
		ByteArrayOutputStream baos = null;
		DataOutputStream dos = null;

		try {
			byte[] b;
			int l;

			baos = new ByteArrayOutputStream();
			dos = new DataOutputStream(baos);

			if ((task == null) || (task.length() == 0)) {
				b = ".".getBytes();
			} else {
				b = task.getBytes();
			}
			l = b.length;
			dos.writeByte(l);
			dos.write(b, 0, l);

			data = baos.toByteArray();
		} catch (Exception ex) {
			throw new Exception(Trl.tNoPack);
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
	 * Load a Task record from the RecordStore.
	 *
	 * @param rs RecordStore.
	 * @param idx Record index.
	 * @exception Exception.
	 */
	public void load(RecordStore rs, int idx) throws Exception {
		byte[] b = rs.getRecord(idx);
		int l = b[0];

		task = new String(b, 1, l);
	}
}
