using Newtonsoft.Json;

namespace dk.schau.AzureApps.Modules.Lichess.DomainModel
{
	class LichessOpponent
	{
		[JsonProperty("username")]
		public string Username { get; set; }
	}
}