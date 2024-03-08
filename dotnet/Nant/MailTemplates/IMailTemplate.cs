namespace Nant.MailTemplates
{
	public interface IMailTemplate
	{
		string Subject { get; }
		string Body { get; }
	}
}
