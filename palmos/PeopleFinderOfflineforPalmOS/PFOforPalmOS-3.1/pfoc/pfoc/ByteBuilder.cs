using System;
using System.Text;

namespace pfoc
{
	/// <summary>
	/// Summary description for ByteBuilder.
	/// </summary>
	public class ByteBuilder
	{
		private int grwth;
		private byte[] bucket;
		private int capacity;
		public int Count;
		private ASCIIEncoding enc;

        /// <summary>
        /// Create a new ByteBuilder.
        /// </summary>
		public ByteBuilder()
		{
			InitByteBuilder(256);
		}

        /// <summary>
        /// Create a new ByteBuilder.
        /// </summary>
        /// <param name="growth">How much to grow on each resize.</param>
		public ByteBuilder(int growth)
		{
			InitByteBuilder(growth);
		}

        /// <summary>
        /// Initialize this ByteBuilder.
        /// </summary>
        /// <param name="g">Growth - how much to grow on each resize.</param>
		private void InitByteBuilder(int g)
		{
			grwth=g;
			capacity=grwth;
			enc=new System.Text.ASCIIEncoding();
			bucket=new byte[capacity];
			Count=0;
		}

        /// <summary>
        /// Grow the ByteBuilder.
        /// </summary>
        /// <param name="minSize">Grow with at least this amount of bytes.</param>
		private void Grow(int minSize)
		{
			minSize/=grwth;
			minSize++;
			minSize*=grwth;

			byte[] s=bucket;

			capacity+=minSize;
			bucket=new byte[capacity];
			Array.Copy(s, 0, bucket, 0, s.Length);
		}

        /// <summary>
        /// Append a byte to ByteBuilder.
        /// </summary>
        /// <param name="b">Byte.</param>
		public void Append(byte b)
		{
			if (Count>=capacity) 
				Grow(capacity+grwth);

			bucket[Count++]=b;
		}

        /// <summary>
        /// Append an unsigned Short to ByteBuilder.
        /// </summary>
        /// <param name="b">Byte.</param>
        public void Append(ushort s)
        {
            if ((Count+2) >= capacity)
                Grow(capacity + grwth);

            bucket[Count++] = (byte)((s>>8)&0xff);
            bucket[Count++] = (byte)(s & 0xff);
        }

        /// <summary>
        /// Append string to ByteBuilder.
        /// </summary>
        /// <param name="src">String.</param>
		public void Append(string src)
		{
			if (src==null)
				return;

			byte[] b=enc.GetBytes(src);
			int len=b.Length;

			if (len>0) 
			{
				if ((Count+len)>capacity)
					Grow(Count+len);

				Array.Copy(b, 0, bucket, Count, len);
				Count+=len;
			}
		}

        /// <summary>
        /// Append byte array to ByteBuilder.
        /// </summary>
        /// <param name="src">Byte array.</param>
		public void Append(byte[] src)
		{
			if (src==null)
				return;

			int len=src.Length;

			if (len>0) 
			{
				if ((Count+len)>capacity)
					Grow(Count+len);

				Array.Copy(src, 0, bucket, Count, len);
				Count+=len;
			}
		}

        /// <summary>
        /// Convert ByteBuilder to byte array.
        /// </summary>
        /// <returns>Byte Array (or null if empty).</returns>
		public byte[] ToBytes()
		{
			if (Count==0)
				return null;

			byte[] b=new byte[Count];

			Array.Copy(bucket, 0, b, 0, Count);
			return b;
		}

        /// <summary>
        /// Convert max bytes of ByteBuilder to byte array.
        /// </summary>
        /// <param name="max">Threshold.</param>
        /// <returns>Byte Array (or null if empty).</returns>
		public byte[] ToBytes(int max)
		{
			if (Count==0)
				return null;

			int m=(Count>max) ? max : Count;
		
			byte[] b=new byte[m];
			Array.Copy(bucket, 0, b, 0, m);
			return b;
		}

        /// <summary>
        /// Reset the ByteBuilder.
        /// </summary>
		public void Reset()
		{
			Count=0;
		}
	}
}
