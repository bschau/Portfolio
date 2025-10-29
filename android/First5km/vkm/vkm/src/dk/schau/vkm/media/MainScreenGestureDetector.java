package dk.schau.vkm.media;

import dk.schau.vkm.activity.VkmActivity;
import dk.schau.vkm.util.Program;
import android.view.MotionEvent;
import android.view.GestureDetector.SimpleOnGestureListener;

public class MainScreenGestureDetector extends SimpleOnGestureListener {
	private static final int SWIPE_MAX_OFF_PATH = 250;
	private static final int SWIPE_THRESHOLD_VELOCITY = 200;
	private VkmActivity _vkmActivity;
	private int _swipeMinimumDistance;
	private int _currentActivity;

	public MainScreenGestureDetector(VkmActivity vkmActivity) {
		_vkmActivity = vkmActivity;
	}

	public void setMinimumSwipeDistance(int distance) {
		_swipeMinimumDistance = distance;
	}
	
	public void resetCurrentActivity() {
		_currentActivity = 0;
	}
	
	public int getCurrentActivity() {
		return _currentActivity;
	}

	@Override
	public boolean onFling(MotionEvent e1, MotionEvent e2, float velocityX, float velocityY) {
		int day = Program.getDay();

		if ((e1.getX() - e2.getX()) > _swipeMinimumDistance &&
				Math.abs(velocityX) > SWIPE_THRESHOLD_VELOCITY &&
				Math.abs(e1.getY() - e2.getY()) < SWIPE_MAX_OFF_PATH) {
			if (day < (Program.MAX_DAYS - 1)) {
				Program.selectDay(day + 1);
				_currentActivity = 0;
				_vkmActivity.redraw();
			}

			return true;
		} else if ((e2.getX() - e1.getX()) > _swipeMinimumDistance &&
				Math.abs(velocityX) > SWIPE_THRESHOLD_VELOCITY &&
				Math.abs(e1.getY() - e2.getY()) < SWIPE_MAX_OFF_PATH) {
			if (day > 0) {
				Program.selectDay(day - 1);
				_currentActivity = 0;
				_vkmActivity.redraw();
			}

			return true;
		} else if ((e1.getY() - e2.getY()) > _swipeMinimumDistance &&
				Math.abs(velocityY) > SWIPE_THRESHOLD_VELOCITY &&
				Math.abs(e1.getX() - e2.getX()) < SWIPE_MAX_OFF_PATH) {
			if (_currentActivity < (Program.activities - 1)) {
				_currentActivity++;
				_vkmActivity.redraw();
			}

			return true;
		} else if ((e2.getY() - e1.getY()) > _swipeMinimumDistance &&
				Math.abs(velocityY) > SWIPE_THRESHOLD_VELOCITY &&
				Math.abs(e1.getX() - e2.getX()) < SWIPE_MAX_OFF_PATH) {
			if (_currentActivity > 0) {
				_currentActivity--;
				_vkmActivity.redraw();
			}

			return true;
		}

		return false;
	}
}
