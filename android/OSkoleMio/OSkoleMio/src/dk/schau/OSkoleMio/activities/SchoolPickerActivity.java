package dk.schau.OSkoleMio.activities;

import dk.schau.OSkoleMio.R;
import dk.schau.OSkoleMio.SchoolsCollection;
import dk.schau.OSkoleMio.adapters.SchoolPickerIndexerAdapter;
import dk.schau.OSkoleMio.handlers.DownloadSchoolsHandler;
import dk.schau.OSkoleMio.threads.DownloadSchoolsThread;
import dk.schau.OSkoleMio.vos.Login;
import dk.schau.OSkoleMio.vos.School;

import android.app.ListActivity;
import android.app.ProgressDialog;
import android.content.Intent;
import android.content.res.Configuration;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.widget.ListView;
import android.widget.Toast;

public class SchoolPickerActivity extends ListActivity {
	private Login _bundleLogin = null;
	private ProgressDialog _progressDialog = null;
	private ListView _schoolPicker;
	private SchoolPickerIndexerAdapter<String> _adapter;
	private SchoolPickerActivity _schoolPickerActivity;

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.schoolpicker);
		
		_schoolPickerActivity = this;

		Bundle bundle = getIntent().getExtras();
		if (bundle != null && bundle.containsKey("child")) {
			_bundleLogin = new Login(bundle.getString("child"));
		}

		resetSchoolPicker();
		Toast.makeText(this, SchoolsCollection.schools.size() + " skoler ...", Toast.LENGTH_SHORT).show();
	}

	private void resetSchoolPicker() {
		_schoolPicker = (ListView) getListView();
		_adapter = new SchoolPickerIndexerAdapter<String>(this, SchoolsCollection.getAllNames(this));
		_schoolPicker.setAdapter(_adapter);
	}

	@Override
	public void onConfigurationChanged(Configuration newConfig) {
		super.onConfigurationChanged(newConfig);
	}

	@Override
	protected void onListItemClick(ListView listView, View view, int position, long id) {
		goToEditLogin(SchoolsCollection.schools.get(position));
	}

	private void goToEditLogin(School school) {
		Intent intent = new Intent(this, EditLoginActivity.class);
		if (school != null) {
			if (_bundleLogin == null) {
				_bundleLogin = new Login();
			}

			if (_bundleLogin.getName().trim().length() == 0) {
				_bundleLogin.setName(school.getName());
			}

			_bundleLogin.setUrl(school.getUrl());
		}

		if (_bundleLogin != null) {
			intent.putExtra("child", _bundleLogin.toString());
		}

		startActivity(intent);
	}
	
	@Override
    public boolean onCreateOptionsMenu(Menu menu) {
		super.onCreateOptionsMenu(menu);
		MenuInflater inflater = getMenuInflater();
		inflater.inflate(R.menu.schoolpicker_actionbar_menu, menu);
		return true;
    }

	@Override
	public boolean onOptionsItemSelected(MenuItem item) {
		switch (item.getItemId()) {
			case R.id.menu_back:
				finish();
				return true;
			
			case R.id.menu_update:
				_progressDialog = ProgressDialog.show(this, "Øjeblik", "Henter den seneste Skole liste ...", true, false);

				DownloadSchoolsHandler downloadSchoolsHandler = new DownloadSchoolsHandler(_schoolPickerActivity);
				DownloadSchoolsThread downloadSchoolsThread = new DownloadSchoolsThread(downloadSchoolsHandler);
				downloadSchoolsThread.start();
				return true;
				
			case R.id.menu_email:
				sendEmail();
				return true;
				
			case R.id.menu_info:
				startActivity(new Intent(this, AboutActivity.class));
				return true;
		}

		return super.onOptionsItemSelected(item);
	}

	private void sendEmail() {
		Intent email = new Intent(Intent.ACTION_SEND);
		email.setType("message/rfc822");
		email.putExtra(Intent.EXTRA_EMAIL, new String[] { getString(R.string.email_to) });
		email.putExtra(Intent.EXTRA_SUBJECT, getString(R.string.email_subject));
		email.putExtra(Intent.EXTRA_TEXT, getString(R.string.email_body));
		
		startActivity(Intent.createChooser(email, "Send mail ..."));
	}

	public void onDownloadFinished(String xml) {
		try {
			if (xml == null) {
				throw new Exception("Error");
			}

			SchoolsCollection.setRemoteXml(xml, this);
			if (SchoolsCollection.init(_schoolPickerActivity)) {
				resetSchoolPicker();
				Toast.makeText(_schoolPickerActivity, "Seneste Skole liste er hentet og " + SchoolsCollection.schools.size() + " skoler er installeret.", Toast.LENGTH_LONG).show();
			} else {
				Toast.makeText(_schoolPickerActivity, "Åh nej! Skolelisten er korrupt - prøv igen senere.", Toast.LENGTH_LONG).show();
			}
		} catch (Exception exception) {
			Toast.makeText(_schoolPickerActivity, "Åh nej! Kan ikke hente den seneste Skole liste. Prøv igen senere.", Toast.LENGTH_LONG).show();
		} finally {
			if (_progressDialog != null) {
				_progressDialog.dismiss();
				_progressDialog = null;
			}
		}
	}

	public void onMenuButtonClick(View view) {
		openOptionsMenu();
    }
}
