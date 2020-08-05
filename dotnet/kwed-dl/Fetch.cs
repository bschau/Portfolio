using System;
using System.Net.Http;
using System.Threading.Tasks;

namespace kweddl
{
	public class Fetch
	{
		public async Task<byte[]> ExecuteAsync(string url)
		{
			using (var httpClient = new HttpClient())
			{
				httpClient.BaseAddress = new Uri(url);
				httpClient.DefaultRequestHeaders.Accept.Clear();
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
