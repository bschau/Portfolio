using System;

namespace jc.Extension
{
	public static class DateTimeExtensions
	{
		public static string JsonDate(this DateTime source)
		{
			return source.ToString("yyyy-MM-ddTHH:mm:ss.fffffff");
		}
	}
}