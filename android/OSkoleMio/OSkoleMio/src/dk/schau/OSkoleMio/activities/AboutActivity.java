package dk.schau.OSkoleMio.activities;

import java.io.InputStream;

import org.apache.http.util.EncodingUtils;

import dk.schau.OSkoleMio.R;

import android.app.Activity;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.webkit.WebSettings;
import android.webkit.WebView;

public class AboutActivity extends Activity {
	private final static String _ABOUTPAGE = "about.html";
	private final static String _ABOUTPAGEFULL = "file:///android_asset/" + _ABOUTPAGE;

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.about);

		WebView webView = (WebView) findViewById(R.id.aboutview);

		WebSettings webSettings = webView.getSettings();
		webSettings.setBuiltInZoomControls(true);
		webSettings.setLoadsImagesAutomatically(true);
		webSettings.setSupportZoom(true);

		try {
			String page = loadPage(_ABOUTPAGE);

			page = page.replace("@@VERSION@@", getPackageVersion());
			webView.loadDataWithBaseURL("fake://not/needed", page, "text/html", "utf-8", "");
		} catch (Exception e) {
			webView.loadUrl(_ABOUTPAGEFULL);
		}
	}

	private String loadPage(String file) throws Exception {
		InputStream inputStream = null;
		byte[] buffer = null;

		try {
			inputStream = getResources().getAssets().open(file);
			buffer = new byte[inputStream.available()];

			inputStream.read(buffer);
			inputStream.close();
			return EncodingUtils.getAsciiString(buffer);
		} finally {
			if (inputStream != null) {
				try {
					inputStream.close();
				} catch (Exception exception) { }
			}
		}
	}

	private String getPackageVersion() {
		try {
			return getPackageManager().getPackageInfo(getPackageName(), 0).versionName;
		} catch (Exception exception) { }

		return "?";
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		super.onCreateOptionsMenu(menu);
		MenuInflater inflater = getMenuInflater();
		inflater.inflate(R.menu.about_actionbar_menu, menu);
		return true;
	}

	@Override
	public boolean onOptionsItemSelected(MenuItem item) {
		switch (item.getItemId()) {
			case R.id.menu_back:
				finish();
				return true;
		}

		return super.onOptionsItemSelected(item);
	}

	public void onMenuButtonClick(View view) {
		openOptionsMenu();
    }
}
