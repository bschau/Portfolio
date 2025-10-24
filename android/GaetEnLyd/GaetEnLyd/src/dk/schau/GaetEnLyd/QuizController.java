package dk.schau.GaetEnLyd;

import java.util.Date;
import java.util.Random;

public class QuizController {
	private static int[][] data = new int[][] {
		{ R.drawable.aesel, R.raw.aesel },
		{ R.drawable.bavian, R.raw.bavian },
		{ R.drawable.bil, R.raw.bil },
		{ R.drawable.bi, R.raw.bi },
		{ R.drawable.bjoern, R.raw.bjoern },
		{ R.drawable.boremaskine, R.raw.boremaskine },
		{ R.drawable.chimpanse, R.raw.chimpanse },
		{ R.drawable.cykel, R.raw.cykel },
		{ R.drawable.delfin, R.raw.delfin },
		{ R.drawable.elguitar, R.raw.elguitar },
		{ R.drawable.faarekylling, R.raw.faarekylling },
		{ R.drawable.faar, R.raw.faar },
		{ R.drawable.faerge, R.raw.faerge },
		{ R.drawable.fasan, R.raw.fasan },
		{ R.drawable.fisk, R.raw.fisk },
		{ R.drawable.flamingo, R.raw.flamingo },
		{ R.drawable.fly, R.raw.fly },
		{ R.drawable.froe, R.raw.froe },
		{ R.drawable.gaffeltruck, R.raw.gaffeltruck },
		{ R.drawable.ged, R.raw.ged },
		{ R.drawable.gorilla, R.raw.gorilla },
		{ R.drawable.gravko, R.raw.gravko },
		{ R.drawable.gris, R.raw.gris },
		{ R.drawable.guitar, R.raw.guitar },
		{ R.drawable.gummiged, R.raw.gummiged },
		{ R.drawable.hammer, R.raw.hammer },
		{ R.drawable.hane, R.raw.hane },
		{ R.drawable.harmonika, R.raw.harmonika },
		{ R.drawable.hest, R.raw.hest },
		{ R.drawable.hoene, R.raw.hoene },
		{ R.drawable.hund, R.raw.hund },
		{ R.drawable.isbjoern, R.raw.isbjoern },
		{ R.drawable.kat, R.raw.kat },
		{ R.drawable.klaver, R.raw.klaver },
		{ R.drawable.ko, R.raw.ko },
		{ R.drawable.loeve, R.raw.loeve },
		{ R.drawable.marsvin, R.raw.marsvin },
		{ R.drawable.motorsav, R.raw.motorsav },
		{ R.drawable.mus, R.raw.mus },
		{ R.drawable.paafugl, R.raw.paafugl },
		{ R.drawable.panda, R.raw.panda },
		{ R.drawable.papegoeje, R.raw.papegoeje },
		{ R.drawable.racerbil, R.raw.racerbil },
		{ R.drawable.radio, R.raw.radio },
		{ R.drawable.sael, R.raw.sael },
		{ R.drawable.sav, R.raw.sav },
		{ R.drawable.saxofon, R.raw.saxofon },
		{ R.drawable.solsort, R.raw.solsort },
		{ R.drawable.stoevsuger, R.raw.stoevsuger },
		{ R.drawable.symaskine, R.raw.symaskine },
		{ R.drawable.tastatur, R.raw.tastatur },
		{ R.drawable.telefon, R.raw.telefon },
		{ R.drawable.tog, R.raw.tog },
		{ R.drawable.toilet, R.raw.toilet },
		{ R.drawable.tromme, R.raw.tromme },
		{ R.drawable.trompet, R.raw.trompet },
		{ R.drawable.tukan, R.raw.tukan },
		{ R.drawable.ugle, R.raw.ugle },
		{ R.drawable.vandhane, R.raw.vandhane },
		{ R.drawable.veteranbil, R.raw.veteranbil },
		{ R.drawable.violin, R.raw.violin },
		{ R.drawable.xylofon, R.raw.xylofon }
	};
	private static Random random = new Random(new Date().getTime());
	private static int lastToGuess = -1;
	
	public static Quiz newQuiz() {
		int[] i = new int[3];
		
		i[0] = random.nextInt(data.length);
		i[1] = i[0];
		while (i[1] == i[0]) {
			i[1] = random.nextInt(data.length);
		}
		
		i[2] = i[1];
		while ((i[2] == i[1]) || (i[2] == i[0])) {
			i[2] = random.nextInt(data.length);
		}
		
		Quiz q = new Quiz();

		q.setImage1(data[i[0]][0]);
		q.setSound1(data[i[0]][1]);
		q.setImage2(data[i[1]][0]);
		q.setSound2(data[i[1]][1]);
		q.setImage3(data[i[2]][0]);
		q.setSound3(data[i[2]][1]);

		int toGuess = -1;
	
		while (toGuess == -1) {
			toGuess = random.nextInt(3);
			if (data[toGuess][0] == lastToGuess) {
				toGuess = -1;
			}
		}

		lastToGuess = toGuess;
		q.setWhichToGuess(toGuess);
		return q;
	}
}
