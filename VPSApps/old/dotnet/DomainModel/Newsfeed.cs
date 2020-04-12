namespace VPSApps.DomainModel
{
	public class Newsfeed
	{
		public Newsfeed()
		{
			Dtl = 90;
		}

		public string Name { get; set; }
		public string Url { get; set; }
		public int Dtl { get; set; }
	}
}