package dk.schau.vkm;

import javax.microedition.lcdui.Graphics;

public class C
{
	public static String DBCOMPLETED = "Spongebob";
	public static String DBSTATE = "Squidward";

	public static final int ScreenInitGfx = 1;
	public static final int ScreenInitPrefs = 10;
	public static final int ScreenPrefs = 12;
	public static final int ScreenInitRun = 20;
	public static final int ScreenRun = 22;
	public static final int ScreenDoneSave = 30;
	public static final int ScreenDone = 31;
	public static final int ScreenContinue = 40;
	public static final int ScreenInfoInit = 50;
	public static final int ScreenInfo = 51;
	public static final int ScreenAbout = 60;

	public static final int ANCHOR = Graphics.LEFT | Graphics.TOP;
	public static final int LOGOIDX = 0;
	public static final int SYMBOLIDX = 1;
	public static final int ACTIVITYIDX = 2;
	public static final int INTROIDX = 3;
	public static final int SYMBOLSIZE = 4;
	public static final int ACTIVITYSIZE = 5;
	public static final int INTROSIZE = 6;
	public static final int RSYMBOLSIZE = 7;
	public static final int RACTIVITYSIZE = 8;
	public static final int RINTROSIZE = 9;
	public static final int BARHEIGHT = 10;
	public static final int BARPADDING = 11;
	public static final int SPACING = 12;
	public static final int COMPLETEDIDX = 13;
	public static final int KIND_SYMBOL = 0;
	public static final int KIND_ACTIVITY = 1;
	public static final int KIND_INTRO = 2;

	public static final String[] LOGOS = new String[]
	{
		"/logo-1.png",
		"/logo-2.png",
		"/logo-3.png"
	};

	public static final String[] SYMBOLS = new String[]
	{
		"/symbols-30.png",
		"/symbols-40.png",
		"/symbols-80.png",
		"/symbols-160.png",
		"/symbols-200.png"
	};

	public static final String[] RETURNS = new String[]
	{
		"/return-30.png",
		"/return-40.png",
		"/return-80.png",
		"/return-160.png",
		"/return-200.png"
	};

	public static final String[] DONES = new String[]
	{
		"/done-30.png",
		"/done-40.png",
		"/done-80.png",
		"/done-160.png",
		"/done-200.png"
	};

	public static final String[] COMPLETEDS = new String[]
	{
		"/completed-30.png",
		"/completed-60.png"
	};

	public static final int[][] CONFIG = new int[][]
	{
		{ 0, 2, 0, 1, 80, 30, 40, 9, 4, 5, 2, 1, 4, 0 },		// 128 x 128
		{ 1, 3, 1, 2, 160, 40, 80, 18, 5, 9, 3, 1, 4, 1 },		// 160 x 160
		{ 2, 4, 2, 2, 200, 80, 80, 20, 9, 18, 4, 2, 8, 1 }		// 200 x 200
	};
}
