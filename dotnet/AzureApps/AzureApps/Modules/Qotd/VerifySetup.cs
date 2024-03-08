using System;
using dk.schau.AzureApps.DomainModel;

namespace dk.schau.AzureApps.Modules.Qotd
{
	class VerifySetup
	{
		readonly AppsRc _appsRc;

		internal VerifySetup(AppsRc appsRc)
		{
			_appsRc = appsRc;
		}

		internal void Execute()
		{
			if (string.IsNullOrWhiteSpace(_appsRc.QotdFileUrl))
			{
				throw new Exception("QotdFileUrl not defined");
			}
		}
	}
}