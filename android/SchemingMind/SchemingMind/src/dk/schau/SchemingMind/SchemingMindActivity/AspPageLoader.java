package dk.schau.SchemingMind.SchemingMindActivity;

public class AspPageLoader {
	private HttpGetClient _httpGetClient;
	private String _page = "";

	public AspPageLoader(HttpGetClient httpGetClient) {
		_httpGetClient = httpGetClient;
	}
	
	public void loadPage() throws Exception {
        _page = _httpGetClient.getPage();
	}
	
    public String getViewState() {
    	return getAspxState("__VIEWSTATE");
    }
    
    public String getEventValidation() {
    	return getAspxState("__EVENTVALIDATION");
    }
    
    private String getAspxState(String stateKey) {
    	int index = _page.indexOf(stateKey);
    	
    	if (index < 0) {
    		return null;
    	}
    	
    	index = _page.indexOf("value", index);
    	if (index < 0) {
    		return null;
    	}
    	
    	index += 7;	// value="
    	int end = _page.indexOf((int) '"', index);
    	if (end < 0) {
    		return null;
    	}
    	
    	return _page.substring(index, end);
    }
}
