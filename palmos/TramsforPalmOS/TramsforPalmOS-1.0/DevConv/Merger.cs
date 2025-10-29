using System;
using System.Collections;
using System.IO;
using System.Text;
using System.Text.RegularExpressions;

public class Merger  {
	private SortedList currencies=null, countries=null;

	public Merger(Currency cu, Country co) {
		currencies=cu.currencies;
		countries=co.countries;
	}

	/*
	 * Validate the currencies and countries list.
	 *
	 * Some entries present in 'country.dat' are missing in 'currency.dat'.
	 * A missing entry is added to the currencies list.
	 *
	 * F.ex.:
	 *
	 *  'ZRN - Zaire'
	 */
	public void Validate() {
		string cKey, curr;
		int idx;

		for (int i=0; i<countries.Count; i++) {
			cKey=countries.GetByIndex(i).ToString().Substring(2).Trim();
			if (cKey.Length==0)
				cKey="EUR";

			if ((idx=currencies.IndexOfKey(cKey))==-1) {
				Console.WriteLine("Warning: Currency '{0}' not found in table", cKey);
				curr=cKey+" - "+countries.GetKey(i).ToString();
				Console.WriteLine("      -> Loading '{0}' to currency table", curr);
				currencies.Add(cKey, curr);
			}
		}	
	}

	/*
	 * Save the XRef file to a binary file.
	 */
	public void Save(string output) {
		FileStream fOut=null;
		BinaryWriter bOut=null;

		try {
			Encoding enc=Encoding.GetEncoding("iso-8859-1");
			string cKey;
			int idx;
			byte[] UInt16=new byte[2];

			fOut=new FileStream(output+Path.DirectorySeparatorChar+"xref.bin", FileMode.Create);
			bOut=new BinaryWriter(fOut);

			for (int i=0; i<countries.Count; i++) {
				cKey=countries.GetByIndex(i).ToString().Substring(2).Trim();
				if (cKey.Length==0)
					cKey="EUR";

				if ((idx=currencies.IndexOfKey(cKey))==-1) {
					Console.WriteLine("Currency {0} not found in table", cKey);
					Environment.Exit(1);
				}

				UInt16[1]=(byte)(idx&0xff);
				UInt16[0]=(byte)((idx>>8)&0xff);
				bOut.Write(UInt16);
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
