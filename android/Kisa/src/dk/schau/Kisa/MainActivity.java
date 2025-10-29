package dk.schau.Kisa;

import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.TextView;
import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;

public class MainActivity extends Activity {
	private SharedPreferences _sharedPreferences;
	
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        _sharedPreferences = getSharedPreferences("Kisa", Context.MODE_PRIVATE);
        
        try {
        	int startYear = getPreferenceValue("StartYear");
        	int startMonth = getPreferenceValue("StartMonth");
        	int startKm = getPreferenceValue("StartKm");
        	int maxKmPrYear = getPreferenceValue("MaxKmPrYear");
        
        	Mileage mileage = new Mileage(startYear, startMonth, startKm, maxKmPrYear);
        	int max = mileage.CalculateMaxMilage();
        
        	super.onCreate(savedInstanceState);
        	setContentView(R.layout.activity_main);
        	TextView kmTextView = (TextView) findViewById(R.id.kmTextView);
        	kmTextView.setText(Integer.toString(max));
        } catch (Exception ex) {
        	super.onCreate(savedInstanceState);
        	startActivity(new Intent(this, PrefsActivity.class));
        }
    }

    private int getPreferenceValue(String preference) throws NullPointerException,NumberFormatException {
        String value = _sharedPreferences.getString(preference, null);
        
        if (value == null) {
        	throw new NullPointerException("Preference not found");
        }
        
        return Integer.parseInt(value);
    }
    
    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
    	menu.add(Menu.NONE, 0, 0, "Stamdata");
    	return super.onCreateOptionsMenu(menu);
    }
    
	@Override
	public boolean onOptionsItemSelected(MenuItem item) {
		if (item.getItemId() != 0) {
			return false;
		}
		
		startActivity(new Intent(this, PrefsActivity.class));
		return true;
	}
}
