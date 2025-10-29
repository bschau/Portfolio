package dk.schau.CitiZr.CredentialsActivity;

import dk.schau.CitiZr.CitiZrActivity.CitiZrActivity;
import dk.schau.CitiZr.R;
import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.view.View;
import android.widget.EditText;

public class CredentialsActivity extends Activity {
	private static SharedPreferences sp;
	
	@Override
    public void onCreate(Bundle savedInstanceState) {
		sp = getSharedPreferences("CitiZr", Context.MODE_PRIVATE);
		
        super.onCreate(savedInstanceState);
        setContentView(R.layout.credentials);

        EditText et = (EditText) findViewById(R.id.tvlogin);
        et.setText(sp.getString("CITIZR_LOGIN", ""));
        
        et = (EditText) findViewById(R.id.tvpassword);
        et.setText(sp.getString("CITIZR_PASSWORD", ""));
	}

    public void buttonOkClicked(View view) {
    	SharedPreferences.Editor ed = sp.edit();
    	
    	ed.putString("CITIZR_LOGIN", gt(R.id.tvlogin));
    	ed.putString("CITIZR_PASSWORD", gt(R.id.tvpassword));
    	ed.commit();

    	startActivity(new Intent(this, CitiZrActivity.class));
    }

    private String gt(int guiId) {
    	EditText et = (EditText) findViewById(guiId);
    	String txt = et.getText().toString();
    	
    	if (txt != null && txt.length() > 0) {
    		return txt.trim();
    	}
    	
    	return "";
    }

    public void buttonCancelClicked(View view) {
    	startActivity(new Intent(this, CitiZrActivity.class));
    }
}
