using System;
using System.Collections;
using System.IO;
using System.Text;
using System.Text.RegularExpressions;
//using System.Threading;
using System.Globalization;
using System.Windows.Forms;

namespace pfoc
{
	/// <summary>
	/// Summary description for Convert.
	/// </summary>
	public class Convert
	{
		internal class InvariantComparer : IComparer 
		{
			private CompareInfo compareInfo;
			internal static readonly InvariantComparer Default=new InvariantComparer();

			internal InvariantComparer() 
			{
				compareInfo=CultureInfo.InvariantCulture.CompareInfo;
			}
    
			public int Compare(Object a, Object b)
			{
				String sa = a as String;
				String sb = b as String;
				if (sa!=null && sb!=null)
					return compareInfo.Compare(sa, sb);
				else
					return Comparer.Default.Compare(a, b);
			}
		}

		private string hexChars="0123456789ABCDEF";
        private SortedList[] slPhones = new SortedList[10];
        private string dbPath;
		private Zip zip;
		private SortedList slMain;
		private Address addr;
		private Manager mgr;
        private fWizard wizard;

        /// <summary>
        /// Setup conversion.
        /// </summary>
        /// <param name="w">Wizard.</param>
        /// <param name="p">Path to database file</param>
		public Convert(fWizard w, string p)
		{
            wizard = w;
            dbPath = p;
			zip=new Zip();
		}

        /// <summary>
        /// Return number of files in Zip (can only be done after open!)
        /// </summary>
        /// <returns></returns>
        public int FileCount()
        {
            return zip.FileCount();
        }

        /// <summary>
        /// Open the conversion process.
        /// </summary>
        public void Open() {
            try
            {
                zip.Open(dbPath);
            }
            catch
            {
                throw new Exception("Failed to open database file!");
            }

            slMain = new SortedList(InvariantComparer.Default);
            addr = new Address();
            addr.Add("Unknown");

            mgr = new Manager();
        }

        /// <summary>
        /// Close the conversion process.
        /// </summary>
        public void Close()
        {
            if (zip != null)
                zip.Close();
        }

        /// <summary>
        /// Build the Person array. Reads and parses the zip file. Build a sorted
        /// list with all the records.
        /// </summary>
		public void BuildPersonArray()
		{
			int cnt=0, bump=0;
			Regex regex=new Regex("\t");
			long uid=1;
			string s;

			for (;;) 
			{
                if (bump > 8)
                {
                    Application.DoEvents();
                    if (fWizard.appStopped)
                        return;

                    bump = 0;
                }
                else
                    bump++;

				if ((s=zip.GetLine())!=null) {
					Person p=new Person(addr, mgr, s, regex, uid);
					int i;
					string surname;

					if (p.valid==true) 
					{
						if ((surname=p.iSurname).Length==0)
							surname=".";

						if ((i=slMain.IndexOfKey(surname))==-1) 
						{
							slMain.Add(surname, new SortedList(InvariantComparer.Default));
							if ((i=slMain.IndexOfKey(surname))==-1) 
							{
								throw new Exception("Cannot find newly created node - low on memory?");
							}
						}

						SortedList sl=(SortedList)slMain.GetByIndex(i);
						sl.Add(p.uid, p);
						
						cnt++;
						uid++;
					}
				} 
				else
				{
					if (zip.contFlag<0) 
					{
						break;
					}
				}

                if (zip.SwitchedFile() == true)
                {
                    wizard.cProgressBar.PerformStep();
                    wizard.cStatus.Text = "Reading from: " + zip.currentFile;
                }
			}

            zip.Close();
            zip = null;
		}

        /// <summary>
        /// XRef manager/employees.
        /// </summary>
        public void MgrXRef()
        {
            mgr.XRef(slMain);
        }

