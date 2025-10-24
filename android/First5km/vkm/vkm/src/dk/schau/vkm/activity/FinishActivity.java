package dk.schau.vkm.activity;

import dk.schau.vkm.R;
import dk.schau.vkm.util.Locale;
import dk.schau.vkm.util.Program;
import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.view.View.OnClickListener;
import android.widget.TextView;

public class FinishActivity extends Activity implements OnClickListener {
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
	     
        this.requestWindowFeature(Window.FEATURE_NO_TITLE);
        this.getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
        setContentView(R.layout.finishactivity);

        findViewById(R.id.finishbutton).setOnClickListener(this);

        ((TextView) findViewById(R.id.finish1text)).setText(Locale.get("FINISH1"));
        ((TextView) findViewById(R.id.finish2text)).setText(Locale.get("FINISH2"));
    }
	    
    @Override
    public void onPause() {
    	super.onPause();
    	
    	Program.saveState();
    	Program.saveCompleted();
    }
	    
    public void onClick(View view) {
		startActivity(new Intent(this, VkmActivity.class));
	}
}