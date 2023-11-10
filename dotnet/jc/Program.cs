using System;
using System.Collections.Generic;
using System.Globalization;
using System.IO;
using System.Reflection;
using jc.DomainModel;
using jc.Extension;
using jc.HttpApi;
using jc.Loaders;
using Newtonsoft.Json;

namespace jc
{
	class Program
	{
		static void Main(string[] args)
		{
			try
			{
				var configuration = new Configuration();
				var variables = configuration.Variables;
				var now = DateTime.Now.JsonDate();
				variables.Add("now", now);

				var arguments = new Arguments(variables);
				arguments.Parse(args);

				var log = new Log(arguments.Quiet, arguments.Verbose);
				var rcFile = new RcFileLoader(log).Execute(arguments.ConfigFile);
				configuration.Profile = arguments.Profile;
				new ConfigurationMerger(configuration).Merge(rcFile);

				var postData = GetPostData(log, arguments, configuration);
				if (arguments.ParseOnly)
				{
					log.WriteLine("In Parse Only mode - will not send any request");
					log.WriteLine("{0}", postData);
					Environment.Exit(0);
				}

				var httpApi = GetHttpApi(log, arguments, configuration, postData);
				httpApi.Connect();
				httpApi.Send();

				var response = httpApi.ReadResponse();
				if (response != null)
				{
					log.WriteLine("{0}", response);
					if (!string.IsNullOrWhiteSpace(arguments.OutputFile))
					{
						File.WriteAllText(arguments.OutputFile, response);
					}
				}

				Environment.Exit(0);
			}
			catch (Exception exception)
			{
				Console.Error.WriteLine("{0}", exception.Message);
				Environment.Exit(1);
			}
		}

		static string GetPostData(Log log, Arguments arguments, Configuration configuration)
		{
			if (arguments.Request.ToLower().StartsWith("get:"))
			{
				return null;
			}

			var template = new TemplateLoader(log, configuration).Execute(arguments);
			template = new Expander(configuration).Execute(template);
			if (!string.IsNullOrWhiteSpace(arguments.TemplatesFile))
			{
				log.Verbose("Saving templates file to: {0}", arguments.TemplatesFile);
				File.WriteAllText(arguments.TemplatesFile, template);
			}

			return template;
		}

		static IHttpApi GetHttpApi(Log log, Arguments arguments, Configuration configuration, string postData)
		{
			var request = arguments.Request;
			var ifModifiedSince = arguments.IfModifiedSince;
			var httpMethod = request.ToLower();

			if (httpMethod.StartsWith("get:", StringComparison.CurrentCulture))
			{
				return new HttpApiGet(log, configuration, request.Substring(4), ifModifiedSince);
			}

			if (httpMethod.StartsWith("post:", StringComparison.CurrentCulture))
			{
				return new HttpApiPost(log, configuration, request.Substring(5), postData, ifModifiedSince);
			}

			log.Verbose("HTTP method not set, defaulting to POST");
			return new HttpApiPost(log, configuration, request, postData, ifModifiedSince);
		}
	}
}
