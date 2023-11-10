package dk.schau.SchemingMind.CredentialsActivity;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.text.SpannableString;
import android.text.util.Linkify;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.widget.EditText;
import android.widget.TextView;
import dk.schau.SchemingMind.Preferences;
import dk.schau.SchemingMind.R;
import dk.schau.SchemingMind.SchemingMindActivity.SchemingMindActivity;

public class CredentialsActivity extends Activity {
	private Preferences _preferences;

    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.credentials);

        TextView message = (TextView) findViewById(R.id.credentials_intro);
		SpannableString s = new SpannableString(getString(R.string.credentials_intro));
		message.setText(s);
		Linkify.addLinks(message, Linkify.ALL);

        _preferences = new Preferences(this);
        
        EditText editText = (EditText) findViewById(R.id.credentials_username);
        editText.setText(_preferences.getUsername());
        
        editText = (EditText) findViewById(R.id.credentials_password);
        editText.setText(_preferences.getPassword());        
    }
    
	@Override
 	public boolean onCreateOptionsMenu(Menu menu) {
		super.onCreateOptionsMenu(menu);
		MenuInflater inflater = getMenuInflater();
		inflater.inflate(R.menu.submenu, menu);
		return true;
 	}
	
	@Override
 	public boolean onOptionsItemSelected(MenuItem item) {
		switch (item.getItemId()) {
            case R.id.back:
            	goHome();
                return true;
		}

		return false;
	}

	private void goHome() {
		Intent intent = new Intent(this, SchemingMindActivity.class);
		intent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
		startActivity(intent);
	}
	
	public void buttonSaveClicked(View view) {
    	_preferences.setUsername(getEditTextContent(R.id.credentials_username));
    	_preferences.setPassword(getEditTextContent(R.id.credentials_password));
    	goHome();
	}
	
	private String getEditTextContent(int editTextId) {
    	EditText editText = (EditText) findViewById(editTextId);
    	String text = editText.getText().toString();
    	
    	if (text != null && text.length() > 0) {
    		return text.trim();
    	}
    	
    	return "";
	}
	
	public void buttonCancelClicked(View view) {
		goHome();
	}
}
