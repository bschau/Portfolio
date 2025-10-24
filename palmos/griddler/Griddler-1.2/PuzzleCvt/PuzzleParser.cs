using System;
using System.Collections;
using System.Globalization;
using System.IO;

public class PuzzleParser {
	internal class InvariantComparer : IComparer
	{
		private CompareInfo compareInfo;
		internal static readonly InvariantComparer Default=new InvariantComparer();

		internal InvariantComparer()
		{
			compareInfo=CultureInfo.InvariantCulture.CompareInfo;
		}

		public int Compare(Object a, Object b)
		{
			String sa=a as string;
			String sb=b as string;

			if (sa!=null && sb!=null)
				return compareInfo.Compare(sa, sb);
			
			return Comparer.Default.Compare(a, b);
		}
	}

	public SortedList list=new SortedList(InvariantComparer.Default);
	private string author=null, site=null;
	private string puzzleFile;
	private int lineNo;
	private bool fileInvalid;

	public PuzzleParser(string f) {
		puzzleFile=f;
		fileInvalid=false;
	}

	public void Load() {
		lineNo=0;
		try {
			Puzzle puzzle=null;

			using (StreamReader r=new StreamReader(puzzleFile)) {
				string line=null, token;
				string[] elems;

				while ((line=r.ReadLine())!=null) {
					lineNo++;

					line=line.Trim();
					if (line.Length<1)
						continue;

					if (line.StartsWith("#")==true)
						continue;
					
					if (line.StartsWith("[")==true) {
						if (puzzle!=null) {
							AddPuzzle(puzzle);
							puzzle=null;
						}

						if (line.EndsWith("]")==true)
							line=line.Substring(1, line.Length-2);
						else
							line=line.Substring(1);

						puzzle=new Puzzle(line, lineNo);
						puzzle.author=author;
						puzzle.site=site;
						continue;
					}

					elems=line.Split();
					token=elems[0].Trim().ToLower();
					
					if (token.CompareTo("x")==0)
						puzzle.Parse("x", line.Substring(1).Trim(), ref puzzle.xAxis, ref puzzle.width);
					else if (token.CompareTo("y")==0)
						puzzle.Parse("y", line.Substring(1).Trim(), ref puzzle.yAxis, ref puzzle.height);
					else if (token.CompareTo("author")==0)
						puzzle.author=line.Substring(7).Trim();
					else if (token.CompareTo("site")==0)
						puzzle.site=line.Substring(4).Trim();
					else
						throw new Exception(puzzleFile+":"+lineNo+": invalid character");
				}
			}

			if (puzzle!=null)
				AddPuzzle(puzzle);
		} catch (Exception ex) {
			Console.WriteLine(ex.TargetSite+": "+ex.Message);
			Environment.Exit(1);
		}
	}

	private void AddPuzzle(Puzzle puzzle) {
		if (puzzle.IsValid()==true)
			list.Add(puzzle.name, puzzle);
		else
			throw new Exception(puzzleFile+":"+puzzle.puzzleStart+": the puzzle is not complete");

		author=puzzle.author;
		site=puzzle.site;
	}

	public void Write(string name, string file) {
		ByteBuilder bb=new ByteBuilder(1024);
		PDB pdb=new PDB(name);
		string key;
		Puzzle puzzle;

		if (fileInvalid)
			Environment.Exit(1);

		for (int i=0; i<list.Count; i++) {
			key=(string)list.GetKey(i);
			puzzle=(Puzzle)list.GetByIndex(i);

			bb.Reset();
			bb.Append(1);
			bb.Append((byte)puzzle.width);
			bb.Append((byte)puzzle.height);
			bb.Append(key);
			bb.Append(0);
			bb.Append(puzzle.author);
			bb.Append(0);
			bb.Append(puzzle.site);
			bb.Append(0);
			bb.Append(puzzle.xAxis);
			bb.Append(puzzle.yAxis);
			Console.WriteLine("---> Adding ["+key+"] ("+puzzle.width+" x "+puzzle.height+")");
			pdb.AddRecord(bb.ToBytes());
		}

		Console.WriteLine("---> "+list.Count+" puzzl"+((list.Count==1) ? "e" : "es")+" added");
		pdb.Write(file);
	}
}
