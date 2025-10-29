package dk.schau.SchemingMind.SchemingMindActivity;

import android.app.Activity;
import android.webkit.WebChromeClient;
import android.webkit.WebView;
import dk.schau.SchemingMind.R;

public class SchemingMindWebChromeClient extends WebChromeClient {
	private Activity _activity;

	public SchemingMindWebChromeClient(Activity activity) {
		_activity = activity;		
	}

	public void onProgressChanged(WebView view, int progress) {
		_activity.setTitle("Loading ...");
		
        if (progress >= 100) {
			_activity.setTitle(R.string.app_name);
		}
	}
}
