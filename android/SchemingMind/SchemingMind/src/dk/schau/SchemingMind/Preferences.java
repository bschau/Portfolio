package dk.schau.SchemingMind;

import android.app.Activity;
import android.content.Context;
import android.content.SharedPreferences;

public class Preferences {
    SharedPreferences.Editor _editor;
	private String _username;
	private String _password;
	private String _site;

	public Preferences(Activity activity) {
        SharedPreferences _sharedPreferences = activity.getSharedPreferences("SchemingMind", Context.MODE_PRIVATE);
    	_editor = _sharedPreferences.edit();
    	
    	_username = _sharedPreferences.getString("USERNAME", "").trim();
    	_password = _sharedPreferences.getString("PASSWORD", "").trim();
    	_site = _sharedPreferences.getString("SITE", "desktop").trim();
	}
	
	public String getUsername() {
		return _username;
	}

	public void setUsername(String username) {
		_username = username;
		_editor.putString("USERNAME", _username);
		_editor.commit();
	}
	
	public String getPassword() {
		return _password;
	}
	
	public void setPassword(String password) {
		_password = password;
		_editor.putString("PASSWORD", _password);
		_editor.commit();
	}
	
	public String getSite() {
		return _site;
	}
	
	public void setSite(String site) {
		_site = site;
		_editor.putString("SITE", _site);
		_editor.commit();
	}
}
