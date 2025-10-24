// vim: ts=2 sw=2 et
using System;
using System.Collections;
using System.Drawing;
using System.IO;
using System.Web;

/// <summary>
/// Output stack plan in HTML format.  Also create corresponding images and
/// stylesheet (or copy stylesheet).
/// This class piggy backs on the OutputFormat class.
/// </summary>
public class HtmlOutput : OutputFormat
{
  private const int MaxImgWidth=600;
  private const int MaxBarWidth=20;
  private Color white=Color.FromName("White");
  private Color light=Color.FromArgb(247, 239, 145);
  private Color dark=Color.FromArgb(239, 225, 52);
  private SortedList mapDecl;
  private int imgW, imgH;
  private SolidBrush background, lightBack, darkBack;

  /// <summary>
  /// Constructor.
  /// </summary>
  /// <param name="s">Scenario</summary>
  /// <param name="dir">Output directory</summary>
  public HtmlOutput(Scenario s, string dir) : base(s, dir)
  {
    background=new SolidBrush(white);
    lightBack=new SolidBrush(light);
    darkBack=new SolidBrush(dark);
  }

  /// <summary>
  /// Output ... all!
  /// </summary>
  public override void Output()
  {
    using (StreamWriter w=new StreamWriter(outDir+"/index.html")) {
      OutputPrologue(w);

      string pre="";

      for (int g=0; g<sc.gPhantoms.Count; g++) {
        ArrayList a=(ArrayList)sc.gPhantoms[g];

        CreateImage(g, a);
        w.WriteLine(pre);
        CreateHtml(w, g, a);
        pre="<P><HR STYLE=\"width: 80%\"><P><BR>";
      }

      OutputUnstackable(w, pre);
      OutputUnused(w, pre);
      OutputEpilogue(w);
    }

    OutputStylesheet();
  }

