namespace dk.schau.AzureApps.DomainModel
{
	public class MailSettings
	{
		public string MailServer { get; set; }
		public int MailServerPort { get; set; }
		public string MailServerAccount { get; set; }
		public string MailServerPassword { get; set; }
		public string From { get ; set; }
		public string To { get; set; }
	}
}