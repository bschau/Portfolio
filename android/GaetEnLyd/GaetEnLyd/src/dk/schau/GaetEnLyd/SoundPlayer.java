package dk.schau.GaetEnLyd;

import android.content.Context;
import android.media.MediaPlayer;

public class SoundPlayer {
	private MediaPlayer mediaPlayer = null;
	private Context context;
	
	public SoundPlayer(Context ctx) {
		context = ctx;
	}

	public void play(int sndId, MediaPlayer.OnCompletionListener listener) {
		if (mediaPlayer != null) {
			mediaPlayer.release();
			mediaPlayer = null;
		}

		mediaPlayer = MediaPlayer.create(context, sndId);
		if (listener != null) {
			mediaPlayer.setOnCompletionListener(listener);
		}

		mediaPlayer.start();
	}	
}
