// vim: ts=2 sw=2 et
using System;

/// <summary>
/// This class semi-implements the getopt function from the Unix world.
/// Only short hands arguments with no combining can be used - so, where you
/// would normally specify -vvv you must specify -v -v -v.
/// </summary>
public class GetOpt
{
  public int Index=0;
  public string Error, Argument;

  private int optChar=0, argsLen=0;
  private bool seenDash=false;
  private string[] args;
  private string options;

  /// <summary>
  /// Setup the command line parser.
  /// </summary>
  /// <param name="a">Argument list</param>
  /// <param namr="o">Switches</param>
  public GetOpt(string[] a, string o)
  {
    if ((args=a)!=null) {
      argsLen=a.Length;
    }

    options=o;
  }

  /// <summary>
  /// Get the next switch from the argument list.
  /// </summary>
  /// <returns>The next switch
  ///          '-' if no more switches,
  ///          '?' if unknown switch.
  /// </returns>
  public char Next()
  {
    int idx;

    Error="";
    Argument=null;

    if (Index>=argsLen) {
      return '-';
    }

    if (!seenDash && (args[Index][0]!='-')) {
      return '-';
    }

    if (!seenDash && (args[Index][0]=='-') && (args[Index].Length==1)) {
      return '-';
    }

    if (args[Index].CompareTo("--")==0) {
      Index++;
      return '-';
    }

    if (!seenDash) {
      seenDash=true;
      optChar=1;
    }

    if ((args[Index].Length>=2) && (args[Index][optChar]==':')) {
      seenDash=false;
      Index++;

      Error=": in flags";
      return '?';
    }

    if ((idx=options.IndexOf(args[Index][optChar]))==-1) {
      Error="Unknown option "+args[Index][optChar];

      if ((optChar+1)>=args[Index].Length) {
        seenDash=false;
        Index++;
      } else {
        optChar++;
      }

      return '?';
    }

    if ((options.Length>(idx+1)) && (options[idx+1]==':')) {
      seenDash=false;
      Index++;

      if (Index>=argsLen) {
        Error="Option "+options[idx]+" needs an argument";
        return '?';
      }

      Argument=args[Index++];
    } else {
      if ((optChar+1)>=args[Index].Length) {
        seenDash=false;
        Index++;
      } else {
        optChar++;
      }
    }

    return options[idx];
  }
}
