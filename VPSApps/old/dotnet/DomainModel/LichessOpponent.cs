using Newtonsoft.Json;

namespace VPSApps.DomainModel
{
	class LichessOpponent
	{
		[JsonProperty("username")]
		public string Username { get; set; }
	}
}