  /// <summary>
  /// Header, javascript etc.
  /// </summary>
  private void OutputPrologue(StreamWriter w)
  {
    w.WriteLine("<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01//EN\" \"http://www.w3.org/TR/html4/strict.dtd\">");
    w.WriteLine("<HTML><HEAD>");
    w.WriteLine("<TITLE>Scenario: "+HttpUtility.HtmlEncode(sc.name)+"</TITLE>");
    w.WriteLine("<META HTTP-EQUIV=\"Content-Type\" CONTENT=\"text/html; charset=ISO-8859-1\">");
    w.WriteLine("<STYLE TYPE=\"text/css\">");
    w.WriteLine("div#srvflt { display: none; position: absolute; border: 1px solid black; background-color: #ddeeff; padding: 5px }");
    w.WriteLine("</STYLE>");
    w.WriteLine("<LINK REL=\"STYLESHEET\" TYPE=\"text/css\" HREF=\"stylesheet.css\">");
    w.WriteLine("<SCRIPT TYPE=\"text/javascript\">");
    w.WriteLine("<!--");
    w.WriteLine("function Show(s, e) {");
    w.WriteLine("  var x, y, d;");
    w.WriteLine("  if (!e) {");
    w.WriteLine("    e=window.event;");
    w.WriteLine("    if (!e) {");
    w.WriteLine("      return;");
    w.WriteLine("    }");
    w.WriteLine("  }");
    w.WriteLine("  if (typeof(e.layerX)=='number') {");
    w.WriteLine("    x=e.layerX;");
    w.WriteLine("    y=e.layerY;");
    w.WriteLine("  } else if (typeof(e.pageX)=='number') {");
    w.WriteLine("    x=e.pageX;");
    w.WriteLine("    y=e.pageY;");
    w.WriteLine("  } else if (typeof(e.clientX)=='number') {");
    w.WriteLine("    x=e.clientX;");
    w.WriteLine("    y=e.clientY;");
    w.WriteLine("    if (document.body && (document.body.scrollLeft || document.body.scrollTop)) {");
    w.WriteLine("      x+=document.body.scrollLeft;");
    w.WriteLine("      y+=document.body.scrollTop;");
    w.WriteLine("    } else if (document.documentElement && (document.documentElement.scrollLeft || document.documentElement.scrollTop)) {");
    w.WriteLine("      x+=document.documentElement.scrollLeft;");
    w.WriteLine("      y+=document.documentElement.scrollTop;");
    w.WriteLine("    }");
    w.WriteLine("  } else {");
    w.WriteLine("    return;");
    w.WriteLine("  }");
    w.WriteLine("  d=document.getElementById(\"srvflt\");");
    w.WriteLine("  d.innerHTML=s;");
    w.WriteLine("  d.style.display=\"block\";");
    w.WriteLine("  d.style.left=x+\"px\";");
    w.WriteLine("  d.style.top=y+\"px\";");
    w.WriteLine("}");
    w.WriteLine("function Hide() {");
    w.WriteLine("  document.getElementById(\"srvflt\").style.display=\"none\";");
    w.WriteLine("}");
    w.WriteLine("// -->");
    w.WriteLine("</SCRIPT>");
    w.WriteLine("</HEAD><BODY><DIV CLASS=\"intro\"><H1>"+HttpUtility.HtmlEncode(sc.name)+"</H1>");
    w.WriteLine("<P><TABLE>");
    w.WriteLine("<TR><TH>Algorithm:</TH><TD>"+sc.algorithm.GetName()+"</TD></TR>");
    w.WriteLine("<TR><TH></TH><TD>"+HttpUtility.HtmlEncode(sc.algorithm.GetDescription())+"</TD></TR>");
    w.WriteLine("<TR><TH>Start time:</TH><TD>"+sc.start.ToString("D")+" "+sc.start.ToString("T")+"</TD></TR>");
    w.WriteLine("<TR><TH>Processing time:</TH><TD>"+((int)sc.runTime.TotalMilliseconds)+"ms</TD></TR>");

    OutputSelectors(w, "<TR><TH>Selectors:</TH><TD>", "</TD></TR>");
    OutputPhantomsInUseCount(w, "<TR><TH>Phantom servers:</TH><TD>", "</TD></TR>");
    OutputUnstackableCount(w, "<TR><TH>Unstackable servers:</TH><TD>", "</TD></TR>");
    OutputUnusedPhantomsCount(w, "<TR><TH>Unused phantoms:</TH><TD>", "</TD></TR>");

    w.WriteLine("</TABLE></DIV>");
    if (Errors.e.Count>0) {
      w.WriteLine("<P><DIV CLASS=\"messages\">Errors and warnings:<P><PRE>");
      string e;

      for (int i=0; i<Errors.e.Count; i++) {
        e=(string)Errors.e[i];
        w.WriteLine(HttpUtility.HtmlEncode(e));
      }
      w.WriteLine("</PRE></DIV>");
    }

    string pre="";
    w.WriteLine("<P><DIV ID=\"ctrls\">");
    for (int g=0; g<sc.gPhantoms.Count; g++) {
      w.WriteLine(pre+"<A HREF=\"#phg"+g+"\">Group "+(g+1)+"</A>");
      pre=" | ";
    }
    w.WriteLine("</DIV><DIV ID=\"srvflt\"></DIV>");
    w.WriteLine("<P><BR>");
  }

  /// <summary>
  /// Output footer.
  /// </summary>
  private void OutputEpilogue(StreamWriter w)
  {
    w.WriteLine("</BODY></HTML>");
  }

  /// <summary>
  /// Create the image for this phantom group.
  /// </summary>
  /// <param name="index">Phantom index</param>
  /// <param name="phantoms">Phantom list</param>
  private void CreateImage(int index, ArrayList phantoms)
  {
    int barWidth=(MaxImgWidth/phantoms.Count);

    if (barWidth>MaxBarWidth) {
      barWidth=MaxBarWidth;
    }

    imgW=2+(phantoms.Count*barWidth)+(phantoms.Count-1);
    imgH=102;

    Bitmap bmp=new Bitmap(imgW, imgH);
    Graphics g=Graphics.FromImage(bmp);

    g.FillRectangle(background, 0, 0, imgW, imgH);
    g.DrawRectangle(Pens.Gray, 0, 0, imgW-1, imgH-1);

    Phantom p;
    int xPos=1;
 
    mapDecl=new SortedList(sc.servers.Count);

    Rectangle rct;

    for (int pIdx=0; pIdx<phantoms.Count; pIdx++) {
      p=(Phantom)phantoms[pIdx];

      int yPos=100, h;
      bool drawLight=true;
      Server s;
      for (int sIdx=0; sIdx<p.srvs.Count; sIdx++) {
        s=(Server)p.srvs.GetByIndex(sIdx);
        switch (sc.selectors[0]) {
          case 'c':
            h=(int)((((float)s.cpu)/((float)p.tCpu))*100);
            break;
          case 'd':
            h=(int)((((float)s.dsk)/((float)p.tDsk))*100);
            break;
          case 'm':
            h=(int)((((float)s.mem)/((float)p.tMem))*100);
            break;
          case 'n':
            h=(int)((((float)s.net)/((float)p.tNet))*100);
            break;
          default:
            h=0;
            break;
        }

        yPos-=h;
        g.FillRectangle(drawLight ? lightBack : darkBack, xPos, yPos, barWidth, h);

        rct=new Rectangle(xPos, yPos, barWidth, h);
        mapDecl.Add(s.name, rct);

        g.DrawLine(Pens.Black, xPos, yPos, xPos+barWidth, yPos);
        drawLight=!drawLight;
      }

      xPos+=barWidth+1;
    }

    xPos=1+barWidth;
    for (int pIdx=0; pIdx<phantoms.Count-1; pIdx++) {
      g.DrawLine(Pens.Black, xPos, 1, xPos, imgH-1);
      xPos+=barWidth+1;
    }

    bmp.Save(outDir+"/image"+index+".gif", System.Drawing.Imaging.ImageFormat.Gif);
    bmp.Dispose();
    g.Dispose();
  }

