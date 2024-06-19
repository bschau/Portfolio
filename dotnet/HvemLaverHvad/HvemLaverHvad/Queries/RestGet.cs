using Newtonsoft.Json;
using System.Configuration;
using System.Net;
using System.Text;

namespace HvemLaverHvad.Queries
{
    public class RestGet
    {
        const string _baseUrl = "https://{your-company}.tpondemand.com/api/v1/";
        readonly string _username = ConfigurationManager.AppSettings["TPOnDemandLogin"];
        readonly string _password = ConfigurationManager.AppSettings["TPOnDemandPassword"];

        public T Execute<T>(string url) where T : new()
        {
            if (!url.StartsWith(_baseUrl, System.StringComparison.CurrentCulture))
            {
                url = string.Format("{0}{1}", _baseUrl, url);
            }

            var webClient = new WebClient();
            webClient.Credentials = new NetworkCredential(_username, _password);
            webClient.Encoding = Encoding.UTF8;
            webClient.Headers.Add(HttpRequestHeader.Accept, "application/json");
            var data = webClient.DownloadString(url);
            return JsonConvert.DeserializeObject<T>(data);
        }
    }
}
