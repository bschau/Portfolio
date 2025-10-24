package dk.schau.SchemingMind.AboutActivity;

import java.io.InputStream;

import android.app.Activity;
import android.view.Menu;
import android.view.MenuItem;
import dk.schau.SchemingMind.R;
import dk.schau.SchemingMind.SchemingMindActivity.SchemingMindActivity;
import org.apache.http.util.EncodingUtils;

import android.content.Intent;
import android.content.pm.PackageInfo;
import android.os.Bundle;
import android.view.MenuInflater;
import android.webkit.WebSettings;
import android.webkit.WebView;

public class AboutActivity extends Activity {
	private final static String ABOUT_PAGE = "about.html";
	private final static String ABOUT_PAGE_FULL = "file:///android_asset/" + ABOUT_PAGE;

	@Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.about);

        WebView wv = (WebView) findViewById(R.id.aboutview);

        WebSettings ws = wv.getSettings();
        ws.setBuiltInZoomControls(true);
        ws.setLoadsImagesAutomatically(true);
        ws.setSupportZoom(true);

        try {
        	String page = loadPage(ABOUT_PAGE);

        	page = page.replace("@@VERSION@@", getPackageVersion());
        	wv.loadDataWithBaseURL("fake://not/needed", page, "text/html", "utf-8", "");
        } catch (Exception e) {
        	wv.loadUrl(ABOUT_PAGE_FULL);
        }
	}
	
	@Override
 	public boolean onCreateOptionsMenu(Menu menu) {
		super.onCreateOptionsMenu(menu);
		MenuInflater inflater = getMenuInflater();
		inflater.inflate(R.menu.submenu, menu);
		return true;
 	}
	
	@Override
 	public boolean onOptionsItemSelected(MenuItem item) {
		switch (item.getItemId()) {
            case R.id.back:
                Intent intent = new Intent(this, SchemingMindActivity.class);
                intent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
                startActivity(intent);
                return true;
		}

		return false;
	}
	
	private String loadPage(String file) throws Exception {
        InputStream is = null;
        byte[] buffer;
 
        try {
        	is = getResources().getAssets().open(file);
        	buffer = new byte[is.available()];  

        	is.read(buffer);
        	is.close();
        	return EncodingUtils.getAsciiString(buffer);
        } finally {
        	if (is != null) {
        		try {
        			is.close();
        		} catch (Exception e) {
        		}
        	}
        }        
	}
	
	private String getPackageVersion() {
		try {
			PackageInfo packageInfo = getPackageManager().getPackageInfo(getPackageName(), 0); 
			return packageInfo.versionName;
		} catch (Exception e) { 
		}

		return "?";	
	}
}
