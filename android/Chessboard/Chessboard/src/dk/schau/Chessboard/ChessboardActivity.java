package dk.schau.Chessboard;

import wei.mark.standout.StandOutWindow;
import android.app.Activity;
import android.os.Bundle;

public class ChessboardActivity extends Activity {
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		
		StandOutWindow.closeAll(this, MultiWindow.class);
		StandOutWindow.show(this, MultiWindow.class, new MultiWindow().getUniqueId());
		finish();
	}
}