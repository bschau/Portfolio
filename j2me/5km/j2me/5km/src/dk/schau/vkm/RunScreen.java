package dk.schau.vkm;

import javax.microedition.lcdui.AlertType;
import javax.microedition.lcdui.Canvas;
import javax.microedition.lcdui.Graphics;

public class RunScreen
{
	private static Thread thread;
	private static int exitTime;
	private static boolean showExit, showPaused;
	public static boolean threadRunning = false, runThread;

	public static void initFirst()
	{
		Program.selectDay(Program.getDay());

		State.activity = 0;
		nextActivity(0);
		bothRuns(false);
	}

	public static void initSubsequent()
	{
		Program.selectDay(Program.getDay());
		bothRuns(true);
	}

	private static void bothRuns(boolean pauseFlag)
	{
		Utils.screen = C.ScreenRun;
		showExit = false;
		showPaused = pauseFlag;

		runThread = true;
	}

	public static void startThread(Runnable r) {
		State.running = true;

		thread = new Thread(r);
		thread.start();
	}

	private static boolean nextActivity(int add)
	{
		short act;

		State.activity += add;
		
		act = Program.getActivity(State.activity);
		if (act == -1)
		{
			Program.setCompleted(Program.getDay());
			State.activityTime = -1;
			return false;
		}

		State.activityTime = Program.getTimeFromActivity(act);
		return true;
	}

	public static void draw(Graphics g)
	{
		int s = C.CONFIG[Utils.cfg][C.SYMBOLSIZE], a = C.CONFIG[Utils.cfg][C.ACTIVITYSIZE];
		short act = Program.getActivity(State.activity), nextAct;
		int x, y, l, t;

		Utils.clearScreen(g);

		x = (Utils.screenW - s) / 2;
		y = (Utils.screenH - s - (2 * C.CONFIG[Utils.cfg][C.BARHEIGHT]) - (4 * C.CONFIG[Utils.cfg][C.BARPADDING]) - C.CONFIG[Utils.cfg][C.SPACING]) / 2;
		l = x;
		t = y;
		Utils.drawSymbol(g, act, C.KIND_SYMBOL, l, t);

		g.setColor(0xcccccc);
		g.drawRect(l - 1, t - 1, s + 1, s + 1);

		g.setColor(0xffffff);
		l += (s - a);
		t += (s - a);
		g.fillRect(l, t, a, a);

		nextAct = Program.getActivity(State.activity + 1);
		if (Program.isDone(nextAct))
		{
			g.drawImage(Utils.imgDone[1], l, t, C.ANCHOR);
		}
		else
		{
			Utils.drawSymbol(g, nextAct, C.KIND_ACTIVITY, l, t);
		}

		g.setColor(0xcccccc);
		g.drawRect(l - 1, t - 1, a + 1, a + 1);

		g.setColor(0x000000);
		y += s + C.CONFIG[Utils.cfg][C.SPACING];
		t = y + (C.CONFIG[Utils.cfg][C.BARHEIGHT] * 2) + (C.CONFIG[Utils.cfg][C.BARPADDING] * 4);
		g.drawLine(x, y, x, t);
		g.drawLine(x + s - 1, y, x + s - 1, t);

		y += C.CONFIG[Utils.cfg][C.BARPADDING];
		g.setColor(0x0000dd);

		t = s - 2;
		l = t - (((State.activityTime - 1) * t) / Program.getTimeFromActivity(act));
		g.fillRect(x + 1, y, l, C.CONFIG[Utils.cfg][C.BARHEIGHT]);
		y += (C.CONFIG[Utils.cfg][C.BARPADDING] * 2) + C.CONFIG[Utils.cfg][C.BARHEIGHT];

		g.setColor(0x00dd00);
		l = (((State.activity + 1) * t) / Program.activities);
		g.fillRect(x + 1, y, l, C.CONFIG[Utils.cfg][C.BARHEIGHT]);

		if (showExit)
		{
			Utils.popup(g, Locale.get("EXIT"));
		}
		else if (showPaused) 
		{
			Utils.popup(g, Locale.get("PAUSED"));
		}
	}

	public static void keyPressed(int ga, int kc)
	{
		if (showPaused)
		{
			showPaused = false;
			return;
		}

		if (ga == Canvas.UP || ga == Canvas.GAME_B || kc == Canvas.KEY_NUM0)
		{
			showPaused = true;
		}
		else if (ga == Canvas.FIRE || ga == Canvas.GAME_A || kc == Canvas.KEY_NUM5)
		{
			if (showExit)
			{
				State.running = false;
				stopThread();
				Utils.screen = C.ScreenInitPrefs;
			}
			else
			{
				showExit = true;
				exitTime = 3;
			}
		 }
	}

	public static void stopThread()
	{
		runThread = false;

		if (thread != null)
		{
			while (threadRunning)
			{
				try
				{
					Thread.sleep(300);
				}
				catch (Exception ex)
				{
				}
			}

			thread = null;
		}
	}

	public static void run(VkmMIDlet mid) {
		if (!showPaused)
		{
			if (showExit)
			{
				exitTime--;
				if (exitTime == 0)
				{
					showExit = false;
				}
			}
			else
			{
				State.activityTime--;
				if (State.activityTime <= 0)
				{
					if (nextActivity(1) == false)
					{
						State.running = false;
						if (Program.getDay() < Program.MAX_DAYS)
						{
							Program.selectDay(Program.getDay() + 1);
						}

						Utils.screen = C.ScreenDoneSave;
						runThread = false;
					}

					AlertType.INFO.playSound(mid.display);
					mid.display.vibrate(2000);
				}
			}
		}
	}
}
