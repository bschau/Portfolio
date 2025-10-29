package dk.schau.vkm.media;

import android.content.Context;
import android.media.MediaPlayer;
import android.os.Vibrator;

public class Feedback {
	private static Vibrator vibrator = null;
	private static MediaPlayer mediaPlayer = null;
	private static Context context;
	
	public static void init(Context client) {
		context = client;

		vibrator = (Vibrator) context.getSystemService(Context.VIBRATOR_SERVICE);
	}

	public static void buzz(long millis) {
		if (vibrator != null) {
			vibrator.vibrate(millis);
		}
	}
	
	public static void stopBuzz() {
		if (vibrator != null) {
			vibrator.cancel();
		}
	}
	
	public static void sound() {
		stopSound();

		mediaPlayer = MediaPlayer.create(context, dk.schau.vkm.R.raw.turn);
		mediaPlayer.start();
	}
	
	public static void stopSound() {
		if (mediaPlayer != null) {
			mediaPlayer.release();
			mediaPlayer = null;
		}
	}
	
	public static void stopFeedback() {
		stopSound();
		stopBuzz();
	}	
}
