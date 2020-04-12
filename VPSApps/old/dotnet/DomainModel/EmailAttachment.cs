using MimeKit;

namespace VPSApps.DomainModel
{
	class EmailAttachment
	{
		public byte[] Data { get; set; }
		public string Filename { get; set; }
		public ContentType ContentType { get; set; }
	}
}