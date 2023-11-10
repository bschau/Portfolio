using System;

public class Puzzle {
	public byte[] xAxis=null, yAxis=null;
	public string author=null, site=null, name;
	private ByteBuilder bb=new ByteBuilder(100);
	public int puzzleStart;
	public int width, height;

	public Puzzle(string n, int l) {
		name=n;
		puzzleStart=l;
		width=0;
		height=0;
	}

	public bool IsValid() {
		if ((name==null) ||
		    (xAxis==null) ||
		    (yAxis==null))
			return false;

		return true;
	}

	public void Parse(string axisName, string axis, ref byte[] dstAxis, ref int dimension) {
		string[] m=axis.Split();
		bool error=false;
		string s;
		int i;
		
		dimension=0;
		bb.Reset();
		for (i=0; i<m.Length; i++) {
			s=m[i].Trim();
			if (s.Length>0) {
				string[] h=s.Split(',');
				string k;
				int j;

				bb.Append((byte)h.Length);
				dimension++;

				for (j=0; j<h.Length; j++) {
					k=h[j].Trim();
					if (k.Length<1) {
						Console.WriteLine("*** ["+name+"]:"+puzzleStart+":"+axisName+" axis: garbage sequence on index "+(i+1));
						error=true;
					} else {
						try {
							byte v;

							v=(byte)int.Parse(k);
							if (v<0 || v>100)
								throw new Exception("");

							bb.Append(v);
						} catch {
							Console.WriteLine("*** ["+name+"]:"+puzzleStart+":"+axisName+" axis: garbage number (index "+(i+1)+")");
							error=true;
						}
					}
				}
			}
		}

		if (error)
			dstAxis=null;
		else
			dstAxis=bb.ToBytes();
	}
}
