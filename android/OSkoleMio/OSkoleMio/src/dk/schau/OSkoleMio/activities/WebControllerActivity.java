package dk.schau.OSkoleMio.activities;

import java.io.File;
import java.util.ArrayList;
import java.util.List;

import org.apache.http.NameValuePair;
import org.apache.http.client.entity.UrlEncodedFormEntity;
import org.apache.http.message.BasicNameValuePair;
import org.apache.http.util.EntityUtils;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.app.AlertDialog;
import android.app.ProgressDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.res.Configuration;
import android.net.Uri;
import android.os.Bundle;
import android.view.KeyEvent;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.Window;
import android.webkit.CookieManager;
import android.webkit.CookieSyncManager;
import android.webkit.DownloadListener;
import android.webkit.WebChromeClient;
import android.webkit.WebSettings;
import android.webkit.WebView;
import android.webkit.WebViewClient;
import android.widget.FrameLayout;
import android.widget.Toast;

import dk.schau.OSkoleMio.DB;
import dk.schau.OSkoleMio.Danish;
import dk.schau.OSkoleMio.R;
import dk.schau.OSkoleMio.handlers.DownloadDocumentHandler;
import dk.schau.OSkoleMio.threads.DownloadDocumentThread;
import dk.schau.OSkoleMio.vos.Login;
import android.webkit.SslErrorHandler;
import android.net.http.SslError;

public class WebControllerActivity extends Activity {
	private ProgressDialog _progressDialog = null;
	protected int _loginId;
	private WebView _webView;
	private WebControllerActivity _webControllerActivity;
	private Login _login;
	private CookieManager _cookieManager;
	private FrameLayout _webViewPlaceHolder;

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		if (getIntent().hasExtra("id") == false) {
			this.finish();
			return;
		}

		Bundle bundle = getIntent().getExtras();
		DB database = new DB(this);
		_login = database.getLoginById(database.getReadableDatabase(), bundle.getInt("id"));
		database.close();

		if (_login == null) {
			this.finish();
			return;
		}

		getWindow().requestFeature(Window.FEATURE_PROGRESS);
		getWindow().requestFeature(Window.PROGRESS_VISIBILITY_ON);
		
		setContentView(R.layout.webview);
		getWindow().setFeatureInt((int) Window.FEATURE_PROGRESS, Window.PROGRESS_VISIBILITY_ON);

		verify(_login.getId(), _login.getUrl(), "ForældreIntra Mobil URL");
		verify(_login.getId(), _login.getLogin(), "Brugernavn");
		verify(_login.getId(), _login.getPassword(), "Adgangskode");

		CookieSyncManager.createInstance(this);
		_cookieManager = CookieManager.getInstance();
		_cookieManager.setAcceptCookie(true);
		_cookieManager.removeSessionCookie();
		_cookieManager.removeExpiredCookie();
		CookieSyncManager.getInstance().sync();

