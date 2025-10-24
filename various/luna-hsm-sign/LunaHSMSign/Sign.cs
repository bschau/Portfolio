using iTextSharp.text;
using iTextSharp.text.pdf;
using iTextSharp.text.pdf.security;
using Org.BouncyCastle.Security;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Security.Cryptography.X509Certificates;
using X509Certificate = Org.BouncyCastle.X509.X509Certificate;

namespace LunaHSMSign
{
	public class Sign
	{
		public byte[] Execute(byte[] pdf, X509Certificate2 certificate)
		{
			using (var ms = new MemoryStream())
			{
				var chain = CreateCertificateChain(certificate);
				MakeSignature.SignDetached(CreateAppearance(pdf, ms),
											new X509Certificate2Signature(certificate, DigestAlgorithms.SHA256),
											chain,
											new List<ICrlClient> { new CrlClientOnline(chain) },
											new OcspClientBouncyCastle(),
											CreateTsaClient(chain),
											estimatedSize: 0,
											sigtype: CryptoStandard.CADES);
				return ms.ToArray();
			}
		}

		List<X509Certificate> CreateCertificateChain(X509Certificate2 certificate)
		{
			var chain = new X509Chain();
			chain.Build(certificate);

			return (from X509ChainElement x509ChainElement in chain.ChainElements
					select DotNetUtilities.FromX509Certificate(x509ChainElement.Certificate)
					).ToList();
		}

		ITSAClient CreateTsaClient(List<X509Certificate> chain)
		{
			return (from cert in chain
					select CertificateUtil.GetTSAURL(cert)
						into tsaUrl
					where tsaUrl != null
					select new TSAClientBouncyCastle(tsaUrl)
					).FirstOrDefault();
		}

		// This creates a hidden field. Change the values to the Rectangle below
		// to make a visible signature field.
		PdfSignatureAppearance CreateAppearance(byte[] pdfData, MemoryStream memoryStream)
		{
			var now = DateTime.Now.ToString("ddMMyyyyHHmmss");
			var signatureField = string.Format("signatureField{0}", now);
			var reader = new PdfReader(pdfData);
			var stamper = PdfStamper.CreateSignature(reader, memoryStream, '\0');
			var appearance = stamper.SignatureAppearance;
			appearance.Reason = "Signed by me";							// Change to something appropriate
			appearance.Location = "Here, there or somewhere ...";		// ... same here :-)
			appearance.SetVisibleSignature(new Rectangle(0, 0, 0, 0), 1, signatureField);
			return appearance;
		}
	}
}
