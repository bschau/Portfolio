package dk.schau.BatteryRightNow;

import android.app.Activity;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.text.Html;
import android.text.SpannableStringBuilder;
import android.text.method.LinkMovementMethod;
import android.text.style.ClickableSpan;
import android.text.style.UnderlineSpan;
import android.view.View;
import android.widget.TextView;
import android.widget.Toast;

public class BatteryRightNowActivity extends Activity {
	private BroadcastReceiver batteryInformation = null;

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.main);

		setAboutText(this);
	}

	@Override
	public void onPause() {
		super.onPause();

		deleteBatteryReceiver();
	}

	@Override
	public void onResume() {
		super.onResume();

		deleteBatteryReceiver();
		
		batteryInformation = (BroadcastReceiver) new BatteryInformation(this);
		this.registerReceiver(batteryInformation, new IntentFilter(Intent.ACTION_BATTERY_CHANGED));
	}

	private void deleteBatteryReceiver() {
		if (batteryInformation != null) {
			this.unregisterReceiver(batteryInformation);
			batteryInformation = null;
		}
	}

	private void setAboutText(Context context) {
		String versionString = "";

		try {
			PackageInfo packageInfo = context.getPackageManager().getPackageInfo(context.getPackageName(), PackageManager.GET_META_DATA);
			versionString = String.format(getString(R.string.about_text), packageInfo.versionName);
		} catch (Exception ex) {}

		final Activity activity = this;
		TextView aboutText = (TextView) findViewById(R.id.about);		
		CharSequence sequence = Html.fromHtml(versionString);
		SpannableStringBuilder stringBuilder = new SpannableStringBuilder(sequence);
		UnderlineSpan[] underlines = stringBuilder.getSpans(0, stringBuilder.length(), UnderlineSpan.class);
		for (UnderlineSpan span : underlines) {
			int start = stringBuilder.getSpanStart(span);
			int end = stringBuilder.getSpanEnd(span);
			int flags = stringBuilder.getSpanFlags(span);
			ClickableSpan myActivityLauncher = new ClickableSpan() {
				@Override
				public void onClick(View view) {
					Intent i = new Intent(Intent.ACTION_SEND);
					i.setType("message/rfc822");
					i.putExtra(Intent.EXTRA_EMAIL, new String[] { activity.getString(R.string.email_to) } );
					i.putExtra(Intent.EXTRA_SUBJECT, activity.getString(R.string.email_subject));
					i.putExtra(Intent.EXTRA_TEXT, activity.getString(R.string.email_body));
					try {
					    startActivity(Intent.createChooser(i, "Send mail..."));
					} catch (android.content.ActivityNotFoundException ex) {
					    Toast.makeText(activity, "There are no configured email clients installed.", Toast.LENGTH_LONG).show();
					}
				}
			};

			stringBuilder.setSpan(myActivityLauncher, start, end, flags);
		}

		aboutText.setText(stringBuilder);
		aboutText.setMovementMethod(LinkMovementMethod.getInstance());
//		aboutText.setText(versionString);
	}

	public void updateView(BatteryInformation batteryInformation) {
		((TextView) findViewById(R.id.bi_present)).setText(batteryInformation.getPresent());
		((TextView) findViewById(R.id.bi_level)).setText(batteryInformation.getLevel());
		((TextView) findViewById(R.id.bi_voltage)).setText(batteryInformation.getVoltage());
		((TextView) findViewById(R.id.bi_temperature)).setText(batteryInformation.getTemperature());
		((TextView) findViewById(R.id.bi_technology)).setText(batteryInformation.getTechnology());
		((TextView) findViewById(R.id.bi_status)).setText(batteryInformation.getStatus());
		((TextView) findViewById(R.id.bi_health)).setText(batteryInformation.getHealth());
		((TextView) findViewById(R.id.bi_pluggedin)).setText(batteryInformation.getPluggedIn());
	}
}