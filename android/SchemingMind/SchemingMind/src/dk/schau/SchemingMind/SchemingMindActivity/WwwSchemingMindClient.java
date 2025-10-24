package dk.schau.SchemingMind.SchemingMindActivity;

import java.util.ArrayList;
import java.util.List;

import org.apache.http.NameValuePair;
import org.apache.http.client.entity.UrlEncodedFormEntity;
import org.apache.http.message.BasicNameValuePair;
import org.apache.http.util.EntityUtils;

public class WwwSchemingMindClient extends SchemingMindClient implements ISchemingMindClient {
	private final String COOKIE_BASE = "http://www.schemingmind.con";
	private final String MASTER_URL = "http://www.schemingmind.com/default.aspx";
	private final String SITE_URL = "http://www.schemingmind.com/default.aspx?ReturnUrl=%2fcurrentgames.aspx";

	public void run() {
		try {
			loadPage(MASTER_URL);
			if (Thread.interrupted()) {
				return;
			}

			findViewState();
			addCookiesToSession(COOKIE_BASE);
			if (Thread.interrupted()) {
				return;
			}
		
			loadDesktopSchemingMind();
		} catch (Exception ex) {
			_schemingMindActivity.setErrorMessage(getErrorPage(ex.getMessage()));
		}
	}

    private void loadDesktopSchemingMind() throws Exception {
        List<NameValuePair> params = new ArrayList<NameValuePair>();
        
        params.add(new BasicNameValuePair("ctl00$ContentPlaceHolder1$TextBox1", _username));
        params.add(new BasicNameValuePair("ctl00$ContentPlaceHolder1$TextBox2", _password));
        params.add(new BasicNameValuePair("ctl00$ContentPlaceHolder1$Button1", "Sign On"));
        params.add(new BasicNameValuePair("__VIEWSTATE", _viewState));

        UrlEncodedFormEntity entity = new UrlEncodedFormEntity(params);

        _schemingMindActivity.loadWebView(SITE_URL, EntityUtils.toByteArray(entity));
    }
}
