package dk.schau.vkm.util;

import android.app.Activity;
import android.content.Context;
import android.content.SharedPreferences;

public class Preferences {
	private static SharedPreferences sharedPreferences;
	
	public static void init(Activity activity) {
		sharedPreferences = activity.getSharedPreferences("Vkm", Context.MODE_PRIVATE);
	}
	
	public static String getString(String key) {
		return getString(key, "");
	}
	
	public static String getString(String key, String defaultValue) {
		return sharedPreferences.getString(key, defaultValue);
	}
	
	public static void putString(String key, String value) {
		SharedPreferences.Editor editor = sharedPreferences.edit();

		editor.putString(key, value);
		editor.commit();		
	}

	public static int getInt(String key) {
		return getInt(key, 0);
	}
	
	public static int getInt(String key, int defaultValue) {
		return sharedPreferences.getInt(key, defaultValue);
	}
	
	public static void putInt(String key, int value) {
		SharedPreferences.Editor editor = sharedPreferences.edit();

		editor.putInt(key, value);
		editor.commit();		
	}

	public static void remove(String key) {
		SharedPreferences.Editor editor = sharedPreferences.edit();

		editor.remove(key);
		editor.commit();
	}
}