		_webControllerActivity = this;
		initUI();
		resetProgress();
		siteLogin();
	}

	private void verify(int loginId, String source, String error) {
		if (source != null) {
			if (source.trim().length() > 0) {
				return;
			}
		}

		_loginId = loginId;
		AlertDialog.Builder builder = new AlertDialog.Builder(this);

		builder.setMessage("Du mangler at angive " + error).setCancelable(false).setPositiveButton("OK", new DialogInterface.OnClickListener() {
			public void onClick(DialogInterface dialog, int id) {
				Intent intent = new Intent(getApplicationContext(), EditLoginActivity.class);
				intent.putExtra("id", _loginId);
				startActivity(intent);
			}
		});

		AlertDialog alert = builder.create();
		alert.show();
	}

	@SuppressLint("SetJavaScriptEnabled") private void initUI() {
		_webViewPlaceHolder = (FrameLayout) findViewById(R.id.webviewplaceholder);
		if (_webView == null) {
			_webView = new WebView(this);

			WebSettings webSettings = _webView.getSettings();
			webSettings.setJavaScriptEnabled(true);
			webSettings.setJavaScriptCanOpenWindowsAutomatically(true);
			webSettings.setLoadsImagesAutomatically(true);
			webSettings.setBuiltInZoomControls(true);
			webSettings.setSupportZoom(true);
			webSettings.setUserAgentString("OSkoleMioApp");

			_webView.setWebViewClient(new WebViewClient() {
				public void onReceivedError(WebView view, int errorCode, String description, String failingUrl) {
					Toast.makeText(getApplicationContext(), "Åh nej! " + description, Toast.LENGTH_SHORT).show();
				}

				// Using the very good trick from:
				// http://damianflannery.wordpress.com/2010/09/28/android-webview-with-https-loadurl-shows-blankempty-page/
				public void onReceivedSslError(WebView view, SslErrorHandler handler, SslError error) {
					handler.proceed();
				}
			});

            _webView.setWebChromeClient(new WebChromeClient() {
            	public void onProgressChanged(WebView view, int progress) {
            		_webControllerActivity.setTitle("Henter siden ...");
            		_webControllerActivity.setProgress(progress * 100);
            		if (progress == 100) {
            			_webControllerActivity.setTitle(R.string.app_name);
            		}
            	}
            });

			_webView.setDownloadListener(new DownloadListener() {
				public void onDownloadStart(String url, String userAgent, String contentDisposition, String mimeType, long contentLength) {
					_progressDialog = ProgressDialog.show(_webControllerActivity, "Øjeblik", "Henter dokumentet ...", true, false);

					DownloadDocumentHandler downloadDocumentHandler = new DownloadDocumentHandler(_webControllerActivity);
					DownloadDocumentThread downloadDocumentThread = new DownloadDocumentThread(downloadDocumentHandler, _cookieManager, url, mimeType);
					downloadDocumentThread.start();
				}
			});
		}

		_webViewPlaceHolder.addView(_webView);
	}

	private void resetProgress() {
		_webView.setWebChromeClient(new WebChromeClient() {
			public void onProgressChanged(WebView view, int progress) {
				setTitle("Henter ...");
				setProgress(progress * 100);

				if (progress >= 100) {
					setTitle(R.string.app_name);
				}
			}
		});
	}

	private void siteLogin() {
		List<NameValuePair> params = new ArrayList<NameValuePair>();
		params.add(new BasicNameValuePair("BrowserType", "Netscape"));
		params.add(new BasicNameValuePair("fBrugernavn", _login.getLogin()));
		params.add(new BasicNameValuePair("fAdgangskode", _login.getPassword()));
		params.add(new BasicNameValuePair("B1", "Log ind"));

		try {
			UrlEncodedFormEntity entity = new UrlEncodedFormEntity(params);

			_webView.postUrl(_login.getUrl(), EntityUtils.toByteArray(entity));
		} catch (Exception ex) { }
	}

	@Override
	public void onConfigurationChanged(Configuration newConfiguration) {
		if (_webView != null) {
			_webViewPlaceHolder.removeView(_webView);
		}

		super.onConfigurationChanged(newConfiguration);
		setContentView(R.layout.webview);
		initUI();
	}

	@Override
	protected void onSaveInstanceState(Bundle outState) {
		super.onSaveInstanceState(outState);
		_webView.saveState(outState);
	}

	@Override
	protected void onRestoreInstanceState(Bundle inState) {
		super.onRestoreInstanceState(inState);
		_webView.restoreState(inState);
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
	public boolean onKeyDown(int keyCode, KeyEvent event) {
		if (keyCode == KeyEvent.KEYCODE_BACK && _webView.canGoBack()) {
			_webView.goBack();
			return true;
		}

		return super.onKeyDown(keyCode, event);
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		super.onCreateOptionsMenu(menu);
		MenuInflater inflater = getMenuInflater();
		inflater.inflate(R.menu.webcontroller_actionbar_menu, menu);
		return true;
	}

	@Override
	public boolean onOptionsItemSelected(MenuItem item) {
		switch (item.getItemId()) {
			case R.id.menu_selectschool:
				Intent intent = new Intent(this, OSkoleMioActivity.class);
				intent.putExtra("noauto", "1");
				startActivity(intent);
				return true;
				
			case R.id.menu_info:
				startActivity(new Intent(this, AboutActivity.class));
				return true;
		}

		return super.onOptionsItemSelected(item);
	}

	public void onDownloadFinished(String fullPath, String mimeType) {
		if (_progressDialog != null) {
			_progressDialog.dismiss();
			_progressDialog = null;
		}

		try {
			if (fullPath == null) {
				throw new Exception("Error");
			}

			Intent intent = new Intent(Intent.ACTION_VIEW);
			intent.setDataAndType(Uri.fromFile(new File(fullPath)), mimeType.toLowerCase(Danish.locale));
			startActivity(intent);
		} catch (Exception ex) {
			Toast.makeText(getApplicationContext(), "Øv! Der er ingen programmer der kan åbne filetypen: " + mimeType, Toast.LENGTH_LONG).show();
		}
	}

	public void onMenuButtonClick(View view) {
		openOptionsMenu();
    }
}
