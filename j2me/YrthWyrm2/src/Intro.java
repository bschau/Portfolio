/**
 * Intro.java
 */
class Intro extends Canvas {
	private static final String title="YrthWyrm";
	private Player player=null;

	private byte[] tune=new byte[] {
		ToneControl.VERSION, 1,
		ToneControl.TEMPO, 30,
		ToneControl.BLOCK_START, 0,
		${F4}, ${d}, ${F4}, ${d}, ${C4}, ${d}, ${F4}, ${d},
		${E4}, ${d}, ${E4}, ${d}, ${C4}, ${d}, ${E4}, ${d},
		${rest}, ${d4}, ${rest}, ${d4}, ${rest}, ${d4},
		ToneControl.BLOCK_END, 0,
		ToneControl.PLAY_BLOCK, 0
	};

	public Intro() {
		try {
			player=Manager.createPlayer(Manager.TONE_DEVICE_LOCATOR);
			player.setLoopCount(-1);
			player.realize();
	
			((ToneControl)player.getControl("ToneControl")).setSequence(tune);
			player.start();
		} catch (Exception ex) {
			Silence();
		}
	}

	public void Silence() {
		if (player!=null) {
			if (player.getState()==Player.STARTED) {
				try {
					player.stop();
				} catch (Exception ex) {}
			}

			player.close();
			player=null;
		}
	}

	public void paint(Graphics g) {
		int iH=0, iW=0;
		Image img=null;
		Font f=g.getFont();
		int fS, top;

		screenH=getHeight();
		screenW=getWidth();

		fS=f.getHeight();
		try {
			img=Image.createImage("/yrthwyrm.png");
			iH=img.getHeight();
			iW=img.getWidth();
		} catch (Exception ex) {}

		top=(screenH-iH-fS-4)/2;

		if (img!=null) {
			g.drawImage(img, (screenW-iW)/2, top, Graphics.TOP|Graphics.LEFT);
			img=null;
			top+=iW+4;
		}

		g.drawString(title, (screenW-f.stringWidth(title))/2, top, 0);
	}
}
