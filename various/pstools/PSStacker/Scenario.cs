// vim: ts=2 sw=2 et
using System;
using System.Collections;
using System.IO;

/// <summary>
/// The scenario.  The other classes more or less works on this.
/// </summary>
public class Scenario 
{
  private const string UsAlgorithm="Algorithm algorithm";
  private const string UsMap="Map[phantomname] server1 ... serverX";
  private const string UsModel="Model name cpu,mem,net,dsk [reservation]";
  private const string UsPhantoms="Phantoms count cpu,mem,net,dsk [reservation]";
  private const string UsPhantom="Phantom name cpu,mem,net,dsk [reservation]";
  private const string UsReservation="Reservation cpu,mem,net,dsk";
  private const string UsSelectors="Selectors c..m..n..d";
  private const string UsServer="Server servername,cpu,mem,net,dsk";
  private const string UsServerList="ServerList file";
  private const string UsStack="Stack count";
  private const string UsScenarioName="ScenarioName name";
  private const string UsStylesheet="Stylesheet file";
  public SortedList iServers=new SortedList();
  public SortedList phantoms=new SortedList();
  public SortedList unstackable=new SortedList();
  public ArrayList gPhantoms=new ArrayList();
  public ArrayList uPhantoms=new ArrayList();
  private int lineNo=0;
  public int phantomIdx=1;
  public string file;
  public Reservation gRes=null;
  private Reservation res=null;
  public ArrayList servers;
  public string name;
  public string selectors="cmn";
  public DateTime start, stop;
  public TimeSpan runTime;
  public Algorithm algorithm=null;
  public string styles=null;
  public int srvsPrPhantom=10;
  public Phantom model=null;
  public bool calculate;
  public string algoNames="BestFit, Bosters, FirstFit, NextFit, WorstFit";

  /// <summary>
  /// Constructor.
  /// </summary>
  public Scenario()
  {
    start=DateTime.Now;
  }

  /// <summary>
  /// When stacking is done, this should be called. It will calculate the
  /// processing time.
  /// </summary>
  public void Stop()
  {
    stop=DateTime.Now;
    runTime=stop.Subtract(start);
  }

  /// <summary>
  /// Set the algorithm to use when stacking.
  /// </summary>
  /// <param name="algo">Algorithm</param>
  /// <param name="line">Line where we encountered the algorithm (or 0)</param>
  /// <param name="silent">Output error?</param>
  /// <returns>true if the algorithm was correctly set (eg. valid), false
  /// otherwise</returns>
  public bool SetAlgorithm(string algo, int line, bool silent)
  {
    algo=algo.ToLower();
    if (algo.CompareTo("bestfit")==0) {
      algorithm=new BestFit(this);
    } else if (algo.CompareTo("bosters")==0) {
      algorithm=new Bosters(this);
    } else if (algo.CompareTo("firstfit")==0) {
      algorithm=new FirstFit(this);
    } else if (algo.CompareTo("nextfit")==0) {
      algorithm=new NextFit(this);
    } else if (algo.CompareTo("worstfit")==0) {
      algorithm=new WorstFit(this);
    } else {
      if (!silent) {
        Errors.Error("Invalid algorithm '"+algo+"'"+((line>0) ? " on line "+line : ""));
        Errors.Error("Algorithms defined: "+algoNames+((line>0) ? " ("+line+")" : ""));
      }
      return false;
    }

    return true;
  }

