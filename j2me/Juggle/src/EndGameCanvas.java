/*
vim: ts=2 sw=2 et
*/
import javax.microedition.lcdui.*;

class EndGameCanvas extends Canvas {
	private static final String txtGameOver="Game Over!";
	private Display display;
	private boolean hiScore;
	private int score;
	private Juggle parent;

	public void SetDisplay(Display d) {
		display=d;
	}

	public void SetParent(Juggle j) {
		parent=j;
	}

	public void Show(int type, int s) {
		score=s;
		hiScore=false;
		if (type==1) {
			if (score>parent.scoreB) {
				parent.scoreB=score;
				hiScore=true;
			}
		} else {
			if (score>parent.scoreA) {
				parent.scoreA=score;
				hiScore=true;
			}
		}

		if (hiScore) {
			parent.UpdatePreferences();
    }

		display.setCurrent(this);
	}
	
	public void paint(Graphics g) {
		int width=getWidth();
		int height=getHeight();
		Font defaultFont=g.getFont();
		Font fntLarge=Font.getFont(Font.FACE_SYSTEM, Font.STYLE_PLAIN, Font.SIZE_LARGE);
		int y=g.getColor(), lfS, dfS;
		String nh;

    g.setColor(0xffffff);
    g.fillRect(0, 0, width, height);
    g.setColor(y);

		lfS=fntLarge.getHeight();
		dfS=defaultFont.getHeight();

		g.setFont(fntLarge);
		if (hiScore) {
			y=(height-lfS-4-dfS)/2;
			g.drawString(txtGameOver, (width-fntLarge.stringWidth(txtGameOver))/2, y, 0);
			y+=lfS+4;
			nh="New hi: "+Integer.toString(score);
			g.setFont(defaultFont);
			g.drawString(nh, (width-defaultFont.stringWidth(nh))/2, y, 0);
		} else {
			y=(height-lfS)/2;
			g.drawString(txtGameOver, (width-fntLarge.stringWidth(txtGameOver))/2, y, 0);
		}
	}
}
