package dk.schau.HoerEnLyd;

public class Media {
	private final static int IDX_SAYSOUND = 0;
	private final static int IDX_SOUND = 1;
	private final static int IDX_IMAGE = 2;
	private static int[][] data = new int[][] {
		{ R.raw.aeslet_siger, R.raw.aesel, R.drawable.aesel },
		{ R.raw.bavianen_siger, R.raw.bavian, R.drawable.bavian },
		{ R.raw.bien_siger, R.raw.bi, R.drawable.bi },
		{ R.raw.bilen_siger, R.raw.bil, R.drawable.bil },
		{ R.raw.bjoernen_siger, R.raw.bjoern, R.drawable.bjoern },
		{ R.raw.boremaskinen_siger, R.raw.boremaskine, R.drawable.boremaskine },
		{ R.raw.chimpansen_siger, R.raw.chimpanse, R.drawable.chimpanse },
		{ R.raw.cyklen_siger, R.raw.cykel, R.drawable.cykel },
		{ R.raw.delfinen_siger, R.raw.delfin, R.drawable.delfin },
		{ R.raw.elguitaren_siger, R.raw.elguitar, R.drawable.elguitar },
		{ R.raw.faaret_siger, R.raw.faar, R.drawable.faar },
		{ R.raw.faarekyllingen_siger, R.raw.faarekylling, R.drawable.faarekylling },
		{ R.raw.faergen_siger, R.raw.faerge, R.drawable.faerge },
		{ R.raw.fasanen_siger, R.raw.fasan, R.drawable.fasan },
		{ R.raw.fisken_siger, R.raw.fisk, R.drawable.fisk },
		{ R.raw.flamingoen_siger, R.raw.flamingo, R.drawable.flamingo },
		{ R.raw.flyet_siger, R.raw.fly, R.drawable.fly },
		{ R.raw.froen_siger, R.raw.froe, R.drawable.froe },
		{ R.raw.gaffeltrucken_siger, R.raw.gaffeltruck, R.drawable.gaffeltruck },
		{ R.raw.geden_siger, R.raw.ged, R.drawable.ged },
		{ R.raw.gorillaen_siger, R.raw.gorilla, R.drawable.gorilla },
		{ R.raw.gravkoen_siger, R.raw.gravko, R.drawable.gravko },
		{ R.raw.grisen_siger, R.raw.gris, R.drawable.gris },
		{ R.raw.guitaren_siger, R.raw.guitar, R.drawable.guitar },
		{ R.raw.gummigeden_siger, R.raw.gummiged, R.drawable.gummiged },
		{ R.raw.hammeren_siger, R.raw.hammer, R.drawable.hammer },
		{ R.raw.hanen_siger, R.raw.hane, R.drawable.hane },
		{ R.raw.harmonikaen_siger, R.raw.harmonika, R.drawable.harmonika },
		{ R.raw.hesten_siger, R.raw.hest, R.drawable.hest },
		{ R.raw.hoenen_siger, R.raw.hoene, R.drawable.hoene },
		{ R.raw.hunden_siger, R.raw.hund, R.drawable.hund },
		{ R.raw.isbjoernen_siger, R.raw.isbjoern, R.drawable.isbjoern },
		{ R.raw.katten_siger, R.raw.kat, R.drawable.kat },
		{ R.raw.klaveret_siger, R.raw.klaver, R.drawable.klaver },
		{ R.raw.koen_siger, R.raw.ko, R.drawable.ko },
		{ R.raw.loeven_siger, R.raw.loeve, R.drawable.loeve },
		{ R.raw.marsvinet_siger, R.raw.marsvin, R.drawable.marsvin },
		{ R.raw.motorsaven_siger, R.raw.motorsav, R.drawable.motorsav },
		{ R.raw.musen_siger, R.raw.mus, R.drawable.mus },
		{ R.raw.paafuglen_siger, R.raw.paafugl, R.drawable.paafugl },
		{ R.raw.pandaen_siger, R.raw.panda, R.drawable.panda },
		{ R.raw.papegoejen_siger, R.raw.papegoeje, R.drawable.papegoeje },
		{ R.raw.racerbilen_siger, R.raw.racerbil, R.drawable.racerbil },
		{ R.raw.radioen_siger, R.raw.radio, R.drawable.radio },
		{ R.raw.saelen_siger, R.raw.sael, R.drawable.sael },
		{ R.raw.saven_siger, R.raw.sav, R.drawable.sav },
		{ R.raw.saxofonen_siger, R.raw.saxofon, R.drawable.saxofon },
		{ R.raw.solsorten_siger, R.raw.solsort, R.drawable.solsort },
		{ R.raw.stoevsugeren_siger, R.raw.stoevsuger, R.drawable.stoevsuger },
		{ R.raw.symaskinen_siger, R.raw.symaskine, R.drawable.symaskine },
		{ R.raw.tastaturet_siger, R.raw.tastatur, R.drawable.tastatur },
		{ R.raw.telefonen_siger, R.raw.telefon, R.drawable.telefon },
		{ R.raw.toget_siger, R.raw.tog, R.drawable.tog },
		{ R.raw.toilettet_siger, R.raw.toilet, R.drawable.toilet },
		{ R.raw.trommen_siger, R.raw.tromme, R.drawable.tromme },
		{ R.raw.trompeten_siger, R.raw.trompet, R.drawable.trompet },
		{ R.raw.tukanen_siger, R.raw.tukan, R.drawable.tukan },
		{ R.raw.uglen_siger, R.raw.ugle, R.drawable.ugle },
		{ R.raw.vandhanen_siger, R.raw.vandhane, R.drawable.vandhane },
		{ R.raw.veteranbilen_siger, R.raw.veteranbil, R.drawable.veteranbil },
		{ R.raw.violinen_siger, R.raw.violin, R.drawable.violin },
		{ R.raw.xylofonen_siger, R.raw.xylofon, R.drawable.xylofon }
	};
	private static int _index;
	
	public static void setIndex(int index) {
		_index = index;
	}
	
	public static int getIndex() {
		return _index;
	}

	public static int getSaySoundId() {
		return data[_index][IDX_SAYSOUND];
	}
	
	public static int getSoundId() {
		return data[_index][IDX_SOUND];
	}
	
	public static int getImageId() {
		return data[_index][IDX_IMAGE];
	}
	
	public static void previous() {
		if (_index == 0) {
			_index = data.length - 1;
		} else {
			_index--;
		}
	}

	public static void next() {
		if (_index == (data.length - 1)) {
			_index = 0;
		} else {
			_index++;
		}
	}
}
