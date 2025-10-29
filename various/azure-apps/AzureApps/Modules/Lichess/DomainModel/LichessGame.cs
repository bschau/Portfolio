using Newtonsoft.Json;

namespace dk.schau.AzureApps.Modules.Lichess.DomainModel
{
	class LichessGame
	{
		[JsonProperty("fullId")]
		public string FullId { get; set; }

		[JsonProperty("opponent")]
		public LichessOpponent Opponent { get; set; }

		[JsonProperty("isMyTurn")]
		public bool IsMyTurn { get; set; }
	}
}