using System;

namespace iSecurViewer
{
	public class AES 
	{
		private char[] hex={ '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };
		private uint[] roundConstants={ 0x01000000, 0x02000000, 0x04000000, 0x08000000, 0x10000000, 0x20000000, 0x40000000, 0x80000000, 0x1b000000, 0x36000000 };
		private uint[] fwdSBox, rvsSBox;
		private uint[] fwdTab0, fwdTab1, fwdTab2, fwdTab3;
		private uint[] rvsTab0, rvsTab1, rvsTab2, rvsTab3;
		private uint[] dKey, eKey;

		/// <summary>
		/// Constructor for the AES class. The constructor sets up the various tables.
		/// </summary>
		public AES()
		{
			byte[] pow=new byte[256];
			byte[] log=new byte[256];
			byte x=1, y;
			int i;

			fwdSBox=new uint[256];
			rvsSBox=new uint[256];
			fwdTab0=new uint[256];
			fwdTab1=new uint[256];
			fwdTab2=new uint[256];
			fwdTab3=new uint[256];
			rvsTab0=new uint[256];
			rvsTab1=new uint[256];
			rvsTab2=new uint[256];
			rvsTab3=new uint[256];

			for (i=0; i<256; i++)
			{
				pow[i]=(byte)x;
				log[x]=(byte)i;
				x^=(byte)((x<<1)^(((x&0x80)==0x80) ? 0x1b : 0x00));
			}

			fwdSBox[0x00]=0x63;
			rvsSBox[0x63]=0x00;

			for (i=1; i<256; i++)
			{
				x=pow[255-log[i]];

				y=x;
				y=(byte)((byte)(y<<1)|(byte)(y>>7));
				x^=y;
				y=(byte)((byte)(y<<1)|(byte)(y>>7));
				x^=y;
				y=(byte)((byte)(y<<1)|(byte)(y>>7));
				x^=y;
				y=(byte)((byte)(y<<1)|(byte)(y>>7));
				x^=(byte)(y^0x63);

				fwdSBox[i]=(uint)x;
				rvsSBox[x]=(uint)i;
			}

			for (i=0; i<256; i++)
			{
				x=(byte)fwdSBox[i];
				y=(byte)((x<<1)^(((x&0x80)==0x80) ? 0x1b : 0x00));

				fwdTab0[i]=((uint)(x^y)^((uint)x<<8)^((uint)x<<16)^((uint)y<<24))&0xffffffff;
				fwdTab1[i]=(((fwdTab0[i]<<24)&0xffffffff)|((fwdTab0[i]&0xffffffff)>>8));
				fwdTab2[i]=(((fwdTab1[i]<<24)&0xffffffff)|((fwdTab1[i]&0xffffffff)>>8));
				fwdTab3[i]=(((fwdTab2[i]<<24)&0xffffffff)|((fwdTab2[i]&0xffffffff)>>8));

				y=(byte)rvsSBox[i];

				rvsTab0[i]=((y!=0) ? (uint)(pow[(log[0x0b]+log[y])%255]) : (uint)0);
				rvsTab0[i]^=(((y!=0) ? (uint)(pow[(log[0x0d]+log[y])%255]) : (uint)0)<<8);
				rvsTab0[i]^=(((y!=0) ? (uint)(pow[(log[0x09]+log[y])%255]) : (uint)0)<<16);
				rvsTab0[i]^=(((y!=0) ? (uint)(pow[(log[0x0e]+log[y])%255]) : (uint)0)<<24);
				rvsTab0[i]&=0xffffffff;

				rvsTab1[i]=(((rvsTab0[i]<<24)&0xffffffff)|((rvsTab0[i]&0xffffffff)>>8));
				rvsTab2[i]=(((rvsTab1[i]<<24)&0xffffffff)|((rvsTab1[i]&0xffffffff)>>8));
				rvsTab3[i]=(((rvsTab2[i]<<24)&0xffffffff)|((rvsTab2[i]&0xffffffff)>>8));
			}
		}

		/// <summary>
		/// Convert a byte[4] array to an uint.
		/// </summary>
		/// <param name="src">Source array</param>
		/// <param name="idx">Index into source array to pick the 4 bytes</param>
		/// <returns>Resulting uint</returns>
		private uint ToUInt(ref byte[] src, uint idx)
		{
			return (((uint)(src[idx]<<24))|
				((uint)(src[idx+1]<<16))|
				((uint)(src[idx+2]<<8))|
				((uint)(src[idx+3])));
		}

		/// <summary>
		/// Put an uint to a byte[4] array.
		/// </summary>
		/// <param name="src">Source uint</param>
		/// <param name="dst">Destination array</param>
		/// <param name="idx">Index into destination where to put the uint</param>
		private void PutUInt(uint src, ref byte[] dst, uint idx)
		{
			dst[idx]=(byte)(src>>24);
			dst[idx+1]=(byte)(src>>16);
			dst[idx+2]=(byte)(src>>8);
			dst[idx+3]=(byte)(src);
		}

		/// <summary>
		/// Output the uint[] array to the console, 8 uints pr. line.
		/// </summary>
		/// <param name="src">Source uint array</param>
		/// <param name="sLen">Number of elements to output</param>
		public void XOut(ref uint[] src, int sLen)
		{
			int stripe=0, i;
			uint b, v;

			for (i=0; i<sLen; i++) 
			{
				v=src[i];

				b=(v>>24)&0xff;
				Console.Write(hex[(b>>4)&0x0f]);
				Console.Write(hex[b&0xf]);

				b=(v>>16)&0xff;
				Console.Write(hex[(b>>4)&0x0f]);
				Console.Write(hex[b&0xf]);
			
				b=(v>>8)&0xff;
				Console.Write(hex[(b>>4)&0x0f]);
				Console.Write(hex[b&0xf]);
		
				Console.Write(hex[(v>>4)&0x0f]);
				Console.Write(hex[v&0xf]);

				stripe++;
				if (stripe==8) 
				{
					Console.WriteLine();
					stripe=0;
				}
			}

			if (stripe!=0)
				Console.WriteLine();
		}

		/// <summary>
		/// Output a uint to the console.
		/// </summary>
		/// <param name="src">Source uint</param>
		public void XOut(uint src)
		{
			uint[] b={ src };

			XOut(ref b, 1);
		}

		/// <summary>
		/// Output a byte[] array to console
		/// </summary>
		/// <param name="src">Source array</param>
		/// <param name="len">Number of elements to output</param>
		public void XOut(ref byte[] src, uint len)
		{
			uint stripe=0, idx;

			for (idx=0; idx<len; idx++) 
			{
				Console.Write(hex[(src[idx]>>4)&0x0f]);
				Console.Write(hex[src[idx]&0xf]);

				stripe++;
				if (stripe==32) 
				{
					Console.WriteLine();
					stripe=0;
				}
			}

			if (stripe!=0)
				Console.WriteLine();
		}

		/// <summary>
		/// Create the encryption and decryption keys.
		/// </summary>
		/// <param name="key">Key/password to use</param>
		public void CreateKeys(ref byte[] key)
		{
			int dIdx=4, eIdx=0, idx;
			uint[] k0, k1, k2, k3;

			eKey=new uint[64];
			dKey=new uint[64];

			for (idx=0; idx<8; idx++)
				eKey[idx]=ToUInt(ref key, (uint)(idx*4));

			for (idx=0; idx<7; idx++) 
			{
				eKey[eIdx+8]=eKey[eIdx]^roundConstants[idx]^
					(fwdSBox[(byte)(eKey[eIdx+7]>>16)]<<24) ^
					(fwdSBox[(byte)(eKey[eIdx+7]>>8)]<<16) ^
					(fwdSBox[(byte)(eKey[eIdx+7])]<<8) ^
					(fwdSBox[(byte)(eKey[eIdx+7]>>24)]);

				eKey[eIdx+9]=eKey[eIdx+1]^eKey[eIdx+8];
				eKey[eIdx+10]=eKey[eIdx+2]^eKey[eIdx+9];
				eKey[eIdx+11]=eKey[eIdx+3]^eKey[eIdx+10];

				eKey[eIdx+12]=eKey[eIdx+4]^
					(fwdSBox[(byte)(eKey[eIdx+11]>>24)]<<24) ^
					(fwdSBox[(byte)(eKey[eIdx+11]>>16)]<<16) ^
					(fwdSBox[(byte)(eKey[eIdx+11]>>8)]<<8) ^
					(fwdSBox[(byte)(eKey[eIdx+11])]);

				eKey[eIdx+13]=eKey[eIdx+5]^eKey[eIdx+12];
				eKey[eIdx+14]=eKey[eIdx+6]^eKey[eIdx+13];
				eKey[eIdx+15]=eKey[eIdx+7]^eKey[eIdx+14];
				eIdx+=8;
			}

			k0=new uint[256];
			k1=new uint[256];
			k2=new uint[256];
			k3=new uint[256];

			for (idx=0; idx<256; idx++) 
			{
				k0[idx]=rvsTab0[fwdSBox[idx]];
				k1[idx]=rvsTab1[fwdSBox[idx]];
				k2[idx]=rvsTab2[fwdSBox[idx]];
				k3[idx]=rvsTab3[fwdSBox[idx]];
			}

			dKey[0]=eKey[eIdx++];
			dKey[1]=eKey[eIdx++];
			dKey[2]=eKey[eIdx++];
			dKey[3]=eKey[eIdx++];

			for (idx=1; idx<14; idx++) 
			{
				eIdx-=8;

				dKey[dIdx++]=k0[(byte)(eKey[eIdx]>>24)] ^
					k1[(byte)(eKey[eIdx]>>16)] ^
					k2[(byte)(eKey[eIdx]>>8)] ^
					k3[(byte)(eKey[eIdx])];

				eIdx++;

				dKey[dIdx++]=k0[(byte)(eKey[eIdx]>>24)] ^
					k1[(byte)(eKey[eIdx]>>16)] ^
					k2[(byte)(eKey[eIdx]>>8)] ^
					k3[(byte)(eKey[eIdx])];

				eIdx++;

				dKey[dIdx++]=k0[(byte)(eKey[eIdx]>>24)] ^
					k1[(byte)(eKey[eIdx]>>16)] ^
					k2[(byte)(eKey[eIdx]>>8)] ^
					k3[(byte)(eKey[eIdx])];

				eIdx++;

				dKey[dIdx++]=k0[(byte)(eKey[eIdx]>>24)] ^
					k1[(byte)(eKey[eIdx]>>16)] ^
					k2[(byte)(eKey[eIdx]>>8)] ^
					k3[(byte)(eKey[eIdx])];

				eIdx++;
			}

			eIdx-=8;

			dKey[dIdx++]=eKey[eIdx++];
			dKey[dIdx++]=eKey[eIdx++];
			dKey[dIdx++]=eKey[eIdx++];
			dKey[dIdx]=eKey[eIdx];
		}

		/// <summary>
		/// Encryption round.
		/// </summary>
		private void F(ref uint idx, ref uint x0, ref uint x1, ref uint x2, ref uint x3, ref uint y0, ref uint y1, ref uint y2, ref uint y3)
		{
			idx+=4;

			x0=eKey[idx]^fwdTab0[(byte)(y0>>24)] ^
				fwdTab1[(byte)(y1>>16)] ^
				fwdTab2[(byte)(y2>>8)] ^
				fwdTab3[(byte)(y3)];
			x1=eKey[idx+1]^fwdTab0[(byte)(y1>>24)] ^
				fwdTab1[(byte)(y2>>16)] ^
				fwdTab2[(byte)(y3>>8)] ^
				fwdTab3[(byte)(y0)];
			x2=eKey[idx+2]^fwdTab0[(byte)(y2>>24)] ^
				fwdTab1[(byte)(y3>>16)] ^
				fwdTab2[(byte)(y0>>8)] ^
				fwdTab3[(byte)(y1)];
			x3=eKey[idx+3]^fwdTab0[(byte)(y3>>24)] ^
				fwdTab1[(byte)(y0>>16)] ^
				fwdTab2[(byte)(y1>>8)] ^
				fwdTab3[(byte)(y2)];
		}	

		/// <summary>
		/// Encrypt 128 bits of data.
		/// </summary>
		/// <param name="data">Data to encrypt.</param>
		/// <param name="dIdx">Index into data.</param>
		public void Encrypt(ref byte[] data, uint dIdx)
		{
			uint eIdx=0, y0=0, y1=0, y2=0, y3=0, x0, x1, x2, x3;

			x0=ToUInt(ref data, dIdx)^eKey[0];
			x1=ToUInt(ref data, dIdx+4)^eKey[1];
			x2=ToUInt(ref data, dIdx+8)^eKey[2];
			x3=ToUInt(ref data, dIdx+12)^eKey[3];

			F(ref eIdx, ref y0, ref y1, ref y2, ref y3, ref x0, ref x1, ref x2, ref x3);
			F(ref eIdx, ref x0, ref x1, ref x2, ref x3, ref y0, ref y1, ref y2, ref y3);
			F(ref eIdx, ref y0, ref y1, ref y2, ref y3, ref x0, ref x1, ref x2, ref x3);
			F(ref eIdx, ref x0, ref x1, ref x2, ref x3, ref y0, ref y1, ref y2, ref y3);
			F(ref eIdx, ref y0, ref y1, ref y2, ref y3, ref x0, ref x1, ref x2, ref x3);
			F(ref eIdx, ref x0, ref x1, ref x2, ref x3, ref y0, ref y1, ref y2, ref y3);
			F(ref eIdx, ref y0, ref y1, ref y2, ref y3, ref x0, ref x1, ref x2, ref x3);
			F(ref eIdx, ref x0, ref x1, ref x2, ref x3, ref y0, ref y1, ref y2, ref y3);
			F(ref eIdx, ref y0, ref y1, ref y2, ref y3, ref x0, ref x1, ref x2, ref x3);
			F(ref eIdx, ref x0, ref x1, ref x2, ref x3, ref y0, ref y1, ref y2, ref y3);
			F(ref eIdx, ref y0, ref y1, ref y2, ref y3, ref x0, ref x1, ref x2, ref x3);
			F(ref eIdx, ref x0, ref x1, ref x2, ref x3, ref y0, ref y1, ref y2, ref y3);
			F(ref eIdx, ref y0, ref y1, ref y2, ref y3, ref x0, ref x1, ref x2, ref x3);

			eIdx+=4;
			x0=eKey[eIdx]^(fwdSBox[(byte)(y0>>24)]<<24) ^
				(fwdSBox[(byte)(y1>>16)]<<16) ^
				(fwdSBox[(byte)(y2>>8)]<<8) ^
				(fwdSBox[(byte)(y3)]);

			x1=eKey[eIdx+1]^(fwdSBox[(byte)(y1>>24)]<<24) ^
				(fwdSBox[(byte)(y2>>16)]<<16) ^
				(fwdSBox[(byte)(y3>>8)]<<8) ^
				(fwdSBox[(byte)(y0)]);

			x2=eKey[eIdx+2]^(fwdSBox[(byte)(y2>>24)]<<24) ^
				(fwdSBox[(byte)(y3>>16)]<<16) ^
				(fwdSBox[(byte)(y0>>8)]<<8) ^
				(fwdSBox[(byte)(y1)]);

			x3=eKey[eIdx+3]^(fwdSBox[(byte)(y3>>24)]<<24) ^
				(fwdSBox[(byte)(y0>>16)]<<16) ^
				(fwdSBox[(byte)(y1>>8)]<<8) ^
				(fwdSBox[(byte)(y2)]);

			PutUInt(x0, ref data, dIdx);
			PutUInt(x1, ref data, dIdx+4);
			PutUInt(x2, ref data, dIdx+8);
			PutUInt(x3, ref data, dIdx+12);
		}

		/// <summary>
		/// Decryption round.
		/// </summary>
		private void R(ref uint idx, ref uint x0, ref uint x1, ref uint x2, ref uint x3, ref uint y0, ref uint y1, ref uint y2, ref uint y3)
		{
			idx+=4;

			x0=dKey[idx]^rvsTab0[(byte)(y0>>24)] ^
				rvsTab1[(byte)(y3>>16)] ^
				rvsTab2[(byte)(y2>>8)] ^
				rvsTab3[(byte)(y1)];
			x1=dKey[idx+1]^rvsTab0[(byte)(y1>>24)] ^
				rvsTab1[(byte)(y0>>16)] ^
				rvsTab2[(byte)(y3>>8)] ^
				rvsTab3[(byte)(y2)];
			x2=dKey[idx+2]^rvsTab0[(byte)(y2>>24)] ^
				rvsTab1[(byte)(y1>>16)] ^
				rvsTab2[(byte)(y0>>8)] ^
				rvsTab3[(byte)(y3)];
			x3=dKey[idx+3]^rvsTab0[(byte)(y3>>24)] ^
				rvsTab1[(byte)(y2>>16)] ^
				rvsTab2[(byte)(y1>>8)] ^
				rvsTab3[(byte)(y0)];
		}

		/// <summary>
		/// Decrypt 128 bits of data.
		/// </summary>
		/// <param name="data">Data to encrypt.</param>
		/// <param name="dIdx">Index into data.</param>
		public void Decrypt(ref byte[] data, uint dIdx)
		{
			uint eIdx=0, y0=0, y1=0, y2=0, y3=0, x0, x1, x2, x3;

			x0=ToUInt(ref data, dIdx)^dKey[0];
			x1=ToUInt(ref data, dIdx+4)^dKey[1];
			x2=ToUInt(ref data, dIdx+8)^dKey[2];
			x3=ToUInt(ref data, dIdx+12)^dKey[3];

			R(ref eIdx, ref y0, ref y1, ref y2, ref y3, ref x0, ref x1, ref x2, ref x3);
			R(ref eIdx, ref x0, ref x1, ref x2, ref x3, ref y0, ref y1, ref y2, ref y3);
			R(ref eIdx, ref y0, ref y1, ref y2, ref y3, ref x0, ref x1, ref x2, ref x3);
			R(ref eIdx, ref x0, ref x1, ref x2, ref x3, ref y0, ref y1, ref y2, ref y3);
			R(ref eIdx, ref y0, ref y1, ref y2, ref y3, ref x0, ref x1, ref x2, ref x3);
			R(ref eIdx, ref x0, ref x1, ref x2, ref x3, ref y0, ref y1, ref y2, ref y3);
			R(ref eIdx, ref y0, ref y1, ref y2, ref y3, ref x0, ref x1, ref x2, ref x3);
			R(ref eIdx, ref x0, ref x1, ref x2, ref x3, ref y0, ref y1, ref y2, ref y3);
			R(ref eIdx, ref y0, ref y1, ref y2, ref y3, ref x0, ref x1, ref x2, ref x3);
			R(ref eIdx, ref x0, ref x1, ref x2, ref x3, ref y0, ref y1, ref y2, ref y3);
			R(ref eIdx, ref y0, ref y1, ref y2, ref y3, ref x0, ref x1, ref x2, ref x3);
			R(ref eIdx, ref x0, ref x1, ref x2, ref x3, ref y0, ref y1, ref y2, ref y3);
			R(ref eIdx, ref y0, ref y1, ref y2, ref y3, ref x0, ref x1, ref x2, ref x3);

			eIdx+=4;
			x0=dKey[eIdx]^(rvsSBox[(byte)(y0>>24)]<<24) ^
				(rvsSBox[(byte)(y3>>16)]<<16) ^
				(rvsSBox[(byte)(y2>>8)]<<8) ^
				(rvsSBox[(byte)(y1)]);

			x1=dKey[eIdx+1]^(rvsSBox[(byte)(y1>>24)]<<24) ^
				(rvsSBox[(byte)(y0>>16)]<<16) ^
				(rvsSBox[(byte)(y3>>8)]<<8) ^
				(rvsSBox[(byte)(y2)]);

			x2=dKey[eIdx+2]^(rvsSBox[(byte)(y2>>24)]<<24) ^
				(rvsSBox[(byte)(y1>>16)]<<16) ^
				(rvsSBox[(byte)(y0>>8)]<<8) ^
				(rvsSBox[(byte)(y3)]);

			x3=dKey[eIdx+3]^(rvsSBox[(byte)(y3>>24)]<<24) ^
				(rvsSBox[(byte)(y2>>16)]<<16) ^
				(rvsSBox[(byte)(y1>>8)]<<8) ^
				(rvsSBox[(byte)(y0)]);

			PutUInt(x0, ref data, dIdx);
			PutUInt(x1, ref data, dIdx+4);
			PutUInt(x2, ref data, dIdx+8);
			PutUInt(x3, ref data, dIdx+12);
		}
	}
}
