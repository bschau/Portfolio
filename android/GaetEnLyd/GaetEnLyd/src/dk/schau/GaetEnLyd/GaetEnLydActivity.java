package dk.schau.GaetEnLyd;

import android.media.AudioManager;
import android.media.MediaPlayer;
import android.app.Activity;
import android.content.pm.PackageManager.NameNotFoundException;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.animation.Animation;
import android.view.animation.AnimationUtils;
import android.widget.ImageButton;

public class GaetEnLydActivity extends Activity implements MediaPlayer.OnCompletionListener {
	private final int SNS_SILENT = 0;
	private final int SNS_PLAY_PROEV = 1;
	private final int SNS_PLAY_SOUND = 2;
	private final int FADE_IN = 1;
	private final int FADE_OUT = 2;
	private boolean playWelcome = true;
	private boolean buttonsLocked = true;
	private SoundPlayer soundPlayer;
	private Quiz quiz;
	private int sndNextStep;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
        
        this.setVolumeControlStream(AudioManager.STREAM_MUSIC);

    	((ImageButton) findViewById(R.id.button1)).setImageResource(R.drawable.blank);
    	((ImageButton) findViewById(R.id.button2)).setImageResource(R.drawable.blank);
    	((ImageButton) findViewById(R.id.button3)).setImageResource(R.drawable.blank);

        getNewQuiz();

        soundPlayer = new SoundPlayer(this);
        if (playWelcome) {
        	sndNextStep = SNS_PLAY_PROEV;
        	soundPlayer.play(R.raw.velkommen, this);
        }
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
	
			case R.id.statistics:
				StatisticsDialog.show(this);
				return true;
		}

		return false;
	}

	public void onCompletion(MediaPlayer mp) {
		switch (sndNextStep) {
		case SNS_SILENT:
			buttonsLocked = false;
			break;

		case SNS_PLAY_PROEV:			
			setupFade(FADE_IN);

			play(R.raw.proev, SNS_PLAY_SOUND);
			break;

		case SNS_PLAY_SOUND:
			play(quiz.getSoundToGuess(), SNS_SILENT);
			break;
		}
	}
	
	private void getNewQuiz() {
		quiz = QuizController.newQuiz();
	}

	private void setupFade(int fadeDir) {
		if (fadeDir == FADE_IN) {
			setupAnimation(R.id.button1, quiz.getImage1(), R.anim.fadein, 0);
			setupAnimation(R.id.button2, quiz.getImage2(), R.anim.fadein, 0);
			setupAnimation(R.id.button3, quiz.getImage3(), R.anim.fadein, 0);
		} else {
			setupAnimation(R.id.button1, quiz.getImage1(), R.anim.fadeout, 255);
			setupAnimation(R.id.button2, quiz.getImage2(), R.anim.fadeout, 255);
			setupAnimation(R.id.button3, quiz.getImage3(), R.anim.fadeout, 255);
		}
	}

	private void setupAnimation(int buttonId, int imgId, int fadeId, int alpha) {
		ImageButton ib = (ImageButton) findViewById(buttonId);
		ib.setImageResource(imgId);

		Animation animation = AnimationUtils.loadAnimation(this, fadeId);
		animation.setFillAfter(true);
		ib.startAnimation(animation);
	}

	private void play(int sndId, int nextStep) {
		sndNextStep = nextStep;
		soundPlayer.play(sndId, this);
	}
	
	public void button1Clicked(View view) {
		tryGuess(1);
	}
	
	public void button2Clicked(View view) {
		tryGuess(2);
	}
	
	public void button3Clicked(View view) {
		tryGuess(3);
	}

	private void tryGuess(int guess) {
		if (buttonsLocked == true) {
			return;
		}

		buttonsLocked = true;
		if (quiz.tryGuess(guess)) {
			Statistics.correctGuess();

			setupFade(FADE_OUT);

			getNewQuiz();

			play(R.raw.korrekt, SNS_PLAY_PROEV);
		} else {
			Statistics.wrongGuess();
			
			play(R.raw.forkert, SNS_PLAY_SOUND);
		}
	}
}