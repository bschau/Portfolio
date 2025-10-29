package dk.schau.vkm;

import javax.microedition.lcdui.Graphics;

public class AboutScreen
{
	public static void draw(Graphics g)
	{
		Utils.clearScreen(g);
		Utils.writePage(g, Locale.get("ABOUT"), Utils.sfH);
	}

	public static void keyPressed(int ga, int kc)
	{
		Utils.screen = C.ScreenPrefs;
	}
}
