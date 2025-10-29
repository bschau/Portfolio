using System;
using System.Collections;
using System.Text;

namespace pfoc
{
	/// <summary>
	/// Summary description for Address.
	/// </summary>
	public class Address
	{
		private ASCIIEncoding enc=new System.Text.ASCIIEncoding();
		private SortedList list;
		private int entries;

        /// <summary>
        /// Create a new address.
        /// </summary>
		public Address()
		{
			list=new SortedList();
			entries=0;
		}

        /// <summary>
        /// Add a new address to the address list.
        /// </summary>
        /// <param name="item">Address.</param>
        /// <returns>Index of item.</returns>
		public int Add(string item)
		{
			int idx;

			item=item.Replace("<br>", "\n");

			if ((idx=list.IndexOfValue(item))==-1) 
			{
				list.Add(entries, item);
				idx=entries++;
			}

			return idx;
		}

        /// <summary>
        /// Write the Locations database.
        /// </summary>
        /// <param name="dstDir">Destination directory.</param>
        /// <returns>Full path to destination directory.</returns>
		public string Write(string dstDir)
		{
			PDB pdb=new PDB("Locations");

			for (int i=0; i<list.Count; i++) 
			{
				pdb.AddRecord(enc.GetBytes((string)list[i]));
			}

            pdb.Write(dstDir);
			return pdb.fullPath;
		}
	}
}
