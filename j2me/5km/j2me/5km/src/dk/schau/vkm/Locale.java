package dk.schau.vkm;

import java.util.Hashtable;

public class Locale
{
	private static Hashtable ht = new Hashtable();

	public static void init()
	{
		En.init(ht);
		
		String loc = System.getProperty("microedition.locale").toLowerCase();
		if (loc.indexOf("da") > -1)
		{
			Da.init(ht);
		}
	}

	public static String get(String key)
	{
		String v = (String) ht.get(key);

		if (v == null)
		{
			return "";
		}

		return v;
	}
}
