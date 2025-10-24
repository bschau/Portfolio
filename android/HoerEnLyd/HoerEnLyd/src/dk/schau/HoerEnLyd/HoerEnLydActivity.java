package dk.schau.HoerEnLyd;

import android.app.Activity;
import android.content.pm.PackageManager.NameNotFoundException;
import android.media.AudioManager;
import android.media.MediaPlayer;
import android.os.Bundle;
import android.view.GestureDetector;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnTouchListener;
import android.view.animation.Animation;
import android.view.animation.AnimationUtils;
import android.view.animation.Animation.AnimationListener;
import android.widget.ImageView;

public class HoerEnLydActivity extends Activity implements MediaPlayer.OnCompletionListener {
	private SoundPlayer soundPlayer;
	private GestureDetector gestureDetector;
	private int curImgId;
	private Constants.SoundCommand sndNextStep;
	private MainScreenGestureDetector mainScreenGestureDetector;

	@Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);

        this.setVolumeControlStream(AudioManager.STREAM_MUSIC);

        mainScreenGestureDetector = new MainScreenGestureDetector(this);
        mainScreenGestureDetector.setButtonsLocked(true);
 
        gestureDetector = new GestureDetector((android.view.GestureDetector.OnGestureListener) mainScreenGestureDetector);
        OnTouchListener gestureListener = new OnTouchListener() {
        	public boolean onTouch(View v, MotionEvent event) {
        		if (gestureDetector.onTouchEvent(event)) {
        			return true;
        		}

        		return false;
        	}
        };

        soundPlayer = new SoundPlayer(this);

		if (savedInstanceState != null && savedInstanceState.containsKey("MEDIA_INDEX")) {
			Media.setIndex(savedInstanceState.getInt("MEDIA_INDEX"));
		} else {
			Media.setIndex(0);
		}
		
        setupFade(Constants.FadeDirection.IN);
    }

	@Override
	protected void onSaveInstanceState(Bundle outState) {
		outState.putInt("MEDIA_INDEX", Media.getIndex());
	}
	
	@Override
	protected void onRestoreInstanceState(Bundle inState) {
	}

	@Override
 	public boolean onCreateOptionsMenu(Menu menu) {
		super.onCreateOptionsMenu(menu);
		MenuInflater inflater = getMenuInflater();
		inflater.inflate(R.menu.mainmenu, menu);
		return true;
 	}
	
	public boolean onOptionsItemSelected(MenuItem item) {
		switch (item.getItemId()) {
			case R.id.about:
				try {
					AboutDialog.show(this);
				} catch (NameNotFoundException e) {
				}
				return true;
		}

		return false;
	}

	public void onCompletion(MediaPlayer mp) {
		if (sndNextStep == Constants.SoundCommand.SILENT) {
			mainScreenGestureDetector.setButtonsLocked(false);			
		} else {
			play(Media.getSoundId(), Constants.SoundCommand.SILENT);
		}
	}

	public void play(int sndId, Constants.SoundCommand nextStep) {
		mainScreenGestureDetector.setButtonsLocked(true);
		sndNextStep = nextStep;
		soundPlayer.play(sndId, this);
	}

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        if (gestureDetector.onTouchEvent(event)) {
	        return true;
        } else {
	    	return false;
        }
    }
    
	public void setupFade(Constants.FadeDirection fadeDir) {
		if (fadeDir == Constants.FadeDirection.IN) {
	        curImgId = Media.getImageId();
	        play(Media.getSaySoundId(), Constants.SoundCommand.PLAY);
			setupAnimation(R.anim.fadein, 0);
		} else {
			setupAnimation(R.anim.fadeout, 255);
		}
	}

	private void setupAnimation(int fadeId, int alpha) {
		ImageView iv = (ImageView) findViewById(R.id.image);
		iv.setImageResource(curImgId);

		Animation animation = AnimationUtils.loadAnimation(this, fadeId);
		animation.setFillAfter(true);
		if (fadeId == R.anim.fadeout) {
			animation.setAnimationListener(new AnimationListener() {
				public void onAnimationStart(Animation anim) {
				}
				
				public void onAnimationRepeat(Animation anim) {
				}

				public void onAnimationEnd(Animation end) {
			        curImgId = Media.getImageId();
			        setupFade(Constants.FadeDirection.IN);
				}
			});			
		}

		iv.startAnimation(animation);
	}
}
