using System;
using System.Linq;
using System.Reflection;
using dk.schau.AzureApps.Commands;
using dk.schau.AzureApps.Modules.Comics.DomainModel;

namespace dk.schau.AzureApps.Modules.Comics
{
	internal class GetComicsDriver
	{
		internal IComicsDriver Execute(string driverName)
		{
			var type = typeof(IComicsDriver);
			var types = Assembly.GetExecutingAssembly()
						.GetTypes()
						.Where(x => type.IsAssignableFrom(x) &&
								x.Name != "IComicsDriver");

			var modules = types.Where(x =>
										driverName.Equals(x.Name, StringComparison.CurrentCultureIgnoreCase))
								.ToList();

			if (modules.Count > 1)
			{
				throw new Exception($"ComicsDriver '{driverName}' defined twice - flog the programmer!");
			}

			if (modules.Count < 1)
			{
				throw new Exception($"ComicsDriver '{driverName}' not found");
			}

			var iComicsDriver = Activator.CreateInstance(modules.First(), new object[] {
				new Fetcher()
			}) as IComicsDriver;
			if (iComicsDriver == null)
			{
				throw new Exception($"Cannot activate '{driverName}'");
			}

			return iComicsDriver;
		}
	}
}