using System;
using System.Collections;
using System.Text;
using System.Windows.Forms;

namespace pfoc
{
	/// <summary>
	/// Summary description for Manager.
	/// </summary>
	public class Manager
	{
		private SortedList list;
		private byte[] mgrKey;
		private int[] mgrRIdx;
		private int[] mgrPIdx;

        /// <summary>
        /// Create a new manager record.
        /// </summary>
		public Manager()
		{
			list=new SortedList();
		}

        /// <summary>
        /// Add a manager.
        /// </summary>
        /// <param name="item">Manager.</param>
		public void Add(string item)
		{
			if (list.IndexOfKey(item)==-1) 
				list.Add(item, -1);
		}

        /// <summary>
        /// Cross reference managers.
        /// </summary>
        /// <param name="slMain">Persons records list.</param>
		public void XRef(SortedList slMain)
		{
            int bump = 0;
			SortedList sl;
			int i, j, idx;
			Person p;

			for (i=0; i<slMain.Count; i++) 
			{
                if (bump > 8)
                {
                    Application.DoEvents();
                    bump = 0;

                    if (fWizard.appStopped)
                        return;
                }
                else
                    bump++;

				sl=(SortedList)slMain.GetByIndex(i);
				for (j=0; j<sl.Count; j++)
				{
					p=(Person)sl.GetByIndex(j);

					if ((p.iManager!=null) && ((idx=(int)list.IndexOfKey(p.iManager))!=-1))
						p.mgrIdx=idx;
					else
						p.mgrIdx=-1;
				}
			}

			mgrKey=new byte[list.Count];
			mgrRIdx=new int[list.Count];
			mgrPIdx=new int[list.Count];
			Array.Clear(mgrKey, 0, mgrKey.Length);
			Array.Clear(mgrRIdx, 0, mgrKey.Length);
			Array.Clear(mgrPIdx, 0, mgrKey.Length);
		}

        /// <summary>
        /// Record a person as a manager.
        /// </summary>
        /// <param name="rIdx">Record index.</param>
        /// <param name="pIdx">Person index.</param>
        /// <param name="key">Key.</param>
        /// <param name="email">Manager email.</param>
		public void RecordMgr(int rIdx, int pIdx, int key, string email)
		{
			int idx;

			if ((idx=list.IndexOfKey(email))==-1)
				return;

			mgrKey[idx]=(byte)key;
			mgrRIdx[idx]=rIdx;
			mgrPIdx[idx]=pIdx;
		}

        /// <summary>
        /// Write the managers database.
        /// </summary>
        /// <param name="dstDir">Destination directory.</param>
        /// <returns>Full path to managers database.</returns></returnss>
		public string Write(string dstDir)
		{
			PDB pdb=new PDB("Managers");
			int idx;

			for (idx=0; idx<mgrKey.Length; idx++) 
			{
				byte[] bytes=new byte[5];

				bytes[0]=mgrKey[idx];
				bytes[1]=(byte)((mgrRIdx[idx]>>8)&0xff);
				bytes[2]=(byte)(mgrRIdx[idx]&0xff);
				bytes[3]=(byte)((mgrPIdx[idx]>>8)&0xff);
				bytes[4]=(byte)(mgrPIdx[idx]&0xff);
				pdb.AddRecord(bytes);
			}

			pdb.Write(dstDir);
			return pdb.fullPath;			
		}
	}
}