        /// <summary>
        /// "Collapse" (ie. compress) the records.
        /// </summary>
		public void CollapseAll()
		{
			SortedList list=new SortedList(InvariantComparer.Default), sl;
            int bump=0;
			ArrayList al;
			string key;
			int j, c;

			for (int i=0; i<slMain.Count; i++) 
			{
                if (bump>8) {
                    Application.DoEvents();
                    bump=0;
                } else
                    bump++;

				key=(string)slMain.GetKey(i);
				sl=(SortedList)slMain.GetByIndex(i);

                c=(int)key[0];
				if ((j=list.IndexOfKey(c))==-1) 
				{
                    wizard.cStatus.Text="Creating index '"+key[0]+"'";
                    wizard.cProgressBar.PerformStep();

                    list.Add(c, new ArrayList());
					if ((j=list.IndexOfKey(c))==-1) 
					{
						throw new Exception("Cannot find newly created node when collapsing - low on memory?");
					}
				}

				al=(ArrayList)list.GetByIndex(j);
				for (j=0; j<sl.Count; j++)
					al.Add(sl.GetByIndex(j));
			}

			slMain=list;
		}

        /// <summary>
        /// Build a PDB file.
        /// </summary>
        /// <param name="dstDir">Destination Directory.</param>
        /// <param name="key">Index key.</param>
        /// <param name="list">Index list.</param>
        /// <returns>Returns the full path to the file.</returns>
		public string BuildPDBFile(string dstDir, int key, ArrayList list)
		{
            string prevSurname = null, pdbName = "index" + hexChars[((key >> 4) & 0xf)] + hexChars[(key & 0x0f)];
			ByteBuilder bb=new ByteBuilder(8192);
			int recIdx=0;
			Person p;
            PDB pdb;

            wizard.cStatus.Text="Creating PDB file: " + pdbName;
            wizard.cProgressBar.PerformStep();
            Application.DoEvents();

            pdb = new PDB(pdbName);
			for (int i=0; i<list.Count; i++) 
			{
				p=(Person)list[i];
				if (prevSurname==null) 
				{
					prevSurname=p.iSurname;
					bb.Append(p.iSurname);
					bb.Append(0);
					AppendPerson(recIdx, key, bb, p);
				} 
				else if (p.iSurname.CompareTo(prevSurname)==0) 
				{
					AppendPerson(recIdx, key, bb, p);
				} 
				else 
				{
					pdb.AddRecord(bb.ToBytes());
					recIdx++;
					bb.Reset();
					prevSurname=p.iSurname;
					bb.Append(p.iSurname);
					bb.Append(0);
					AppendPerson(recIdx, key, bb, p);
				}
			}

			if (bb.Count>0) 
			{
				pdb.AddRecord(bb.ToBytes());
			}

			pdb.Write(dstDir);
			return pdb.fullPath;
		}

        /// <summary>
        /// Add a person to the record list.
        /// </summary>
        /// <param name="rIdx">Index.</param>
        /// <param name="key">Key.</param>
        /// <param name="bb">Byte Builder.</param>
        /// <param name="p">Person.</param>
		private void AppendPerson(int rIdx, int key, ByteBuilder bb, Person p)
		{
			int addrIdx=p.addrIdx;

            p.pKey=key;
            p.pRecIndex=rIdx;
            p.pOffset=bb.Count;

			mgr.RecordMgr(rIdx, bb.Count, key, p.iEmail);

            bb.Append((ushort)p.mgrIdx);

			bb.Append((byte)p.iGivenname.Length);
			bb.Append(p.iGivenname);

			if (p.iPhone!=null) 
			{
				bb.Append((byte)p.iPhone.Length);
				bb.Append(p.iPhone);
			} else
				bb.Append(0);

			if (p.iMobile!=null)
			{
				bb.Append((byte)p.iMobile.Length);
				bb.Append(p.iMobile);
			} else
				bb.Append(0);

            bb.Append((ushort)addrIdx);

			p.GetCrunchedEmail(bb);
			bb.Append(0);			// Must follow CrunchedEmail, must be last!
		}

        /// <summary>
        /// Install a file for HotSync.
        /// </summary>
        /// <param name="p">Profile.</param>
        /// <param name="f">File.</param>
		public void InstallFile(string p, string f)
		{
            if (wizard.win32 && wizard.config.queue) {
                wizard.cStatus.Text="Queueing file '"+f+"' for HotSync";
                Application.DoEvents();
                if (wizard.palm.InstallToMemory(p, f)>0)
                    throw new Exception("Failed to queue '" + f + "' for HotSync");
            }

            wizard.cProgressBar.PerformStep();
		} 