  /// <summary>
  /// Cleanup the selectors string.
  /// </summary>
  /// <param name="sel">Original selectors</param>
  /// <param name="line">Line where we saw this (or 0)</param>
  /// <returns>New selectors string</returns>
  public string TrimSelectors(string sel, int line)
  {
    string s="";
    bool sawCpu=false, sawMem=false, sawNet=false, sawDsk=false;

    for (int i=0; i<sel.Length; i++) {
      switch (sel[i]) {
        case 'c':
          if (!sawCpu) {
            s+="c";
            sawCpu=true;
          }
          break;
        case 'm':
          if (!sawMem) {
            s+="m";
            sawMem=true;
          }
          break;
        case 'n':
          if (!sawNet) {
            s+="n";
            sawNet=true;
          }
          break;
        case 'd':
          if (!sawDsk) {
            s+="d";
            sawDsk=true;
          }
          break;
        default:
          Errors.Error("Invalid selector '"+sel[i]+"'"+((line>0) ? " on line "+line : ""));
          return null;
        }
      }

      return s;
  }

  /// <summary>
  /// Initialize scenario.
  /// </summary>
  /// <param name="file">Scenario file</param>
  /// <param name="g">Global reservation</param>
  /// <param name="name">Scenario name</param>
  /// <param name="css">Stylesheet</param>
  /// <param name="c">Calculate mode?</param>
  public void Init(string file, Reservation g, string name, string css, bool c)
  {
    if (!File.Exists(file)) {
      throw new Exception("No such scenario file: "+file);
    }

    this.file=file;

    gRes=g;
    res=gRes;
    styles=css;
    calculate=c;
  }

  /// <summary>
  /// Load the scenario file.
  /// </summary>
  public void Load()
  {
    using (StreamReader r=new StreamReader(file)) {
      string line, l;

      while ((line=r.ReadLine())!=null) {
        line=line.Trim();
        lineNo++;

        if ((line.Length==0) || (line[0]=='#')) {
          continue;
        }

        l=line.ToLower();

        if (l.StartsWith("algorithm")) {
          CmdAlgorithm(l.Substring(9).Trim());
        } else if (l.StartsWith("map[")) {
          CmdMap(l.Substring(4));
        } else if (l.StartsWith("model")) {
          CmdModel(line.Substring(5).Trim());
        } else if (l.StartsWith("phantoms")) {
          CmdPhantoms(line.Substring(8).Trim());
        } else if (l.StartsWith("phantom")) {
          CmdPhantom(line.Substring(7).Trim());
        } else if (l.StartsWith("reservation")) {
          res=CmdReservation(line.Substring(11).Trim());
        } else if (l.StartsWith("scenarioname")) {
          CmdScenarioName(line.Substring(12).Trim());
        } else if (l.StartsWith("serverlist")) {
          CmdServerList(line.Substring(10).Trim());
        } else if (l.StartsWith("selectors")) {
          CmdSelectors(line.Substring(9).Trim());
        } else if (l.StartsWith("server")) {
          CmdServer(line.Substring(6).Trim());
        } else if (l.StartsWith("stack")) {
          CmdStack(line.Substring(5).Trim());
        } else if (l.StartsWith("stylesheet")) {
          CmdStylesheet(line.Substring(10).Trim());
        } else {
          Errors.Error("Unknown directive ("+lineNo+")");
        }
      }
    }

    if (Errors.eCnt>0) {
      throw new Exception("!!! "+Errors.eCnt+" error"+((Errors.eCnt==1) ? " " : "s ")+"seen");
    }
  }

  /// <summary>
  /// Handle the Algorithm command.
  /// </summary>
  /// <param name="l">Algorithm</param>
  private void CmdAlgorithm(string l)
  {
    if (SetAlgorithm(l, lineNo, true)==false) {
      Errors.Error("Usage: "+UsAlgorithm+" ("+lineNo+")");
      Errors.Error("Algorithms defined: "+algoNames+" ("+lineNo+")");
    }
  }

