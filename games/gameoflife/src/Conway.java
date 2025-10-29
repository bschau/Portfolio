package dk.schau.GameOfLife;

import java.util.Arrays;

public class Conway {
	private int generation = 0, width, height;
	private PetriDish pd;
	private int[] neighbours;

	public Conway(PetriDish pd) {
		this.pd = pd;
		width = pd.getWidth();
		height = pd.getHeight();
		neighbours = new int[width * height];
	}

	public int getGeneration() {
		return generation;
	}

	public void nextGeneration() {
		int x, y, z;

		generation++;

		Arrays.fill(neighbours, 0);

		for (y = 0; y < height; y++) {
			for (x = 0; x < width; x++) {
				neighbours[(y * width) + x] = getNeighbourCount(x, y);
			}
		}
		
		// Regel 1: en celle med mindre end 2 naboer dor
		// Regel 2: en celle med mere end 3 naboer dor
		// Regel 3: en celle med 2 eller 3 naboer lever videre
		// Regel 4: et felt med 3 naboer foder en celle
		for (y = 0; y < height; y++) {
			for (x = 0; x < width; x++) {
				z = (y * width) + x;

				if ((neighbours[z] != 2) && (neighbours[z] != 3)) {
					pd.clear(x, y);
				} else if (neighbours[z] == 3) {
					pd.set(x, y);
				}
			}
		}
	}

	private int getNeighbourCount(int x, int y) {
		int cnt = countCell(x - 1, y - 1);

		cnt += countCell(x, y - 1);
		cnt += countCell(x + 1, y -1 );
		cnt += countCell(x - 1, y);
		cnt += countCell(x + 1, y);
		cnt += countCell(x - 1, y + 1);
		cnt += countCell(x, y + 1);
		cnt += countCell(x + 1, y + 1);

		return cnt;
	}

	private int countCell(int x, int y) {
		if ((x < 0) || (x >= width) || (y < 0) || (y >= height)) {
			return 0;
		}

		if (pd.isSet(x, y)) {
			return 1;
		}

		return 0;
	}
}
