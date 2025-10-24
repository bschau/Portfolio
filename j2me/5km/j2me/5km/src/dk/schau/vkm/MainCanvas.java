package dk.schau.vkm;

import javax.microedition.lcdui.Canvas;
import javax.microedition.lcdui.Graphics;

public class MainCanvas extends Canvas implements Runnable
{
	private VkmMIDlet mid;

	public void init(VkmMIDlet m, int ns) throws Exception
	{
		mid = m;
		Utils.nextScreen = ns;
		Utils.screen = C.ScreenInitGfx;
	}

	public void forceRepaint()
	{
		repaint();
		serviceRepaints();
	}

	protected void sizeChanged(int w, int h)
	{
		try
		{
			Utils.initialize(w, h, true);
			InfoScreen.reset();
			if (Utils.screen == C.ScreenInfo)
			{
				InfoScreen.init();
			}

			repaint();
		}
		catch (Exception ex)
		{
			mid.error(Locale.get("NIT"), ex.getMessage());
		}
	}

	public void paint(Graphics g)
	{
		switch (Utils.screen)
		{
			case C.ScreenInitGfx:
				try
				{
					Utils.initialize(getWidth(), getHeight(), false);
				}
				catch (Exception ex)
				{
					mid.error(Locale.get("NIT"), ex.getMessage());
				}
				break;

			case C.ScreenInitPrefs:
				RunScreen.stopThread();
				PreferencesScreen.init(mid);
				break;

			case C.ScreenPrefs:
				PreferencesScreen.draw(g);
				break;

			case C.ScreenInitRun:
				RunScreen.initFirst();
				RunScreen.startThread(this);
				RunScreen.draw(g);
				break;

			case C.ScreenRun:
				RunScreen.draw(g);
				break;

			case C.ScreenDoneSave:
				Program.saveCompleted();
				State.save();
				Utils.screen = C.ScreenDone;
				break;

			case C.ScreenDone:
				DoneScreen.draw(g);
				break;

			case C.ScreenContinue:
				RunScreen.initSubsequent();
				RunScreen.startThread(this);
				break;

			case C.ScreenInfoInit:
				InfoScreen.init();
				break;

			case C.ScreenInfo:
				InfoScreen.draw(g);
				break;

			case C.ScreenAbout:
				AboutScreen.draw(g);
				break;
		}

		if (Utils.screen != C.ScreenRun)
		{
			repaint();
		}
	}

	protected void keyPressed(int keyCode)
	{
		int ga = 0;

		/*
		 * As per:
		 *
		 * http://www.j2meforums.com/wiki/index.php/GetGameAction
		 *
		 * Be careful with getGameAction, on some phones it will throw
		 * an exception when pressing certain keys (for instance on
		 * SE K700 passing the key code for the softkey).
		 *
		 * It's a good idea to always wrap it in a try block.
		 *
		 * Also on certain phones (e.g. some Samsungs) getGameAction
		 * doesn't automatically convert the 2,4,6,8 and 5 keys into
		 * direction and fire actions, so you have to check for these
		 * being pressed manually too. Do not rely on getgameaction
		 * doing it for you just because it does so in most emulators!
		 */
		try
		{
			ga = getGameAction(keyCode);
		}
		catch (Exception ex)
		{
		}

		switch (Utils.screen)
		{
			case C.ScreenPrefs:
				PreferencesScreen.keyPressed(ga, keyCode);
				break;

			case C.ScreenRun:
				RunScreen.keyPressed(ga, keyCode);
				break;

			case C.ScreenDone:
				DoneScreen.keyPressed(ga, keyCode);
				break;

			case C.ScreenInfo:
				InfoScreen.keyPressed(ga, keyCode);
				break;

			case C.ScreenAbout:
				AboutScreen.keyPressed(ga, keyCode);
				break;
		}

		repaint();
	}

	protected void hideNotify()
	{
		State.save();
		Program.saveCompleted();
	}

	public void run()
	{
		long next, diff;

		RunScreen.threadRunning = true;

		next = System.currentTimeMillis() + 1000;
		while (RunScreen.runThread)
		{
			while ((diff = System.currentTimeMillis()) < next)
			{
				try
				{
					Thread.sleep(next - diff);
				}
				catch (Exception ex)
				{
				}
			}
			
			next = System.currentTimeMillis() + 1000;
			RunScreen.run(mid);
			repaint();
		}

		RunScreen.threadRunning = false;
	}
}
