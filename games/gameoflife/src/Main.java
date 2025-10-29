package dk.schau.GameOfLife;

import java.io.IOException;

public class Main {
	public static void main(String[] args) {
		PetriDish pd = new PetriDish(10, 10);
		Conway cnw = new Conway(pd);
		Outputter out = new Outputter(pd, cnw);

		// Create a glider
		pd.set(1, 0);
		pd.set(2, 1);
		pd.set(0, 2);
		pd.set(1, 2);
		pd.set(2, 2);

		do {
			out.showDish();
			waitForKeyPress();

			cnw.nextGeneration();

		} while (true);
	}

	private static void waitForKeyPress() {
		int i;

		while (true) {
			try {
				if (System.in.read() == (int)'\n') {
					break;
				}
			} catch (IOException e) {}
		}
	}
}
