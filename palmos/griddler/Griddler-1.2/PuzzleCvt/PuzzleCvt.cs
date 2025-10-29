using System;
using System.IO;

public class PuzzleCvt {
	/*
	 * Usage:
	 *
	 *	PuzzleCvt.exe puzzlefile title [output]
	 */
	public static void Main(string[] args)  {
		string output, file, title;
		PuzzleParser qp;

		if (args.Length<2) {
			Console.WriteLine(@"Usage: PuzzleCvt.exe puzzlefile title [output]");
			Environment.Exit(1);
		}

		file=args[0];
		title=args[1];
		if (title.Length>31)
			title=title.Substring(0, 31);

		if (args.Length==3)
			output=args[2];
		else
			output=Path.ChangeExtension(file, null);

		Console.WriteLine("Puzzle File:       {0}", file);
		Console.WriteLine("Puzzle File Title: {0}", title);
		Console.WriteLine("Output File:       {0}", output);

		Console.WriteLine("---> Loading Puzzles");
		qp=new PuzzleParser(file);
		qp.Load();

		Console.WriteLine("---> Writing .pdb file");
		qp.Write(title, output);

		Console.WriteLine("---> Done");
		Environment.Exit(0);
	}
}