  /// <summary>
  /// Handle the Map command.
  /// </summary>
  /// <param name="l">Map</param>
  private void CmdMap(string l)
  {
    if (calculate) {
      Verbose.Out("Calculation mode - skipping Map");
      return;
    }

    int i;

    if ((i=l.IndexOf(']'))==-1) {
      Errors.Error("Usage: "+UsMap+" ("+lineNo+")");
    } else {
      string p=l.Substring(0, i);

      if (phantoms.ContainsKey(p)) {
        char[] delim=new char[] { ' ' };
        Phantom ph=(Phantom)phantoms[p];
        string[] a;

        l=l.Substring(i+1);
        a=l.Split(delim, StringSplitOptions.RemoveEmptyEntries);
        for (i=0; i<a.Length; i++) {
          if (iServers.ContainsKey(a[i])) {
            if (ph.srvs.ContainsKey(a[i])==true) {
              Errors.Warning("Server '"+a[i]+"' is already bound to this phantom - skipping ("+lineNo+")");
            } else {
              Verbose.Out("Binding '"+a[i]+"' to phantom '"+ph.name+"' ("+lineNo+")");
              ph.Stack((Server)iServers[a[i]]);
            }

            iServers.Remove(a[i]);
          } else {
            Errors.Error("No such server: '"+a[i]+"' ("+lineNo+")");
          }
        }
      } else {
        Errors.Error("No such phantom server: '"+p+"' ("+lineNo+")");
      }
    } 
  }

  /// <summary>
  /// Handle the Model command.
  /// </summary>
  /// <param name="line">Model specification</param>
  private void CmdModel(string line)
  {
    if (!calculate) {
      Verbose.Out("Not in calculation mode - skipping Model");
      return;
    }

    char[] seps=new char[] { ' ' };
    string[] a;

    if (model!=null) {
      Errors.Error("Default model already created");
    } else {
      a=line.Split(seps, StringSplitOptions.RemoveEmptyEntries);
      if (a.Length<2) {
        Errors.Error("Usage: "+UsModel+" ("+lineNo+")");
      } else {
        Phantom p;

        if (a.Length==1) {
          p=new Phantom(this, "Model", res, a[0]);
        } else {
          p=new Phantom(this, "Model", res, a[0], a[1]);
        }

        p.SetStack(srvsPrPhantom);

        Verbose.Out("Creating default model: "+p.lCpu+"/"+p.lMem+"/"+p.lNet+"/"+p.lDsk+" ("+lineNo+")");
        model=p;
      }
    }
  }

  /// <summary>
  /// Handle the Phantoms command.
  /// </summary>
  /// <param name="line">Phantom specification</param>
  private void CmdPhantoms(string line)
  {
    if (calculate) {
      Verbose.Out("Calculation mode - skipping Phantoms");
      return;
    }
    char[] seps=new char[] { ' ' };
    string[] a;

    a=line.Split(seps, StringSplitOptions.RemoveEmptyEntries);
    if (a.Length<2) {
      Errors.Error("Usage: "+UsPhantoms+" ("+lineNo+")");
    } else {
      int cnt=0;

      try {
        cnt=Int32.Parse(a[0]);
        if (cnt<1) {
          throw new Exception("");
        }
      } catch {
        Errors.Error("Invalid count ("+lineNo+")");
      }

      if (cnt>0) {
        Phantom p;

        for (; cnt>0; cnt--) {
          if (a.Length==2) {
            p=new Phantom(this, "phantom"+phantomIdx, res, a[1]);
          } else {
            p=new Phantom(this, "phantom"+phantomIdx, res, a[1], a[2]);
          }

          p.SetStack(srvsPrPhantom);

          Verbose.Out("Adding phantom to phantomslist: "+p.name+" ("+lineNo+")");
          AddToPhantomList(p);
          phantomIdx++;
        }
      }
    }
  }

