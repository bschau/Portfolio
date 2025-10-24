package dk.schau.vkm;

import javax.microedition.lcdui.Canvas;
import javax.microedition.lcdui.Graphics;
import javax.microedition.lcdui.Image;

public class PreferencesScreen
{
	private static final int PADDING_LOGO = 3;
	private static final int PADDING_TEXT = 4;
	private static final String[] mainMenuKeys = new String[] {
		"MAINMENU1",
		"MAINMENU2",
		"MAINMENU3",
		"MAINMENU4",
		"MAINMENU5",
		"MAINMENU6",
		"MAINMENU7"
	};
	private static VkmMIDlet mid;
	private static boolean inMenu;
	private static int mainMenuWidth;
	private static int barY;
	public static Image imgLogo;
	public static int origDay;

	public static void bootstrap(int cfg) throws Exception
	{
		imgLogo = Image.createImage(C.LOGOS[C.CONFIG[cfg][C.LOGOIDX]]);
	}

	public static void init(VkmMIDlet m)
	{
		mid = m;
		Program.selectDay(Program.getDay());
		State.activity = 0;
		Utils.screen = C.ScreenPrefs;
		inMenu = false;
	}

	public static void draw(Graphics g)
	{
		short act = Program.getActivity(State.activity);
		int y = (Utils.screenH - imgLogo.getHeight() - PADDING_LOGO - Utils.sfH - PADDING_TEXT - Utils.sfH - PADDING_TEXT - C.CONFIG[Utils.cfg][C.INTROSIZE] - PADDING_TEXT - Utils.sfH) / 2, w;

		Utils.clearScreen(g);

		g.drawImage(imgLogo, (Utils.screenW - imgLogo.getWidth()) / 2, y, C.ANCHOR);

		y += imgLogo.getHeight() + PADDING_LOGO;
		g.setColor(0x000000);

		Utils.sBuf.delete(0,Utils. sBuf.length());
		Utils.sBuf.append(Locale.get("DAY"));
		Utils.sBuf.append("  ");

		if (Program.getDay() > 0)
		{
			Utils.sBuf.append("<<");
			Utils.sBuf.append("  ");
		}

		Utils.sBuf.append(Program.getDay() + 1);

		if (Program.getDay() < Program.MAX_DAYS - 1)
		{
			Utils.sBuf.append("  ");
			Utils.sBuf.append(">>");
		}

		Utils.centerString(g, Utils.sBuf.toString(), y);

		y += Utils.sfH + PADDING_TEXT;

		Utils.sBuf.delete(0, Utils.sBuf.length());
		Utils.sBuf.append(Locale.get("RUNTIME"));
		Utils.sBuf.append(' ');
		Utils.sBuf.append(Integer.toString(Program.totalTime / 60));
		Utils.sBuf.append(' ');
		Utils.sBuf.append(Locale.get("MINUTES"));
		Utils.centerString(g, Utils.sBuf.toString(), y);

		y += Utils.sfH + PADDING_TEXT;
		
		Utils.sBuf.delete(0, Utils.sBuf.length());
		w = Program.getTimeFromActivity(act);

		Utils.sBuf.append(Integer.toString(w / 60));
		Utils.sBuf.append(':');
		w = w % 60;
		if (w < 10)
		{
			Utils.sBuf.append('0');
		}
		Utils.sBuf.append(Integer.toString(w));

		String s = Utils.sBuf.toString();
		g.setFont(Utils.largeFont);
		w = (Utils.screenW - C.CONFIG[Utils.cfg][C.INTROSIZE] - PADDING_TEXT - Utils.largeFont.stringWidth(s)) / 2;
		Utils.drawSymbol(g, act, C.KIND_INTRO, w, y);

		if (Program.getCompleted(Program.getDay()))
		{
			g.drawImage(Utils.imgCompleted, w + ((C.CONFIG[Utils.cfg][C.INTROSIZE] - Utils.imgCompleted.getWidth()) / 2), y + ((C.CONFIG[Utils.cfg][C.INTROSIZE] - Utils.imgCompleted.getHeight()) / 2), C.ANCHOR);
		}

		w += C.CONFIG[Utils.cfg][C.INTROSIZE] + PADDING_TEXT;
		g.drawString(s, w, y + ((C.CONFIG[Utils.cfg][C.INTROSIZE] - Utils.sfH) / 2), C.ANCHOR);

		y += C.CONFIG[Utils.cfg][C.INTROSIZE] + PADDING_TEXT;

		Utils.centerString(g, Locale.get("ACCESSMENU"), y);

		if (inMenu)
		{
			drawMainMenu(g);
		}
	}

