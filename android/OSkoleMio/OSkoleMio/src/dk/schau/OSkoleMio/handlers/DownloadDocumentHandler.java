package dk.schau.OSkoleMio.handlers;

import android.app.Activity;
import android.os.Handler;
import android.os.Message;
import dk.schau.OSkoleMio.activities.WebControllerActivity;
import dk.schau.OSkoleMio.vos.Document;

public class DownloadDocumentHandler extends Handler {
	private WebControllerActivity _parent;

	public DownloadDocumentHandler(WebControllerActivity parent) {
		_parent = parent;
	}
	
	public Activity getParentActivity() {
		return _parent;
	}

	@Override
	public void handleMessage(Message message) {
		Document downloadDocumentPackage = (Document) message.obj;
		
		_parent.onDownloadFinished(downloadDocumentPackage.fullPath, downloadDocumentPackage.mimeType);
	}
}
