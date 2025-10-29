import java.util.Hashtable;

public class Locale {
	private static Hashtable ht = new Hashtable();

	/**
	 * Initialize Locale subsystem - also initializes country specific
	 * locale.
	 */
	public static void init() {
		ht.put("fatalError", "Fatal Error");

		ht.put("constW", "W");
		ht.put("constB", "B");

		ht.put("cmdBack", "Back");
		ht.put("cmdOptions", "Options");
		ht.put("cmdSave", "Save");
		ht.put("cmdMarks", "Marks");
		ht.put("cmdDone", "Done");

		ht.put("gAboutTitle", "About");
		ht.put("gAboutVersion", "Version");
		ht.put("gAboutURL", "URL");

		ht.put("molEditBoard", "Edit board");
		ht.put("molResetBoard", "Reset board");
		ht.put("molAbout", "About");
		ht.put("molExit", "Exit");

		ht.put("mrkAddMark", "Add mark");
		ht.put("mrkMarks", "Marks");

		ht.put("feTitle", "Edit");
		ht.put("feTurn", "Turn:");
		ht.put("feWhite", "White in turn");
		ht.put("feBlack", "Black in turn");

		ht.put("mcTurn", "Turn");
		ht.put("mcEditMode", "*** Edit ***");

		ht.put("defaultSetup", "Initial setup");

		ht.put("cYes", "Yes");
		ht.put("cNo", "No");
		ht.put("cResetBoardTitle", "Reset board?");
		ht.put("cResetBoardBody", "Do you want to reset the current board layout? This will delete all your marks.");
		ht.put("cGoToMarkTitle", "Go to mark?");
		ht.put("cGoToMarkBody", "Do you want to go to the selected mark? This will delete all marks set after this mark.");

		String loc = System.getProperty("microedition.locale").toLowerCase();
		if (loc.indexOf("da") > -1) {
			da.init(ht);
		}
	}

	/**
	 * Get the value of the specified key.
	 *
	 * @param key Key to get.
	 * @return value of empty string if key was not set.
	 */
	public static String get(String key) {
		String v = (String) ht.get(key);

		if (v == null) {
			return "";
		}

		return v;
	}
}
