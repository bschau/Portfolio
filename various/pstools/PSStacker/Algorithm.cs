// vim: ts=2 sw=2 et
using System;
using System.Collections;

/// <summary>
/// This is the base/super class for the various algorithms.  The algorithms
/// are expected to override some of the functions.  The rest of the functions
/// are common to all algorithms.
/// </summary>
public class Algorithm
{
  protected Scenario sc;

  public Algorithm(Scenario s)
  {
    sc=s;
  }

  /// <summary>
  /// Sort the server list into descending order.
  /// </summary>
  public void SortServersDescending()
  {
    SortServers(false);
  }

  /// <summary>
  /// Sort the server list into ascending order.
  /// </summary>
  public void SortServersAscending()
  {
    SortServers(true);
  }

  /// <summary>
  /// Sort the server list based on the main selector.
  /// </summary>
  /// <param name="asc">Sort in ascending order?</param>
  private void SortServers(bool asc)
  {
    Server s;

    sc.servers=new ArrayList(sc.iServers.Count);

    for (int i=0; i<sc.iServers.Count; i++) {
      s=(Server)sc.iServers.GetByIndex(i);
      sc.servers.Add(s);
    }

    SrvSort c=new SrvSort(sc.selectors[0], asc);
    sc.servers.Sort(c);
  }

  /// <summary>
  /// Allocate a new Phantom server and put the current server on the new
  /// phantom.
  /// </summary>
  /// <param name="srv">Server to add</param>
  public void AllocNewPhantom(Server srv)
  {
    if (Fit('c', srv.cpu, sc.model.lCpu)) {
      if (Fit('d', srv.dsk, sc.model.lDsk)) {
        if (Fit('m', srv.mem, sc.model.lMem)) {
          if (Fit('n', srv.net, sc.model.lNet)) {
            Verbose.Out("Creating new phantom server for '"+srv.name+"'");
            Phantom p=new Phantom("phanom"+sc.phantomIdx, sc.model);
            sc.AddToPhantomList(p);
            sc.phantomIdx++;
            return;
          }
        }
      }
    }

    Errors.Error("Server '"+srv.name+"' is too big - cannot stack");
  }

  /// <summary>
  /// Validate if the current source spec is equal to or less than the dest.
  /// spec, if and only if a selector governs so.
  /// </summary>
  /// <param name="sel">Selector (c, m, n, d)</param>
  /// <param name="src">Source value</param>
  /// <param name="dst">Dest value</param>
  /// <returns>true if src&lt;=dst, false otherwise</param>
  private bool Fit(char sel, int src, int dst)
  {
    if (sc.selectors.IndexOf(sel)==-1) {
      return true;
    }

    if (src<=dst) {
      return true;
    }

    return false;
  }

  /// <summary>
  /// Return name of the Algoritm. Must be overridden.
  /// </summary>
  /// <returns>Algorithm name</returns>
  public virtual string GetName()
  {
    return "";
  }

  /// <summary>
  /// Return description of the Algoritm. Must be overridden.
  /// </summary>
  /// <returns>Algorithm description</returns>
  public virtual string GetDescription()
  {
    return "";
  }

  /// <summary>
  /// Run the Algoritm. Must be overridden.
  /// </summary>
  public virtual void Run()
  {
  }
}
