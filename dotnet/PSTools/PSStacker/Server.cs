// vim: ts=2 sw=2 et
using System;
using System.IO;

/// <summary>
/// Each instance of this class represents one server and its attributes.
/// </summary>
public class Server
{
  private static char[] delims=new char[] { ',' };
  public bool isValid=false;
  public string name;
  public int cpu, mem, net, dsk;

  /// <summary>
  /// Constructor.
  /// </summary>
  /// <param name="src">Source description</param>
  /// <param name="post">Postfix (used in errors)</param>
  public Server(string src, string post)
  {
    string[] a=src.Split(delims);

    if (a.Length>5) {
      Errors.Warning("Ignored server: "+src+" "+post);
      return;
    }

    if (a.Length==0) {
      Errors.Warning("Ignored garbage "+post);
      return;
    }

    string s=a[0].Trim();
    if (s.Length==0) {
      return;
    }

    name=s.ToLower();
    isValid=true;

    cpu=PickUp(a, 1);
    mem=PickUp(a, 2);
    net=PickUp(a, 3);
    dsk=PickUp(a, 4);
  }

  /// <summary>
  /// Pick up the 'idx' argument from 'a'.
  /// </summary>
  /// <param name="a">Source descriptor</param>
  /// <param name="idx">Index</param>
  /// <returns>Integer or (0 - not found)</returns>
  private int PickUp(string[] a, int idx)
  {
    if (a.Length<=idx) {
      return 0;
    }

    string s=a[idx].Trim();
    try {
      return Int32.Parse(s);
    } catch {}

    return 0;
  }
}
