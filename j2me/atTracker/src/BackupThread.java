import java.io.DataOutputStream;

import javax.microedition.io.Connector;
import javax.microedition.io.file.FileConnection;

import javax.microedition.rms.RecordEnumeration;
import javax.microedition.rms.RecordStore;

public class BackupThread extends IOGUIThread implements Runnable {
	protected DataOutputStream dos;

	/**
	 * Run the backup thread.
	 */
	public void run() {
		if (createFile()) {
			dos = null;
			try {
				dos = file.openDataOutputStream();

				error = Trl.btWrite;
				dos.writeUTF("ATB1");
				error = null;
				dumpTasks();
				if (error == null) {
					dumpPeriods();
				}
			} catch (Exception ex) {
			} finally {
				if (dos != null) {
					try {
						dos.close();
					} catch (Exception ex) {}
				}
			}
		}

		runThread = false;

		if (error != null) {
			try {
				((FileConnection)file).delete();
			} catch (Exception ex) {}
			closeFile();

			GUI.popup(Trl.errBackup, error);
		} else {
			closeFile();
			GUI.info(Trl.btrtDone, Trl.btDone);
		}
		thread = null;
	}

	/**
	 * Create the file.
	 */
	private boolean createFile() {
		try {
			file = (FileConnection)Connector.open(dstDir);
			if (!file.exists()) {
				file.mkdir();
			}
			closeFile();

			file = (FileConnection)Connector.open(dst);
			if (file.exists()) {
				FileConnection bck = null;

				try {
					bck = (FileConnection)Connector.open(dst+"~");
					if (bck.exists()) {
						bck.delete();
					}
				} catch (Exception ex) {
				} finally {
					if (bck!=null) {
						try {
							bck.close();
						} catch (Exception ex) {}
						bck=null;
					}
				}

				file.rename("atTrackerBackup.atb~");
				closeFile();

				file = (FileConnection)Connector.open(dst);
			}

			file.create();
			return true;
		} catch (Exception ex) {
			error = Trl.btCreate + ": " + ex.getMessage() + " " + dst;
		}

		closeFile();
		return false;
	}

	/**
	 * Write tasks to backup file.
	 */
	private void dumpTasks() {
		firstLine = "[" + Trl.ioTasks + "]";
		try {
			RecordEnumeration re = main.mid.dbTasks.enumerateRecords(null, null, false);

			dos.writeUTF("[tasks]");
			dos.writeUTF(Integer.toString(re.numRecords()));
			int idx;
			Task t = new Task();
			while (re.hasNextElement()) {
				idx = re.nextRecordId();
				t.load(main.mid.dbTasks, idx);
				if (t.task.charAt(0) == '.') {
					dos.writeUTF("." + t.task);
				} else {
					dos.writeUTF(t.task);
				}
				secondLine = t.task;
				animate();
			}
		} catch (Exception ex) {
			error = Trl.btTasks;
		}
	}

	/**
	 * Write period data to backup file.
	 */
	private void dumpPeriods() {
		firstLine = "[" + Trl.ioPeriods + "]";
		try {
			RecordEnumeration re = main.mid.dbPeriod.enumerateRecords(null, null, false);

			dos.writeUTF("[periods]");
			dos.writeUTF(Integer.toString(re.numRecords()));
			int idx;
			Record r = new Record();
			PeriodPicker pp = new PeriodPicker();
			long lastPeriod = 0;

			while (re.hasNextElement()) {
				idx = re.nextRecordId();
				r.load(main.mid.dbPeriod, idx);
				dos.writeUTF(Long.toString(r.period));
				dos.writeUTF(Integer.toString(r.mon));
				dos.writeUTF(Integer.toString(r.tue));
				dos.writeUTF(Integer.toString(r.wed));
				dos.writeUTF(Integer.toString(r.thu));
				dos.writeUTF(Integer.toString(r.fri));
				dos.writeUTF(Integer.toString(r.sat));
				dos.writeUTF(Integer.toString(r.sun));
				dos.writeUTF(r.task);

				if (r.period != lastPeriod) {
					pp.setStartOfPeriod(r.period);
					secondLine = pp.toString();
				}
				animate();
			}
		} catch (Exception ex) {
			error = Trl.btPeriods;
		}
	}
}
