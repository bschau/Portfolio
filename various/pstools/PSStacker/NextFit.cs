// vim: ts=2 sw=2 et
using System;

/// <summary>
/// Implements the Next Fit algorithm. The algorithm stacks servers from the
/// start on the list on the first phantom, then the next phantoms get the
/// next servers and so on.
/// This algorithm never looks back ... unless a unstackable server is met it
/// will bubble of the list and we will continue from where we left.
/// </summary>
public class NextFit : Algorithm
{
  private int pIdx=0;

  public NextFit(Scenario s) : base(s)
  {
  }

  public override string GetName()
  {
    return "Next Fit";
  }

  public override string GetDescription()
  {
    return "This algorithm will fill the first phantom server with as many of the servers as possible. When there is no more room on this phantom the next phantom will be used. This algorithm will never look back ...";
  }

  public override void Run()
  {
    Server srv;

    SortServersAscending();

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

    p=(Phantom)sc.phantoms.GetByIndex(pIdx);
    if (p.CanAccomodate(srv)) {
      p.Stack(srv);
      return true;
    }

    pIdx++;
    p=(Phantom)sc.phantoms.GetByIndex(pIdx);
    if (p.CanAccomodate(srv)) {
      p.Stack(srv);
      return true;
    }
    
    pIdx--; // OK, I lied - we will look back ...
    return false;
  }
}
