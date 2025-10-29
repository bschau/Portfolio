using System.Collections.Generic;

namespace jc.HttpApi
{
	public interface IHttpApi
	{
		void Connect();
		void Send();
		string ReadResponse();
	}
}
