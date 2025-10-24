// vim: ts=2 sw=2 et
using System;
using System.Collections;

/// <summary>
/// Server sorter class.
/// </summary>
public class SrvSort : IComparer
{
  private char selector='c';
  private bool asc;

  /// <summary>
  /// Constructor.
  /// </summary>
  /// <param name="s">Selector</param>
  /// <param name="a">Ascending sort?</param>
  public SrvSort(char s, bool a)
  {
    selector=s;
    asc=a;
  }

  /// <summary>
  /// Comparer.
  /// </summary>
  /// <param name="x">Object 1</param>
  /// <param name="y">Object 2</param>
  int IComparer.Compare(Object x, Object y)
  {
    int a, b;

    switch (selector) {
      case 'c':
        a=((Server)x).cpu;
        b=((Server)y).cpu;
        break;
      case 'd':
        a=((Server)x).dsk;
        b=((Server)y).dsk;
        break;
      case 'm':
        a=((Server)x).mem;
        b=((Server)y).mem;
        break;
      case 'n':
        a=((Server)x).net;
        b=((Server)y).net;
        break;
      default:
        return 0;
    }

    if (a<b) {
      return ((asc) ? -1 : 1);
    } else if (a>b) {
      return ((asc) ? 1 : -1);
    }

    return 0;
  }
}
