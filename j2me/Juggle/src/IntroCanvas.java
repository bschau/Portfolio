/*
vim: ts=2 sw=2 et
*/
import javax.microedition.lcdui.*;

class IntroCanvas extends Canvas {
	private static final String strTitle1="Juggle!";
	private static final String strTitle2="By Brian Schau";

	public void paint(Graphics g) {
		int width=getWidth();
		int height=getHeight();
		Font defaultFont=g.getFont();
		Font fntLarge=Font.getFont(Font.FACE_SYSTEM, Font.STYLE_PLAIN, Font.SIZE_LARGE);
		int y=g.getColor();

    g.setColor(0xffffff);
    g.fillRect(0, 0, width, height);
    g.setColor(y);

		y=(height-(defaultFont.getHeight()+4+fntLarge.getHeight()))/2;

		g.setFont(fntLarge);
		g.drawString(strTitle1, (width-fntLarge.stringWidth(strTitle1))/2, y, 0);
		y+=fntLarge.getHeight()+4;
		g.setFont(defaultFont);
		g.drawString(strTitle2, (width-defaultFont.stringWidth(strTitle2))/2, y, 0);
	}
}
