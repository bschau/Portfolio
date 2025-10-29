using jc.DomainModel;
using jc.Queries;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net;
using System.Text;

namespace jc.HttpApi
{
	public class HttpApiBase
	{
		readonly GetVariable _getVariable;
		HttpWebRequest _httpWebRequest;
		protected readonly Log _log;
		protected readonly Configuration _configuration;
		protected readonly DateTime? _ifModifiedSince;


		protected HttpApiBase(Log log, Configuration configuration, DateTime? ifModifiedSince)
		{
			_log = log;
			_configuration = configuration;
			_ifModifiedSince = ifModifiedSince;
			_getVariable = new GetVariable(configuration.Variables);
		}

		protected void HttpApiBaseConnect(string httpMethod, string request)
		{
			var endpoint = GetEndpoint(request);
			_log.WriteLine("{0} {1}", httpMethod, endpoint);
			try
			{
				_httpWebRequest = (HttpWebRequest)WebRequest.Create(endpoint);
				_log.WriteLine("Request headers:");
				var headers = _httpWebRequest.Headers;
				foreach (var header in _configuration.Headers)
				{
					if ("content-type".Equals(header.Key.ToLower()))
					{
						_httpWebRequest.ContentType = header.Value;
						_log.WriteLine("\tContent-Type: {0}", header.Value);
					}
					else
					{
						headers.Add(header.Key, header.Value);
					}
				}

				if (_ifModifiedSince.HasValue)
				{
					_httpWebRequest.IfModifiedSince = _ifModifiedSince.Value;
					_log.WriteLine("\tIf-Modified-Since: {0}", _ifModifiedSince.Value.ToString());
				}

				foreach (string key in headers)
				{
					var value = headers[key];
					_log.WriteLine("\t{0}: {1}", key, value);
				}

				_httpWebRequest.Method = httpMethod.ToUpper();
				_httpWebRequest.Proxy = GetProxy();
			}
			catch (WebException webException)
			{
				throw new Exception(webException.Message);
			}
		}

		string GetEndpoint(string request)
		{
			var endpoint = _getVariable.Execute("endpoint");
			return string.Format("{0}/{1}", endpoint, request);
		}

		IWebProxy GetProxy()
		{
			var proxy = WebRequest.DefaultWebProxy;
			proxy.Credentials = CredentialCache.DefaultCredentials;
			return proxy;
		}

		protected void HttpApiBaseSend(string data)
		{
			_log.Verbose("Sending {0} bytes", data.Length.ToString());
			using (var streamWriter = new StreamWriter(_httpWebRequest.GetRequestStream()))
			{
				streamWriter.Write(data);
			}
		}

		protected string HttpApiBaseReadResponse()
		{
			HttpWebResponse httpResponse = null;

			try
			{
				httpResponse = (HttpWebResponse)_httpWebRequest.GetResponse();
				_log.Verbose("Reading response");
				var stream = httpResponse.GetResponseStream();
				if (stream == null)
				{
					throw new Exception("Connection prematurely closed");
				}

				using (var streamReader = new StreamReader(stream))
				{
					var result = streamReader.ReadToEnd();
					ShowResponseHeaders(httpResponse, result);
					return result;
				}
			}
			catch (WebException webException)
			{
				if (webException.Status == WebExceptionStatus.ProtocolError)
				{
					var httpWebResponse = (HttpWebResponse)webException.Response;
					ShowResponseHeaders(httpWebResponse, webException.Message);
					return GetResponse(webException);
				}

				ShowResponseHeaders(httpResponse, webException.Message);
				return GetResponse(webException);
			}
		}

		void ShowResponseHeaders(HttpWebResponse httpResponse, string response)
		{
			if (httpResponse == null)
			{
				_log.WriteLine("No response ...");
				return;
			}

			_log.WriteLine("Status: {0} ({1})", ((int)httpResponse.StatusCode).ToString(), httpResponse.StatusDescription);
			_log.WriteLine("Response headers:");
			foreach (var headerKey in httpResponse.Headers.AllKeys)
			{
				_log.WriteLine("\t{0}: {1}", headerKey, httpResponse.Headers[headerKey]);
			}

			if (httpResponse.StatusCode != HttpStatusCode.OK)
			{
				_log.WriteLine("Response: {0}", response);
			}
		}

		string GetResponse(WebException webException)
		{
			var stream = webException.Response.GetResponseStream();
			if (stream == null) 
			{
				return null;
			}

			using (var ms = new MemoryStream())
			{
				stream.CopyTo(ms);
				return Encoding.UTF8.GetString(ms.ToArray());
			}
		}
	}
}
