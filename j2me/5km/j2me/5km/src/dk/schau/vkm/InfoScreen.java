package dk.schau.vkm;

import java.util.Vector;

import javax.microedition.lcdui.Canvas;
import javax.microedition.lcdui.Graphics;

public class InfoScreen
{
	private static StringBuffer sBuf = new StringBuffer(80);
	private static StringBuffer wBuf = new StringBuffer(30);
	private static Vector lines = null;
	private static String txt;
	private static int curLine, maxLines, pageSize, xOff;

	public static void init()
	{
		if (lines == null)
		{
			lines = new Vector(50, 10);
			txt = Locale.get("HELP");

			pageSize = Utils.screenH / Utils.sfH;

			findLines();
			maxLines = lines.size();
			xOff = (Utils.screenW - findWidth()) / 2;
		}

		curLine = 0;
		Utils.screen = C.ScreenInfo;
	}

	public static void reset()
	{
		lines = null;
	}

	static private void findLines()
	{
		int len = txt.length(), idx = 0, x = 0;
		char c;

		sBuf.delete(0, sBuf.length());
		wBuf.delete(0, wBuf.length());
		while (idx < len)
		{
			c = txt.charAt(idx);
			if (c == '\n')
			{
				addWBuf(x);
				if (sBuf.length() > 0)
				{
					lines.addElement(sBuf.toString().trim());
					sBuf.delete(0, sBuf.length());
				}
				else
				{
					lines.addElement(" ");
				}
				x = 0;
			}
			else
			{
				wBuf.append(c);
				if (c == ' ')
				{
					x = addWBuf(x);
				}
			}

			idx++;
		}
	}

	static private int addWBuf(int x)
	{
		String s = wBuf.toString();

		if (s.trim().length() > 0)
		{
			int w = Utils.stdFont.stringWidth(s);

			x += w;
			if (x > Utils.screenW)
			{
				if (sBuf.length() > 0)
				{
					lines.addElement(sBuf.toString().trim());
					sBuf.delete(0, sBuf.length());
					x = w;
				}
				else
				{
					x = 0;
				}
			}

			sBuf.append(s);
		}

		wBuf.delete(0, wBuf.length());
		return x;
	}

	static private int findWidth()
	{
		int width = 0, i, w;
		String s;

		for (i = 0; i < maxLines; i++)
		{
			s = (String) lines.elementAt(i);
			w = Utils.stdFont.stringWidth(s);
			if (w > width)
			{
				width = w;
			}
		}

		return width;
	}

	public static void draw(Graphics g)
	{
		int y = 0, idx = curLine, l;

		Utils.clearScreen(g);
		g.setFont(Utils.stdFont);
		g.setColor(0x000000);

		for (l = 0; l < pageSize; l++)
		{
			g.drawString((String) lines.elementAt(idx), xOff, y, C.ANCHOR);
			y += Utils.sfH;
			idx++;
		}
	}

	public static void keyPressed(int ga, int kc)
	{
		if (ga == Canvas.UP || kc == Canvas.KEY_NUM2)
		{
			if (curLine > 0)
			{
				curLine--;
			}
		}
		else if (ga == Canvas.DOWN || kc == Canvas.KEY_NUM8)
		{
			if (curLine < maxLines - pageSize)
			{
				curLine++;
			}
		}
		else if (ga == Canvas.FIRE || ga == Canvas.GAME_A || kc == Canvas.KEY_NUM5)
		{
			Utils.screen = C.ScreenPrefs;
		}
	}
}