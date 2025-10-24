using System;
using System.Text;

public class ByteBuilder {
	private int grwth;
	private byte[] bucket;
	private int capacity;
	public int Count;
	private Encoding enc;

	public ByteBuilder() {
		InitByteBuilder(256);
	}

	public ByteBuilder(int growth) {
		InitByteBuilder(growth);
	}

	private void InitByteBuilder(int g) {
		grwth=g;
		capacity=grwth;
		enc=Encoding.GetEncoding("iso-8859-1");
		bucket=new byte[capacity];
		Count=0;
	}

	private void Grow(int minSize) {
		minSize/=grwth;
		minSize++;
		minSize*=grwth;

		byte[] s=bucket;

		capacity+=minSize;
		bucket=new byte[capacity];
		Array.Copy(s, 0, bucket, 0, s.Length);
	}

	public void Append(byte b) {
		if (Count>=capacity) 
			Grow(capacity+grwth);

		bucket[Count++]=b;
	}

	public void Append(string src) {
		if (src==null)
			return;

		byte[] b=enc.GetBytes(src);
		int len=b.Length;

		if (len>0) {
			if ((Count+len)>capacity)
				Grow(Count+len);

			Array.Copy(b, 0, bucket, Count, len);
			Count+=len;
		}
	}

	public void Append(byte[] src) {
		if (src==null)
			return;

		int len=src.Length;

		if (len>0) {
			if ((Count+len)>capacity)
				Grow(Count+len);

			Array.Copy(src, 0, bucket, Count, len);
			Count+=len;
		}
	}

	public byte[] ToBytes() {
		if (Count==0)
			return null;

		byte[] b=new byte[Count];

		Array.Copy(bucket, 0, b, 0, Count);
		return b;
	}

	public byte[] ToBytes(int max) {
		if (Count==0)
			return null;

		int m=(Count>max) ? max : Count;
		
		byte[] b=new byte[m];
		Array.Copy(bucket, 0, b, 0, m);
		return b;
	}

	public void Reset() {
		Count=0;
	}
}
