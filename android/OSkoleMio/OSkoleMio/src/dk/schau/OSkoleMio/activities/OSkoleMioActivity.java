package dk.schau.OSkoleMio.activities;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.Collections;
import java.util.List;

import dk.schau.OSkoleMio.DB;
import dk.schau.OSkoleMio.R;
import dk.schau.OSkoleMio.SchoolsCollection;
import dk.schau.OSkoleMio.adapters.LoginAdapter;
import dk.schau.OSkoleMio.comparators.LoginComparator;
import dk.schau.OSkoleMio.vos.Login;

import android.app.AlertDialog;
import android.app.Dialog;
import android.app.ListActivity;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.view.ContextMenu;
import android.view.ContextMenu.ContextMenuInfo;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.AdapterView;
import android.widget.Button;
import android.widget.ListView;

public class OSkoleMioActivity extends ListActivity {
	private static final String _SCHOOLSPREFSNAME = "SchoolsPrefs";
	private static final String _SAVEDVERSION = "SavedVersion";
	private static int _CURRENT_MESSAGES_LEVEL = 4;
	private boolean _autoLaunch = true;
	private List<Login> _logins;
	private LoginAdapter _loginAdapter;

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		
		try {
			copyBundledSchoolsFile();
		} catch (Exception ex) { }
		
		SchoolsCollection.init(this);
		DB database = new DB(this);
		_logins = database.selectAll(database.getReadableDatabase());
		Collections.sort(_logins, new LoginComparator());
		database.close();

		Bundle bundle = getIntent().getExtras();

		if (bundle != null) {
			if (bundle.containsKey("noauto")) {
				_autoLaunch = false;
			}
		}

		setContentView(R.layout.loginlist);

		_loginAdapter = new LoginAdapter(this, R.layout.listitem, _logins);
		setListAdapter(_loginAdapter);

		registerForContextMenu(getListView());