  /// <summary>
  /// Handle the Phantom command.
  /// </summary>
  /// <param name="line">Phantom specification</param>
  private void CmdPhantom(string line)
  {
    if (calculate) {
      Verbose.Out("Calculation mode - skipping Phantom");
      return;
    }
    char[] seps=new char[] { ' ' };
    string[] a;

    a=line.Split(seps, StringSplitOptions.RemoveEmptyEntries);
    if (a.Length<2) {
      Errors.Error("Usage: "+UsPhantom+" ("+lineNo+")");
    } else {
      Phantom p;

      if (a.Length==2) {
        p=new Phantom(this, a[0], res, a[1]);
      } else {
        p=new Phantom(this, a[0], res, a[1], a[2]);
      }

      p.SetStack(srvsPrPhantom);

      Verbose.Out("Adding phantom to phantomslist: "+p.name+" ("+lineNo+")");
      AddToPhantomList(p);
    }
  }

  /// <summary>
  /// Handle the Reservation command.
  /// </summary>
  /// <param name="line">Reservation descriptor</param>
  /// <returns>Reservation</returns>
  private Reservation CmdReservation(string line)
  {
    Reservation res=null;

    if (line.Length==0) {
      Errors.Error("Usage: "+UsReservation+" ("+lineNo+")");
    } else {
      if (gRes==null) {
        res=new Reservation(line);
      } else {
        res=gRes;
      }

      Verbose.Out("Setting global reservation to: "+res.c+"/"+res.m+"/"+res.n+"/"+res.d);
    }

    return res;
  }

  /// <summary>
  /// Handle the ServerList command.
  /// </summary>
  /// <param name="file">ServerList file</param>
  private void CmdServerList(string file)
  {
    if (!LoadServers(file)) {
      Errors.Error("Usage: "+UsServerList+" ("+lineNo+")");
    }
  }

  /// <summary>
  /// Handle the Selectors command.
  /// </summary>
  /// <param name="line">Selectors</param>
  private void CmdSelectors(string line)
  {
    if ((line=TrimSelectors(line, lineNo))==null) {
      Errors.Error("Usage: "+UsSelectors+" ("+lineNo+")");
    } else {
      selectors=line;
      Verbose.Out("Setting selectors to: "+selectors);
    }
  }

  /// <summary>
  /// Handle the Server command.
  /// </summary>
  /// <param name="line">Server descriptor</param>
  private void CmdServer(string line)
  {
    if (line.Length==0) {
      Errors.Error("Usage: "+UsServer+" ("+lineNo+")");
    } else {
      Server srv=new Server(line, "("+lineNo+")");
      if (srv.isValid) {
        Verbose.Out("Adding server to serverlist: "+srv.name+" ("+lineNo+")");
        AddToServerList(srv);
      } else {
        Errors.Warning("Blank server - ignored ("+lineNo+")");
      }
    }
  }

  /// <summary>
  /// Handle the Stack command.
  /// </summary>
  /// <param name="line">Stack size</param>
  /// <returns></returns>
  private void CmdStack(string line)
  {
    if (line.Length==0) {
      Errors.Error("Usage: "+UsStack+" ("+lineNo+")");
    } else {
      try {
        srvsPrPhantom=Int32.Parse(line);
      } catch {
        Errors.Error("Cannot parse stack value ("+lineNo+")");
      }
    }
  }

  /// <summary>
  /// Handle the Scenario Name command.
  /// </summary>
  /// <param name="line">Scenario name.</param>
  /// <returns></returns>
  private void CmdScenarioName(string line)
  {
    if (line.Length==0) {
      Errors.Error("Usage: "+UsScenarioName+" ("+lineNo+")");
    } else {
      name=line;
      Verbose.Out("Setting new scenario name: "+name);
    }
  }

  /// <summary>
  /// Handle the Stylesheet command.
  /// </summary>
  /// <param name="line">Stylesheet</param>
  private void CmdStylesheet(string line)
  {
    if (line.Length==0) {
      Errors.Error("Usage: "+UsStylesheet+" ("+lineNo+")");
    } else {
      if (styles==null) {
        if (File.Exists(line)) {
          styles=line;
          Verbose.Out("Setting new stylesheet: "+styles);
        } else {
          Errors.Error("Cannot read new stylesheet: "+line);
        }
      } else {
        Verbose.Out("Stylesheet already set from command line");
      }
    }
  }