  /// <summary>
  /// Create map and tables.
  /// </summary>
  /// <param name="w">StreamWriter</param>
  /// <param name="index">Phantom index</param>
  /// <param name="phantoms">Phantoms list</param>
  private void CreateHtml(StreamWriter w, int index, ArrayList phantoms)
  {
    Rectangle rct;
    string s;

    w.WriteLine("<MAP NAME=\"scenario"+index+"\">");

    for (int i=0; i<mapDecl.Count; i++) {
      rct=(Rectangle)mapDecl.GetByIndex(i);
      s=(string)mapDecl.GetKey(i);

      w.WriteLine("<AREA SHAPE=\"RECT\" COORDS=\""+rct.X+","+rct.Y+","+(rct.Width+rct.X)+","+(rct.Height+rct.Y)+"\" ALT=\""+s+"\" HREF=\"#"+s+"\" onMouseOver=\"Show('"+s+"', event)\" onMouseOut=\"Hide()\">");
    }

    w.WriteLine("</MAP>");
    w.WriteLine("<P><DIV CLASS=\"group\"><A NAME=\"phg"+index+"\"></A><H3>Phantom Group "+(index+1)+"</H3>");
    Phantom pht=(Phantom)phantoms[0];

    w.WriteLine("<P><DIV CLASS=\"groupleft\">");
    w.WriteLine("<TABLE>");
    w.WriteLine("<TR><TH></TH><TH>Cpu</TH><TH>Mem</TH><TH>Net</TH><TH>Dsk</TH></TR>");
    w.WriteLine("<TR><TH>Model</TH><TD>"+pht.tCpu+"</TD><TD>"+pht.tMem+"</TD><TD>"+pht.tNet+"</TD><TD>"+pht.tDsk+"</TD></TR>");

    if (pht.pRes==null) {
      w.WriteLine("<TR><TH>Reservation</TH><TD>-</TD><TD>-</TD><TD>-</TD><TD>-</TD></TR>");
    } else {
      w.WriteLine("<TR><TH>Reservation</TH><TD>"+((pht.pRes.c==null) ? "-" : pht.pRes.c)+"</TD><TD>"+((pht.pRes.m==null) ? "-" : pht.pRes.m)+"</TD><TD>"+((pht.pRes.n==null) ? "-" : pht.pRes.n)+"</TD><TD>"+((pht.pRes.d==null) ? "-" : pht.pRes.d)+"</TD></TR>");
    }

    w.WriteLine("<TR><TH>Real</TH><TD>"+pht.lCpu+"</TD><TD>"+pht.lMem+"</TD><TD>"+pht.lNet+"</TD><TD>"+pht.lDsk+"</TD></TR>");
    w.WriteLine("</TABLE>");
    w.WriteLine("</DIV><DIV CLASS=\"groupmiddle\">");
    w.WriteLine("<TABLE><TR><TH>Stacksize</TH></TR>");
    w.WriteLine("<TR><TD>"+pht.tStack+"</TD></TR>");
    w.WriteLine("</TABLE>");
    
    w.WriteLine("</DIV><DIV CLASS=\"groupright\">");
    w.WriteLine("<TABLE><TR><TH>Distribution</TH></TR>");
    w.WriteLine("<TR><TD><IMG SRC=\"image"+index+".gif\" WIDTH=\""+imgW+"\" HEIGHT=\""+imgH+"\" ALT=\"Server distribution\" ISMAP USEMAP=\"#scenario"+index+"\" STYLE=\"border: none\"></TD></TR>");
    w.WriteLine("</TABLE>");
    w.WriteLine("</DIV>");

    Server srv;
    for (int pIdx=0; pIdx<phantoms.Count; pIdx++) {
      pht=(Phantom)phantoms[pIdx];
      w.WriteLine("<DIV CLASS=\"groupphantoms\"><TABLE>");
      w.WriteLine("<CAPTION>"+HttpUtility.HtmlEncode(pht.name)+"</CAPTION>");
      w.WriteLine("<TR><TH>Server name</TH><TH>Cpu</TH><TH>Mem</TH><TH>Net</TH><TH>Dsk</TH></TR>");
      for (int sIdx=0; sIdx<pht.srvs.Count; sIdx++) {
        srv=(Server)pht.srvs.GetByIndex(sIdx);
        w.WriteLine("<TR><TD><A NAME=\""+HttpUtility.HtmlEncode(srv.name)+"\">"+HttpUtility.HtmlEncode(srv.name)+"</A></TD><TD>"+srv.cpu+"</TD><TD>"+srv.mem+"</TD><TD>"+srv.net+"</TD><TD>"+srv.dsk+"</TD></TR>");
      }
      w.WriteLine("<TR><TH>Usage:</TH><TD><B>"+pht.cpu+"</B></TD><TD><B>"+pht.mem+"</B></TD><TD><B>"+pht.net+"</B></TD><TD><B>"+pht.dsk+"</B></TD></TR>");
      w.WriteLine("<TR><TD>Remaining:</TD>");
      if (pht.tCpu>0) {
        w.WriteLine("<TD>"+(pht.tCpu-pht.cpu)+"</TD>");
      } else {
        w.WriteLine("<TD>-</TD>");
      }
      
      if (pht.tMem>0) {
        w.WriteLine("<TD>"+(pht.tMem-pht.mem)+"</TD>");
      } else {
        w.WriteLine("<TD>-</TD>");
      }
      
      if (pht.tNet>0) {
        w.WriteLine("<TD>"+(pht.tNet-pht.net)+"</TD>");
      } else {
        w.WriteLine("<TD>-</TD>");
      }
      
      if (pht.tDsk>0) {
        w.WriteLine("<TD>"+(pht.tDsk-pht.dsk)+"</TD>");
      } else {
        w.WriteLine("<TD>-</TD>");
      }
      
      w.WriteLine("</TABLE></DIV>");
    }
    w.WriteLine("</DIV><P><BR>");
  }

