using System;
using System.IO;
using System.Text;
using System.Security.Cryptography;
using System.Collections;
using System.Globalization;
using System.Windows.Forms;

namespace iSecurViewer
{
	/// <summary>
	/// Exceptions thrown by PDB.
	/// </summary>
	public class PDBException : Exception
	{
		/// <summary>
		/// Constructor for the exception.
		/// </summary>
		/// <param name="msg">Error message</param>
		public PDBException(String msg) : base(msg)
		{
		}
	}

	/// <summary>
	/// Palm Database logic.
	/// </summary>
	public class PDB
	{
		//	Database Header
		//	00	UInt8	name[dmDBNameLength];
		//	32	UInt16	attributes;
		//	34	UInt16	version;
		//	36	UInt32	creationDate;
		//	40	UInt32	modificationDate;
		//	44	UInt32	lastBackupDate;
		//	48	UInt32	modificationNumber;
		//	52	LocalID	appInfoID;
		//	56	LocalID	sortInfoID;
		//	60	UInt32	type;
		//	64	UInt32	creator;
		//	68	UInt32	uniqueIDSeed;
		//	72	[start: RecordListType recordList;]

		//	Record List
		//	00	LocalID	nextRecordListID;
		//	04	UInt16	numRecords;
		//	06	UInt16	firstEntry;
		public AES aes=new AES();
		public int Length;
		public Encoding encoder;
		public CategoryMgr catMgr;
		private FileStream fs=null;
		private BinaryReader r=null;
		private FileInfo fI=null;
		private byte[] pdbHeader=new byte[78];
		private int version=0;
		private byte[] appInfo;
		private int appInfoLength;
		private SortedList records;
		
		public PDB()
		{
			encoder=Encoding.GetEncoding("iso-8859-1");
			catMgr=new CategoryMgr();
		}

		/// <summary>
		/// Reads all required bytes.
		/// </summary>
		/// <param name="r">BinaryReader</param>
		/// <param name="srcStart">Start offset in file</param>
		/// <param name="dst">Where to store data</param>
		/// <param name="dstIndex">Index into dst</param>
		/// <param name="len">Bytes to read</param>
		private void ReadFully(BinaryReader r, int srcStart, ref byte[] dst, int dstIndex, int len)
		{
			int read;

			try 
			{
				((Stream)(r.BaseStream)).Seek(srcStart, SeekOrigin.Begin);
				while (len>0) 
				{
					if ((read=r.Read(dst, dstIndex, len))>0) 
					{
						len-=read;
						dstIndex+=read;
					} 
					else if (read==0)
						break;
					else throw new PDBException("Cannot read from file");
				}
			} 
			catch (Exception ex) {
				throw new PDBException(ex.Message);
			}
		}

		/// <summary>
		/// Convert 4 bytes to UInt32.
		/// </summary>
		/// <param name="src">Source data</param>
		/// <param name="index">Index into source data</param>
		/// <returns>UInt32</returns>
		uint GetUInt32(ref byte[] src, int index)
		{
			uint v=0;

			v=src[index++];
			v<<=8;
			v|=src[index++];
			v<<=8;
			v|=src[index++];
			v<<=8;
			v|=src[index++];
			return v;
		}
		
		/// <summary>
		///  Preload the pdb file.
		/// </summary>
		/// <param name="pdbFile"></param>
		/// <param name="type"></param>
		/// <param name="crid"></param>
		public void PreLoad(string pdbFile, string type, string crid)
		{
			bool failure=true;
			
			if (File.Exists(pdbFile)==false)
				throw new PDBException("File not found: "+pdbFile);
			
			try 
			{
				byte[] app=new byte[4];
				
				fI=new FileInfo(pdbFile);

				fs=new FileStream(pdbFile, FileMode.Open, FileAccess.Read);
				r=new BinaryReader(fs);

				ReadFully(r, 0, ref pdbHeader, 0, pdbHeader.Length);

				if ((((string)(encoder.GetString(pdbHeader, 60, 4)))!=type) ||
					(((string)(encoder.GetString(pdbHeader, 64, 4)))!=crid))
					throw new PDBException("Type or Creator ID error");

				Length=(((pdbHeader[76])<<8)&0xff00)|(pdbHeader[77]);
				version=(((pdbHeader[34])<<8)&0xff00)|(pdbHeader[35]);

				ReadFully(r, 78, ref app, 0, 4);
				appInfoLength=(int)(GetUInt32(ref app, 0)-GetUInt32(ref pdbHeader, 52));
				appInfo=new byte[appInfoLength];
				ReadFully(r, (int)GetUInt32(ref pdbHeader, 52), ref appInfo, 0, appInfoLength);
				failure=false;
			}
			catch (Exception ex) 
			{
				throw new PDBException(ex.Message);
			} 
			finally 
			{
				if (failure) {
					if (r!=null)
						r.Close();
					if (fs!=null)
						fs.Close();
				}
			}
		}

