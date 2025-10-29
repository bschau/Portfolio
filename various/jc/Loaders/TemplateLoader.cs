using System;
using System.IO;
using System.Linq;
using System.Text;
using jc.DomainModel;

namespace jc
{
	internal class TemplateLoader
	{
		readonly Log _log;
		readonly Configuration _configuration;

		internal TemplateLoader(Log log, Configuration configuration)
		{
			_log = log;
			_configuration = configuration;
		}

		internal string Execute(Arguments arguments)
		{
			var fileType = GetExtension(arguments);
			var path = ResolvePath(arguments, fileType);
			_log.Verbose("Request file: {0}", path);

			AddContentType(fileType);

			return File.ReadAllText(path);
		}

		string GetExtension(Arguments arguments)
		{
			if (arguments.Xml)
			{
				return "xml";
			}

			var type = _configuration.Headers.Keys.SingleOrDefault(
				x => "content-type".Equals(x, StringComparison.CurrentCultureIgnoreCase)
			);

			if (type == null)
			{
				return "json";
			}

			if ("application/json".Equals(type))
			{
				return "json";
			}

			if ("application/xml".Equals(type) || "text/xml".Equals(type))
			{
				return "xml";
			}

			throw new ArgumentException($"Don't know how to handle Content-Type: {type}");
		}

		string ResolvePath(Arguments arguments, string fileType)
		{
			const string templatesKey = "templates";
			var builder = new StringBuilder();

			if (_configuration.Variables.ContainsKey(templatesKey))
			{
				builder.Append(_configuration.Variables[templatesKey]);
				builder.Append(Path.DirectorySeparatorChar);
			}

			var request = GetRequestPath(arguments.Request);
			builder.Append(request);
			if (!string.IsNullOrWhiteSpace(arguments.PostData))
			{
				builder.AppendFormat("-{0}", arguments.PostData);
			}

			builder.AppendFormat(".{0}", fileType);
			return builder.ToString();
		}

		string GetRequestPath(string request)
		{
			var parameterStart = request.IndexOf('?');
			if (parameterStart > -1)
			{
				request = request.Substring(0, parameterStart - 1);
			}

			request = request.Trim();
			while (!string.IsNullOrWhiteSpace(request))
			{
				if (!request.EndsWith('/'))
				{
					break;
				}

				request = request.Substring(0, request.Length - 1);
			}

			return request;
		}

		void AddContentType(string fileType)
		{
			var contentType = "xml".Equals(fileType, StringComparison.CurrentCultureIgnoreCase) ?
					"application/xml" : "application/json";

			_log.Verbose("Setting Content-Type to: {0}", contentType);
			var dictionary = _configuration.Headers;
			var key = "Content-Type";
			var testKey = dictionary.Keys.SingleOrDefault(x => key.Equals(x, StringComparison.CurrentCultureIgnoreCase));
			if (testKey == null)
			{
				dictionary.Add(key, contentType);
				return;
			}

			dictionary[testKey] = contentType;
		}
	}
}