  /// <summary>
  /// Output unstackable servers list.
  /// </summary>
  /// <param name="w">StreamWriter</param>
  /// <param name="pre">Prefix</param>
  private void OutputUnstackable(StreamWriter w, string pre)
  {
    if (sc.unstackable.Count>0) {
      Server srv;

      w.WriteLine(pre);
      w.WriteLine("<P><DIV CLASS=\"unstackable\"><H3>Unstackable servers</H3>");
      w.WriteLine("<TABLE>");
      w.WriteLine("<TR><TH>Server name</TH><TH>Cpu</TH><TH>Mem</TH><TH>Net</TH><TH>Dsk</TH></TR>");
      for (int sIdx=0; sIdx<sc.unstackable.Count; sIdx++) {
        srv=(Server)sc.unstackable.GetByIndex(sIdx);
        w.WriteLine("<TR><TD>"+HttpUtility.HtmlEncode(srv.name)+"</TD><TD>"+srv.cpu+"</TD><TD>"+srv.mem+"</TD><TD>"+srv.net+"</TD><TD>"+srv.dsk+"</TD></TR>");
      }
      w.WriteLine("</TABLE></DIV>");
    }
  }

  /// <summary>
  /// Output unused phantoms list.
  /// </summary>
  /// <param name="w">StreamWriter</param>
  /// <param name="pre">Prefix</param>
  private void OutputUnused(StreamWriter w, string pre)
  {
    if (sc.uPhantoms.Count>0) {
      Phantom p;

      w.WriteLine(pre);
      w.WriteLine("<P><DIV CLASS=\"unused\"><H3>Unused phantoms</H3>");
      w.WriteLine("<TABLE>");
      w.WriteLine("<TR><TH>Phantom name</TH><TH>Cpu</TH><TH>Mem</TH><TH>Net</TH><TH>Dsk</TH></TR>");
      for (int pIdx=0; pIdx<sc.uPhantoms.Count; pIdx++) {
        p=(Phantom)sc.uPhantoms[pIdx];
        w.WriteLine("<TR><TD>"+HttpUtility.HtmlEncode(p.name)+"</TD><TD>"+p.tCpu+"</TD><TD>"+p.tMem+"</TD><TD>"+p.tNet+"</TD><TD>"+p.tDsk+"</TD></TR>");
      }
      w.WriteLine("</TABLE></DIV>");
    }
  }

