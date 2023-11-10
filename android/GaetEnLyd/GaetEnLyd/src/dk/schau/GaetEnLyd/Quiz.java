package dk.schau.GaetEnLyd;

public class Quiz {
	private int img1, img2, img3;
	private int snd1, snd2, snd3;
	private int whichToGuess;

	public void setImage1(int img) {
		img1 = img;
	}

	public void setImage2(int img) {
		img2 = img;
	}

	public void setImage3(int img) {
		img3 = img;
	}

	public void setSound1(int snd) {
		snd1 = snd;
	}

	public void setSound2(int snd) {
		snd2 = snd;
	}

	public void setSound3(int snd) {
		snd3 = snd;
	}

	public void setWhichToGuess(int guess) {
		whichToGuess = guess;
	}

	public int getImage1() {
		return img1;
	}

	public int getImage2() {
		return img2;
	}

	public int getImage3() {
		return img3;
	}

	public int getSound1() {
		return snd1;
	}

	public int getSound2() {
		return snd2;
	}

	public int getSound3() {
		return snd3;
	}

	public int getWhichToGuess() {
		return whichToGuess;
	}
	
	public int getSoundToGuess() {
		return whichToGuess == 0 ? snd1 : whichToGuess == 1 ? snd2 : snd3;
	}
	
	public boolean tryGuess(int guess) {
		return (whichToGuess + 1) == guess;
	}
}
