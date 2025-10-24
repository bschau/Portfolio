package dk.schau.vkm.util;

import java.util.Hashtable;

public class Locale {
	private static Hashtable<String, String> hashTable = new Hashtable<String, String>();

	public static void init() {
		hashTable.clear();
		
		_english();

		if (getLocale().compareTo("da") == 0) {
			_danish();
		}
	}
	
	private static void _english() {
		hashTable.put("RUNFOR", "Run for");
		hashTable.put("WALKFOR", "Walk for");
		hashTable.put("RETURNRUNFOR", "Return run for");
		hashTable.put("RETURNWALKFOR", "Return walk for");
		hashTable.put("DAY", "Day");
		hashTable.put("ABOUT", "About");
		hashTable.put("VERSION", "Version");
		hashTable.put("ABOUTTEXT", "For support, tips and tricks, please visit:\n\nhttp://www.5km.dk/");
		hashTable.put("OK", "OK");
		hashTable.put("NO", "No");
		hashTable.put("YES", "Yes");
		hashTable.put("FINISH1", "Well done!");
		hashTable.put("FINISH2", "Tap the happy face to continue.");
		hashTable.put("PAUSETITLE", "Paused");
		hashTable.put("PAUSETEXT", "Running paused.\n\nDismiss this dialog to continue.\n");
		hashTable.put("STOPTITLE", "Ok to stop?");
		hashTable.put("STOPTEXT", "Is it ok to stop?\n");
		hashTable.put("EMAILBODY", "Hi Brian,\n\nI have a question about the 5km app:\n\n");
	}
	
	private static void _danish() {
		hashTable.put("RUNFOR", "Løb, ");
		hashTable.put("WALKFOR", "Gå, ");
		hashTable.put("RETURNRUNFOR", "Løb tilbage, ");
		hashTable.put("RETURNWALKFOR", "Gå tilbage, ");
		hashTable.put("DAY", "Dag");
		hashTable.put("ABOUT", "Om");
		hashTable.put("VERSION", "Version");
		hashTable.put("ABOUTTEXT", "For hjælp, tips og tricks, besøg:\n\nhttp://www.5km.dk/");
		hashTable.put("OK", "OK");
		hashTable.put("NO", "Nej");
		hashTable.put("YES", "Ja");
		hashTable.put("FINISH1", "Super!");
		hashTable.put("FINISH2", "Tryk på det gule ansigt for at fortsætte.");
		hashTable.put("PAUSETITLE", "På pause");
		hashTable.put("PAUSETEXT", "Løbet er sat på pause.\n\nFor at fortsætte tryk da OK.\n");
		hashTable.put("STOPTITLE", "Stop med at løbe?");
		hashTable.put("STOPTEXT", "Er det iorden at stoppe dette løb?\n");
		hashTable.put("EMAILBODY", "Hej Brian,\n\nJeg har et spørgsmål angående 5km appen:\n\n");
	}
	
	public static String getLocale() {
		String language = Preferences.getString("LANGUAGE", null);
		
		if (language != null) {
			if (language.compareTo("en") == 0 || language.compareTo("da") == 0) {
				return language;
			}
		}
		
		return "";
	}

	public static String get(String key) {
		String value = (String) hashTable.get(key);
		
		return value == null ? "" : value;
	}
	
	private static void _select(String locale) {
		Preferences.putString("LANGUAGE", locale);
		init();
	}

	public static void selectDanish() {
		_select("da");
	}
	
	public static void selectEnglish() {
		_select("en");
	}
}
