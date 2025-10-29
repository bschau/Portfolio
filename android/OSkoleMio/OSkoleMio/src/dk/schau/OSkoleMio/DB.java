package dk.schau.OSkoleMio;

import java.util.ArrayList;
import java.util.List;

import dk.schau.OSkoleMio.vos.Login;

import android.content.ContentValues;
import android.content.Context;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteOpenHelper;

public class DB extends SQLiteOpenHelper {
	private static final String _DATABASENAME = "schools.db";
	private static final int _DATABASEVERSION = 1;

	public DB(Context context) {
		super(context, _DATABASENAME, null, _DATABASEVERSION);
	}

	@Override
	public void onCreate(SQLiteDatabase database) {
		String sql = "CREATE TABLE login (id INTEGER PRIMARY KEY AUTOINCREMENT," + "name TEXT COLLATE NOCASE," + "url TEXT," + "login TEXT," + "password TEXT)";
		database.execSQL(sql);
	}

	@Override
	public void onUpgrade(SQLiteDatabase database, int oldVersion, int newVersion) {
		/*
		 * if (oldVersion < DATABASE_VERSION) { String sql =
		 * "ALTER TABLE children RENAME TO login";
		 * 
		 * db.execSQL(sql); }
		 */
	}

	private void closeCursor(Cursor cursor) {
		if (cursor == null || cursor.isClosed()) {
			return;
		}
		
		cursor.close();
	}

	public List<Login> selectAll(SQLiteDatabase database) {
		Cursor cursor = database.query("login", new String[] { "id", "name", "url", "login", "password" }, null, null, null, null, "name");
		if (!cursor.moveToFirst()) {
			closeCursor(cursor);
			return new ArrayList<Login>();
		}
		
		List<Login> list = getLogins(cursor);
		closeCursor(cursor);
		return list;
	}

	private List<Login> getLogins(Cursor cursor) {
		List<Login> list = new ArrayList<Login>();
		
		do {
			list.add(new Login(cursor.getInt(0), cursor.getString(1), cursor.getString(2), cursor.getString(3), cursor.getString(4)));
		} while (cursor.moveToNext());
		
		return list;
	}

	public void saveLogin(SQLiteDatabase database, Login login) {
		ContentValues contentValues = new ContentValues();

		contentValues.put("name", login.getName());
		contentValues.put("url", login.getUrl());
		contentValues.put("login", login.getLogin());
		contentValues.put("password", login.getPassword());

		if (login.getId() == -1) {
			long id = database.insert("login", null, contentValues);
			login.setId((int) id);
		} else {
			database.update("login", contentValues, "id=" + login.getId(), null);
		}
	}

	public Login getLoginById(SQLiteDatabase database, int id) {
		Cursor cursor = database.query("login", new String[] { "id", "name", "url", "login", "password" }, "id=" + id, null, null, null, null);

		Login login = null;
		if (cursor.moveToFirst()) {
			login = new Login(cursor.getInt(0), cursor.getString(1), cursor.getString(2), cursor.getString(3), cursor.getString(4));
		}

		closeCursor(cursor);
		return login;
	}

	public void deleteChildById(SQLiteDatabase database, int id) {
		database.delete("login", "id=" + id, null);
	}
}
