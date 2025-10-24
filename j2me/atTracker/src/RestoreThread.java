import java.io.DataInputStream;

import java.util.Vector;

import javax.microedition.io.Connector;
import javax.microedition.io.file.FileConnection;

import javax.microedition.rms.RecordStore;

public class RestoreThread extends IOGUIThread implements Runnable {
	private DataInputStream dis;
	public RecordStore rsPeriod, rsTasks;

	/**
	 * Run the restore.
	 */
	public void run() {
		if (openFile()) {
			dis = null;
			try {
				dis = file.openDataInputStream();
				match("ATB1");
				if (error == null) {
					restoreTasks();
					if (error == null) {
						restorePeriods();
					}
				}
			} catch (Exception ex) {
			} finally {
				if (dis != null) {
					try {
						dis.close();
					} catch (Exception ex) {}
				}
			}
		}

		runThread = false;
		closeFile();

		main.restoreDone();
		thread = null;
	}

	/**
	 * Open the attracker backup file.
	 *
	 * @return true if successful, false otherwise.
	 */
	private boolean openFile() {
		try {
			file = (FileConnection)Connector.open(dst, Connector.READ);
			return true;
		} catch (Exception ex) {}

		error = Trl.rtNoOpen;
		closeFile();
		return false;
	}

	/**
	 * Match next string from input stream with this.
	 *
	 * @param m This string.
	 * @exception Exception.
	 */
	private void match(String m) throws Exception {
		error = Trl.rtNotValid;
		String s = dis.readUTF().trim();
		if (s.compareTo(m) == 0) {
			error = null;
		}
	}

	/**
	 * Restore tasks.
	 */
	private void restoreTasks() {
		firstLine = "[" + Trl.ioTasks + "]";
		try {
			match("[tasks]");
			if (error != null) {
				throw new Exception();
			}

			int cnt = Integer.parseInt(dis.readUTF()), i;
			Task t = new Task();
			byte[] b;

			for (i = 0; i < cnt; i++) {
				t.task = dis.readUTF();
				b = t.pack();
				rsTasks.addRecord(b, 0, b.length);
			}
		} catch (Exception ex) {
			error = Trl.rtTasks;
		}
	}

	/**
	 * Restore Period Data.
	 */
	private void restorePeriods() {
		firstLine= "[" + Trl.ioPeriods + "]";
		try {
			match("[periods]");
			if (error != null) {
				throw new Exception();
			}

			int cnt = Integer.parseInt(dis.readUTF()), i;
			Record r = new Record();

			for (i = 0; i < cnt; i++) {
				r.clear();
				r.period = Long.parseLong(dis.readUTF());
				r.mon = Integer.parseInt(dis.readUTF());
				r.tue = Integer.parseInt(dis.readUTF());
				r.wed = Integer.parseInt(dis.readUTF());
				r.thu = Integer.parseInt(dis.readUTF());
				r.fri = Integer.parseInt(dis.readUTF());
				r.sat = Integer.parseInt(dis.readUTF());
				r.sun = Integer.parseInt(dis.readUTF());
				r.task = dis.readUTF();
				r.save(rsPeriod, -1);
			}
		} catch (Exception ex) {
			error = Trl.rtPeriods;
		}
	}
}
