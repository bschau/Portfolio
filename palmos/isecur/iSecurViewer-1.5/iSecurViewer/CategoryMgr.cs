using System;
using System.Collections;
using System.Globalization;
using System.Text;

namespace iSecurViewer
{
	/// <summary>
	/// Category Manager.
	/// </summary>
	public class CategoryMgr
	{
//		public SortedList sortedCategories=new SortedList(InvariantComparer.Default);
		public SortedList sortedCategories=new SortedList(new CaseInsensitiveComparer());
		public string[] categories=new string[16];
		private Encoding encoder;

		/// <summary>
		/// Comparer function.
		/// </summary>
//		internal class InvariantComparer : IComparer 
//		{
//			private CompareInfo compareInfo;
//			internal static readonly InvariantComparer Default=new InvariantComparer();

//			internal InvariantComparer() 
//			{
//				compareInfo=CultureInfo.InvariantCulture.CompareInfo;
//			}
    
//			public int Compare(Object a, Object b)
//			{
//				String sa = a as String;
//				String sb = b as String;
//				if (sa!=null && sb!=null)
//					return compareInfo.Compare(sa, sb);
//				else
//					return Comparer.Default.Compare(a, b);
//			}
//		}

		public CategoryMgr()
		{
			encoder=Encoding.GetEncoding("iso-8859-1");
		}

		/// <summary>
		/// Setup the Category Magr.
		/// </summary>
		/// <param name="appInfo">AppInfo block with categories.</param>
		public void Init(byte[] appInfo)
		{
			int appInfoIdx=2, cnt;

			for (cnt=0; cnt<16; cnt++) 
			{
				if (appInfo[appInfoIdx]!=0) {
					int bCnt;

					for (bCnt=0; (bCnt<16) && (appInfo[appInfoIdx+bCnt]!=0); bCnt++)
						;
					categories[cnt]=encoder.GetString(appInfo, appInfoIdx, bCnt);
				} 
				else 
					categories[cnt]=null;

				appInfoIdx+=16;
			}

			for (cnt=1; cnt<16; cnt++) 
			{
				if (categories[cnt]!=null)									   
					sortedCategories.Add(categories[cnt], null);
			}
		}

		/// <summary>
		/// Get the category name at index.
		/// </summary>
		/// <param name="index">Index.</param>
		/// <returns>Category name (or null if unset)</returns>
		public string GetCategoryName(int index)
		{
			if (index>-1 && index<16)
				return categories[index];

			return null;
		}

		/// <summary>
		/// Get index of category name.
		/// </summary>
		/// <param name="name">Category name.</param>
		/// <returns>Category index.</returns>
		public int GetCategoryIndex(string name)
		{
			int idx;

			if (name==null || name.Length==0)
				return -1;

			for (idx=0; idx<16; idx++) 
			{
				if (name.CompareTo(categories[idx])==0)
					return idx;
			}

			return -1;
		}
	}
}
