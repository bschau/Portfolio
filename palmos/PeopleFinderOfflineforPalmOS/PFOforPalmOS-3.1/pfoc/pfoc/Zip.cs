using System;
using System.IO;
using System.Text;
using ICSharpCode.SharpZipLib.Zip;
using ICSharpCode.SharpZipLib.Zip.Compression;
using ICSharpCode.SharpZipLib.Zip.Compression.Streams;

namespace pfoc
{
	/// <summary>
	/// Summary description for Zip.
	/// </summary>
	public class Zip
	{
		private Stream zis=null;
		private int dbFileCnt=0, oldDbFileCnt;
		private ZipFile zif=null;
		private StringBuilder sb=new StringBuilder();
		public int contFlag=0;
        public string error;
        public string currentFile;

        /// <summary>
        /// Open the zip archive.
        /// </summary>
        /// <param name="n">Archive</param>
		public void Open(string a)
		{
    		zif=new ZipFile(a);
		}

        /// <summary>
        /// Close the Zip archive.
        /// </summary>
		public void Close()
		{
			if (zif==null)
				return;

			try 
			{
				zif.Close();
			} 
			catch {}
		}

        /// <summary>
        /// Return number of files in Zip archive.
        /// </summary>
        /// <returns>File Count.</returns>
        public int FileCount()
        {
            return zif.Size;
        }

        /// <summary>
        /// Get the next line from the zip file.
        /// </summary>
        /// <returns>Line or null (error/eof).</returns>
		public string GetLine() 
		{
			int c;

            /*
			if (dbFileCnt>3)	// FIXME: not in production
			{
				contFlag=-1;
				return null;
			}
            */

            error = null;
            oldDbFileCnt = dbFileCnt;

			if (zis==null) 
			{
				int idx;

                currentFile = "db_" + dbFileCnt;
				try 
				{
					if ((idx=zif.FindEntry(currentFile, true))==-1) 
					{
						contFlag=-1;
						return null;
					}
				} 
				catch (Exception ex) 
				{
                    error = ex.TargetSite + ": " + ex.Message;
					contFlag=-1;
					return null;
				}

				dbFileCnt++;

				try 
				{
					zis=zif.GetInputStream(idx);
				} 
				catch (Exception ex)
				{
                    error = ex.TargetSite + ": " + ex.Message;
					return null;
				}
			}

			sb.Remove(0, sb.Length);
			while (true)
			{
				try 
				{
					if ((c=zis.ReadByte())==-1) 
					{
						zis.Close();
						zis=null;
						break;
					}
				} 
				catch (Exception ex) 
				{
                    error = ex.TargetSite + ": " + ex.Message;
					contFlag=-1;
					return null;
				}

				if (c==(char)'\n') 
				{
					sb.Append((char)'\t');
					sb.Append((char)0);
					break;
				}

				sb.Append((char)c);
			}

			if (sb.Length>0) 
				return sb.ToString();

			contFlag=1;
			return null;
		}

        /// <summary>
        /// Did we switch to a new file while reading?
        /// </summary>
        /// <returns>True (yes) / False (no)</returns>
        public bool SwitchedFile()
        {
            return dbFileCnt != oldDbFileCnt;
        }
	}
}
