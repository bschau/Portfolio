import javax.microedition.lcdui.Command;
import javax.microedition.lcdui.CommandListener;
import javax.microedition.lcdui.Display;
import javax.microedition.lcdui.Displayable;
import javax.microedition.lcdui.Form;
import javax.microedition.lcdui.Image;
import javax.microedition.lcdui.ImageItem;
import javax.microedition.lcdui.Item;
import javax.microedition.lcdui.StringItem;

import javax.microedition.midlet.MIDlet;

public class TestMIDlet extends MIDlet implements CommandListener {
	private final int LAYOUT = Item.LAYOUT_LEFT|Item.LAYOUT_NEWLINE_AFTER;
	private Display display;
	protected Form fMain;
	protected Command cmdExit;
	
	/**
	 * Constructor.
	 */
	public TestMIDlet() {}

	/**
	 * Pause app.
	 */
	public void pauseApp() {}

	/**
	 * Destroy app.
	 *
	 * @param unconditional unused.
	 */
	public void destroyApp(boolean unconditional) {}

	/**
	 * Start app. Open and load preferences and build GUI.
	 */
	public void startApp() {
		if (display == null) {
			StringItem s;

			display = Display.getDisplay(this);
			cmdExit = new Command(Trl.exit, Command.BACK, 0);

			fMain = new Form("Test MIDlet");
			try {
				Image logo = Image.createImage("/logo.png");
				fMain.append(new ImageItem("", logo, LAYOUT, Trl.noLogo));
			} catch (Exception ex) {
				s = new StringItem("", Trl.loadError);
				s.setLayout(LAYOUT);
				fMain.append(s);
			}

			s = new StringItem("", Trl.msg1);
			s.setLayout(LAYOUT);
			fMain.append(s);

			s = new StringItem("", Trl.msg2);
			s.setLayout(LAYOUT);
			fMain.append(s);

			fMain.addCommand(cmdExit);
			fMain.setCommandListener(this);

			display.setCurrent(fMain);
		}
	}

	/**
	 * Handle command.
	 *
	 * @param c Command.
	 * @param d Current displayable.
	 */
	public void commandAction(Command c, Displayable d) {
		if (c == cmdExit) {
			notifyDestroyed();
		}
	}
}
