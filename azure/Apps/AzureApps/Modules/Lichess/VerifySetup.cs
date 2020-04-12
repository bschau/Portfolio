using System;
using dk.schau.AzureApps.DomainModel;

namespace dk.schau.AzureApps.Modules.Lichess
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
			if (string.IsNullOrWhiteSpace(_appsRc.LichessToken))
			{
				throw new Exception("LichessToken not defined");
			}
		}
	}
}