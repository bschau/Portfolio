package dk.schau.SchemingMind.SchemingMindActivity;

import java.net.URI;
import java.text.SimpleDateFormat;
import java.util.List;
import java.util.SimpleTimeZone;

import org.apache.http.client.methods.HttpGet;
import org.apache.http.cookie.Cookie;
import org.apache.http.impl.client.DefaultHttpClient;

import android.webkit.CookieManager;
import android.webkit.CookieSyncManager;

public abstract class SchemingMindClient implements Runnable {
	protected SchemingMindActivity _schemingMindActivity;
	protected String _username;
	protected String _password;
	private HttpGetClient _httpGetClient;
	protected AspPageLoader _aspPageLoader;
	protected String _viewState;
	
	public void setSchemingMindActivity(SchemingMindActivity schemingMindActivity) {
		_schemingMindActivity = schemingMindActivity;
	}

	public void setUsername(String username) {
		_username = username;
	}
	
	public void setPassword(String password) {
		_password = password;
	}
	
	protected void loadPage(String masterUrl) throws Exception {
		HttpGet httpGet = new HttpGet();
		httpGet.setURI(new URI(masterUrl));
		_httpGetClient = new HttpGetClient(new DefaultHttpClient(), httpGet);

        _aspPageLoader = new AspPageLoader(_httpGetClient);
        _aspPageLoader.loadPage();
	}
	
	protected void findViewState() throws Exception {
		_viewState = _aspPageLoader.getViewState();

		if (_viewState == null) {
        	throw new Exception("The server did not send __VIEWSTATE information.");
        }
	}

	protected void addCookiesToSession(String cookieBase) {
    	CookieManager cookieManager = CookieManager.getInstance();
    	
    	List<Cookie> cookies = _httpGetClient.getCookies();
    	StringBuilder sb = new StringBuilder();

        for (Cookie cookie : cookies) {
    		sb.delete(0, sb.length());
    		
    		sb.append(cookie.getName());
    		sb.append("=");
    		sb.append(cookie.getValue());
    		sb.append("; domain=");
    		sb.append(cookie.getDomain());
    		sb.append("; expires=");
    		
    		SimpleDateFormat simpleDateFormat = new SimpleDateFormat();
    		simpleDateFormat.setTimeZone(new SimpleTimeZone(0, "GMT"));
    		simpleDateFormat.applyPattern("dd MMM yyyy HH:mm:ss z");
    		
    		sb.append(simpleDateFormat.format(cookie.getExpiryDate()));
    		sb.append("; path=");
    		sb.append(cookie.getPath());
    		
    		cookieManager.setCookie(cookieBase, sb.toString());
    	}

    	CookieSyncManager.getInstance().sync();
    }
	
    protected String getErrorPage(String error) {
    	StringBuilder sb = new StringBuilder();
    	
    	sb.append("<!DOCTYPE html>");
    	sb.append("<html lang=\"en\">");
    	sb.append("<head>");
    	sb.append("<title>SchemingMind</title>");
    	sb.append("<meta charset=\"utf-8\" />");
    	sb.append("</head>");   	 
    	sb.append("<body>");
    	sb.append("<p>Error: ");
    	sb.append(error);
    	sb.append("</p><p>Please try again later or contact App Support on <a href=\"mailto:brian@schau.dk?subject=[SchemingMind App] Help\">brian@schau.dk</a>.</p>");
    	sb.append("</body>");
    	sb.append("</html>");
    	
    	return sb.toString();
    }
}
