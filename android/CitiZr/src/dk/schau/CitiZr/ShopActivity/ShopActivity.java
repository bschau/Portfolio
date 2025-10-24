package dk.schau.CitiZr.ShopActivity;

import java.util.ArrayList;
import android.net.http.SslError;

import java.util.List;
import java.util.Locale;

import dk.schau.CitiZr.CitiZrActivity.CitiZrActivity;
import dk.schau.CitiZr.CredentialsActivity.CredentialsActivity;
import dk.schau.CitiZr.InstructionsActivity.InstructionsActivity;
import org.apache.http.client.entity.UrlEncodedFormEntity;
import org.apache.http.message.BasicNameValuePair;
import org.apache.http.util.EntityUtils;
import org.apache.http.NameValuePair;

import dk.schau.CitiZr.R;
import dk.schau.CitiZr.Dialog.AboutDialog;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.pm.PackageManager.NameNotFoundException;
import android.os.Bundle;
import android.view.KeyEvent;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.Window;
import android.webkit.CookieSyncManager;
import android.webkit.WebChromeClient;
import android.webkit.WebSettings;
import android.webkit.WebView;
import android.webkit.WebViewClient;
import android.widget.Toast;
import android.webkit.SslErrorHandler;

public class ShopActivity extends Activity {
	private WebView _webview;

	@Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        getWindow().setFeatureInt(Window.FEATURE_PROGRESS, Window.PROGRESS_VISIBILITY_ON);
        getWindow().requestFeature(Window.FEATURE_PROGRESS);
        getWindow().requestFeature(Window.PROGRESS_VISIBILITY_ON);

		SharedPreferences sp = getSharedPreferences("CitiZr", Context.MODE_PRIVATE);

		String login = sp.getString("CITIZR_LOGIN", "");
        verify(login, getString(R.string.shop_username));
        String password = sp.getString("CITIZR_PASSWORD", "");
        verify(password, getString(R.string.shop_password));

        setContentView(R.layout.nespressoclub);
        getWindow().setFeatureInt((int) Window.FEATURE_PROGRESS, Window.PROGRESS_VISIBILITY_ON);

        CookieSyncManager.createInstance(this);

        initializeWebView();
        resetProgress();

        List<NameValuePair> params = new ArrayList<NameValuePair>();
        params.add(new BasicNameValuePair("capsule", ""));
        params.add(new BasicNameValuePair("appname", "nese3"));
        params.add(new BasicNameValuePair("prgname", "loginMember"));
        params.add(new BasicNameValuePair("arguments", ""));
        params.add(new BasicNameValuePair("hdn_nestor2nesclub", "1"));
        params.add(new BasicNameValuePair("hdn_items", ""));
        params.add(new BasicNameValuePair("hdn_url2redir", ""));
        params.add(new BasicNameValuePair("ed_login", login));
        params.add(new BasicNameValuePair("ed_pwd", password));

        if (Locale.getDefault().getCountry().toLowerCase().equals("dk")) {
            params.add(new BasicNameValuePair("pd_lang", "DA"));
        } else {
            params.add(new BasicNameValuePair("pd_lang", "EN"));
        }
        try {
        	UrlEncodedFormEntity entity = new UrlEncodedFormEntity(params);

        	_webview.postUrl(getString(R.string.url), EntityUtils.toByteArray(entity));
        } catch (Exception ignored) {
        }
	}
	
	private void initializeWebView() {
		_webview = (WebView) findViewById(R.id.webview);
		WebSettings ws = _webview.getSettings();
		ws.setJavaScriptEnabled(true);
		ws.setJavaScriptCanOpenWindowsAutomatically(true);
		ws.setLoadsImagesAutomatically(true);
		ws.setBuiltInZoomControls(true);
		ws.setSupportZoom(true);

        _webview.setWebViewClient(new WebViewClient() {
        	public void onReceivedError(WebView view, int errorCode, String description, String failingUrl) {
        		Toast.makeText(getApplicationContext(), errorCode + ": " + description + " / " + failingUrl, Toast.LENGTH_LONG).show();
        	}

        	// Using the very good trick from:
        	// http://damianflannery.wordpress.com/2010/09/28/android-webview-with-https-loadurl-shows-blankempty-page/
        	public void onReceivedSslError(WebView view, SslErrorHandler handler, SslError error) {
        		handler.proceed();
        	}
        	
            @Override  
            public boolean shouldOverrideUrlLoading(WebView view, String url) {  
              view.loadUrl(url);
              return true;
            }
        });
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

	private void resetProgress() {
        _webview.setWebChromeClient(new WebChromeClient() {
        	public void onProgressChanged(WebView view, int progress) {
        		setTitle(getString(R.string.shop_fetching));
        		setProgress(progress * 100);

        		if (progress >= 100) {
        			setTitle(R.string.app_name);
        		}
        	}
        });
	}

	@Override
	public boolean onKeyDown(int keyCode, KeyEvent event) {
	    if ((keyCode == KeyEvent.KEYCODE_BACK) && _webview.canGoBack()) {
	        _webview.goBack();
	        return true;
	    }

	    return super.onKeyDown(keyCode, event);
	}

	private void verify(String str, String err) {
		if (str != null && str.trim().length() > 0) {
			return;
		}

		AlertDialog.Builder builder = new AlertDialog.Builder(this);

		builder.setMessage(getString(R.string.shop_error) + " " + err + "!")
		.setCancelable(false)
		.setPositiveButton(getString(R.string.ok), new DialogInterface.OnClickListener() {
			public void onClick(DialogInterface dialog, int id) {
				startActivity(new Intent(getApplicationContext(), CredentialsActivity.class));
			}
		});

		AlertDialog alert = builder.create();
		alert.show();
	}
	
	@Override
 	public boolean onCreateOptionsMenu(Menu menu) {
		super.onCreateOptionsMenu(menu);
		MenuInflater inflater = getMenuInflater();
		inflater.inflate(R.menu.shopmenu, menu);
		return true;
 	}
	
	@Override
 	public boolean onOptionsItemSelected(MenuItem item) {
		switch (item.getItemId()) {
			case R.id.list:
				startActivity(new Intent(this, CitiZrActivity.class));
				return true;
	
			case R.id.instructions:
				startActivity(new Intent(this, InstructionsActivity.class));
				return true;
	
			case R.id.addlogin:
				startActivity(new Intent(this, CredentialsActivity.class));
				return true;
	
			case R.id.about:
				try {
					AboutDialog.show(this);
				} catch (NameNotFoundException e) {
					e.printStackTrace();
				}
				return true;
		}

		return false;
	}
}
