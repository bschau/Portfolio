package dk.schau.jStar;

/**
 * jStar.
 */
public class jStar {
	/** */
	private static GravityCanvas gc = new GravityCanvas();

	/**
	 * Main entry.
	 *
	 * @param args Command line arguments.
	 */
	public static void main(String[] args) {
		javax.swing.SwingUtilities.invokeLater(new Runnable() {
			public void run() {
				gc.main();
			}
		});
	}
}
