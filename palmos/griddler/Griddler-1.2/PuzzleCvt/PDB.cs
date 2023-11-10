using System;
using System.Collections;
using System.IO;
using System.Text;

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

	private ASCIIEncoding enc=new System.Text.ASCIIEncoding();
	private byte[] pdbHeader=new byte[72];
	private byte[] recordList=new byte[6];
	private ArrayList records=new ArrayList();
	private int recCnt;
	public string fullPath;

	public PDB(string pdbName) {
		DateTime now=DateTime.Now;

		Array.Clear(pdbHeader, 0, pdbHeader.Length);
		Array.Clear(recordList, 0, recordList.Length);

		byte [] n=enc.GetBytes(pdbName);

		if (n.Length>31)
			Array.Copy(n, pdbHeader, 31);
		else
			Array.Copy(n, pdbHeader, n.Length);

		// Attributes: dmHdrAttrBundle
		SetUInt16(ref pdbHeader, 32, 0x0800);
		// Version: 1
		SetUInt16(ref pdbHeader, 34, 0x0001);
		// CreationDate
		SetUInt32(ref pdbHeader, 36, DotNetToPalm(now));
		// ModificationDate
		Array.Copy(pdbHeader, 36, pdbHeader, 40, 4);
		// Type
		Array.Copy(enc.GetBytes("DATA"), 0, pdbHeader, 60, 4);
		// CreatorID
		Array.Copy(enc.GetBytes("griD"), 0, pdbHeader, 64, 4);

		recCnt=0;
	}

	public uint DotNetToPalm(System.DateTime dotNetTimestamp) {
		System.TimeSpan span=new System.TimeSpan(System.DateTime.Parse("1/1/1970").Ticks);
		System.DateTime time=dotNetTimestamp.Subtract(span);
		return (uint)((time.Ticks/10000000)+2082844800);
	}

	private void SetUInt16(ref byte[] dst, int index, int v) {
		dst[index]=(byte)((v>>8)&0xff);
		dst[index+1]=(byte)(v&0xff);
	}

	private void SetUInt32(ref byte[] dst, int index, uint v) {
		dst[index]=(byte)((v>>24)&0xff);
		dst[index+1]=(byte)((v>>16)&0xff);
		dst[index+2]=(byte)((v>>8)&0xff);
		dst[index+3]=(byte)(v&0xff);
	}

	public string Write(string dstFile) {
		FileStream f=null;
		BinaryWriter b=null;
		string retStat=null;

		SetUInt16(ref recordList, 4, recCnt);
		try {
			//	00	LocalID	localChunkID;
			//	04	UInt8	attributes;
			//	05	UInt8	uniqueID[3];
			byte[] recordEntry=new byte[8];
			uint offset=(uint)(78+(recCnt*8));
			int idx;
			string name;

			if (dstFile.ToLower().EndsWith(".pdb")==true)
				name=dstFile;
			else
				name=dstFile+".pdb";

			Array.Clear(recordEntry, 0, 8);
			f=new FileStream(name, FileMode.Create);
			b=new BinaryWriter(f);

			b.Write(pdbHeader);
			b.Write(recordList);

			for (idx=0; idx<recCnt; idx++) {
				// Attributes is = 0. UniqueID will be the same as the record index.
				SetUInt32(ref recordEntry, 0, offset);
				SetUInt16(ref recordEntry, 6, idx);
				b.Write(recordEntry);

				offset+=(uint)(((byte [])records[idx]).Length);
			}

			for (idx=0; idx<recCnt; idx++) 
				b.Write((byte [])records[idx]);
		} 
		catch (Exception ex) {
			retStat=ex.Message;
		} finally {
			if (b!=null)
				b.Close();

			if (f!=null)
				f.Close();
		}

		return retStat;
	}

	public void AddRecord(byte[] recData) {
		records.Add(recData);
		recCnt++;
	}
}
