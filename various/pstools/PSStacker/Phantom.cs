// vim: ts=2 sw=2 et
using System;
using System.Collections;
using System.IO;

/// <summary>
/// Each instance of this class holds one phantoms and its attributes.
/// </summary>
public class Phantom
{
  public SortedList srvs=new SortedList();
  private Scenario sc;
  public string name;
  public int cpu=0, mem=0, net=0, dsk=0;
  public int tStack=10;
  public int lCpu, lMem, lNet, lDsk;
  public int tCpu, tMem, tNet, tDsk;
  public Reservation pRes;

  /// <summary>
  /// Constructor.
  /// </summary>
  /// <param name="name">Phantom name</param>
  /// <param name="src">Source phantom to copy</param>
  public Phantom(string name, Phantom src)
  {
    this.name=name;
    tStack=src.tStack;
    lCpu=src.lCpu;
    lMem=src.lMem;
    lNet=src.lNet;
    lDsk=src.lDsk;
    tCpu=src.tCpu;
    tMem=src.tMem;
    tNet=src.tNet;
    tDsk=src.tDsk;
    pRes=src.pRes;
    sc=src.sc;
  }

  /// <summary>
  /// Constructor.
  /// </summary>
  /// <param name="s">Scenario</param>
  /// <param name="name">Phantom name</param>
  /// <param name="g">Global reservation descriptor</param>
  /// <param name="desc">Phantom descriptor</param>
  public Phantom(Scenario s, string name, Reservation g, string desc)
  {
    _setup(s, name, desc);
    AdjustReservation(g);
  }

  /// <summary>
  /// Constructor.
  /// </summary>
  /// <param name="s">Scenario</param>
  /// <param name="name">Phantom name</param>
  /// <param name="g">Global reservation descriptor</param>
  /// <param name="desc">Phantom descriptor</param>
  /// <param name="res">Phantom reservation descriptor</param>
  public Phantom(Scenario s, string name, Reservation g, string desc, string res)
  {
    _setup(s, name, desc);
    Reservation r=new Reservation(res);
    AdjustReservation(r);
  }

  /// <summary>
  /// Setup this phantom.
  /// </summary>
  /// <param name="s">Scenario</param>
  /// <param name="name">Phantom name</param>
  /// <param name="desc">Phantom descriptor</param>
  private void _setup(Scenario s, string name, string desc)
  {
    this.name=name;
    sc=s;

    string[] a=desc.Split(new char[] { ',' });

    tCpu=PickUp(a, 0);
    tMem=PickUp(a, 1);
    tNet=PickUp(a, 2);
    tDsk=PickUp(a, 3);
  }

  /// <summary>
  /// Pickup the 'idx' argument from 'a'.
  /// </summary>
  /// <param name="a">Source descriptor</param>
  /// <param name="idx">Index</param>
  /// <returns>Value (or 0 if not found)</returns>
  private int PickUp(string[] a, int idx)
  {
    if (a.Length<=idx) {
      return 0;
    }

    string s=a[idx].Trim();
    try {
      return Int32.Parse(s);
    } catch {
      Errors.Error("Not a number in Phantom.PickUp: '"+a[idx]+"'");
    }

    return 0;
  }

  /// <summary>
  /// Recalculate new thresholds based on this reservation.
  /// </summary>
  /// <param name="res">Reservation</param>
  public void AdjustReservation(Reservation res)
  {
    pRes=res;

    if (res==null) {
      lCpu=tCpu;
      lMem=tMem;
      lNet=tNet;
      lDsk=tDsk;
    } else {
      lCpu=res.CpuRes(tCpu);
      lMem=res.MemRes(tMem);
      lNet=res.NetRes(tNet);
      lDsk=res.DskRes(tDsk);
    }
  }

  /// <summary>
  /// Set stack size.
  /// </summary>
  /// <param name="c">Stack size</param>
  public void SetStack(int c)
  {
    tStack=c;
  }

  /// <summary>
  /// Compare this phantom to other phantom.
  /// </summary>
  /// <param name="p">Other phantom</param>
  /// <returns>true if phantoms are equal, false otherwise</returns>
  public bool Equals(Phantom p)
  {
    if ((tCpu==p.tCpu) && (lCpu==p.lCpu)) {
      if ((tDsk==p.tDsk) && (lDsk==p.lDsk)) {
        if ((tMem==p.tMem) && (lMem==p.lMem)) {
          if ((tNet==p.tNet) && (lNet==p.lNet)) {
            if (tStack==p.tStack) {
              return true;
            }
          }
        }
      }
    }

    return false;
  }

  /// <summary>
  /// Check if this phantom can hold the server.
  /// </summary>
  /// <param name="s">Server</param>
  /// <returns>True if server can be hosted here, false otherwise</returns>
  public bool CanAccomodate(Server s)
  {
    if (srvs.Count>=tStack) {
      return false;
    }

    for (int i=0; i<sc.selectors.Length; i++) {
      switch (sc.selectors[i]) {
        case 'c':
          if ((cpu+s.cpu)>tCpu) {
            return false;
          }
          break;
        case 'd':
          if ((dsk+s.dsk)>tDsk) {
            return false;
          }
          break;
        case 'm':
          if ((mem+s.mem)>tMem) {
            return false;
          }
          break;
        case 'n':
          if ((net+s.net)>tNet) {
            return false;
          }
          break;
      }
    }

    return true;
  }

  /// <summary>
  /// Calculate the primary "waste" if this server hosted here.  The waste is
  /// defined as the amount left if this server is hosted here.
  /// </summary>
  /// <param name="s">Server</param>
  /// <returns>Waste</returns>
  public int GetWaste(Server s)
  {
    switch (sc.selectors[0]) {
      case 'c':
        return lCpu-(cpu+s.cpu);
      case 'd':
        return lDsk-(dsk+s.dsk);
      case 'm':
        return lMem-(mem+s.mem);
      case 'n':
        return lNet-(net+s.net);
    }

    return 0;
  }

  /// <summary>
  /// Stack/host this server here.
  /// </summary>
  /// <param name="s">Server</param>
  public void Stack(Server s)
  {
    srvs.Add(s.name, s);

    cpu+=s.cpu;
    mem+=s.mem;
    net+=s.net;
    dsk+=s.dsk;
  }
}
