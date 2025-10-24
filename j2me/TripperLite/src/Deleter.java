#include "Include.j"
import java.util.Date;
import javax.microedition.rms.RecordEnumeration;
import javax.microedition.rms.RecordStore;

public class Deleter {
	private RecordStore db;

	/**
	 * Setup deleter.
	 *
	 * @param rs RecordStore.
	 */
	public Deleter(RecordStore rs) {
		db=rs;
	}

	/**
	 * Run deleter.
	 *
	 * @param mid TripperLiteMIDlet.
	 * @param date Date.
	 * @param billState Bill State.
	 */
	public void delete(TripperLiteMIDlet mid, long date, int billState) {
		RecordEnumeration re=null;
		DeleterFilter df=new DeleterFilter(date, billState);

		try {
			int cnt=0;
			re=db.enumerateRecords(df, null, true);

			while (re.hasNextElement()) {
				db.deleteRecord(re.nextRecordId());
				cnt++;
			}
		} catch (Exception ex) {
			mid.popup("#=LExcCannotEnumerate", ex.getMessage());
		} finally {
			df=null;
			re=null;
			System.gc();

			mid.goToMain();
		}
	}
}
