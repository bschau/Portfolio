// vim: ts=2 sw=2 et
using System;
using System.Collections;
using System.IO;

/// <summary>
/// Generate test data for PSStacker.  This program will output an optional
/// amount of servers to a csv file.
/// You can override the builtin specifications by using the -s switch.
/// The specifile consists of one or more entries of the form:
///
///  cpu x1 x2 ... xN
///  dsk x1 x2 ... xN
///  mem x1 x2 ... xN
///  net x1 x2 ... xN
///
/// The entries can be repeated multiple times and will be concatenated.
/// </summary>
public class PSGenTestData 
{
  private static string[] cLoad=new string[] { "100", "200", "300", "500", "700", "800", "1000", "1200", "1400", "1600", "1800", "2000", "2400", "2600", "3000", "3200", "3400" };
  private static string[] mLoad=new string[] { "256", "512", "1024", "2048", "4096" };
  private static string[] nLoad=new string[] { "10", "10", "100", "100", "100", "100", "1000" };
  private static string[] dLoad=new string[] { "9000", "9000", "18000", "18000", "18000", "36000", "36000", "36000", "72000", "72000" };
  private static ArrayList cpu=new ArrayList();
  private static ArrayList mem=new ArrayList();
  private static ArrayList net=new ArrayList();
  private static ArrayList dsk=new ArrayList();

  /// <summary>
  /// Write usage information and exit with exit-code.
  /// </summary>
  /// <param name="ec">Exit code</param>
  private static void Usage(int ec)
  {
    TextWriter t;

    if (ec==0) {
      t=Console.Out;
    } else {
      t=Console.Error;
    }

    t.WriteLine("Usage: PSGenTestData [OPTIONS] outputfile server#");
    t.WriteLine();
    t.WriteLine("[OPTIONS]");
    t.WriteLine("  -D       Dump specifications to output immediately");
    t.WriteLine("  -h       Show this page");
    t.WriteLine("  -s spec  Load specification from file");
    System.Environment.Exit(ec);
  }

  /// <summary>
  /// Load the given spec onto the given array list.
  /// </summary>
  /// <param name="l">ArrayList to load on to</param>
  /// <param name="e">String array to load from</param>
  /// <param name="o">Offset into 'e' to start from</param>
  /// <param name="line">Current line number (0 - skip)</param>
  private static void Load(ArrayList l, string[] e, int o, int line)
  {
    string s;
    int pos;

    for (int i=o; i<e.Length; i++) {
      if ((pos=e[i].IndexOf(':'))>-1) {
        s=e[i].Substring(0, pos);
        try {
          pos=Int32.Parse(e[i].Substring(pos+1));
        } catch {
          Console.Error.WriteLine("Failed to parse '"+e[i]+"'"+((line>0) ? " on line "+line : ""));
          System.Environment.Exit(1);
        }

        for (; pos>0; pos--) {
          l.Add(s);
        }
      } else {
        l.Add(e[i]);
      }
    }
  }

  /// <summary>
  /// Main entry.
  /// </summary>
  public static void Main(string[] args)
  {
    GetOpt opt=new GetOpt(args, "Dhs:");
    char c;

    Load(cpu, cLoad, 0, 0);
    Load(mem, mLoad, 0, 0);
    Load(net, nLoad, 0, 0);
    Load(dsk, dLoad, 0, 0);

    while ((c=opt.Next())!='-') {
      switch (c) {
        case 'D':
          DumpSpecs();
          System.Environment.Exit(0);
          break;
        case 'h':
          Usage(0);
          break;
        case 's':
          LoadSpecs(opt.Argument);
          break;
        default:
          Console.Error.WriteLine(opt.Error);
          Usage(1);
          break;
      }
    }

    if ((args.Length-opt.Index)<2) {
      Usage(1);
    }

    Random r=new Random();

    try {
      int cnt=Int32.Parse(args[opt.Index+1]);

      using (StreamWriter w=new StreamWriter(args[opt.Index])) {
        for (int i=0; i<cnt; i++) {
          w.Write("srv"+(i+1)+", ");
          w.Write(cpu[r.Next(0, cpu.Count-1)]+", ");
          w.Write(mem[r.Next(0, mem.Count-1)]+", ");
          w.Write(net[r.Next(0, net.Count-1)]+", ");
          w.WriteLine(dsk[r.Next(0, dsk.Count-1)]);
        }
      }
    } catch (Exception ex) {
      Console.Error.WriteLine(ex.Message);
    }
  }

  /// <summary>
  /// Dump current specifications.
  /// </summary>
  private static void DumpSpecs()
  {
    _dump("CPU", cpu);
    Console.WriteLine();
    _dump("Memory", mem);
    Console.WriteLine();
    _dump("Network", net);
    Console.WriteLine();
    _dump("Disc", dsk);
  }

  /// <summary>
  /// Dump specification from the arraylist.
  /// </summary>
  /// <param name="t">Leading text</param>
  /// <param name="src">Source specs</param>
  private static void _dump(string t, ArrayList src)
  {
    string pre="";
    int w=0, l;

    Console.WriteLine(t+":");
    for (int i=0; i<src.Count; i++) {
      l=((string)src[i]).Length;

      if ((w+l+pre.Length)>72) {
        Console.WriteLine();
        w=0;
        pre="";
      }

      if (w==0) {
        Console.Write("    ");
      }

      Console.Write(pre+src[i]);
      w+=l+pre.Length;
      pre=", ";
    }

    if (w!=0) {
      Console.WriteLine();
    }
  }

  /// <summary>
  /// Load specs from file.
  /// </summary>
  /// <param name="file">File to load specs from</param>
  private static void LoadSpecs(string file)
  {
    if (!File.Exists(file)) {
      Console.Error.WriteLine("No such scenario file: "+file);
      System.Environment.Exit(1);
    }

    using (StreamReader r=new StreamReader(file)) {
      int lineNo=0;
      bool resetCpu=true, resetMem=true, resetNet=true, resetDsk=true;
      char[] delim=new char[] { ' ' };
      string line, l;
      string[] a;

      while ((line=r.ReadLine())!=null) {
        lineNo++;
        line=line.Trim();

        if ((line.Length==0) || (line[0]=='#')) {
          continue;
        }

        l=line.ToLower();
        a=l.Split(delim, StringSplitOptions.RemoveEmptyEntries);

        if (a.Length>1) {
          if (a[0]=="cpu") {
            if (resetCpu) {
              cpu.Clear();
              resetCpu=false;
            }
            Load(cpu, a, 1, lineNo);
          } else if (a[0]=="mem") {
            if (resetMem) {
              mem.Clear();
              resetMem=false;
            }
            Load(mem, a, 1, lineNo);
          } else if (a[0]=="net") {
            if (resetNet) {
              net.Clear();
              resetNet=false;
            }
            Load(net, a, 1, lineNo);
          } else if (a[0]=="dsk") {
            if (resetDsk) {
              dsk.Clear();
              resetDsk=false;
            }
            Load(dsk, a, 1, lineNo);
          } else {
            Console.Error.WriteLine("Unknown specifier '"+a[0]+"' on line: "+lineNo);
            System.Environment.Exit(1);
          }
        } else {
          Console.Error.WriteLine("Garbage on line: "+lineNo);
          System.Environment.Exit(1);
        }
      }
    }
  }
}
