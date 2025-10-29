import javax.microedition.lcdui.*;
import javax.microedition.midlet.*;

/**
 * TestMIDlet
 *
 * A simple test MIDlet for the j2mec suite.   It shows how to use
 * some of the pre-processor constants.
 */
public class TestMIDlet extends MIDlet implements CommandListener {
	protected boolean started=false;
	private Form fTest;
	private Display display;

	/**
	 * startApp
	 *
	 * MIDlet entry point.
	 */
	protected void startApp() {
		if (!started) {
			started=true;
			display=Display.getDisplay(this);

			try {
				Image img=Image.createImage("/${IMAGENAME}");	// Point to the Neko image.

				fTest=new Form("Test");
				fTest.append(new ImageItem(null, img, ImageItem.${POSITION}, null));	// Position is just a shorthand for:
			//
			// LAYOUT_CENTER

				fTest.append("\n${@include/file.txt}\n");	// Text
				fTest.append("\nj2mec can be found at\nhttp://www.schau.com/j2mec/index.html\n\n");
				fTest.append("The setting of the LANG variable was '${$LANG}' at compile time.\n");
			} catch (Exception ex) {
				fTest=new Form("Error");
				fTest.append("Cannot load image ... (or som'thin!)");
			}

			fTest.setCommandListener(this);
			display.setCurrent(fTest);
		}
	}

	/**
	 * pauseApp
	 *
	 * Send destroy signal.
	 */
	protected void pauseApp() {
		notifyDestroyed();
	}

	/**
	 * destroyApp
	 *
	 * Terminate MIDlet.
	 */
	protected void destroyApp(boolean unconditional) {}

	/**
	 * commandAction
	 *
	 * Command handler.
	 */
	public void commandAction(Command c, Displayable d) {}

	/*
	 * Below follows some funky comment magic ..
	 *
	 * // Embedding a single line comment into a multiline comment.
	 */

	// /* /* /* more untermintate comments */ */ */
	
	/* /* /* multi embedded comments */ */ */

	/* ${A-UNCOMMENTED-SWITCH} */

	// ${MUST-BE-IGNORED}
}
