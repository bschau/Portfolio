using System;
using System.IO;
using System.Text.RegularExpressions;

namespace pfoc
{
	/// <summary>
	/// Configuration class. Handles load/save of configuration file.
	/// </summary>
	public class Config
	{
		public string dbPath=null, outputPath=null, palmProfile=null;
        public bool queue = false;
		private string cFile;

		public Config()
		{
			cFile=Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData)+Path.DirectorySeparatorChar+"pfoc.ini";
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
						string line;
						string s;
						Match m;

						while ((line=r.ReadLine())!=null) 
						{
							m=exp.Match(line);
							if (m.Success) 
							{
								s=m.Groups["key"].ToString();

                                if (s == "DatabasePath")
                                    dbPath = m.Groups["val"].ToString();
                                else if (s == "OutputPath")
                                    outputPath = m.Groups["val"].ToString();
                                else if (s == "PalmProfile")
                                    palmProfile = m.Groups["val"].ToString();
                                else if (s == "Queue")
                                {
                                    s = m.Groups["val"].ToString();

                                    if (s.CompareTo("true") == 0)
                                        queue = true;
                                    else
                                        queue = false;
                                } 
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
					w.WriteLine("DatabasePath={0}", dbPath);
                    w.WriteLine("OutputPath={0}", outputPath);
					w.WriteLine("PalmProfile={0}", palmProfile);
                    w.WriteLine("Queue={0}", queue == true ? "true" : "false");
				}
			}
			catch {}
		}					  
	}
}