using Core;

namespace Nant.Builders
{
	public class TrelloServiceBuilder
	{
		private readonly Settings _settings;

		public TrelloServiceBuilder(Settings settings)
		{
			_settings = settings;
		}

		public TrelloService Build()
		{
			return new TrelloService(_settings.ApiKey, _settings.Token);
		}
	}
}
