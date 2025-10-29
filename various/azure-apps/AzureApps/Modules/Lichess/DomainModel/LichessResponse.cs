using System.Collections.Generic;
using Newtonsoft.Json;

namespace dk.schau.AzureApps.Modules.Lichess.DomainModel
{
	class LichessResponse
	{
		[JsonProperty("nowPlaying")]
		public List<LichessGame> NowPlaying { get; set; }
	}
}