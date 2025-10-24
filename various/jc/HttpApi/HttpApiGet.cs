using jc.DomainModel;
using System;

namespace jc.HttpApi
{
	public class HttpApiGet : HttpApiBase, IHttpApi
	{
		readonly string _request;

		public HttpApiGet(Log log, Configuration configuration, string request, DateTime? ifModifiedSince)
			: base(log, configuration, ifModifiedSince)
		{
			_request = request;
		}

		public void Connect()
		{
			HttpApiBaseConnect("GET", _request);
		}

		public void Send()
		{
		}

		public string ReadResponse()
		{
			return HttpApiBaseReadResponse();
		}
	}
}