  /// <summary>
  /// Create or copy stylesheet.
  /// </summary>
  private void OutputStylesheet()
  {
    if ((sc.styles!=null) && (sc.styles.Length>0)) {
      try {
        File.Copy(sc.styles, outDir+"/stylesheet.css", true);
      } catch (Exception ex) {
        Errors.Error("Cannot copy stylesheet: "+ex.Message);
      }
      return;
    }

    using (StreamWriter w=new StreamWriter(outDir+"/stylesheet.css")) {
      w.WriteLine("body { font-family: tahoma,verdana,geneva,sans-serif; }");
      w.WriteLine(".intro th { text-align: left; padding-right: 12px; }");
      w.WriteLine(".intro td { text-align: left; }");
      w.WriteLine(".messages p { color: red; }");
      w.WriteLine(".messages pre { color: red; font-family: monospace; font-size: 90%; }");
      w.WriteLine("div.messages { background-color: #ffeeee; color: red; border: 1px dotted red; padding: 2px; width: auto;}");
      w.WriteLine("/* div#ctrls a { ... } */");
      w.WriteLine("div#ctrls a:link { color: blue; text-decoration: none; }");
      w.WriteLine("div#ctrls a:visited { color: blue; text-decoration: none; }");
      w.WriteLine("/* div#ctrls a:focus { ... } */");
      w.WriteLine("/* div#ctrls a:hover { ... } */");
      w.WriteLine("/* div#ctrls a:active { ... } */");
      w.WriteLine("div.groupleft { float: left; clear: right; }");
      w.WriteLine(".groupleft tr { vertical-align: top; }");
      w.WriteLine(".groupleft th { text-align: left; font-size: 90%; padding-left: 10px;}");
      w.WriteLine(".groupleft td { text-align: left; font-size: 90%; padding-left: 10px; }");
      w.WriteLine("div.groupmiddle { float: left; clear: right; padding-left: 10px; padding-right: 10px; }");
      w.WriteLine(".groupmiddle tr { vertical-align: top; }");
      w.WriteLine(".groupmiddle th { text-align: left; font-size: 90%; padding-right: 10px; }");
      w.WriteLine(".groupmiddle td { text-align: left; font-size: 90%; }");
      w.WriteLine("div.groupright { float: left; clear: right; }");
      w.WriteLine(".groupright tr { vertical-align: top; }");
      w.WriteLine(".groupright th { text-align: left; font-size: 90%; padding-right: 10px; }");
      w.WriteLine(".groupright td { text-align: left; font-size: 90%; }");
      w.WriteLine("div.groupphantoms { float: none; clear: both; }");
      w.WriteLine(".groupphantoms tr { vertical-align: top; }");
      w.WriteLine(".groupphantoms th { text-align: left; font-size: 90%; padding-left: 10px;}");
      w.WriteLine(".groupphantoms td { text-align: left; font-size: 90%; padding-left: 10px; }");
      w.WriteLine(".groupphantoms caption { font-size: 80%; padding-top: 20px; }");
      w.WriteLine(".unstackable tr { vertical-align: top; }");
      w.WriteLine(".unstackable th { text-align: left; font-size: 90%; padding-left: 10px;}");
      w.WriteLine(".unstackable td { text-align: left; font-size: 90%; padding-left: 10px; }");
      w.WriteLine(".unused tr { vertical-align: top; }");
      w.WriteLine(".unused th { text-align: left; font-size: 90%; padding-left: 10px;}");
      w.WriteLine(".unused td { text-align: left; font-size: 90%; padding-left: 10px; }");
    }
  }
}
