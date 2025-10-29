using System;
using System.Collections;
using System.IO;
using System.Text;
using System.Text.RegularExpressions;

public class DevConv  {
	/*
	 * Usage:
	 *
	 *	DevConv.exe output-directory [path to country.dat currency.dat]
	 *
	 * Valid invocations (on windows):
	 *
	 * 	DevConv.exe ..\prc\src\data
	 * 	DevConv.exe ..\prc\src\data c:\Trams\Mobile\traprg
	 *
	 * DevConv.exe understands full and partial paths.
	 */
	public static void Main(string[] args)  {
		if (args.Length<1) {
			Console.WriteLine(@"Usage: DevConv.exe output-directory [path to country.dat currency.dat]");
			Environment.Exit(1);
		}

		string output=args[0];
		string trams;

		if (args.Length==2)
			trams=args[1];
		else
			trams=".";

		Console.WriteLine("Using files in {0}", (trams=="." ? "current directory" : trams));
		Console.WriteLine("Writing files to {0}", output);

		Currency cu=new Currency();
		cu.Load(trams);

		Country co=new Country();
		co.Load(trams);

		Merger m=new Merger(cu, co);
		m.Validate();

		cu.Save(output);
		co.Save(output);
		m.Save(output);
	}
}
