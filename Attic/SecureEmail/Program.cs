// Init:  dotnet add package System.Security.Cryptography.Pkcs
using System.Net;
using System.Net.Mail;
using System.Security.Cryptography.X509Certificates;
using SecureMail;

var message = new SecureMailMessage
{
	From = new MailAddress("e4cwfdofzq@schau.dk", "Brian Schau"),
	To = new MailAddress("bschau@posteo.net", "Brian Gmail"),
	ReplyTo = new MailAddress("e4cwfdofzq@schau.dk", "Brian Schau"),
	Subject = "Hello World",
	Body = "<h2>Hello World</h2><p>Greetings, earthling!</p",
	EncryptionCertificate = GetCertificate("encrypt"),
	SealCertificate = GetCertificate("seal"),
	SignCertificate = GetCertificate("sign"),
};

message.Attachments.Add(new SecureAttachment
{
	ContentType = "application/pdf",
	Data = File.ReadAllBytes("Minimal.pdf"),
	Filename = "helloworld.pdf"
});

var client = new SmtpClient("mxa.emailforwardmx.com", 587)
{
	EnableSsl = true,
	DeliveryMethod = SmtpDeliveryMethod.Network,
	UseDefaultCredentials = false,
	Credentials = new NetworkCredential("e4cwfdofzq@schau.dk", "y83bCxPl63"),
};
client.Send(message.ToMailMessage());

static X509Certificate2 GetCertificate(string certificate)
{
	var path = $"certs/{certificate}.pfx";
	var raw = File.ReadAllBytes(path);
	return new X509Certificate2(raw, "abcd1234");
}
