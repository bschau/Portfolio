package dk.schau.OSkoleMio.threads;

import org.apache.http.HttpEntity;
import org.apache.http.HttpResponse;
import org.apache.http.client.methods.HttpGet;
import org.apache.http.impl.client.DefaultHttpClient;
import org.apache.http.util.EntityUtils;

import android.os.Message;
import dk.schau.OSkoleMio.handlers.DownloadSchoolsHandler;

public class DownloadSchoolsThread extends Thread {
	private final String _UPDATEFILE = "http://www.oskolemio.dk/schools.xml";
	private DownloadSchoolsHandler _downloadSchoolsHandler;
	
	public DownloadSchoolsThread(DownloadSchoolsHandler downloadSchoolsHandler) {
		_downloadSchoolsHandler = downloadSchoolsHandler;
	}
	
	@Override
	public void run() {
		Message message = new Message();
		message.obj = download();

		_downloadSchoolsHandler.sendMessage(message);
	}

	private String download() {
		try {
			DefaultHttpClient httpClient = new DefaultHttpClient();
			HttpGet request = new HttpGet(_UPDATEFILE);

			HttpResponse response = httpClient.execute(request);
			HttpEntity entity = response.getEntity();

			return EntityUtils.toString(entity, "UTF-8");
		} catch (Exception e) { }

		return null;
	}
}