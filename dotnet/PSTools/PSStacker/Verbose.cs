// vim: ts=2 sw=2 et
using System;
using System.IO;

/// <summary>
/// General diagnoses class based on verbose.
/// </summary>
public class Verbose
{
  public static bool verbose=false;

  /// <summary>
  /// Output message.
  /// </summary>
  /// <param name="msg">Message</param>
  public static void Out(string msg)
  {
    if (verbose) {
      Console.WriteLine(msg);
    }
  }
}
