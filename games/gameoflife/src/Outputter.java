package dk.schau.GameOfLife;

import java.lang.StringBuffer;

public class Outputter {
	private StringBuffer sBuf = new StringBuffer();
	private PetriDish pd;
	private Conway cnw;

	public Outputter(PetriDish pd, Conway cnw) {
		this.pd = pd;
		this.cnw = cnw;
	}

	public void showDish() {
		int w = pd.getWidth(), h = pd.getHeight();

		System.out.println("Generation: " + cnw.getGeneration());

		sBuf.delete(0, sBuf.length());
		frame(w);

		for (int y = 0; y < h; y++) {
			sBuf.append('|');
			for (int x = 0; x < w; x++) {
				if (pd.isSet(x, y)) {
					sBuf.append('*');
				} else {
					sBuf.append(' ');
				}
			}
			sBuf.append("|\r\n");
		}

		frame(w);
		System.out.println(sBuf.toString());
	}

	private void frame(int width) {
		sBuf.append('+');
		for (int i = 0; i < width; i++) {
			sBuf.append('-');
		}
		sBuf.append("+\r\n");
	}
}
