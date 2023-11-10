package dk.schau.vkm.activity;

import dk.schau.vkm.R;
import dk.schau.vkm.util.Locale;
import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.webkit.WebSettings;
import android.webkit.WebView;
import android.widget.TextView;

public class AboutActivity extends Activity{
	private final static String _ABOUTPAGESPATH = "file:///android_asset/about-";

	@Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        this.requestWindowFeature(Window.FEATURE_NO_TITLE);
        this.getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
        setContentView(R.layout.aboutactivity);

        String title = "5km";
        try
        {
        	title = "5km\nv" + getPackageVersion();
        }
        catch (Exception ex) {}
        
        ((TextView) findViewById(R.id.titletext)).setText(title);
        
        WebView webView = (WebView) findViewById(R.id.aboutwebview);

    	WebSettings webSettings = webView.getSettings();
    	webSettings.setBuiltInZoomControls(true);
    	webSettings.setLoadsImagesAutomatically(true);
    	webSettings.setSupportZoom(true);

    	if (Locale.getLocale().compareTo("da") == 0)
    	{
    		webView.loadUrl(_ABOUTPAGESPATH + "da.html");    		
    	}
    	else
    	{
    		webView.loadUrl(_ABOUTPAGESPATH + "en.html");
    	}
    }

	private String getPackageVersion()
	{
		try
		{
			return getPackageManager().getPackageInfo(getPackageName(), 0).versionName;
		}
		catch (Exception exception)
		{
		}

		return "?";
	}

    public void onButtonClick(View view) {
    	switch (view.getId()) {
    		case R.id.backbutton:
    			startActivity(new Intent(this, VkmActivity.class));
    			break;

    		case R.id.emailbutton:
    			sendEmail();
    			break;
    	}
    }
    
	private void sendEmail()
	{
		Intent email = new Intent(Intent.ACTION_SEND);
		email.setType("message/rfc822");
		email.putExtra(Intent.EXTRA_EMAIL, new String[] { getString(R.string.email_to) });
		email.putExtra(Intent.EXTRA_SUBJECT, getString(R.string.email_subject));
		email.putExtra(Intent.EXTRA_TEXT, Locale.get("EMAILBODY"));
		
		startActivity(Intent.createChooser(email, getString(R.string.email_send)));
	}
}
