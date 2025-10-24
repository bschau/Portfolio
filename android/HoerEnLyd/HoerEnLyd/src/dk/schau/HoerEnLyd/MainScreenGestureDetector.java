package dk.schau.HoerEnLyd;

import android.view.MotionEvent;
import android.view.GestureDetector.SimpleOnGestureListener;

public class MainScreenGestureDetector extends SimpleOnGestureListener {
    private static final int SWIPE_MIN_DISTANCE = 80;
    private static final int SWIPE_MAX_OFF_PATH = 250;
	private static final int SWIPE_THRESHOLD_VELOCITY = 200;
	private HoerEnLydActivity _activity;
	private boolean _buttonsLocked;

	public MainScreenGestureDetector(HoerEnLydActivity activity) {
		_activity = activity;
	}

	public void setButtonsLocked(boolean locked) {
		_buttonsLocked = locked;
	}

    @Override
    public boolean onFling(MotionEvent e1, MotionEvent e2, float velocityX, float velocityY) {
    	if (_buttonsLocked) {
    		return false;
    	}

    	try {          
            if ((e1.getX() - e2.getX()) > SWIPE_MIN_DISTANCE &&
                 Math.abs(velocityX) > SWIPE_THRESHOLD_VELOCITY &&
                 Math.abs(e1.getY() - e2.getY()) < SWIPE_MAX_OFF_PATH) {
            	Media.next();
            	_activity.setupFade(Constants.FadeDirection.OUT);
            } else if ((e2.getX() - e1.getX()) > SWIPE_MIN_DISTANCE &&
            		    Math.abs(velocityX) > SWIPE_THRESHOLD_VELOCITY &&
            		    Math.abs(e1.getY() - e2.getY()) < SWIPE_MAX_OFF_PATH) {
            	Media.previous();
            	_activity.setupFade(Constants.FadeDirection.OUT);
            } else if ((e1.getY() - e2.getY()) > SWIPE_MIN_DISTANCE &&
            		    Math.abs(velocityY) > SWIPE_THRESHOLD_VELOCITY &&
            		    Math.abs(e1.getX() - e2.getX()) < SWIPE_MAX_OFF_PATH) {
    	        _activity.play(Media.getSaySoundId(), Constants.SoundCommand.PLAY);
            } else if ((e2.getY() - e1.getY()) > SWIPE_MIN_DISTANCE &&
            		    Math.abs(velocityY) > SWIPE_THRESHOLD_VELOCITY &&
            		    Math.abs(e1.getX() - e2.getX()) < SWIPE_MAX_OFF_PATH) {
    	        _activity.play(Media.getSaySoundId(), Constants.SoundCommand.PLAY);
            }
        } catch (Exception e) {}

        return false;
    }
}
