// vim: ts=2 sw=2 et
using System;
using System.Collections;
using System.IO;

/// <summary>
/// A simple error handling class.   Errors of class Error are saved, warnings
/// are not saved.
/// </summary>
public class Errors
{
  public static ArrayList e=new ArrayList();
  public static int eCnt=0;

  /// <summary>
  /// Save error and write on Stderr.
  /// </summary>
  /// <param name="msg">Error text</param>
  public static void Error(string msg)
  {
    eCnt++;
    e.Add(msg);
    Console.Error.WriteLine("!!! "+msg);
  }

  /// <summary>
  /// Write warning on Stderr.
  /// </summary>
  /// <param name="msg">Warning text</param>
  public static void Warning(string msg)
  {
    e.Add(msg);
    Console.Error.WriteLine("*** "+msg);
  }
}
