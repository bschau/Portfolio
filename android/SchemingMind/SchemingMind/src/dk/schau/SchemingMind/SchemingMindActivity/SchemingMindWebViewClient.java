package dk.schau.SchemingMind.SchemingMindActivity;

import android.app.Activity;
import android.app.ProgressDialog;
import android.graphics.Bitmap;
import android.webkit.WebView;
import android.webkit.WebViewClient;
import android.widget.Toast;

public class SchemingMindWebViewClient extends WebViewClient {
    private final int PAGE_STARTED = 1;
    private final int PAGE_REDIRECTED = 2;
	private int _prevState = 0;
    private ProgressDialog _progressDialog;
	private Activity _activity;

	public SchemingMindWebViewClient(Activity activity) {
		_activity = activity;
	}

	public void showProgressDialog(String title) {
        _progressDialog = new ProgressDialog(_activity);
        _progressDialog.setMessage(title);
        _progressDialog.setCancelable(false);
	}

	public void onReceivedError(WebView view, int errorCode, String description, String failingUrl) {
		dismissProgressDialog();
		Toast.makeText(_activity, "Oh no! " + errorCode + ": " + description + " / " + failingUrl, Toast.LENGTH_LONG).show();
	}
	
	private void dismissProgressDialog() {
		if (_progressDialog != null) {
			_progressDialog.dismiss();
			_progressDialog = null;
		}
	}

    @Override  
    public boolean shouldOverrideUrlLoading(WebView view, String url) {
    	_prevState = PAGE_REDIRECTED;
    	view.loadUrl(url);
    	return true;
    }
    
    @Override
    public void onPageStarted(WebView view, String url, Bitmap favicon) {
        super.onPageStarted(view, url, favicon);

        if (_progressDialog != null && !_progressDialog.isShowing()) {
            _progressDialog.show();
        }

        _prevState = PAGE_STARTED;
    }

    @Override
    public void onPageFinished(WebView view, String url) {
    	if (_prevState == PAGE_STARTED) {
    		dismissProgressDialog();
    	}
    }
}