  /// <summary>
  /// Load servers into scenario.
  /// </summary>
  /// <param name="src">Serverlist</param>
  /// <returns>true if successful, false otherwise</returns>
  private bool LoadServers(string src)
  {
    int srvs=0;

    src=src.Trim();

    if (!File.Exists(src)) {
      Errors.Error("No such serverlist: "+src+" ("+lineNo+")");
      return false;
    }

    using (StreamReader r=new StreamReader(src)) {
      int sLno=0;
      char[] delims=new char[] { ',' };
      string s;
      Server srv;

      while ((s=r.ReadLine())!=null) {
        sLno++;

        srv=new Server(s, "("+src+":"+sLno+")");
        if (srv.isValid) {
          Verbose.Out("Adding server to serverlist: "+srv.name+" ("+src+":"+sLno+")");
          AddToServerList(srv);
          srvs++;
        } else {
          Errors.Warning("Blank server - ignored ("+src+":"+sLno+")");
          srv=null;
        }
      }
    }

    Verbose.Out(srvs+" server"+((srvs==1) ? " " : "s ")+"loaded from "+src);
    return true;
  }

  /// <summary>
  /// Add the server to the server list.
  /// </summary>
  /// <param name="s">Server</param>
  private void AddToServerList(Server s)
  {
    if (iServers.ContainsKey(s.name)) {
      Errors.Warning("Server "+s.name+" already exists on server list - overwriting");
      iServers[s.name]=s;
    } else {
      iServers.Add(s.name, s);
    }
  }

  /// <summary>
  /// Add the phantom to the Phantoms list.
  /// </summary>
  /// <param name="p">Phantom</param>
  public void AddToPhantomList(Phantom p)
  {
    if (phantoms.ContainsKey(p.name)) {
      Errors.Warning("Phantom "+p.name+" already exists on phantoms list - overwriting");
      phantoms[p.name]=p;
    } else {
      phantoms.Add(p.name, p);
    }
  }

  /// <summary>
  /// Group the phantoms according to specifications.
  /// </summary>
  public void GroupPhantoms()
  {
    for (int pIdx=0; pIdx<phantoms.Count; pIdx++) {
      Phantom p=(Phantom)phantoms.GetByIndex(pIdx);
      if (AddToUnused(p)==false) {
        if (AddToGroup(p)==false) {
          ArrayList a=new ArrayList();
          a.Add(p);
          gPhantoms.Add(a);
        }
      }
    }
  }

  /// <summary>
  /// Add phantom to unused list.
  /// </summary>
  /// <param name="p">Phantom</param>
  /// <returns>true if added, false otherwise</returns>
  private bool AddToUnused(Phantom p)
  {
    if ((p.cpu==0) && (p.mem==0) && (p.net==0) && (p.dsk==0)) {
      uPhantoms.Add(p);
      return true;
    }

    return false;
  }

  /// <summary>
  /// Add phantom to group.
  /// </summary>
  /// <param name="p">Phantom</param>
  /// <returns>true if phantom was added to a known group, false if not added
  /// (caller should create new group)</returns>
  private bool AddToGroup(Phantom p)
  {
    for (int g=0; g<gPhantoms.Count; g++) {
      ArrayList a=(ArrayList)gPhantoms[g];
      Phantom x=(Phantom)a[0];

      if (x.Equals(p)) {
        a.Add(p);
        return true;
      }
    }

    return false;
  }

  /// <summary>
  /// Run the scenario.
  /// </summary>
  public void Run()
  {
    if (name==null) {
      this.name=Path.GetFileNameWithoutExtension(file);
    }

    if (calculate) {
      if (model==null) {
        Errors.Error("No model given - please define one using the Model command");
        Stop();
      }
    }

    if (algorithm==null) {
      SetAlgorithm("BestFit", 0, true);
    }

    algorithm.Run();
    Stop();
  }
}
