// vim: ts=2 sw=2 et
using System;

/// <summary>
/// Implements the First Fit algorithm.  Servers are stacked on the first
/// available phantom with enough resources.
/// </summary>
public class FirstFit : Algorithm
{
  public FirstFit(Scenario s) : base(s)
  {
  }

  public override string GetName()
  {
    return "First Fit";
  }

  public override string GetDescription()
  {
    return "This algorithm picks the first phantom to hold the current server.";
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
    Phantom p;

    for (int i=0; i<sc.phantoms.Count; i++) {
      p=(Phantom)sc.phantoms.GetByIndex(i);
      if (p.CanAccomodate(srv)) {
        p.Stack(srv);
        return true;
      }
    }

    return false;
  }
}
