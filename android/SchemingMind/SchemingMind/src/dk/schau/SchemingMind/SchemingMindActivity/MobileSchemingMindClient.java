package dk.schau.SchemingMind.SchemingMindActivity;

import java.util.ArrayList;
import java.util.List;

import org.apache.http.NameValuePair;
import org.apache.http.client.entity.UrlEncodedFormEntity;
import org.apache.http.message.BasicNameValuePair;
import org.apache.http.util.EntityUtils;

public class MobileSchemingMindClient extends SchemingMindClient implements ISchemingMindClient {
    private final String COOKIE_BASE = "http://mobile.schemingmind.com/";
    private final String MASTER_URL = "http://mobile.schemingmind.com/server.aspx?server_id=1";
	private final String SITE_URL = "http://mobile.schemingmind.com/server.aspx?server_id=1";

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

			loadMobileSchemingMind();
		} catch (Exception ex) {
			_schemingMindActivity.setErrorMessage(getErrorPage(ex.getMessage()));
		}
	}

    private void loadMobileSchemingMind() throws Exception {
        List<NameValuePair> params = new ArrayList<NameValuePair>();
	        
        params.add(new BasicNameValuePair("ctl00$MainContent$TextBox1", _username));
        params.add(new BasicNameValuePair("ctl00$MainContent$TextBox2", _password));
        params.add(new BasicNameValuePair("ctl00$MainContent$Button1", "Log on"));
        params.add(new BasicNameValuePair("__VIEWSTATE", _viewState));
        params.add(new BasicNameValuePair("__EVENTVALIDATION", getEventValidation()));

        UrlEncodedFormEntity entity = new UrlEncodedFormEntity(params);

        _schemingMindActivity.loadWebView(SITE_URL, EntityUtils.toByteArray(entity));
    }
	    
    private String getEventValidation() throws Exception {
		String eventValidation = _aspPageLoader.getEventValidation();

		if (eventValidation == null) {
        	throw new Exception("The server did not send __EVENTVALIDATION information.");
        }
			
		return eventValidation;
	}
}
