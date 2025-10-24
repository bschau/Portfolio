package dk.schau.GaetEnLyd;

import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.text.SpannableString;
import android.widget.TextView;

public class StatisticsDialog {
    public static void show(Context ctx) {
    	AlertDialog.Builder dlg = new AlertDialog.Builder(ctx);
    	String text;

    	if (Statistics.getTotalGuesses() == 0) {
    		text = ctx.getString(R.string.stat_no_statistics);	
    	} else {
        	StringBuilder sBld = new StringBuilder();

        	sBld.append(ctx.getString(R.string.stat_correct_guesses));
        	sBld.append(": ");
    		sBld.append(Statistics.getCorrectGuesses());

    		sBld.append("\n");
    		sBld.append(ctx.getString(R.string.stat_wrong_guesses));
    		sBld.append(": ");
    		sBld.append(Statistics.getWrongGuesses());

    		sBld.append("\n");
    		sBld.append(ctx.getString(R.string.stat_guesses));
    		sBld.append(": ");
    		sBld.append(Statistics.getTotalGuesses());

    		sBld.append("\n\n");
    		sBld.append(ctx.getString(R.string.stat_guess_percentage));
    		sBld.append(": ");
    		sBld.append((Statistics.getCorrectGuesses() * 100) / Statistics.getTotalGuesses());
    		
    		text = sBld.toString();
    	}
    	
		TextView message = new TextView(ctx);
		message.setPadding(5, 5, 5, 5);
		message.setText(new SpannableString(text));

		dlg.setPositiveButton("OK", new DialogInterface.OnClickListener() {
			public void onClick(DialogInterface dlg, int id) {
				dlg.cancel();
			}
		});
		
		dlg.setNegativeButton("Nulstil", new DialogInterface.OnClickListener() {
			public void onClick(DialogInterface dlg, int id) {
				Statistics.reset();
				dlg.cancel();
			}
		});
		
    	AlertDialog alert = dlg.create();
    	alert.setTitle("Statistik");
    	alert.setView(message);
    	alert.setCancelable(true);
    	alert.setIcon(R.drawable.icon);
    	alert.show();
    }
}