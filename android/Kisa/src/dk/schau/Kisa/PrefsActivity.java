package dk.schau.Kisa;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.view.View;
import android.widget.TextView;
import android.widget.Toast;

public class PrefsActivity extends Activity {
	private SharedPreferences _sharedPreferences;
	
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        _sharedPreferences = getSharedPreferences("Kisa", Context.MODE_PRIVATE);
        
    	super.onCreate(savedInstanceState);
     	setContentView(R.layout.activity_prefs);
     	
     	SetFieldValue(R.id.prefs_et_year, "StartYear");
     	SetFieldValue(R.id.prefs_et_month, "StartMonth");
     	SetFieldValue(R.id.prefs_et_vdo, "StartKm");
     	SetFieldValue(R.id.prefs_et_maxkmpryear, "MaxKmPrYear");
    }
    
    private void SetFieldValue(int id, String preference) {
    	String value = _sharedPreferences.getString(preference,  null);
    	if (value == null) {
    		return;
    	}
    	
    	TextView textView = (TextView) findViewById(id);
    	textView.setText(value);
    }
    
    public void buttonOkClicked(View view) {
   		SharedPreferences.Editor editor = _sharedPreferences.edit();
   		
    	try {
    		int value = getPositiveValue(R.id.prefs_et_year, R.string.prefs_start_year);
    		editor.putString("StartYear", Integer.toString(value));
    	
    		value = getPositiveValue(R.id.prefs_et_month, R.string.prefs_start_month);
    		if (value < 1 || value > 12) {
    			throw new NumberFormatException(getString(R.string.prefs_start_month) + ": skal være mellem 1 og 12");
    		}
    		editor.putString("StartMonth", Integer.toString(value));
    		
    		value = getPositiveValue(R.id.prefs_et_vdo, R.string.prefs_vdo);
    		editor.putString("StartKm", Integer.toString(value));
    		
    		value = getPositiveValue(R.id.prefs_et_maxkmpryear, R.string.prefs_max_km_pr_year);
    		editor.putString("MaxKmPrYear", Integer.toString(value));
    		
    		startActivity(new Intent(this, MainActivity.class));
    	} catch (Exception ex) {
    		Toast.makeText(this, ex.getMessage(), Toast.LENGTH_LONG).show();
    	} finally {
    		editor.commit();
    	}
    }
  
    private int getPositiveValue(int textId, int fieldId) {
    	TextView textView = (TextView) findViewById(textId);
    	String value = textView.getText().toString();
    	if (value == null || value.trim().length() == 0) {
    		throw new NullPointerException(getString(fieldId) + ": skal udfyldes");
    	}
    	
    	int number;
    	try {
    		number = Integer.parseInt(value);
    	} catch (Exception ex) {
    		throw new NullPointerException(getString(fieldId) + ": skal være et tal >= 0");
    	}
    	
    	if (number < 0) {
    		throw new NullPointerException(getString(fieldId) + ": skal være et tal >= 0");
    	}
    	
    	return number;
    }
    
    public void buttonCancelClicked(View view) {
		startActivity(new Intent(this, MainActivity.class));
    }
}
