package dk.schau.GameOfLife;

import java.util.Arrays;

public class PetriDish {
	private boolean[] dish = null;
	private int width, height;

	public PetriDish(int w, int h) {
		width = w;
		height = h;

		dish = new boolean[w * h];
		Arrays.fill(dish, false);
	}

	public int getWidth() {
		return width;
	}

	public int getHeight() {
		return height;
	}

	public boolean isSet(int x, int y) {
		return dish[(width * y) + x];
	}

	public void set(int x, int y) {
		dish[(width * y) + x] = true;
	}

	public void clear(int x, int y) {
		dish[(width * y) + x] = false;
	}
}
