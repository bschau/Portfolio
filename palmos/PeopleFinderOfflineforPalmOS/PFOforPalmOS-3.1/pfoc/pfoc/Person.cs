using System;
using System.Text;
using System.Text.RegularExpressions;

namespace pfoc
{
	/// <summary>
	/// Summary description for Person.
	/// </summary>
	public class Person
	{
		private enum Index 
		{
			ECARDID=0,
			SURNAME,
			GIVENNAME,
			PHONE,
			TELNET,
			ALTPHONE,
			MOBILE,
			STREET,
			LOCATION,
			MANAGER,
		};
		public bool valid=false;
		public int mgrIdx=-1;
		public string iEmail, iSurname, iGivenname, iManager;
        public string cPhone, cMobile;
		public byte[] iPhone;
		public byte[] iMobile;
		public int addrIdx;
		public long uid;
        public int pKey, pRecIndex, pOffset;

        /// <summary>
        /// Setup a new Person record.
        /// </summary>
        /// <param name="addr">Address.</param>
        /// <param name="mgr">Manager ID.</param>
        /// <param name="src">Source line.</param>
        /// <param name="r">Regex (to split src).</param>
        /// <param name="u">User ID.</param>
		public Person(Address addr, Manager mgr, string src, Regex r, long u)
		{
			ByteBuilder bb=new ByteBuilder(64);
			string[] p=r.Split(src);
			string s="";
			string phn=null;

			if (p.Length==11) 
			{
				if (p[(int)Index.ECARDID].Length>0)
					iEmail=p[(int)Index.ECARDID].ToLower();
				else
					iEmail="";

				if ((iSurname=p[(int)Index.SURNAME].ToLower()).Length>255)
					iSurname=iSurname.Substring(0, 255);

				if ((iGivenname=p[(int)Index.GIVENNAME].ToLower()).Length>255)
					iGivenname=iGivenname.Substring(0, 255);

                if (((phn = p[(int)Index.PHONE]) == null) || (phn.Length == 0))
                    phn = "";
                else
                    phn = p[(int)Index.PHONE];

                cPhone = CleanReversePhone(phn);
				iPhone=CrunchPhone(bb, phn);

				if (((phn=p[(int)Index.MOBILE])==null) || (phn.Length==0)) 
				{
					if (((phn=p[(int)Index.ALTPHONE])==null) || (phn.Length==0))
						phn="";
				}
                cMobile = CleanReversePhone(phn);
				iMobile=CrunchPhone(bb, phn);

				uid=u;

				if (p[(int)Index.LOCATION].Length>0)
					s=p[(int)Index.STREET];

				if (p[(int)Index.LOCATION].Length>0)
				{
					if (s.Length>0)
						s+="<br>"+p[(int)Index.LOCATION];
					else
						s=p[(int)Index.LOCATION];
				}

				if (s.Length<1)
					s="Unknown";

				addrIdx=addr.Add(s);

				if (p[(int)Index.MANAGER].Length>0)
				{
					iManager=p[(int)Index.MANAGER].ToLower();
					if (iManager.CompareTo(iEmail)==0)
						iManager=null;
					else
						mgr.Add(iManager);
				}
				else
					iManager=null;

				valid=true;
			}
		}

        /// <summary>
        /// Compact a string representation of a Phone number to packed bytes.
        /// </summary>
        /// <param name="bb">ByteBuilder.</param>
        /// <param name="src">Source (with phone no.#).</param>
        /// <returns>The bytearray with the packed number.</returns>
		private byte[] CrunchPhone(ByteBuilder bb, string src)
		{
			StringBuilder sb=new StringBuilder();
			bool ext=false;
			int idx;
			char c;
			byte b;

			for (idx=0; idx<src.Length; idx++) 
			{
				c=src[idx];
				if (c>='0' && c<='9')
					sb.Append(c);
				else if (c==' ' || c=='+')
					continue;
				else if (ext==false) 
				{
					sb.Append('e');
					ext=true;
				}
			}

			src=sb.ToString();

			bb.Reset();

			idx=src.Length-1;
			while (idx>-1) 
			{
				c=src[idx];
				if (c>='0' && c<='9')
					b=(byte)((c-'0')<<4);
				else
					b=176;

				idx--;
				if (idx<0)
				{
					b|=12;
					bb.Append(b);
					break;
				}

				c=src[idx];
				if (c>='0' && c<='9')
					b|=(byte)(c-'0');
				else 
					b|=11;

				bb.Append(b);
				idx--;
			}

			if (bb.Count>0)
				return bb.ToBytes(255);

			return null;
		}