        /// <summary>
        /// Create the files given the records.
        /// </summary>
        /// <param name="dstDir">Destination directory.</param>
 		public void WriteFiles(string dstDir)
		{
            string f;

			if (Directory.Exists(dstDir)==false) 
				Directory.CreateDirectory(dstDir);

            for (int i=0; i<slMain.Count; i++)
			{
                Application.DoEvents();
                if (fWizard.appStopped)
                    return;
 
                if ((f = BuildPDBFile(dstDir, (int)slMain.GetKey(i), (ArrayList)slMain.GetByIndex(i))) != null)
                    InstallFile(wizard.config.palmProfile, f);
                else
                    break;
			}

            wizard.cStatus.Text = "Writing Address file";
            wizard.cProgressBar.PerformStep();
            Application.DoEvents();
            if ((f = addr.Write(dstDir)) != null)
				InstallFile(wizard.config.palmProfile, f);

            wizard.cStatus.Text = "Writing Manager file";
            wizard.cProgressBar.PerformStep();
            Application.DoEvents();
            if ((f = mgr.Write(dstDir)) != null)
				InstallFile(wizard.config.palmProfile, f);
		}

        /// <summary>
        /// Create the Phone Database lists.
        /// </summary>
        public void BuildPhoneDBLists()
        {
            ArrayList list;
            int i, j;
            Person p;

            wizard.cStatus.Text="Creating the Phone Lists";
            wizard.cProgressBar.PerformStep();
            Application.DoEvents();

            for (i=0; i<10; i++)
                slPhones[i]=new SortedList(InvariantComparer.Default);

            Console.WriteLine(slMain.Count);
            for (i = 0; i < slMain.Count; i++)
            {
                Application.DoEvents();
                if (fWizard.appStopped)
                    return;

                wizard.cProgressBar.PerformStep();

                list = (ArrayList)slMain.GetByIndex(i);
                for (j = 0; j < list.Count; j++)
                {
                    p = (Person)list[j];

                    if (p.cPhone != null)
                        (slPhones[p.cPhone[0]-'0'])[p.cPhone] = p;

                    if (p.cMobile != null)
                        (slPhones[p.cMobile[0]-'0'])[p.cMobile] = p;
                }
            }
        }

        /// <summary>
        /// Write the phone databases.
        /// <param name="dstDir">Destination directory.</param>
        /// </summary>
        public void WritePhoneDBs(string dstDir)
        {
            string prevKey="", curKey;
            ByteBuilder bb = new ByteBuilder(32);
            SortedList sl;
            int i, j, x;
            Person p;
            string phone;
            PDB pdb;
            int idx;
            byte b;

            for (i = 0; i < 10; i++)
            {
                sl = slPhones[i];

                if (sl.Count == 0)
                    continue;

                wizard.cStatus.Text = "Writing Phone database " + (i + 1);
                wizard.cProgressBar.PerformStep();

                pdb = new PDB("phones"+(char)(i+'0'));
                bb.Reset();

                for (j = 0; j < sl.Count; j++)
                {
                    Application.DoEvents();
                    if (fWizard.appStopped)
                        return;

                    phone = (string)sl.GetKey(j);
                    p = (Person)sl.GetByIndex(j);

                    curKey = phone.Substring(1, 2);
                    phone = phone.Substring(3);

                    if ((curKey != prevKey) || (bb.Count>64000))
                    {
                        if (bb.Count > 0)
                        {
                            pdb.AddRecord(bb.ToBytes());
                            bb.Reset();
                        }
                        prevKey = curKey;
                        bb.Append(curKey);
                    }

                    bb.Append((ushort)p.pRecIndex);
                    bb.Append((ushort)p.pOffset);
                    bb.Append((byte)p.pKey);

                    idx=phone.Length>>1;
                    if ((phone.Length&1)==1)
                        idx++;
                    bb.Append((byte)idx);

                    for (idx = 0; idx < ((phone.Length >> 1) << 1); )
                    {
                        x = (phone[idx++] - '0') << 4;
                        b = (byte)x;
                        x = (phone[idx++] - '0');
                        b |= (byte)x;
                        bb.Append(b);
                    }

                    if (idx<phone.Length)
                        bb.Append((byte)((phone[idx]-'0')<<4));
                }

                if (bb.Count > 0)
                {
                    pdb.AddRecord(bb.ToBytes());
                }

                pdb.Write(dstDir);
                InstallFile(wizard.config.palmProfile, pdb.fullPath);
            }
        }
    }
}
