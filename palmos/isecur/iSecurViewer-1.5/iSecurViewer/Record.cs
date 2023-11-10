using System;

namespace iSecurViewer
{
	/// <summary>
	/// iSecurViewer Record.
	/// </summary>
	public class Record
	{
		public string rTitle=null;
		public byte[] rSecret=null;
		public bool rTemplate=false;
		public byte rCategory=0;

		public Record(string title, byte[] secret, bool ist, byte cat)
		{
			rTitle=title;
			rSecret=secret;
			rTemplate=ist;
			rCategory=cat;
		}
	}
}