        /// <summary>
        /// Remove garbage from phone number and reverse representation.
        /// </summary>
        /// <param name="phone">Source phone number.</param>
        /// <returns>Cleansed phone number.</returns>
        public string CleanReversePhone(string phone)
        {
            string p = "";
            char[] cArr;

            for (int i=0; i<phone.Length; i++) {
                if (phone[i] >= '0' && phone[i] <= '9')
                    p += phone[i];
                else if (phone[i] != '+' && phone[i] != '.' && phone[i] != ' ')
                    break;
            }

            if (p.Length == 0)
                return null;

            cArr = p.ToCharArray();
            Array.Reverse(cArr);
            p = new string(cArr);
            if (p.Length < 4)
                return null;

            return p;
        }

        /// <summary>
        /// Store a packed representation of the email address for this person into
        /// the bytebuilder.
        /// </summary>
        /// <param name="bb">ByteBuilder</param>
		public void GetCrunchedEmail(ByteBuilder bb)
		{
			bool isHPDomain=false;
			int mailboxNameType=0, atPos;
			string gN, gNDash, sNDash, sN, mbName, dmName;

			if ((atPos=iEmail.IndexOf("@", 0))==-1) 
			{
				bb.Append(0);
				return;
			}

			mbName=iEmail.Substring(0, atPos).ToLower();
			dmName=iEmail.Substring(atPos+1).ToLower();
			gN=iGivenname.ToLower();
			sN=iSurname.ToLower();
			gNDash=iGivenname.Replace(" ", "-");
			sNDash=iSurname.Replace(" ", "-");

			if (mbName.CompareTo(gN+"."+sN)==0)
				mailboxNameType=2;
			else if (mbName.CompareTo(gN+"_"+sN)==0)
				mailboxNameType=3;
			else if (mbName.CompareTo(gNDash+"."+sNDash)==0)
				mailboxNameType=4;
			else if (mbName.CompareTo(gNDash+"_"+sNDash)==0)
				mailboxNameType=5;
			else if (mbName.CompareTo(gN+sN)==0)
				mailboxNameType=6;
			else if (mbName.CompareTo(gNDash+sNDash)==0)
				mailboxNameType=7;
			else if (mbName.CompareTo(sN+"."+gN)==0)
				mailboxNameType=8;
			else if (mbName.CompareTo(sN+"_"+gN)==0)
				mailboxNameType=9;
			else if (mbName.CompareTo(sNDash+"."+gNDash)==0)
				mailboxNameType=10;
			else if (mbName.CompareTo(sNDash+"_"+gNDash)==0)
				mailboxNameType=11;
			else if (mbName.CompareTo(sN+gN)==0)
				mailboxNameType=12;
			else if (mbName.CompareTo(sNDash+gNDash)==0)
				mailboxNameType=13;

			if (dmName.CompareTo("hp.com")==0)
				isHPDomain=true;

			if (mailboxNameType==0) 
			{
				if (isHPDomain==false) 
				{
					bb.Append(32);
					bb.Append(iEmail);
				} 
				else 
				{
					bb.Append(16);
					bb.Append(mbName);
				}
			} 
			else 
			{
				if (isHPDomain==true) 
				{
					mailboxNameType|=16;
					bb.Append((byte)mailboxNameType);
				} 
				else 
				{
					bb.Append((byte)mailboxNameType);
					bb.Append(dmName);
				}
			}
		}
	}
}
