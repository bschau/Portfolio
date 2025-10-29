package dk.schau.CitiZr.Dialog;

import dk.schau.CitiZr.R;
import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.content.pm.PackageManager.NameNotFoundException;
import android.text.SpannableString;
import android.text.util.Linkify;
import android.widget.TextView;
import android.widget.Toast;

public class AboutDialog {
    public static void show(Context ctx) throws NameNotFoundException {
    	AlertDialog.Builder about = new AlertDialog.Builder(ctx);    	
		PackageInfo pInfo = ctx.getPackageManager().getPackageInfo(ctx.getPackageName(), PackageManager.GET_META_DATA);
		String versionInfo = pInfo.versionName;
		String aboutTitle = String.format("%s %s", ctx.getString(R.string.about_about), ctx.getString(R.string.app_name));
		String versionString = String.format("%s %s", ctx.getString(R.string.about_version), versionInfo);
		String aboutText = ctx.getString(R.string.about);

		TextView message = new TextView(ctx);
		SpannableString s = new SpannableString(aboutText);
		message.setPadding(5, 5, 5, 5);
		message.setText(versionString + "\n\n" + s);
		Linkify.addLinks(message, Linkify.ALL);

		final Context context = ctx;
		about.setPositiveButton(ctx.getString(R.string.ok), new DialogInterface.OnClickListener() {
			public void onClick(DialogInterface dlg, int id) {
				dlg.cancel();
			}
		});
		
		about.setNegativeButton(ctx.getString(R.string.help), new DialogInterface.OnClickListener() {
			public void onClick(DialogInterface dialog, int which) {
				Intent email = new Intent(Intent.ACTION_SEND);
				email.setType("message/rfc822");
				email.putExtra(Intent.EXTRA_EMAIL, new String[] { context.getString(R.string.email_to) } );
				email.putExtra(Intent.EXTRA_SUBJECT, context.getString(R.string.email_subject));
				email.putExtra(Intent.EXTRA_TEXT, context.getString(R.string.email_body));
				try {
				    context.startActivity(Intent.createChooser(email, context.getString(R.string.about_send_email)));
				} catch (android.content.ActivityNotFoundException ex) {
				    Toast.makeText(context, context.getString(R.string.about_noemail), Toast.LENGTH_LONG).show();
				}
			}
		});
		
    	AlertDialog alert = about.create();
    	alert.setTitle(aboutTitle);
    	alert.setView(message);
    	alert.setCancelable(true);
    	alert.setIcon(R.drawable.icon);
    	alert.show();
    }
}
