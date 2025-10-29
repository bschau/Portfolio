using System;
using System.Collections;
using System.Collections.Generic;
using System.Globalization;
using System.IO;
using System.Text;
using System.Windows.Forms;

namespace TramsCvt
{
    class Converter
    {
        internal class InvariantComparer : IComparer
        {
            private CompareInfo compareInfo;
            internal static readonly InvariantComparer Default = new InvariantComparer();

            internal InvariantComparer()
            {
                compareInfo = CultureInfo.InvariantCulture.CompareInfo;
            }

            public int Compare(Object a, Object b)
            {
                String sa = a as String;
                String sb = b as String;
                if (sa != null && sb != null)
                    return compareInfo.Compare(sa, sb);
                else
                    return Comparer.Default.Compare(a, b);
            }
        }

        private SortedList sList = new SortedList(InvariantComparer.Default);
        private Encoding enc = Encoding.GetEncoding("iso-8859-1");
        private ByteBuilder bb = new ByteBuilder();
        public string pdbFile = null;
        private fWizard wiz;

        public Converter(fWizard w)
        {
            wiz=w;
        }

        public void LoadFile(string file)
        {
            sList.Clear();
            try
            {
                wiz.cStatus.Text="Reading expenses categories from " + file;
                using (StreamReader r = new StreamReader(file))
                {
                    int lNo = 0;
                    string line;
                    string[] items;

                    while ((line = r.ReadLine()) != null)
                    {
                        Application.DoEvents();
                        if (fWizard.appStopped==true)
                            return;

                        lNo++;
                        items = line.Split('`');
                        if (items.Length < 5)
                        {
                            wiz.cStatus.Text="Line " + lNo + " unparsable: " + line;
                            continue;
                        }

                        sList.Add(items[3].ToString().Trim(), items[5].ToString().Trim()+items[2].ToString().Trim());
                    }
                }
            }
            catch
            {
                throw new Exception("Failed to Load expenses categories");
            }
        }
 
        public void CreatePDB(string dstDir)
        {
            try
            {
                PDB pdb = new PDB();
                string key, value;
                int idx;

                wiz.cStatus.Text="Creating in-memory representation of the Palm OS database file";
                for (idx = 0; idx < sList.Count; idx++)
                {
                    Application.DoEvents();
                    if (fWizard.appStopped==true)
                        return;

                    key = sList.GetKey(idx).ToString();
                    if (key.Length > 64)
                        key = key.Substring(0, 64);

                    value = sList.GetByIndex(idx).ToString();

                    bb.Reset();
                    bb.Append((byte)key.Length);
                    if (value[0] == 'N')
                        bb.Append(0);
                    else
                        bb.Append(1);

                    bb.Append(key);
                    bb.Append(0);
                    bb.Append(value.Substring(1));
                    bb.Append(0);

                    pdb.AddRecord(bb.ToBytes());
                }

                pdbFile = dstDir + Path.DirectorySeparatorChar + pdb.pdbName + ".pdb";
                wiz.cStatus.Text="Writing a copy of the database file to "+pdbFile;
                pdb.Write(dstDir);
            }
            catch
            {
                throw new Exception("Failed to write file");
            }
        }
    }
}