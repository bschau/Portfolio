package dk.schau.vkm.activity;

import dk.schau.vkm.R;
import dk.schau.vkm.media.Feedback;
import dk.schau.vkm.media.MainScreenGestureDetector;
import dk.schau.vkm.util.Locale;
import dk.schau.vkm.util.Preferences;
import dk.schau.vkm.util.Program;
import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.util.DisplayMetrics;
import android.view.GestureDetector;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.view.GestureDetector.OnGestureListener;
import android.widget.ImageView;
import android.widget.TextView;

public class VkmActivity extends Activity {
	private StringBuilder stringBuilder = new StringBuilder();
	private int[] activityToImageMap = { R.drawable.running, R.drawable.walking,
			R.drawable.return_running, R.drawable.return_walking,
			R.drawable.running_completed, R.drawable.walking_completed,
			R.drawable.return_running_completed, R.drawable.return_walking_completed };
	private String[] activityToTextMap = null;
	private MainScreenGestureDetector mainScreenGestureDetector;
	private GestureDetector gestureDetector;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        Preferences.init(this);
        Locale.init();

        if (Locale.getLocale().compareTo("") == 0) {
        	startActivity(new Intent(this, LanguageActivity.class));
        	return;
        }

        activityToTextMap = new String[] { Locale.get("RUNFOR"),
        		Locale.get("WALKFOR"),
        		Locale.get("RETURNRUNFOR"),
        		Locale.get("RETURNWALKFOR")
        };

        Feedback.init(getApplicationContext());
        Program.loadCompleted();
        Program.setFirstUncompleted();

        this.requestWindowFeature(Window.FEATURE_NO_TITLE);
        this.getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
        setContentView(R.layout.main);

        mainScreenGestureDetector = new MainScreenGestureDetector(this);
        mainScreenGestureDetector.resetCurrentActivity();
        
        DisplayMetrics displayMetrics = new DisplayMetrics();
        getWindowManager().getDefaultDisplay().getMetrics(displayMetrics);
        
        if (displayMetrics.densityDpi == DisplayMetrics.DENSITY_LOW) {
        	mainScreenGestureDetector.setMinimumSwipeDistance(40);
        } else {
        	mainScreenGestureDetector.setMinimumSwipeDistance(80);
        }

        gestureDetector = new GestureDetector((OnGestureListener) mainScreenGestureDetector);

        redraw();
    }
    
	@Override
 	public boolean onCreateOptionsMenu(Menu menu) {
		super.onCreateOptionsMenu(menu);
		MenuInflater inflater = getMenuInflater();
		inflater.inflate(R.menu.vkmmenu, menu);
		return true;
 	}
	
	@Override
 	public boolean onOptionsItemSelected(MenuItem item) {
		startActivity(new Intent(this, LanguageActivity.class));
		return true;
	}
    
    @Override
    public void onPause() {
    	super.onPause();

    	Program.saveCompleted();
    }
    
    @Override
    public void onResume() {
    	super.onResume();
    	
    	if (Program.loadState()) {
    		startActivity(new Intent(this, RunningActivity.class));
    	}
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {
    	gestureDetector.onTouchEvent(event);
    	
    	return false;
    }
    
    public void redraw() {
    	TextView textView = (TextView) findViewById(R.id.daytext);
    	ImageView imageView = (ImageView) findViewById(R.id.activity);
    	short activity = Program.getActivity(mainScreenGestureDetector.getCurrentActivity()), imageIndex, textIndex;
    	
    	stringBuilder.delete(0, stringBuilder.length());
    	
    	stringBuilder.append(Locale.get("DAY"));
    	stringBuilder.append(" ");
    	stringBuilder.append(Program.getDay() + 1);
    	stringBuilder.append(" - ");
    	
    	formatTime(Program.totalTime);
    	textView.setText(stringBuilder.toString());

    	if (Program.isRun(activity)) {
    		imageIndex = 0;
    		textIndex = 0;
    	} else {
    		imageIndex = 1;
    		textIndex = 1;
    	}
    	
    	if (Program.isTurned(activity)) {
    		imageIndex += 2;
    		textIndex += 2;
    	}
    	
    	if (Program.isCompleted(Program.getDay())) {
    		imageIndex += 4;
    	}
    	
    	imageView.setImageResource(activityToImageMap[imageIndex]);

    	stringBuilder.delete(0, stringBuilder.length());
    	stringBuilder.append(mainScreenGestureDetector.getCurrentActivity() + 1);
    	stringBuilder.append(". ");
    	stringBuilder.append(activityToTextMap[textIndex]);
    	stringBuilder.append(' ');
    	formatTime(Program.getTimeFromActivity(activity));
 
    	textView = (TextView) findViewById(R.id.activitytext);
    	textView.setText(stringBuilder.toString());
    }
    
    private void formatTime(int time) {
    	int value = time / 60;
    	
    	stringBuilder.append(value);
    	stringBuilder.append(':');
    	if ((value = time % 60) < 10) {
    		stringBuilder.append('0');
    	}
    	
    	stringBuilder.append(value);
    }

    public void onButtonClick(View view) {
    	switch (view.getId()) {
    		case R.id.togglebutton:
    			Program.toggleCompleted();
    			redraw();
    			break;

    		case R.id.infobutton:
    			startActivity(new Intent(this, AboutActivity.class));
    			break;
    		
    		case R.id.resetbutton:
    			Program.setFirstUncompleted();
    			mainScreenGestureDetector.resetCurrentActivity();
    			redraw();
    			break;
    		
    		case R.id.runbutton:
    			Program.startRun();
    			startActivity(new Intent(this, RunningActivity.class));
    			break;
    	}
    }
}