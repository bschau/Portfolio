package dk.schau.vkm;

import javax.microedition.lcdui.Font;
import javax.microedition.lcdui.Graphics;
import javax.microedition.lcdui.Image;

import javax.microedition.lcdui.game.Sprite;

public class Utils
{
	public static Font largeFont = Font.getFont(Font.FACE_PROPORTIONAL, Font.STYLE_BOLD, Font.SIZE_LARGE);
	public static Font stdFont = Font.getDefaultFont();
	public static StringBuffer sBuf = new StringBuffer();
	public static int screenW, screenH;
	public static int screen, nextScreen;
	public static int cfg, lfH, sfH;
	public static Image[] imgSymbols = new Image[2];
	public static Image[] imgActivities = new Image[2];
	public static Image[] imgIntro = new Image[2];
	public static Image[] imgDone = new Image[2];
	public static Image imgCompleted;

	public static void initialize(int w, int h, boolean resized) throws Exception
	{
		screenW = w;
		screenH = h;

		if (screenH >= 240)
		{
			cfg = 2;
		}
		else if (screenH >= 176)
		{
			cfg = 1;
		}
		else
		{
			cfg = 0;
		}

		imgSymbols[0] = null;
		imgSymbols[1] = null;
		imgActivities[0] = null;
		imgActivities[1] = null;
		imgIntro[0] = null;
		imgIntro[1] = null;
		imgDone[0] = null;
		imgDone[1] = null;
		imgCompleted = null;
		System.gc();

		imgSymbols[0] = Image.createImage(C.SYMBOLS[C.CONFIG[cfg][C.SYMBOLIDX]]);
		imgSymbols[1] = Image.createImage(C.RETURNS[C.CONFIG[cfg][C.SYMBOLIDX]]);
		imgActivities[0] = Image.createImage(C.SYMBOLS[C.CONFIG[cfg][C.ACTIVITYIDX]]);
		imgActivities[1] = Image.createImage(C.RETURNS[C.CONFIG[cfg][C.ACTIVITYIDX]]);
		imgIntro[0] = Image.createImage(C.SYMBOLS[C.CONFIG[cfg][C.INTROIDX]]);
		imgIntro[1] = Image.createImage(C.RETURNS[C.CONFIG[cfg][C.INTROIDX]]);

		imgDone[0] = Image.createImage(C.DONES[C.CONFIG[cfg][C.SYMBOLIDX]]);
		imgDone[1] = Image.createImage(C.DONES[C.CONFIG[cfg][C.ACTIVITYIDX]]);
		imgCompleted = Image.createImage(C.COMPLETEDS[C.CONFIG[cfg][C.COMPLETEDIDX]]);

		if (!resized)
		{
			lfH = largeFont.getHeight();
			sfH = stdFont.getHeight();

			PreferencesScreen.bootstrap(cfg);
			screen = nextScreen;
		}
	}

	public static void clearScreen(Graphics g)
	{
		g.setColor(0xffffff);
		g.fillRect(0, 0, screenW, screenH);
	}

	public static void drawSymbol(Graphics g, short act, int kind, int x, int y)
	{
		Image lrg, trn;
		int s;

		if (kind == C.KIND_SYMBOL)
		{
			lrg = imgSymbols[0];
			trn = imgSymbols[1];
		}
		else if (kind == C.KIND_ACTIVITY)
		{
			lrg = imgActivities[0];
			trn = imgActivities[1];
		}
		else
		{
			lrg = imgIntro[0];
			trn = imgIntro[1];
		}

		s = C.CONFIG[cfg][C.SYMBOLSIZE + kind];
		if (Program.isTurned(act))
		{
			g.drawRegion(trn, Program.isRun(act) ? 0 : s, 0, s, s, Sprite.TRANS_NONE, x, y, C.ANCHOR);
		}
		else
		{
			g.drawRegion(lrg, Program.isRun(act) ? 0 : s, 0, s, s, Sprite.TRANS_NONE, x, y, C.ANCHOR);
		}

		lrg = null;
		trn = null;
	}

	public static void popup(Graphics g, String txt)
	{
		int x, y, l;

		g.setFont(largeFont);
		l = largeFont.stringWidth(txt);
		x = (screenW - l) / 2;
		y = (screenH - 2 * lfH) / 2;
		g.setColor(0xeeeeee);
		g.fillRect(x, y, l, 2 * lfH);

		g.setColor(0x000000);
		g.drawString(txt, x, y + lfH / 2, C.ANCHOR);
		g.drawRect(x, y, l, 2 * lfH);
	}

	public static void centerString(Graphics g, String txt, int y)
	{
		int w = stdFont.stringWidth(txt);

		g.setFont(stdFont);
		g.drawString(txt, (screenW - w) / 2, y, C.ANCHOR);
	}

	public static void writePage(Graphics g, String txt, int y)
	{
		g.setColor(0x000000);
		g.setFont(stdFont);

		int len = txt.length();
		int idx = 0;
		while (idx < len)
		{
			int l, end;

			for (end = idx; end < len; end++)
			{
				if (txt.charAt(end) == '\n')
				{
					break;
				}
			}

			l = end - idx;
			g.drawSubstring(txt, idx, l, (Utils.screenW - Utils.stdFont.substringWidth(txt, idx, l)) / 2, y, C.ANCHOR);
			idx = end + 1;

			y += Utils.sfH + 2;
		}
	}
}
