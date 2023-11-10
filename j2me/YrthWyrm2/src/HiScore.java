/***
 * HiScore.java
 */
class HiScore extends Canvas {
	private static final String txtHiScore="Hi Score";
	private static final String txtGameOver="* Game Over *";
	private static final String txtNewHiScore="New Hi: ";
	private static final String txtScore="Score: ";
	private boolean showHiScore=false, newHiScore=false;
	private int score;

	public void ShowHiScore() {
		showHiScore=true;
		newHiScore=false;
	}

	public void ShowGameOver(int s) {
		game=null;
		showHiScore=false;
		score=s;
		if (s>=hiScore) {
			hiScore=s;
			newHiScore=true;
			PreferencesSet();
		} else
			newHiScore=false;
	}

	public void paint(Graphics g) {
		Font fntDefault=g.getFont();
		Font fntLarge=Font.getFont(Font.FACE_SYSTEM, Font.STYLE_PLAIN, Font.SIZE_LARGE);
		int fDs, fLs, top;
		String s;

		g.setColor(0xffffff);
		g.fillRect(0, 0, screenW, screenH);

		g.setColor(0x000000);
		fDs=fntDefault.getHeight();
		fLs=fntLarge.getHeight();

		s=Integer.toString(hiScore);
		if (showHiScore==true) {
			top=(screenH-fLs-6-fLs)/2;
			g.setFont(fntLarge);
			g.drawString(txtHiScore, (screenW-fntLarge.stringWidth(txtHiScore))/2, top, 0);
			top+=6+fLs;
			g.drawString(s, (screenW-fntLarge.stringWidth(s))/2, top, 0);
		} else {
			top=(screenH-fLs-6-fDs)/2;
			g.setFont(fntLarge);
			g.drawString(txtGameOver, (screenW-fntLarge.stringWidth(txtGameOver))/2, top, 0);

			g.setFont(fntDefault);
			top+=fLs+6;

			if (newHiScore==true)
				s=txtNewHiScore+Integer.toString(hiScore);
			else
				s=txtScore+Integer.toString(score);

			g.setFont(fntDefault);
			g.drawString(s, (screenW-fntDefault.stringWidth(s))/2, top, 0);
		}
	}
}
