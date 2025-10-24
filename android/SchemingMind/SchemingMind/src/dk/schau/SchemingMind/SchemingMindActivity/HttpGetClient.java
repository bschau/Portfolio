package dk.schau.SchemingMind.SchemingMindActivity;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.List;

import org.apache.http.HttpResponse;
import org.apache.http.client.methods.HttpGet;
import org.apache.http.cookie.Cookie;
import org.apache.http.impl.client.DefaultHttpClient;

public class HttpGetClient {
    private DefaultHttpClient _client;
	private HttpGet _httpGet;

	public HttpGetClient(DefaultHttpClient client, HttpGet httpGet) {
		_client = client;
		_httpGet = httpGet;
	}
	
	public String getPage() throws Exception {
        BufferedReader in = null;
        StringBuilder sb = new StringBuilder();

        try {
            HttpResponse response = _client.execute(_httpGet);
            InputStream inputStream = response.getEntity().getContent();
            in = new BufferedReader(new InputStreamReader(inputStream));

            String line;
            String NL = System.getProperty("line.separator");
            while ((line = in.readLine()) != null) {
                sb.append(line);
                sb.append(NL);
            }
        } catch (Exception ex) {
        	sb.delete(0, sb.length());
        } finally {
            if (in != null) {
                try {
                    in.close();
                } catch (IOException e) {
                }
            }
        }
        
        return sb.toString();
    }
	
	public List<Cookie> getCookies() {
		return _client.getCookieStore().getCookies();
	}
}
