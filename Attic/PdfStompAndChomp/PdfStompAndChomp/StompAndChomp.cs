using iTextSharp.text.pdf;
using System.IO;
using System.Linq;

namespace PdfStompAndChomp
{
	public class StompAndChomp
	{
		const string _sourceFile = @"C:\Users\BrianSchau\Desktop\s.pdf";
		const string _outputFile = @"C:\Users\BrianSchau\Desktop\StompAndChomp.pdf";

		public void Execute()
		{
			var pdf = File.ReadAllBytes(_sourceFile);
			pdf = Magic(pdf);
			File.WriteAllBytes(_outputFile, pdf);
		}

		byte[] Magic(byte[] pdf)
		{
			using (var reader = new PdfReader(pdf))
			{
				// reader.Permissions = PdfWriter.AllowCopy | PdfWriter.AllowPrinting | PdfWriter.AllowScreenReaders;
				using (var ms = new MemoryStream())
				{
					var stamper = new PdfStamper(reader, ms);

					stamper.FormFlattening = true;
					var fields = stamper.AcroFields.Fields.Select(x => x.Key).ToArray();
					stamper.AcroFields.GenerateAppearances = true;
					for (var key = 0; key < fields.Count(); key++)
					{
						stamper.AcroFields.SetFieldProperty(fields[key], "setfflags", PdfFormField.FF_READ_ONLY, null);
					}

					stamper.Writer.CloseStream = false;
					stamper.Close();
					ms.Flush();
					ms.Position = 0;
					return ms.ToArray();
				}
			}
		}
	}
}
