using System;
using System.Collections;
using System.IO;
using System.Text;
using System.Text.RegularExpressions;

public class Country  {
	public SortedList countries=new SortedList();
	private byte[] nul=new byte[1];

	public Country() {
		nul[0]=0;
	}

	/*
	 * Load the 'country.dat' file.
	 *
	 * This will build the countries array.  The key of the array is the
	 * country name.  The value is the country code and country currency.
	 *
	 * F.ex:
	 *
	 *  'Denmark' = 'DKDKK'
	 *  'France' = 'FREUR'
	 */
	public void Load(string trams) {
		string fCountry=trams+Path.DirectorySeparatorChar+"country.dat";

		try {
			using (StreamReader r=new StreamReader(fCountry)) {
				Regex exp=new Regex(@"(?<code>(\w+))`(?<country>(.*))`(?<currency>(\w*))`");
				string line;
				Match m;

				while ((line=r.ReadLine())!=null) {
					if (line.Length<5)
						continue;

					m=exp.Match(line);
					if (m.Success)
						countries.Add(m.Groups["country"].ToString(), m.Groups["code"].ToString()+m.Groups["currency"].ToString());
					else
						Console.WriteLine("Failed to parse: {0}", line);
				}
			}
		} catch (Exception ex) {
			Console.WriteLine(ex.TargetSite+": "+ex.Message);
			Environment.Exit(1);
		}
	}

	/*
	 * Save the country list to a binary file.
	 *
	 * The file is a collection of zero terminated strings. First string
	 * is the count of elements.
	 */
	public void Save(string output) {
		FileStream fOut=null;
		BinaryWriter bOut=null;

		try {
			Encoding enc=Encoding.GetEncoding("iso-8859-1");

			fOut=new FileStream(output+Path.DirectorySeparatorChar+"country.bin", FileMode.Create);
			bOut=new BinaryWriter(fOut);

			bOut.Write(enc.GetBytes(countries.Count.ToString()));
			bOut.Write(nul);

			for (int i=0; i<countries.Count; i++) {
				bOut.Write(enc.GetBytes(countries.GetKey(i).ToString()));
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

		bOut=null;
		fOut=null;
		try {
			Encoding enc=Encoding.GetEncoding("iso-8859-1");

			fOut=new FileStream(output+Path.DirectorySeparatorChar+"countrycodes.bin", FileMode.Create);
			bOut=new BinaryWriter(fOut);

			for (int i=0; i<countries.Count; i++)
				bOut.Write(enc.GetBytes(countries.GetByIndex(i).ToString().Substring(0, 2)));
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
