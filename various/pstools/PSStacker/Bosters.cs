// vim: ts=2 sw=2 et
using System;

/// <summary>
/// Implements the Ulrich Bosters algorithm.  Just stack servers &lt; 1Gig CPU
/// until you reach one of 80% of full capacity. This works very well in real
/// life!
/// </summary>
public class Bosters : Algorithm
{
  public Bosters(Scenario s) : base(s)
  {
  }

  public override string GetName()
  {
    return "Bosters";
  }

  public override string GetDescription()
  {
    return "This algorithm will stack servers up to 1GHz cpu on Phantoms up to 80% of the capacity.";
  }

  public override void Run()
  {
    Verbose.Out("Bosters: Setting selectors to 'c'");
    sc.selectors="c";

    Reservation r;
    if (sc.gRes!=null) {
      r=sc.gRes;
    } else {
      r=new Reservation("20%,20%,20%,20%");
    }

    Phantom p;
    for (int i=0; i<sc.phantoms.Count; i++) {
      p=(Phantom)sc.phantoms.GetByIndex(i);
      p.AdjustReservation(r);
    }

    SortServersDescending();

    Server s;
    for (int i=sc.servers.Count-1; i>0; i--) {
      s=(Server)sc.servers[i];
      if (s.cpu>1000) {
        Errors.Error("Server '"+s.name+"' is too big - cannot stack");
        sc.servers.RemoveAt(i);
      }
    }


    Algorithm a=new BestFit(sc);
    a.Run();
  }
}
