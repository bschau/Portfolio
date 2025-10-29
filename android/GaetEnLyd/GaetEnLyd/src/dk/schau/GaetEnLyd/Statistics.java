package dk.schau.GaetEnLyd;

public class Statistics {
	private static int correctGuesses = 0;
	private static int wrongGuesses = 0;
	
	public static void reset() {
		correctGuesses = 0;
		wrongGuesses = 0;
	}
	
	public static int getCorrectGuesses() {
		return correctGuesses;
	}
	
	public static int getWrongGuesses() {
		return wrongGuesses;
	}
	
	public static int getTotalGuesses() {
		return correctGuesses + wrongGuesses;
	}
	
	public static void correctGuess() {
		correctGuesses++;
	}
	
	public static void wrongGuess() {
		wrongGuesses++;
	}
}
