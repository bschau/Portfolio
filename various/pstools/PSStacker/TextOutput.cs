// vim: ts=2 sw=2 et
using System;
using System.Collections;
using System.IO;

/// <summary>
/// Output Stack plan in text format.
/// </summary>
public class TextOutput : OutputFormat
{
  private const int MaxColumn=78;

  /// <summary>
  /// Constructor.
  /// </summary>
  /// <param name="s">Scenario</param>
  /// <param name="dir">Output directory</param>
  public TextOutput(Scenario s, string dir) : base(s, dir)
  {
  }

  /// <summary>
  /// Generate output.
  /// </summary>
  public override void Output()
  {
    using (StreamWriter w=new StreamWriter(outDir+"\\index.txt")) {
      OutputPrologue(w);

      for (int g=0; g<sc.gPhantoms.Count; g++) {
        ArrayList a=(ArrayList)sc.gPhantoms[g];

        OutputText(w, g, a);
      }

      OutputUnstackable(w);
      OutputUnused(w);
    }
  }

  /// <summary>
  /// Output indented string.
  /// </summary>
  /// <param name="w">StreamWriter</param>
  /// <param name="str">String</param>
  /// <param name="start">Indent</param>
  private void TextOut(StreamWriter w, string str, int start)
  {
    char[] delim=new char[] { ' ' };
    string[] words=str.Split(delim, StringSplitOptions.RemoveEmptyEntries);
    int col=start, i, len;
    string indent="";

    for (i=0; i<start; i++) {
      indent+=" ";
    }

    for (i=0; i<words.Length; i++) {
      len=words[i].Length;
      if (col+len>=MaxColumn) {
        w.WriteLine();
        w.Write(indent);
        col=start;
      }

      w.Write(words[i]);
      col+=len;
      if (col<MaxColumn) {
        w.Write(" ");
      }
    }
    if (col!=start) {
      w.WriteLine();
    }
  }

  /// <summary>
  /// Output header.
  /// </summary>
  /// <param name="w">StreamWriter</param>
  private void OutputPrologue(StreamWriter w)
  {
    w.WriteLine("Scenario: "+sc.name);
    w.WriteLine("  Algorithm:           "+sc.algorithm.GetName());
    w.Write("                       ");
    TextOut(w, sc.algorithm.GetDescription(), 23);

    w.WriteLine("  Start time:          "+sc.start.ToString("D")+" "+sc.start.ToString("T"));
    w.WriteLine("  Processing time:     "+((int)sc.runTime.TotalMilliseconds)+"ms");

    OutputSelectors(w, "  Selectors:           ", "");
    OutputPhantomsInUseCount(w, "  Phantom servers:     ", "");
    OutputUnstackableCount(w, "  Unstackable servers: ", "");
    OutputUnusedPhantomsCount(w, "  Unused phantoms:     ", "");

    if (Errors.e.Count>0) {
      w.WriteLine();
      w.WriteLine("Errors and warnings:");

      w.Write("  ");
      for (int i=0; i<Errors.e.Count; i++) {
        TextOut(w, (String)Errors.e[i], 2);
      }
    }

    w.WriteLine();
  }

  /// <summary>
  /// Output phantom group.
  /// </summary>
  /// <param name="w">StreamWriter</param>
  /// <param name="index">Index into phantoms list</param>
  /// <param name="phantoms">Phantoms list</param>
  private void OutputText(StreamWriter w, int index, ArrayList phantoms)
  {
    w.WriteLine("Phantom Group "+(index+1));
    Phantom pht=(Phantom)phantoms[0];

    w.WriteLine();
    w.WriteLine("  Stacksize:               "+pht.tStack);
    w.WriteLine("  Model (C/M/N/D):         "+pht.tCpu+" / "+pht.tMem+" / "+pht.tNet+" / "+pht.tDsk);

    if (pht.pRes==null) {
      w.WriteLine("  Reservation (C/M/N/D):   - / - / - / -");
    } else {
      w.WriteLine("  Reservation (C/M/N/D):   "+((pht.pRes.c==null) ? "-" : pht.pRes.c)+" / "+((pht.pRes.m==null) ? "-" : pht.pRes.m)+" / "+((pht.pRes.n==null) ? "-" : pht.pRes.n)+" / "+((pht.pRes.d==null) ? "-" : pht.pRes.d));
    }

    w.WriteLine("  Real (C/M/N/D):          "+pht.lCpu+" / "+pht.lMem+" / "+pht.lNet+" / "+pht.lDsk);

    Server srv;
    w.WriteLine();
    for (int pIdx=0; pIdx<phantoms.Count; pIdx++) {
      pht=(Phantom)phantoms[pIdx];
      w.WriteLine("  ["+pht.name+"]");
      for (int sIdx=0; sIdx<pht.srvs.Count; sIdx++) {
        srv=(Server)pht.srvs.GetByIndex(sIdx);
        w.WriteLine("        "+srv.name+"   "+srv.cpu+" / "+srv.mem+" / "+srv.net+" / "+srv.dsk);
      }
      w.WriteLine("    Usage:     "+pht.cpu+" / "+pht.mem+" / "+pht.net+" / "+pht.dsk);
      w.Write("    Remaining: ");
      if (pht.tCpu>0) {
        w.Write((pht.tCpu-pht.cpu)+" / ");
      } else {
        w.Write("- /");
      }
      
      if (pht.tMem>0) {
        w.Write((pht.tMem-pht.mem)+" / ");
      } else {
        w.Write("- / ");
      }
      
      if (pht.tNet>0) {
        w.Write((pht.tNet-pht.net)+" / ");
      } else {
        w.Write("- / ");
      }
      
      if (pht.tDsk>0) {
        w.WriteLine(pht.tDsk-pht.dsk);
      } else {
        w.WriteLine("-");
      }
      w.WriteLine();
    }
  }

  /// <summary>
  /// Output unstackable servers.
  /// </summary>
  /// <param name="w">StreamWriter</param>
  private void OutputUnstackable(StreamWriter w)
  {
    if (sc.unstackable.Count>0) {
      Server srv;

      w.WriteLine();
      w.WriteLine();
      w.WriteLine("Unstackable servers");
      for (int sIdx=0; sIdx<sc.unstackable.Count; sIdx++) {
        srv=(Server)sc.unstackable.GetByIndex(sIdx);
        w.WriteLine("  "+srv.name+"   "+srv.cpu+" / "+srv.mem+" / "+srv.net+" / "+srv.dsk);
      }
    }
  }

  /// <summary>
  /// Output unused servers.
  /// </summary>
  /// <param name="w">StreamWriter</param>
  private void OutputUnused(StreamWriter w)
  {
    if (sc.uPhantoms.Count>0) {
      Phantom p;

      w.WriteLine();
      w.WriteLine();
      w.WriteLine("Unused phantoms");
      for (int pIdx=0; pIdx<sc.uPhantoms.Count; pIdx++) {
        p=(Phantom)sc.uPhantoms[pIdx];
        w.WriteLine("  "+p.name+"   "+p.tCpu+" / "+p.tMem+" / "+p.tNet+" / "+p.tDsk);
      }
    }
  }
}