	public static void keyPressed(int ga, int kc)
	{
		if (inMenu)
		{
			if (ga == Canvas.FIRE ||
			    ga == Canvas.GAME_A ||
			    kc == Canvas.KEY_NUM5)
			{
				switch (barY)
				{
					case 1:
						Utils.screen = C.ScreenInitRun;
						break;

					case 2:
						Program.toggleCompleted();
						break;

					case 3:
						Program.selectDay(origDay);
						State.activity = 0;
						break;

					case 4:
						Utils.screen = C.ScreenInfoInit;
						break;

					case 5:	
						Utils.screen = C.ScreenAbout;
						break;

					case 6:
						mid.exit();
						break;
				}

				inMenu = false;
			}
			else if (ga == Canvas.UP || kc == Canvas.KEY_NUM2)
			{
				if (barY > 0)
				{
					barY--;
				}
			}
			else if (ga == Canvas.DOWN || kc == Canvas.KEY_NUM8)
			{
				if (barY < mainMenuKeys.length - 1)
				{
					barY++;
				}
			}
		}
		else
		{
			if (ga == Canvas.LEFT || kc == Canvas.KEY_NUM4)
			{
				if (Program.getDay() > 0)
				{
					Program.selectDay(Program.getDay() - 1);
					State.activity = 0;
				}
			}
			else if (ga == Canvas.RIGHT || kc == Canvas.KEY_NUM6)
			{
				if (Program.getDay() < Program.MAX_DAYS - 1)
				{
					Program.selectDay(Program.getDay() + 1);
					State.activity = 0;
				}
			}
			else if (ga == Canvas.UP || kc == Canvas.KEY_NUM2)
			{
				if (State.activity > 0)
				{
					State.activity--;
				}
			}
			else if (ga == Canvas.DOWN || kc == Canvas.KEY_NUM8)
			{
				if (State.activity < Program.activities - 1)
				{
					State.activity++;
				}
			}
			else if (ga == Canvas.FIRE || ga == Canvas.GAME_A || kc == Canvas.KEY_NUM5)
			{
				showMainMenu();
			}
		}
	}

	private static void showMainMenu()
	{
		int l, idx;

		mainMenuWidth = 0;
		for (idx = 0; idx < mainMenuKeys.length; idx++)
		{
			l = Utils.stdFont.stringWidth(Locale.get(mainMenuKeys[idx]));
			if (l > mainMenuWidth)
			{
				mainMenuWidth = l;
			}
		}

		mainMenuWidth += 8;
		barY = 0;
		inMenu = true;
	}

	private static void drawMainMenu(Graphics g)
	{
		int mH = (mainMenuKeys.length * Utils.sfH) + 6, idx;
		int x = (Utils.screenW - mainMenuWidth) / 2, y = (Utils.screenH - mH) / 2;

		g.setColor(0xffffff);
		g.fillRect(x, y, mainMenuWidth, mH);

		g.setColor(0xaaaaaa);
		g.drawRect(x, y, mainMenuWidth, mH);

		g.fillRect(x + 2, y + 3 + (barY * Utils.sfH), mainMenuWidth - 3, Utils.sfH);

		g.setColor(0x700777);
		y += 3;
		for (idx = 0; idx < mainMenuKeys.length; idx++)
		{
			Utils.centerString(g, Locale.get(mainMenuKeys[idx]), y);
			y += Utils.sfH;
		}
	}
}
