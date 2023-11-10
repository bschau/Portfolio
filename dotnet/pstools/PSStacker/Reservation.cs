// vim: ts=2 sw=2 et
using System;

/// <summary>
/// A class representing the Reservation of a Phantom.
/// </summary>
public class Reservation
{
  public string c=null, m=null, n=null, d=null;

  /// <summary>
  /// Constructor.
  /// </summary>
  /// <param name="s">Reservation (c,m,n,d)</param>
  public Reservation(string s)
  {
    char[] delims=new char[] { ',' };
    string[] a=s.Split(delims);

    c=PickUp(a, 0);
    m=PickUp(a, 1);
    n=PickUp(a, 2);
    d=PickUp(a, 3);
  }

  /// <summary>
  /// Pick up the 'i' argument of 'a'.
  /// </summary>
  /// <param name="a">Reservation descriptor</param>
  /// <param name="i">Index</param>
  /// <returns>The argument (or "0" if not found)</param>
  private string PickUp(string[] a, int i)
  {
    if (a.Length<=i) {
      return "0";
    }

    string s=a[i].Trim();
    if (s.Length==0) {
      return "0";
    }

    return s;
  }

  /// <summary>
  /// Reserve CPU.
  /// </summary>
  /// <param name="cpu">CPU</param>
  /// <returns>CPU value after reservation</returns>
  public int CpuRes(int cpu)
  {
    return _reserve(cpu, c);
  }

  /// <summary>
  /// Reserve Mem.
  /// </summary>
  /// <param name="mem">Mem</param>
  /// <returns>Mem value after reservation</returns>
  public int MemRes(int mem)
  {
    return _reserve(mem, m);
  }

  /// <summary>
  /// Reserve Network.
  /// </summary>
  /// <param name="net">Network</param>
  /// <returns>Net value after reservation</returns>
  public int NetRes(int net)
  {
    return _reserve(net, n);
  }

  /// <summary>
  /// Reserve Disk.
  /// </summary>
  /// <param name="dsk">Disk</param>
  /// <returns>Disk value after reservation</returns>
  public int DskRes(int dsk)
  {
    return _reserve(dsk, d);
  }

  /// <summary>
  /// Reserve amount.
  /// </summary>
  /// <param name="tot">Total</param>
  /// <param name="r">Reservation</param>
  /// <returns>Total value after reservation</returns>
  private int _reserve(int tot, string r)
  {
    int v;

    if (r=="0") {
      return tot;
    }

    if (r.EndsWith("%")) {
      string s=r.Substring(0, r.Length-1);
      try {
        v=Int32.Parse(s);
        if (v>100) {
          v=100;
        }

        v=100-v;
        return (tot*v)/100;
      } catch {
        Errors.Error("Unknown reservation specifier: '"+r+"'");
      }
    } else {
      try {
        v=Int32.Parse(r);
      
        return tot-v;
      } catch {
        Errors.Error("Unknown reservation specifier: '"+r+"'");
      }
    }

    return tot;
  }
}
