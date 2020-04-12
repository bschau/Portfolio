using System;
using System.Net.Http;
using System.Net.Http.Headers;
using System.Threading.Tasks;

namespace VPSApps.Commands
{
	public class Fetcher
	{
		public string Authorization { get; set; }

		public async Task<byte[]> ExecuteAsync(string url)
		{
			using (var httpClient = new HttpClient())
			{
				httpClient.BaseAddress = new Uri(url);
				httpClient.DefaultRequestHeaders.Accept.Clear();
				if (!string.IsNullOrWhiteSpace(Authorization))
				{
					httpClient.DefaultRequestHeaders.Authorization = new AuthenticationHeaderValue("Bearer", Authorization);
				}
				var response = await httpClient.GetAsync(url);
				if (!response.IsSuccessStatusCode)
				{
					throw new Exception($"Error while fetching {url}: {response.StatusCode} {response.ReasonPhrase}");
				}

				return await response.Content.ReadAsByteArrayAsync();
			}
		}
	}
}
