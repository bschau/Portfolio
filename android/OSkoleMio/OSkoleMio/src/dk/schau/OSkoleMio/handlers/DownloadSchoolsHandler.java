package dk.schau.OSkoleMio.handlers;

import dk.schau.OSkoleMio.activities.SchoolPickerActivity;
import android.os.Handler;
import android.os.Message;

public class DownloadSchoolsHandler extends Handler {
	private SchoolPickerActivity _parent;
	
	public DownloadSchoolsHandler(SchoolPickerActivity parent) {
		_parent = parent;
	}
	
	@Override
	public void handleMessage(Message message) {
		_parent.onDownloadFinished((String) message.obj);
	}
}
