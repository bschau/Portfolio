import java.util.Hashtable;

public class da {
	/**
	 * Initialize the danish locale.
	 *
	 * @param ht Hashtable.
	 */
	public static void init(Hashtable ht) {
		ht.put("fatalError", "Fatal Fejl");

		ht.put("constW", "H");
		ht.put("constB", "S");

		ht.put("cmdBack", "Tilbage");
		ht.put("cmdOptions", "Valg");
		ht.put("cmdSave", "Gem");
		ht.put("cmdMarks", "Mærke");
		ht.put("cmdDone", "Færdig");

		ht.put("gAboutTitle", "Om");
		ht.put("gAboutVersion", "Version");
		ht.put("gAboutURL", "URL");

		ht.put("molEditBoard", "Ret brædt");
		ht.put("molResetBoard", "Nulstil brædt");
		ht.put("molAbout", "Om");
		ht.put("molExit", "Afslut");

		ht.put("mrkAddMark", "Tilføj mærke");
		ht.put("mrkMarks", "Mærker");

		ht.put("feTitle", "Ret");
		ht.put("feTurn", "Træk nummer:");
		ht.put("feWhite", "Hvid i træk");
		ht.put("feBlack", "Sort i træk");

		ht.put("mcTurn", "Træk");
		ht.put("mcEditMode", "*** Retter ***");

		ht.put("defaultSetup", "Start tilstand");

		ht.put("cYes", "Ja");
		ht.put("cNo", "Nej");
		ht.put("cResetBoardTitle", "Nulstil brættet?");
		ht.put("cResetBoardBody", "Vil du nulstille brættet? Det vil også slette alle dine markeringer.");
		ht.put("cGoToMarkTitle", "Gå til markering?");
		ht.put("cGoToMarkBody", "Vil du gå til den valgte markering? Det vil slette alle senere markeringer.");
	}
}
