// vim: ts=2 sw=2 et
using System;
using System.IO;

/// <summary>
/// The main class.
/// </summary>
public class PSStacker
{
  private static Scenario sc=new Scenario();

  /// <summary>
  /// Output usage text and exit with error code.
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

    t.WriteLine("Usage: PSStacker [OPTIONS] [scenario-file]");
    t.WriteLine();
    t.WriteLine("[OPTIONS]");
    t.WriteLine("  -A algo  Set algorithm (default is BestFit)");
    t.WriteLine("           ("+sc.algoNames+")");
    t.WriteLine("  -O       Output format (default is html)");
    t.WriteLine("           (html, text)");
    t.WriteLine("  -S       Stylesheet");
    t.WriteLine("  -c       Calculate number of phantom servers to use (based on model");
    t.WriteLine("  -h       Show this page");
    t.WriteLine("  -n name  Scenario name");
    t.WriteLine("  -o dir   Scenario output directory");
    t.WriteLine("  -r res   Set global reservation (overrides values in scenario)");
    t.WriteLine("  -s sel   Selectors used when determing how to stack. The current implemented");
    t.WriteLine("           selectors are: c (cpu), m (memory), n (network), d (disc.");
    t.WriteLine("           The default is: cmn.");
    t.WriteLine("  -v       Raise verboisity");
    System.Environment.Exit(ec);
  }

  /// <summary>
  /// Main.
  /// </summary>
  /// <param name="args">Command line arguments</param>
  public static void Main(string[] args)
  {
    GetOpt opt=new GetOpt(args, "A:O:S:chn:o:r:s:v");
    Reservation res=null;
    bool v=false, calculate=false;
    string scOutputDir=null, scName=null, selectors="cmn", styles=null;
    string outputFormat="html";
    char c;

    while ((c=opt.Next())!='-') {
      switch (c) {
        case 'A':
          if (sc.SetAlgorithm(opt.Argument, 0, false)==false) {
            Usage(1);
          }
          break;
        case 'O':
          outputFormat=opt.Argument;
          break;
        case 'S':
          styles=opt.Argument;
          break;
        case 'c':
          calculate=true;
          break;
        case 'h':
          Usage(0);
          break;
        case 'n':
          scName=opt.Argument;
          break;
        case 'o':
          scOutputDir=opt.Argument;
          break;
        case 'r':
          res=new Reservation(opt.Argument);
          break;
        case 's':
          if ((selectors=sc.TrimSelectors(opt.Argument, 0))==null) {
            Usage(1);
          }
          sc.selectors=selectors;
          break;
        case 'v':
          v=true;
          break;
        case '?':
          Errors.Error(opt.Error);
          Usage(1);
          break;
      }
    }

    if ((args.Length-opt.Index)<1) {
      Usage(1);
    }

    Verbose.verbose=v;

    try {
      sc.Init(args[opt.Index], res, scName, styles, calculate);
      sc.Load();
      sc.Run();

      string o=outputFormat.ToLower();
      OutputFormat op;
      if (o=="html") {
        op=new HtmlOutput(sc, scOutputDir);
      } else if (o=="text") {
        op=new TextOutput(sc, scOutputDir);
      } else {
        throw new Exception("Unknown output format: '"+outputFormat+"'");
      }

      op.Output();
    } catch (Exception ex) {
      Console.Error.WriteLine(ex.ToString());
      System.Environment.Exit(1);
    }
  }
}
