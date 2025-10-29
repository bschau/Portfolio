namespace dk.schau.AzureApps.Modules.Comics.DomainModel
{
	public interface IComicsDriver
	{
		string Execute(string url, string name);
	}
}