// vim: ts=2 sw=2 et
using System;

/// <summary>
/// Implements the best fit algorithm.   This algorithm tries to pick the
/// phantom server with the least amount of resources to hold the server.
/// </summary>
public class BestFit : Algorithm
{
  public BestFit(Scenario s) : base(s)
  {
  }

  public override string GetName()
  {
    return "Best Fit";
  }

  public override string GetDescription()
  {
    return "This algorithm tries pick the Phantom Server with the least amount of resources which can hold the current server.";
  }

  public override void Run()
  {
    Server srv;

    SortServersDescending();

    for (int sIdx=0; sIdx<sc.servers.Count; sIdx++) {
      srv=(Server)sc.servers[sIdx];
      if (TryPlace(srv)==false) {
        if (sc.calculate) {
          AllocNewPhantom(srv);
        } else {
          Errors.Warning("Cannot stack server '"+srv.name+"'");
          sc.unstackable.Add(srv.name, srv);
        }
      }
    }
  }

  private bool TryPlace(Server srv)
  {
    Phantom p=null, bestP=null;
    int waste=2147483647, w;

    for (int i=0; i<sc.phantoms.Count; i++) {
      p=(Phantom)sc.phantoms.GetByIndex(i);
      if (p.CanAccomodate(srv)) {
        w=p.GetWaste(srv);
        if (w<waste) {
          waste=w;
          bestP=p;
        }
      }
    }

    if (bestP!=null) {
      bestP.Stack(srv);
      return true;
    }

    return false;
  }
}
