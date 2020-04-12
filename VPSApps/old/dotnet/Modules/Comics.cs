using System;
using System.Linq;
using System.Reflection;
using VPSApps.Commands;
using VPSApps.DomainModel;

namespace VPSApps.Modules
{
	class Comics : IModule
	{
		const string _moduleName = "Comics";
		readonly VpsAppsRc _vpsAppsRc;

		public Comics(VpsAppsRc vpsAppsRc)
		{
			_vpsAppsRc = vpsAppsRc;
		}

		public void Execute()
		{
			VerifySetup();

			var htmlBuilder = new HtmlBuilder(_moduleName);
			foreach (var comicsItem in _vpsAppsRc.Comics)
			{
				var comicsDriver = GetComicsDriver(comicsItem.Driver);

				htmlBuilder.AppendFormat("<h1><a href=\"{0}\">{1}</a></h1>", comicsItem.Url, comicsItem.Name);
				htmlBuilder.Append(comicsDriver.Execute(comicsItem.Url, comicsItem.Name));
				htmlBuilder.AppendLine();
			}

			var sendEmail = new SendEmail(_vpsAppsRc.From, _vpsAppsRc.To);
			sendEmail.Execute(htmlBuilder.Title, htmlBuilder.ToString());
		}

		void VerifySetup()
		{
			if (_vpsAppsRc.Comics == null || !_vpsAppsRc.Comics.Any())
			{
				throw new Exception($"No Comics defined in {Configuration.Filename}");
			}
		}

		IComicsDriver GetComicsDriver(string driver)
		{
			var type = typeof(IComicsDriver);
			var types = Assembly.GetExecutingAssembly()
						.GetTypes()
						.Where(x => type.IsAssignableFrom(x) &&
								x.Name != "IComicsDriver");

			var modules = types.Where(x =>
										driver.Equals(x.Name, StringComparison.CurrentCultureIgnoreCase))
								.ToList();

			if (modules.Count > 1)
			{
				throw new Exception($"ComicsDriver '{driver}' defined twice - flog the programmer!");
			}

			if (modules.Count < 1)
			{
				throw new Exception($"ComicsDriver '{driver}' not found");
			}

			var iComicsDriver = Activator.CreateInstance(modules.First(), new object[] {
				new Fetcher()
			}) as IComicsDriver;
			if (iComicsDriver == null)
			{
				throw new Exception($"Cannot activate '{driver}'");
			}

			return iComicsDriver;
		}
	}
}