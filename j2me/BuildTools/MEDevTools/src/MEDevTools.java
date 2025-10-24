package dk.schau.MEDevTools;

/**
 * Parent class for all Creator classes.
 *
 * ... just in case anyone tries to run us from the command line :-)
 */
public class MEDevTools {
	private final static String prefix = "dk.schau.MEDevTools";

	/**
	 * Main.
	 */
	public static void main(String[] args) {
		System.out.println("MEDevTools v" + Version.VERSION);
		System.out.println();
		System.out.println("Defined tasks:");
		System.out.println();
		System.out.println("    " + prefix + ".JadCreator");
		System.out.println("    " + prefix + ".ManifestCreator");
		System.out.println("    " + prefix + ".TrlClassCreator");
	}
}
