#include "Include.j"
import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.util.Date;
import javax.microedition.rms.RecordStore;

public class Record {
	public byte billState;
	public Date date;
	public int mileage;
	public String purpose;

	/**
	 * Constructor.
	 */
	public Record() {
		initRecord();
	}

	/**
	 * Initialize a record.
	 */
	private void initRecord() {
		billState=#=BSNew;
		date=new Date();
		mileage=0;
		purpose=null;
	}

	/**
	 * Serialize this record.
	 *
	 * @return byte array with packed record.
	 * @exception Exception.
	 */
	public byte[] formRecord() throws Exception {
		ByteArrayOutputStream baos=null;
		DataOutputStream dos=null;
		byte[] b=null;

		try {
			baos=new ByteArrayOutputStream();
			dos=new DataOutputStream(baos);

			dos.writeByte(billState);
			if (date==null) {
				dos.writeLong(0);
			} else {
				dos.writeLong(date.getTime());
			}
			dos.writeInt(mileage);

			if ((purpose!=null) && (purpose.length()>0)) {
				byte[] o=purpose.getBytes();
				dos.write(o, 0, o.length);
				o=null;
			} else {
				dos.writeChar(' ');
			}
			b=baos.toByteArray();
		} catch (Exception ex) {
			throw new Exception("#=LExcFailedFormRecord "+ex.getMessage());
		} finally {
			if (dos!=null) {
				try {
					dos.close();
					dos=null;
				} catch (Exception ex) {}
			}

			if (baos!=null) {
				try {
					baos.close();
					baos=null;
				} catch (Exception ex) {}
			}

			System.gc();
		}

		return b;
	}

	/**
	 * Load "this" record from recordstore.
	 *
	 * @param db RecordStore.
	 * @param idx Record Index.
	 * @exception Exception.
	 */
	public void get(RecordStore db, int idx) throws Exception {
		ByteArrayInputStream bais=null;
		DataInputStream dis=null;
		byte[] b=null;
		long l;
		int i;

		initRecord();
		try {
			b=db.getRecord(idx);

			bais=new ByteArrayInputStream(b);
			dis=new DataInputStream(bais);

			billState=dis.readByte();
			l=dis.readLong();
			date=new Date(l);

			mileage=dis.readInt();

			if ((i=dis.available())>0) {
				b=new byte[i];

				dis.read(b);
				purpose=new String(b);
			}
			b=null;
		} catch (Exception ex) {
			throw new Exception("#=LExcFailedGetRecord "+ex.getMessage());
		} finally {
			if (dis!=null) {
				try {
					dis.close();
					dis=null;
				} catch (Exception ex) {}
			}

			if (bais!=null) {
				try {
					bais.close();
					bais=null;
				} catch (Exception ex) {}
			}

			System.gc();
		}
	}
}
