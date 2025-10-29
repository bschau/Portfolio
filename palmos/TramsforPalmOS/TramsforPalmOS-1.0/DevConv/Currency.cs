using System;
using System.Collections;
using System.IO;
using System.Text;

public class Currency  {
	public SortedList currencies=new SortedList();
	private byte[] nul=new byte[1];

	public Currency() {
		nul[0]=0;
	}

	/*
	 * Load the 'currency.dat' file.
	 * 
	 * This will build the currencies array.  The key of the array is the
	 * currency.  The value is the currency name.
	 *
	 * F.ex:
	 *
	 *  'DKK' = 'Danish Krone'
	 *  'EUR' = 'Euros'
	 */
	public void Load(string trams) {
		string fCurrency=trams+Path.DirectorySeparatorChar+"currency.dat";
		try {
			using (StreamReader r=new StreamReader(fCurrency)) {
				string line, currency, currencyName;

				while ((line=r.ReadLine())!=null) {
					if (line.Length<5)
						continue;

					currency=line.Substring(0, 4).Trim();
					currencyName=line.Substring(4, 20).Trim();
					currencies.Add(currency, currencyName);
				}
			}
		} catch (Exception ex) {
			Console.WriteLine(ex.TargetSite+": "+ex.Message);
			Environment.Exit(1);
		}
	}

	/*
	 * Save the currencies list to a binary file.
	 *
	 * The file is a collection of zero terminated strings. First string
	 * is the count of elements.
	 * Each element consists of the currency code and the currency name
	 * separated by ' - ':
	 *
	 *  'DKK - Danish Krone'
	 *  'EUR - Euros'
	 */
	public void Save(string output) {
		FileStream fOut=null;
		BinaryWriter bOut=null;

		try {
			Encoding enc=Encoding.GetEncoding("iso-8859-1");
			string s;

			fOut=new FileStream(output+Path.DirectorySeparatorChar+"currency.bin", FileMode.Create);
			bOut=new BinaryWriter(fOut);

			bOut.Write(enc.GetBytes(currencies.Count.ToString()));
			bOut.Write(nul);

			for (int i=0; i<currencies.Count; i++) {
				s=currencies.GetKey(i).ToString()+" - "+currencies.GetByIndex(i).ToString();
				bOut.Write(enc.GetBytes(s));
				bOut.Write(nul);
			}
		} catch (Exception ex) {
			Console.WriteLine(ex.TargetSite+": "+ex.Message);
			Environment.Exit(1);
		} finally {
			if (bOut!=null)
				bOut.Close();

			if (fOut!=null)
				fOut.Close();
		}
	}
}
