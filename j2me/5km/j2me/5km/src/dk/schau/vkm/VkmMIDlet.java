package dk.schau.vkm;

import javax.microedition.lcdui.Alert;
import javax.microedition.lcdui.AlertType;
import javax.microedition.lcdui.Command;
import javax.microedition.lcdui.CommandListener;
import javax.microedition.lcdui.Display;
import javax.microedition.lcdui.Displayable;

import javax.microedition.midlet.MIDlet;
import javax.microedition.midlet.MIDletStateChangeException;

public class VkmMIDlet extends MIDlet implements CommandListener
{
	protected MainCanvas main;
	protected Display display;

	public VkmMIDlet()
	{
	}

	public void pauseApp()
	{
	}

	public void exit()
	{
		destroyApp(false);
		notifyDestroyed();
	}

	public void destroyApp(boolean unconditional) {
		if (main != null)
		{
			RunScreen.stopThread();
		}

		State.save();
		Program.saveCompleted();
	}

	public void startApp() throws MIDletStateChangeException
	{
		display = Display.getDisplay(this);

		try
		{
			Locale.init();

			main = new MainCanvas();
			main.setFullScreenMode(true);

			Program.loadCompleted();
			int startScreen = State.load();
			if (startScreen == C.ScreenInitPrefs)
			{
				Program.setFirstUncompleted();
			}

			PreferencesScreen.origDay = Program.getDay();
			main.init(this, startScreen);
			display.setCurrent(main);
			main.forceRepaint();
		}
		catch (Exception e)
		{
			throw new MIDletStateChangeException(e.getMessage());
		}
	}

	public void error(String title, String text)
	{
		Alert a = new Alert(title, text, null, AlertType.ERROR);

		a.setTimeout(Alert.FOREVER);
		a.setCommandListener(this);
		display.setCurrent(a);
	}

	public void commandAction(Command c, Displayable d)
	{
		if (c == Alert.DISMISS_COMMAND)
		{
			exit();
		}
	}
}