		/// <summary>
		/// Loads and builds the record structure from the PDB file.
		/// </summary>
		/// <param name="passwd">Password</param>
		public void Load(string passwd)
		{
			try 
			{
				string dummy="1234567890abcdefghijklmnopqrstuvwxyz";
				SHA256 sha=new SHA256Managed();
				byte[] rEntry=new byte[8];
				byte[] pwd=new byte[32];
				int rListOffset=0x4e, hashIdx, appInfoIdx, cnt;
				byte[] hash, recordData, secret;
				int sIdx=0, dIdx, chunkSize, tCnt;
				uint offset, nextOff;
				byte attributes;
				ushort flags;
				bool template;
				string title;
				Record rec;

				switch (version) {
					case 1:
						for (dIdx=0; dIdx<32; dIdx++)
						{
							pwd[dIdx]=Convert.ToByte(passwd[sIdx++]);
							if (sIdx==passwd.Length)
								sIdx=0;
						}
						break;
						
					case 2:
						for (dIdx=0; dIdx<passwd.Length; dIdx++)
							pwd[sIdx++]=Convert.ToByte(passwd[dIdx]);
						
						for (dIdx=0; dIdx<(32-passwd.Length); dIdx++)
							pwd[sIdx++]=Convert.ToByte(dummy[dIdx]);
						break;
						
					default:
						throw new Exception("Unsupported database version");
				}

				hash=sha.ComputeHash(pwd);

				appInfoIdx=276;		// Start of digest in appInfo
				for (hashIdx=0; hashIdx<32; hashIdx++) 
				{
					if (appInfo[appInfoIdx]!=hash[hashIdx])
						throw new PDBException("Password Error!");

					appInfoIdx++;
				}

				catMgr.Init(appInfo);

				records=new SortedList(new CaseInsensitiveComparer());
				
				ReadFully(r, rListOffset, ref rEntry, 0, rEntry.Length);
				for (cnt=0; cnt<Length; cnt++) 
				{
					offset=GetUInt32(ref rEntry, 0);
					attributes=rEntry[4];
					if ((attributes&0x80)==0x80) 
					{
						// This record is deleted
						ReadFully(r, rListOffset+8, ref rEntry, 0, rEntry.Length);
					} 
					else 
					{
						if (cnt==(Length-1))
							chunkSize=(int)(fI.Length-offset);
						else 
						{
							ReadFully(r, rListOffset+8, ref rEntry, 0, rEntry.Length);
							nextOff=GetUInt32(ref rEntry, 0);
							chunkSize=(int)(nextOff-offset);
						}

						recordData=new byte[chunkSize];

						ReadFully(r, (int)offset, ref recordData, 0, chunkSize);

						flags=(ushort)((recordData[0]<<8)|recordData[1]);
						if ((flags&0x0001)==0x0001)
							template=true;
						else
							template=false;

						tCnt=0;
						if ((flags&0x4000)==0x4000) 
						{
							for (; recordData[4+tCnt]!=0; tCnt++);

							title=encoder.GetString(recordData, 4, tCnt);
							tCnt++;
							if ((tCnt&1)==1) 
								tCnt++;

							nextOff=(uint)(tCnt+4);
						} 
						else 
						{
							title="";
							nextOff=4;
						}

						if ((flags&0x8000)==0x8000) 
						{
							secret=new byte[chunkSize-4-tCnt];
							Array.Copy(recordData, nextOff, secret, 0, secret.Length);
						} 
						else
							secret=null;

						rec=new Record(title, secret, template, (byte)(attributes&0x0f));
						title+=String.Format("{0:#####}", cnt); //+title;
						records.Add(title, rec);
					}

					rListOffset+=8;
				}

				aes.CreateKeys(ref pwd);
				Array.Copy(appInfo, 308, pwd, 0, 32);
				aes.Decrypt(ref pwd, 0);
				aes.Decrypt(ref pwd, 16);
				aes.CreateKeys(ref pwd);
				Array.Clear(pwd, 0, pwd.Length);
			} 
			catch (Exception ex) 
			{
				throw new PDBException(ex.Message);
			} 
			finally 
			{
				if (r!=null)
					r.Close();
				if (fs!=null)
					fs.Close();
			}
		}

		/// <summary>
		/// Get the record at pos "index".
		/// </summary>
		/// <param name="index">Index</param>
		/// <returns></returns>
		public Record GetRecord(int index)
		{
			if (index<records.Count)
				return (Record)records.GetByIndex(index);

			return null;
		}
		
		/// <summary>
		/// Get extension from extension block.
		/// </summary>
		/// <param name="ext">Extension to get</param>
		/// <returns>Extension value or null</returns>
		public string GetExtension(byte ext) {
			int idx, s;
			
			for (idx=340; idx<appInfoLength; ) {
				Console.WriteLine("Extension at "+idx);
				if (appInfo[idx]==0)
					break;

				s=(appInfo[idx+1]<<8)|(appInfo[idx+2]);

				if (appInfo[idx]==ext) {
					Console.WriteLine("Found extension");
					return encoder.GetString(appInfo, idx+3, s);
				}

				idx+=s+3;
			}

			return null;
		}
	}
}
