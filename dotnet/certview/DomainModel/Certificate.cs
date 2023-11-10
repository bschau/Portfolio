using System.Security.Cryptography.X509Certificates;

namespace certview.DomainModel
{
	public class Certificate
	{
		public string StoreLocation { get; private set; }
		public string StoreName { get; private set; }
		public X509Certificate2 X509Certificate2 { get; private set; }

		public Certificate(string location, string name, X509Certificate2 certificate)
		{
			StoreLocation = location;
			StoreName = name;
			X509Certificate2 = certificate;
		}
	}
}
