using dk.schau.AzureApps.DomainModel;
using Microsoft.Azure.WebJobs;
using Microsoft.Extensions.Configuration;

namespace dk.schau.AzureApps.Queries
{
	public class GetConfiguration
	{
		readonly ExecutionContext _context;

		public GetConfiguration(ExecutionContext context)
		{
			_context = context;
		}

		public AppsRc Execute()
		{
			var config = new ConfigurationBuilder()
						.SetBasePath(_context.FunctionAppDirectory)
						.AddJsonFile("local.settings.json", optional: true, reloadOnChange: true)
						.AddJsonFile("secret.settings.json", optional: true, reloadOnChange: true)
						.AddEnvironmentVariables()
						.Build();

			var appsRc = new AppsRc();
			config.Bind(appsRc);
			return appsRc;
/*
			var myString = config["MyCustomStringSetting"];
			var myNumber = config.GetValue<int>("MyCustomNumberSetting");
			var mailSettings = new MailSettings();
			config.Bind("MailSettings", mailSettings);
 */
/*
			log.LogInformation($"MyCustomStringSetting: {myString}");
			log.LogInformation($"MyCustomNumberSetting: {myNumber}");
			log.LogInformation($"MailSettings: {JsonConvert.SerializeObject(mailSettings)}");
			*/
		}
	}
}