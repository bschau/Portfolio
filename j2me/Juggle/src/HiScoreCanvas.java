/*
vim: ts=2 sw=2 et
*/
import javax.microedition.lcdui.*;

class HiScoreCanvas extends Canvas {
	private static final String strTitle="High Scores";
	private static final String strGameA="Easy: ";
	private static final String strGameB="Hard: ";
	private Juggle parent;

	public void SetParent(Juggle j) {
		parent=j;
	}

	public void paint(Graphics g) {
		int width=getWidth();
		int height=getHeight();
		Font defaultFont=g.getFont();
		Font fntLarge=Font.getFont(Font.FACE_SYSTEM, Font.STYLE_PLAIN, Font.SIZE_LARGE);
		int y=g.getColor(), fLS, fDS, gameAStrW, gameBStrW, gameAW, gameBW;
		int leftW, rightW, dispW, lX, rX;
		String gA, gB;

    g.setColor(0xffffff);
    g.fillRect(0, 0, width, height);
    g.setColor(y);

		gameAStrW=defaultFont.stringWidth(strGameA);
		gameBStrW=defaultFont.stringWidth(strGameB);
		if (gameAStrW>gameBStrW) {
			leftW=gameAStrW;
    } else {
			leftW=gameBStrW;
    }

		gA=Integer.toString(parent.scoreA);
		lX=defaultFont.stringWidth(gA);
		gB=Integer.toString(parent.scoreB);
		rX=defaultFont.stringWidth(gB);
		if (lX>rX) {
			rightW=lX;
    } else {
			rightW=rX;
    }

		dispW=leftW+4+rightW;
		lX=(width-dispW)/2;
		rX=lX+dispW;

		fLS=fntLarge.getHeight();
		fDS=defaultFont.getHeight();

		y=(height-fLS-4-fDS-2-fDS)/2;

		g.setFont(fntLarge);
		g.drawString(strTitle, (width-fntLarge.stringWidth(strTitle))/2, y, 0);
		y+=fLS+4;
		g.setFont(defaultFont);

		g.drawString(strGameA, lX, y, 0);
		g.drawString(gA, rX-defaultFont.stringWidth(gA), y, 0);

		y+=fDS+2;
		g.drawString(strGameB, lX, y, 0);
		g.drawString(gB, rX-defaultFont.stringWidth(gB), y, 0);
	}
}
