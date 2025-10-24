package dk.schau.vkm.activity;

import dk.schau.vkm.R;
import dk.schau.vkm.media.Feedback;
import dk.schau.vkm.util.Locale;
import dk.schau.vkm.util.Program;
import dk.schau.vkm.view.RunningView;
import android.app.Activity;
import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.media.AudioManager;
import android.os.Bundle;
import android.view.KeyEvent;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.view.View.OnClickListener;
import android.widget.TextView;

public class RunningActivity extends Activity implements OnClickListener {
	private RunningView runningView = null;
	private Context saveContext;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
     
        this.requestWindowFeature(Window.FEATURE_NO_TITLE);
        this.getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
        setContentView(R.layout.runningactivity);

        findViewById(R.id.pausebutton).setOnClickListener(this);
        findViewById(R.id.stopbutton).setOnClickListener(this);

        getWindow().addFlags(WindowManager.LayoutParams.FLAG_DISMISS_KEYGUARD);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        runningView = (RunningView) findViewById(R.id.runningview);
        setVolumeControlStream(AudioManager.STREAM_MUSIC);
    }
    
    @Override
    public void onPause() {
    	super.onPause();
    	if (runningView != null) {
    		runningView.setMode(RunningView.PAUSE);
    	}
    	
    	Program.saveState();
    	Program.saveCompleted();
    	Feedback.stopFeedback();
    }
    
    @Override
    public void onResume() {
    	super.onResume();

    	if (runningView != null) {
    		runningView.setMode(RunningView.RUNNING);
    	}
    }
    
    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
    	if (keyCode == KeyEvent.KEYCODE_BACK) {
    		confirmStop(this);
    		return true;
    	}
    	
    	return super.onKeyDown(keyCode, event);
    }

    public void onClick(View view) {
    	if (view.getId() == R.id.pausebutton) {
    		showPause(this);
    	} else {
    		confirmStop(this);
    	}    		
    }
    
    private void showPause(Context context) {
		runningView.setMode(RunningView.PAUSE);

    	AlertDialog.Builder pauseDialog = new AlertDialog.Builder(this);    	
		pauseDialog.setPositiveButton(Locale.get("OK"), new DialogInterface.OnClickListener() {
			public void onClick(DialogInterface dialog, int id) {
				runningView.setMode(RunningView.RUNNING);

				dialog.cancel();
			}
		});

		showDialog(pauseDialog, context, "PAUSETITLE", "PAUSETEXT");
    }
    
    private void confirmStop(Context context) {
		runningView.setMode(RunningView.PAUSE);

		saveContext = context;

		AlertDialog.Builder stopDialog = new AlertDialog.Builder(this);    	
		stopDialog.setNegativeButton(Locale.get("NO"), new DialogInterface.OnClickListener() {
			public void onClick(DialogInterface dialog, int id) {
				dialog.cancel();
				runningView.setMode(RunningView.RUNNING);
			}
		});
		
		stopDialog.setPositiveButton(Locale.get("YES"), new DialogInterface.OnClickListener() {
			public void onClick(DialogInterface dialog, int id) {
				dialog.cancel();

				Program.cancelRun();
	    		Intent intent = new Intent(saveContext, VkmActivity.class);
	    		startActivity(intent);
			}
		});

		showDialog(stopDialog, context, "STOPTITLE", "STOPTEXT");
    }

    private void showDialog(AlertDialog.Builder dialog, Context context, String titleKey, String textKey) {
    	TextView message = new TextView(context);
    	message.setPadding(5, 5, 5, 5);
    	message.setText(Locale.get(textKey));
	
    	AlertDialog alert = dialog.create();
    	alert.setTitle(Locale.get(titleKey));
    	alert.setView(message);
    	alert.setCancelable(true);
    	alert.setIcon(R.drawable.icon);
    	alert.show();
    }
}