		if (!showMessages()) {
			handleAutoStart();
		}
	}
	
	private void copyBundledSchoolsFile() throws IOException {
		File path = SchoolsCollection.getExternalFilesDir(this);
		path.mkdirs();
		
		if (shouldCopy() == false) {
			return;
		}
		
		copyFile();
		
		SharedPreferences settings = getSharedPreferences(_SCHOOLSPREFSNAME, 0);
		SharedPreferences.Editor editor = settings.edit();
		editor.putString(_SAVEDVERSION, getCurrentVersion());
		editor.commit();
	}
	
	private boolean shouldCopy() {
		File file = SchoolsCollection.getSchoolsFile(this);
		if (!file.exists()) {
			return true;
		}
		
		String currentVersion = getCurrentVersion();
		
		SharedPreferences settings = getSharedPreferences(_SCHOOLSPREFSNAME, 0);
		String saveVersion = settings.getString(_SAVEDVERSION, null);
		
		if (saveVersion.compareTo(currentVersion) == 0) {
			return false;
		}
		
		return true;
	}
	
	private String getCurrentVersion() {
		try {
			return getPackageManager().getPackageInfo(getPackageName(), 0).versionName;
		} catch (Exception exception) { }

		return "?";
	}
	
	private void copyFile() throws IOException {
		InputStream input = getAssets().open(getString(R.string.assetsschoolfile));
		File file = SchoolsCollection.getSchoolsFile(this);
		OutputStream output = new FileOutputStream(file);

		byte[] buffer = new byte[2048];
		int length;
		while ((length = input.read(buffer)) > 0) {
			output.write(buffer, 0, length);
		}

		output.flush();
		output.close();
		input.close();
	}

	private void handleAutoStart() {
		if (_logins.size() == 0) {
			startActivity(new Intent(this, EditLoginActivity.class));
			return;
		}

		if (_logins.size() == 1 && _autoLaunch) {
			Intent intent = new Intent(this, WebControllerActivity.class);
			intent.putExtra("id", ((Login) _logins.get(0)).getId());
			startActivity(intent);
		}
	}

	@Override
    public boolean onCreateOptionsMenu(Menu menu) {
		super.onCreateOptionsMenu(menu);
		MenuInflater inflater = getMenuInflater();
		inflater.inflate(R.menu.loginlist_actionbar_menu, menu);
		return true;
    }

	@Override
	public boolean onOptionsItemSelected(MenuItem item) {
		switch (item.getItemId()) {
			case R.id.menu_info:
				startActivity(new Intent(this, AboutActivity.class));
				return true;
				
			case R.id.menu_new:
				startActivity(new Intent(this, EditLoginActivity.class));
				return true;
		}

		return super.onOptionsItemSelected(item);
	}

	@Override
	protected void onListItemClick(ListView listView, View view, int position, long id) {
		Intent intent = new Intent(this, WebControllerActivity.class);
		intent.putExtra("id", ((Login) _logins.get(position)).getId());
		startActivity(intent);
	}

	@Override
	public void onCreateContextMenu(ContextMenu menu, View view, ContextMenuInfo menuInfo) {
		super.onCreateContextMenu(menu, view, menuInfo);

		AdapterView.AdapterContextMenuInfo info = (AdapterView.AdapterContextMenuInfo) menuInfo;

		String header = "";

		if (info.position >= 0) {
			if ((header = _logins.get(info.position).getName()) == null) {
				header = "";
			}
		}

		if (header.length() < 1) {
			header = "Ukendt";
		}

		menu.setHeaderTitle(header);
		menu.add(Menu.NONE, 0, 0, "Rediger");
		menu.add(Menu.NONE, 1, 1, "Slet");
	}

	@Override
	public boolean onContextItemSelected(android.view.MenuItem item) {
		AdapterView.AdapterContextMenuInfo info = (AdapterView.AdapterContextMenuInfo) item.getMenuInfo();
		Login login = _logins.get(info.position);

		if (item.getItemId() == 0) {
			Intent intent = new Intent(this, EditLoginActivity.class);
			intent.putExtra("id", login.getId());
			startActivity(intent);
			return true;
		}
		
		deleteLogin(login);
		return true;
	}

	private void deleteLogin(final Login login) {
		AlertDialog.Builder builder = new AlertDialog.Builder(this);

		builder.setMessage("Er det OK at slette " + login.getName() + "?").setCancelable(false).setPositiveButton("Ja", new DialogInterface.OnClickListener() {
			public void onClick(DialogInterface dialog, int id) {
				DB database = new DB(getApplicationContext());
				database.deleteChildById(database.getWritableDatabase(), login.getId());
				database.close();

				for (Login tmp : _logins) {
					if (tmp.getId() == login.getId()) {
						_logins.remove(tmp);
						break;
					}
				}

				_loginAdapter.notifyDataSetChanged();

				if (_logins.size() == 0) {
					startActivity(new Intent(getApplicationContext(), EditLoginActivity.class));
				}
			}
		}).setNegativeButton("Nej", new DialogInterface.OnClickListener() {
			public void onClick(DialogInterface dialog, int id) {
				dialog.cancel();
			}
		});

		AlertDialog alert = builder.create();
		alert.show();
	}

	private boolean showMessages() {
		SharedPreferences preferences = getSharedPreferences("OSkoleMio", Context.MODE_PRIVATE);
		if (preferences != null) {
			int level = preferences.getInt("MESSAGES_LEVEL", 0);

			if (level < _CURRENT_MESSAGES_LEVEL) {
				popupMessages(preferences);
				return true;
			}
		}

		return false;
	}

	private void popupMessages(final SharedPreferences preferences) {
		final Dialog dialog = new Dialog(this);
		dialog.setContentView(R.layout.messagedialog);
		dialog.setTitle(getString(R.string.msgtitle));
		dialog.setCancelable(false);

		Button button = (Button) dialog.findViewById(R.id.ok);
		button.setOnClickListener(new OnClickListener() {
			public void onClick(View view) {
				SharedPreferences.Editor editor = preferences.edit();
				editor.putInt("MESSAGES_LEVEL", _CURRENT_MESSAGES_LEVEL);
				editor.commit();
				dialog.cancel();
				handleAutoStart();
			}
		});

		dialog.show();
	}

	public void onMenuButtonClick(View view) {
		openOptionsMenu();
    }
}
