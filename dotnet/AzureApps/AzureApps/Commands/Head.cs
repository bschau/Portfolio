using System;
using System.Collections.Generic;
using System.Net.Http;
using System.Threading.Tasks;

namespace dk.schau.AzureApps.Commands
{
	public class Head
	{
		public DateTime IfModifiedSince { get; set; }

		public async Task<Dictionary<string, string>> Execute(string url)
		{
			var responseHeaders = new Dictionary<string, string>();

			using (var client = new HttpClient()) 
			{
				client.BaseAddress = new Uri(url);
				SetHeaders(client);

				var message = new HttpRequestMessage(HttpMethod.Head, url);
				var response = await client.SendAsync(message);
				var enumerator = response.Headers.GetEnumerator();
				while (enumerator.MoveNext())
				{
					var header = enumerator.Current;
					responseHeaders.Add(header.Key, header.Value.ToString());
				}
				responseHeaders.Add("_StatusCode", response.StatusCode.ToString());
				responseHeaders.Add("_StatusText", response.ReasonPhrase);

				return responseHeaders;
			}
		}

		void SetHeaders(HttpClient client)
		{
			client.DefaultRequestHeaders.Accept.Clear();
			if (IfModifiedSince != null)
			{
				client.DefaultRequestHeaders.IfModifiedSince = IfModifiedSince;
			}
		}
	}
}