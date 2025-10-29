package dk.schau.OSkoleMio.threads;

import java.io.File;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.net.URI;

import org.apache.http.HttpEntity;
import org.apache.http.HttpResponse;
import org.apache.http.client.methods.HttpGet;
import org.apache.http.impl.client.DefaultHttpClient;

import android.os.Message;
import android.webkit.CookieManager;
import android.webkit.CookieSyncManager;
import dk.schau.OSkoleMio.SchoolsCollection;
import dk.schau.OSkoleMio.handlers.DownloadDocumentHandler;
import dk.schau.OSkoleMio.vos.Document;

public class DownloadDocumentThread extends Thread {
	private DownloadDocumentHandler _downloadDocumentHandler;
	private CookieManager _cookieManager;
	private String _url;
	private String _mimeType;
	
	public DownloadDocumentThread(DownloadDocumentHandler downloadDocumentHandler, CookieManager cookieManager, String url, String mimeType) {
		_downloadDocumentHandler = downloadDocumentHandler;
		_cookieManager = cookieManager;
		_url = url;
		_mimeType = mimeType;
	}
	
	@Override
	public void run() {
		Message message = new Message();
		String fullPath = download(_url);
		message.obj = new Document(fullPath, _mimeType);
		
		_downloadDocumentHandler.sendMessage(message);
	}
	
	private String download(String url) {
		url = url.replaceAll("\\[", "%5B");
		url = url.replaceAll("\\]", "%5D");
		String file = url.substring(url.lastIndexOf('/') + 1);
		InputStream inputStream = null;
		FileOutputStream fileOutputStream = null;

		try {
			URI uri = new URI(url);
			CookieSyncManager.getInstance().sync();
			HttpGet get = new HttpGet(uri);

			if (_cookieManager.hasCookies()) {
				String[] cookies = _cookieManager.getCookie(uri.getScheme() + "://" + uri.getHost()).split(";");

				for (int index = 0; index < cookies.length; index++) {
					get.addHeader("Cookie", cookies[index].trim());
				}
			}

			HttpResponse response = new DefaultHttpClient().execute(get);
			HttpEntity entity = response.getEntity();
			inputStream = entity.getContent();

			if (entity.getContentLength() > 0) {
				File outputFile = getFilePath(file);
				fileOutputStream = new FileOutputStream(outputFile);

				byte[] buffer = new byte[8192];
				int length;

				while ((length = inputStream.read(buffer)) != -1) {
					fileOutputStream.write(buffer, 0, length);
				}

				return outputFile.getAbsolutePath();
			}
		} catch (Exception ex) {
		} finally {
			if (fileOutputStream != null) {
				try {
					fileOutputStream.close();
				} catch (Exception ex) { }
			}

			if (inputStream != null) {
				try {
					inputStream.close();
				} catch (Exception ex) { }
			}
		}

		return null;
	}
	
	private File getFilePath(String file) {
		return new File(SchoolsCollection.getExternalFilesDir(_downloadDocumentHandler.getParentActivity()), file);
	}
}