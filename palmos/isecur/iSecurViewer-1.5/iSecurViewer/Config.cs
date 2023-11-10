using System;
using System.IO;
using System.Text.RegularExpressions;

namespace iSecurViewer
{
	/// <summary>
	/// Configuration class. Handles load/save of configuration file.
	/// </summary>
	public class Config
	{
		public string currentDB="", categoryName="", profile="";
		public int showIdx=0;
		private string cFile;

		public Config()
		{
			cFile=Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData)+Path.DirectorySeparatorChar+"iSecurViewer.ini";
		}

		/// <summary>
		/// Loads the configuration file.
		/// </summary>
		public void Load()
		{
			if (File.Exists(cFile)) 
			{
				try 
				{
					using (StreamReader r=new StreamReader(cFile)) 
					{
						Regex exp=new Regex(@"(\s*)(?<key>(\w+))(\s*)=((\s*)(?<val>(.+))(\s*))*");
						String line;
						string s;
						Match m;

						while ((line=r.ReadLine())!=null) 
						{
							m=exp.Match(line);
							if (m.Success) 
							{
								s=m.Groups["key"].ToString();

								if (s == "Database")
									currentDB = m.Groups["val"].ToString();
								else if (s == "ShowIdx")
									showIdx = int.Parse(m.Groups["val"].ToString());
								else if (s == "CategoryName")
									categoryName = m.Groups["val"].ToString();
								else if (s == "Profile")
									profile = m.Groups["val"].ToString();
								else
									Console.Error.WriteLine("Unknown configuration key: " + m.Groups["key"]);
							}
						}
					}
				}
				catch (Exception ex) 
				{
					Console.WriteLine(ex.TargetSite+": "+ex.Message);
				}
			}
		}

		/// <summary>
		/// Saves the configuration file.
		/// </summary>
		public void Save()
		{
			try 
			{
				using (StreamWriter w=new StreamWriter(cFile))
				{
					w.WriteLine("Database={0}", currentDB);
					w.WriteLine("ShowIdx={0}", showIdx);
					w.WriteLine("CategoryName={0}", categoryName);
					if (Environment.OSVersion.ToString().IndexOf("Windows")>-1)
						w.WriteLine("Profile={0}", profile);
				}
			}
			catch {}
		}					  
	}
}
