// vim: ts=2 sw=2 et
using System;
using System.Collections;
using System.IO;

/// <summary>
/// Main utility class for the various output classes.
/// </summary>
public class OutputFormat
{
  protected Scenario sc;
  protected String outDir;

  /// <summary>
  /// Constructor.
  /// </summary>
  /// <param name="s">Scenario</param>
  /// <param name="dir">Output directory</param>
  public OutputFormat(Scenario s, string dir)
  {
    sc=s;
   
    if ((dir==null) || (dir=="")) {
      outDir=".";
    } else {
      outDir=dir;
    }

    if (Directory.Exists(outDir)==false) {
      Verbose.Out(outDir+" does not exist - creating");
      Directory.CreateDirectory(outDir);
    }

    sc.GroupPhantoms();
  }

  /// <summary>
  /// Output the stack plan - must be overridden.
  /// </summary>
  public virtual void Output()
  {
  }

  /// <summary>
  /// Output "something" in pluralis (if needed).
  /// </summary>
  /// <param name="w">StreamWriter</param>
  /// <param name="pre">Prefix</param>
  /// <param namr="post">Postfix</param>
  /// <param name="type">"Something"</param>
  /// <param name="cnt">Count of "Something"</param>
  private void Plural(StreamWriter w, string pre, string post, string type, int cnt)
  {
    w.WriteLine(pre+cnt+" "+type+((cnt==1) ? "" : "s")+post);
  }

  /// <summary>
  /// Output the number of phantoms in use.
  /// </summary>
  /// <param name="w">StreamWriter</param>
  /// <param name="pre">Prefix</param>
  /// <param namr="post">Postfix</param>
  public void OutputPhantomsInUseCount(StreamWriter w, string pre, string post)
  {
    int pCnt=0;

    for (int g=0; g<sc.gPhantoms.Count; g++) {
      ArrayList a=(ArrayList)sc.gPhantoms[g];

      pCnt+=a.Count;
    }

    Plural(w, pre, post, "server", pCnt);
  }

  /// <summary>
  /// Output the number of unstackable servers.
  /// </summary>
  /// <param name="w">StreamWriter</param>
  /// <param name="pre">Prefix</param>
  /// <param namr="post">Postfix</param>
  public void OutputUnstackableCount(StreamWriter w, string pre, string post)
  {
    if (sc.unstackable.Count>0) {
      Plural(w, pre, post, "server", sc.unstackable.Count);
    }
  }

  /// <summary>
  /// Output the number of unused phantoms.
  /// </summary>
  /// <param name="w">StreamWriter</param>
  /// <param name="pre">Prefix</param>
  /// <param namr="post">Postfix</param>
  public void OutputUnusedPhantomsCount(StreamWriter w, string pre, string post)
  {
    if (sc.uPhantoms.Count>0) {
      Plural(w, pre, post, "phantom", sc.uPhantoms.Count);
    }
  }

  /// <summary>
  /// Output the selectors used.
  /// </summary>
  /// <param name="w">StreamWriter</param>
  /// <param name="pre">Prefix</param>
  /// <param namr="post">Postfix</param>
  public void OutputSelectors(StreamWriter w, string pre, string post)
  {
    w.Write(pre);
    pre="";
    for (int i=0; i<sc.selectors.Length; i++) {
      switch (sc.selectors[i]) {
        case 'c':
          w.Write(pre+"cpu");
          break;
        case 'd':
          w.Write(pre+"disk");
          break;
        case 'm':
          w.Write(pre+"memory");
          break;
        case 'n':
          w.Write(pre+"network");
          break;
        default:
          w.Write(pre+"? ("+sc.selectors[i]+")");
          break;
      }

      pre=", ";
    }

    w.WriteLine(post);
  }
}
