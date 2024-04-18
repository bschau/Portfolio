using iTextSharp.text.pdf;
using System;
using System.Diagnostics;
using System.IO;

namespace PdfLock
{
	class Program
	{
		static void Main(string[] args)
		{
			var bytes = File.ReadAllBytes(@"c:/eSignatur/UnitTestData/problematic-pfa.pdf");

			using (var output = new MemoryStream())
			{
				using (var reader = new PdfReader(bytes))
				{
					using (var stamper = new PdfStamper(reader, output))
					{
						var acFields = stamper.AcroFields;
						var fieldMap = acFields.Fields;
						foreach (var key in fieldMap.Keys)
						{
							acFields.SetFieldProperty(
								key,
								"setfflags",
								PdfFormField.FF_READ_ONLY,
								null);
						}
					}
				}

				var name = string.Format("c:/temp/pdf-{0}.pdf", DateTime.Now.Ticks);
				File.WriteAllBytes(name, output.ToArray());
				Process.Start(name);
			}
		}
	}
}
