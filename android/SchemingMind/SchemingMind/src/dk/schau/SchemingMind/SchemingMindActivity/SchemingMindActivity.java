package dk.schau.SchemingMind.SchemingMindActivity;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.view.*;
import android.webkit.CookieSyncManager;
import android.webkit.WebSettings;
import android.webkit.WebView;
import dk.schau.SchemingMind.AboutActivity.AboutActivity;
import dk.schau.SchemingMind.CredentialsActivity.CredentialsActivity;
import dk.schau.SchemingMind.Preferences;
import dk.schau.SchemingMind.R;

public class SchemingMindActivity extends Activity {
	private Thread siteThread = null;
	private Preferences _preferences;
	private WebView _webView;
	private String _username;
	private String _password;
	private String _site;
	protected String _url;
	protected byte[] _postData;	
	protected String _errorMessage;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        getWindow().setFeatureInt(Window.FEATURE_PROGRESS, Window.PROGRESS_VISIBILITY_ON);
        setContentView(R.layout.main);

        CookieSyncManager.createInstance(this);

        _preferences = new Preferences(this);
        _username = _preferences.getUsername();
        _password = _preferences.getPassword();
        
        if (_username.length() == 0 || _password.length() == 0) {
        	startActivity(new Intent(this, CredentialsActivity.class));
        	return;
        }
        
        verifyCredential(_username, "username");
        verifyCredential(_password, "password");        

        _site = _preferences.getSite();

        initializeWebView();
        
        loadSite();
    }
    
    private void initializeWebView() {
        _webView = (WebView) findViewById(R.id.webview);
        WebSettings ws = _webView.getSettings();
  
        ws.setJavaScriptEnabled(true);
        ws.setJavaScriptCanOpenWindowsAutomatically(true);
        ws.setLoadsImagesAutomatically(true);
        ws.setBuiltInZoomControls(true);
        ws.setSupportZoom(true);
    }

    private void loadSite() {
    	stopThread();

    	_webView.setWebChromeClient(new SchemingMindWebChromeClient(this));

        SchemingMindWebViewClient schemingMindWebViewClient = new SchemingMindWebViewClient(this);
        boolean isDesktop = _site.equals("desktop");
        schemingMindWebViewClient.showProgressDialog(getString(isDesktop ? R.string.loading_desktop : R.string.loading_mobile));
        _webView.setWebViewClient(schemingMindWebViewClient);

        SchemingMindClient schemingMindClient = isDesktop ? new WwwSchemingMindClient() : new MobileSchemingMindClient();
        schemingMindClient.setSchemingMindActivity(this);
        schemingMindClient.setUsername(_username);
        schemingMindClient.setPassword(_password);
        	
        siteThread = new Thread(schemingMindClient);
        siteThread.start();
    }
    
    private void stopThread() {
    	if (siteThread != null) {
    		siteThread.interrupt();
    		siteThread = null;
    	}
    }
    
    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        MenuInflater inflater = getMenuInflater();

        inflater.inflate(R.menu.mainmenu, menu);
        return true;
    }
    
    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        stopThread();
        switch (item.getItemId()) {
            case R.id.mobile:
                switchView("mobile");
                break;

            case R.id.desktop:
                switchView("desktop");
                break;

            case R.id.credentials:
            	startActivity(new Intent(this, CredentialsActivity.class));
                break;

            case R.id.about:
            	startActivity(new Intent(this, AboutActivity.class));
                break;

			default:
                return super.onOptionsItemSelected(item);
        }

        return true;
    }


    private void switchView(String site) {
        _site = site;
        _preferences.setSite(site);
        loadSite();
    }

	@Override
	public void onResume() {
		super.onResume();
		
		CookieSyncManager.getInstance().startSync();
	}
	
	@Override
	public void onPause() {
		super.onPause();

		CookieSyncManager.getInstance().stopSync();
	}
	
	@Override
	protected void onSaveInstanceState(Bundle outState) {
		((WebView) findViewById(R.id.webview)).saveState(outState);
	}
	
	@Override
	protected void onRestoreInstanceState(Bundle inState) {
		((WebView) findViewById(R.id.webview)).restoreState(inState);
	}

	@Override
	public boolean onKeyDown(int keyCode, KeyEvent event) {
	    if (keyCode == KeyEvent.KEYCODE_BACK && _webView.canGoBack()) {
        	stopThread();
	        _webView.goBack();
	        return true;
	    }

	    return super.onKeyDown(keyCode, event);
	}
	
	private void verifyCredential(String value, String credential) {
		if (value != null && value.trim().length() > 0) {
			return;
		}

		AlertDialog.Builder builder = new AlertDialog.Builder(this);

		builder.setMessage("Missing " + credential + "! Please set the correct " + credential)
		.setCancelable(false)
		.setPositiveButton("OK", new DialogInterface.OnClickListener() {
			public void onClick(DialogInterface dialog, int id) {
				startActivity(new Intent(getApplicationContext(), CredentialsActivity.class));
			}
		});

		AlertDialog alert = builder.create();
		alert.show();
	}
	
	protected void loadWebView(String url, byte[] postData) {
		_url = url;
		_postData = postData;
	
		runOnUiThread(new Runnable() {
        	public void run() {
        		_webView.postUrl(_url, _postData);
        	}
        });
	}
	
	protected void setErrorMessage(String errorMessage) {
		_errorMessage = errorMessage;
		
		runOnUiThread(new Runnable() {
			public void run() {
				_webView.loadDataWithBaseURL("fake://not/needed", _errorMessage, "text/html", "utf-8", "");
			}
		});
	}
}