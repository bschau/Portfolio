/*
vim: ts=2 sw=2 et
*/
import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import javax.microedition.rms.RecordStore;
import javax.microedition.rms.RecordStoreException;

public class Preferences {
	private static final String STORENAME="Preferences";
	private RecordStore r=null;
	private int gameA=0, gameB=0, difficulty=0;

	public Preferences() {
		try {
			r=RecordStore.openRecordStore(STORENAME, true);
		} catch (RecordStoreException rse) {
			rse.printStackTrace();
		}
	}

	public void GetPreferences() {
		if (r!=null) {
			try {
				if (r.getNumRecords()>0) {
					byte[] b=r.getRecord(1);
					DataInputStream d=new DataInputStream(new ByteArrayInputStream(b));
					try {
						gameA=d.readInt();
						gameB=d.readInt();
						difficulty=d.readInt();
					} catch (IOException ex) {
						ex.printStackTrace();
					}
				}
			} catch (RecordStoreException rse) {
				rse.printStackTrace();
			}
		}
	}

	public void Close() {
		if (r!=null) {
			try {
				r.closeRecordStore();
			} catch (RecordStoreException rse) {
				rse.printStackTrace();
			}
		}
	}

	public int GetScoreA() {
		return gameA;
	}

	public int GetScoreB() {
		return gameB;
	}

	public int GetDifficulty() {
		return difficulty;
	}

	public synchronized void UpdatePreferences(int sA, int sB, int diff) {
		gameA=sA;
		gameB=sB;
		difficulty=diff;
		if (r!=null) {
			ByteArrayOutputStream b=new ByteArrayOutputStream();
			DataOutputStream d=new DataOutputStream(b);

			try {
				d.writeInt(gameA);
				d.writeInt(gameB);
				d.writeInt(difficulty);
			} catch (IOException ex) {
				ex.printStackTrace();
			}

			byte[] a=b.toByteArray();
			try {
				if (r.getNumRecords()>0) {
					r.setRecord(1, a, 0, a.length);
        } else {
					r.addRecord(a, 0, a.length);
        }

			} catch (RecordStoreException rse) {
				rse.printStackTrace();
			}
		}
	}
}
