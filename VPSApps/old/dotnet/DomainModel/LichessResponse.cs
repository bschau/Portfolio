using System.Collections.Generic;
using Newtonsoft.Json;

namespace VPSApps.DomainModel
{
	class LichessResponse
	{
		[JsonProperty("nowPlaying")]
		public List<LichessGame> NowPlaying { get; set; }
	}
}