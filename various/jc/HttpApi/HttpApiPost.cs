using jc.DomainModel;
using System;
using System.Collections.Generic;
using System.IO;
using System.Text;

namespace jc.HttpApi
{
	public class HttpApiPost : HttpApiBase, IHttpApi
	{
		readonly string _request;
		readonly string _postData;

		public HttpApiPost(Log log, Configuration configuration, string request, string postData, DateTime? ifModifiedSince)
			: base(log, configuration, ifModifiedSince)
		{
			_request = request;
			_postData = postData;
		}

		public void Connect()
		{
			HttpApiBaseConnect("POST", _request);
		}

		public void Send()
		{
			HttpApiBaseSend(_postData);
		}

		public string ReadResponse()
		{
			return HttpApiBaseReadResponse();
		}
	}
}
