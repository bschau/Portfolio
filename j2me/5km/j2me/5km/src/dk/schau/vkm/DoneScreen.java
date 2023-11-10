package dk.schau.vkm;

import javax.microedition.lcdui.Graphics;

public class DoneScreen
{
	public static void draw(Graphics g)
	{
		int s = C.CONFIG[Utils.cfg][C.SYMBOLSIZE], y;

		Utils.clearScreen(g);

		y = (Utils.screenH - s - C.CONFIG[Utils.cfg][C.SPACING] - Utils.sfH) / 2;
		g.drawImage(Utils.imgDone[0], (Utils.screenW - s) / 2, y, C.ANCHOR);

		y += s + C.CONFIG[Utils.cfg][C.SPACING] + Utils.sfH;
		if (y > Utils.screenH - Utils.sfH)
		{
			y = Utils.screenH - Utils.sfH;
		}

		g.setColor(0x000000);
		Utils.centerString(g, Locale.get("CONTINUE"), y);
	}

	public static void keyPressed(int ga, int kc)
	{
		Utils.screen = C.ScreenInitPrefs;
	}
}
