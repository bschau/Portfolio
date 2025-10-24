using System.Security.Cryptography.X509Certificates;

namespace certview.Queries
{
	public interface IGetPrivateKeyFilename
	{
		string Execute(X509Certificate2 certificate);
	}
}
