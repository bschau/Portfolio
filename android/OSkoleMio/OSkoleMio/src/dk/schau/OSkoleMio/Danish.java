package dk.schau.OSkoleMio;

import java.text.Collator;
import java.text.ParseException;
import java.text.RuleBasedCollator;
import java.util.Locale;

public class Danish {
	public static final Locale locale = new Locale("da", "DK");
	public static final String danishRules = 
		"< 0,0< 1,1< 2,2< 3,3< 4,4< 5,5< 6,6< 7,7< 8,8< 9,"
		+ "9< a,A< b,B< c,C< d,D< e,E< f,F< g,G< h,H< i,"
		+ "I< j,J< k,K< l,L< m,M< n,N< o,O< p,P< q,Q< r,"
		+ "R< s,S< t,T< u,U< v,V< w,W< x,X< y,Y< z,"
		+ "Z< æ=æ,Æ=Æ,ae=æ,AE=Æ,Ae=Æ,aE=æ,"
		+ "Æ< ø=ø,Ø=Ø,oe=ø,OE=Ø,Oe=Ø,oE=æ,"
		+ "Ø< å=å,Å=Å,aa=å,AA=Å,Aa=Å,aA=Å";
	public static RuleBasedCollator collator;
	
	static {
		try {
			collator = new RuleBasedCollator(danishRules);
		} catch (ParseException e) {
			collator = (RuleBasedCollator)Collator.getInstance(Locale.getDefault());
		}
	}
}
