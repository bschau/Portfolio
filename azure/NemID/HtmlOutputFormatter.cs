using Microsoft.AspNetCore.Mvc.Formatters;

public class HtmlOutputFormatter : StringOutputFormatter
{
	public HtmlOutputFormatter()
	{
		SupportedMediaTypes.Add("text/html");
	}
}