using certview.DomainModel;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Security.Cryptography.X509Certificates;

namespace certview.Queries
{
	public class GetCertificates
	{
		readonly List<Certificate> _certificates = new List<Certificate>();

		public IEnumerable<Certificate> Execute(string search)
		{
			foreach (var location in new List<StoreLocation>
				{
					StoreLocation.LocalMachine,
					StoreLocation.CurrentUser
				})
			{
				foreach ( var name in new List<StoreName>
					{
						StoreName.AddressBook,
						StoreName.AuthRoot,
						StoreName.CertificateAuthority,
						StoreName.Disallowed,
						StoreName.My,
						StoreName.Root,
						StoreName.TrustedPeople,
						StoreName.TrustedPublisher
					})
				{
					var store = new X509Store(name, location);
					try
					{
						store.Open(OpenFlags.ReadOnly);
						Search(store, search);
					}
					catch (Exception exception)
					{
						Console.Error.WriteLine("{0}.{1}: {2}", location, name, exception.Message);
					}
					finally
					{
						store.Close();
					}
				}
			}

			return _certificates;
		}

		void Search(X509Store store, string search)
		{
			AddCollection(store, X509FindType.FindByIssuerName, search);
			AddCollection(store, X509FindType.FindBySubjectName, search);
			AddCollection(store, X509FindType.FindByThumbprint, search);

			if (IsSerialNumber(search))
			{
				AddCollection(store, X509FindType.FindBySerialNumber, search);
			}
		}

		void AddCollection(X509Store store, X509FindType findType, string search)
		{
			var certificates = store.Certificates.Find(findType, search, false);
			foreach (var certificate in certificates)
			{
				if (_certificates.Any(x => x.X509Certificate2.Thumbprint == certificate.Thumbprint && x.X509Certificate2.SerialNumber == certificate.SerialNumber))
				{
					continue;
				}

				_certificates.Add(new Certificate(store.Location.ToString(), store.Name, certificate));
			}
		}

		bool IsSerialNumber(string search)
		{
			return !search.ToCharArray().Any(c => !"0123456789abcdefABCDEF".Contains(c));
		}
	}
}
