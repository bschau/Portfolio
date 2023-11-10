import java.io.*;
import java.util.*;

/**
 * Variables is the class handling the global variables.
 *
 * @author	Brian Schau
 * @since	0.1
 */
public class Variables {
	private PathManager pMan;

	/**
	 * Manifest information (from Build.properties).
	 */
	public String bpManifest;

	/**
	 * Package name (from Build.properties).
	 */
	public String bpPackage;

	/**
	 * Version information (from Build.properties).
	 */
	public String bpVersion;

	/**
	 * Vendor information (from Build.properties).
	 */
	public String bpVendor;

	/**
	 * SourceDirectory (set in PreProcessor.java).
	 */
	public String gSrcDir;

	/**
	 * Extension of _real_ sourcefiles.
	 */
	public String extension;

	/**
	 * Include properties.
	 */
	public Properties properties;

	/**
	 * True if PreProcessor should be run, false otherwise.
	 */
	public boolean runPreProcessor;

	/**
	 * Expander
	 */
	public Expander exp;

	/**
	 * Evaluate string for trueness.
	 *
	 * @return	True (if string is enable, enabled, on, true or 1).
	 */
	private boolean IsTrue(String s) {
		if (s.compareToIgnoreCase("enable")==0 ||
		    s.compareToIgnoreCase("enabled")==0 ||
		    s.compareToIgnoreCase("on")==0 ||
		    s.compareToIgnoreCase("true")==0 ||
		    s.compareToIgnoreCase("1")==0)
			return true;

		return false;
	}

	/**
	 * Merge the include properties and Build.properties files.
	 */
	private void MergeProperties() {
		FileInputStream in;
		String s, f;
		boolean keepFirst;

		if ((s=System.getProperty("Properties"))==null) {
			if ((s=System.getProperty("PreProcess"))==null)
				runPreProcessor=false;
			else {
				if (IsTrue(s))
					runPreProcessor=true;
				else
					runPreProcessor=false;
			}
			return;
		}

		runPreProcessor=true;

		keepFirst=false;
		if ((f=System.getProperty("KeepFirst"))!=null) {
			if (IsTrue(f))
				keepFirst=true;
		}

		StringTokenizer sToken=new StringTokenizer(s, File.pathSeparator);

		PullInIncludes(sToken, keepFirst);
	}

	/**
	 * Load and merge an include file.
	 *
	 * @param	sToken	String tokenizer object.
	 * @param	keep	Override other definitions (true = yes).
	 */
	private void PullInIncludes(StringTokenizer sToken, boolean keep) {
		FileInputStream in;
		String s;

		while (sToken.hasMoreTokens()) {
			s=sToken.nextToken();

			try {
				String k, v;

				s=exp.Expand(s);
				Properties p=new Properties();
				in=new FileInputStream(new File(pMan.FullPath(s, true)));
				p.load(in);
				in.close();

				for (Enumeration e=p.propertyNames(); e.hasMoreElements(); ) {
					k=(String)e.nextElement();
					v=p.getProperty(k);

					if (k.compareToIgnoreCase("include")==0 && v!=null) {
						StringTokenizer eToken=new StringTokenizer(v, File.pathSeparator);
						PullInIncludes(eToken, keep);
					} else {
						if (keep) {
							if (properties.getProperty(k)!=null)
								continue;
						}

						properties.setProperty(k, v);
					}
				}

				p=null;
			} catch (Exception ex) {
				System.err.println("Failed to load property file "+s);
				System.err.println(ex.getMessage());
				System.exit(1);
			}
		}
	}

	/**
	 * Initialize variables.
	 *
	 * @param	buildFile	Build file to read properties from.
	 * @throws	IOException
	 */
	public void Initialize(String buildFile) throws IOException {
		FileInputStream in;
		String s;
		File f;

		if (buildFile==null) {
			if ((buildFile=System.getProperty("BuildFile"))==null) {
				buildFile="Build.properties";
				f=new File(buildFile);
				if (f.isFile()==false) {
					buildFile="build.properties";
					f=new File(buildFile);
					if (f.isFile()==false) {
						System.err.println("Cannot find Build Properties file");
						System.exit(1);
					}
				}
			}
		}

		properties=new Properties();

		try {
			in=new FileInputStream(new File(buildFile));
			properties.load(in);
			in.close();
		} catch (Exception ex) {
			System.err.println("Cannot load Build properties");
			System.err.println(ex.getMessage());
			System.exit(1);
		}

		pMan=new PathManager();
		pMan.SetPaths(properties, buildFile);

		exp=new Expander(properties, pMan);
		MergeProperties();

		bpPackage=GetProperty("Package");

		if ((bpVendor=GetProperty("Vendor"))==null)
			bpVendor="Unknown";

		if ((bpVersion=GetProperty("Version"))==null)
			bpVersion="1.0";

		if ((extension=System.getProperty("Extension"))==null) {
			if ((extension=GetProperty("Extension"))==null)
				extension=".java";
		}

		pMan.PackagePaths(properties, bpPackage);
	}

	/**
	 * Get Path Manager.
	 *
	 * @return 	Path Manager object.
	 */
	public PathManager GetPathManager() {
		return pMan;
	}

	/**
	 * Get Expander.
	 *
	 * @return	Expander object.
	 */
	public Expander GetExpander() {
		return exp;
	}

	/**
	 * Get a property from the properties file.
	 *
	 * @param	key	Key to look up.
	 * @return	Property (or null if not found).
	 */
	public String GetProperty(String key) {
		return exp.Expand(properties.getProperty(key));
	}

	/**
	 * Qualify class name (if Package name is given).
	 *
	 * @param	inN	In path.
	 * @return	Class name (classname or package.classname).
	 */
	public String GetPackagedName(String inN) {
		if (bpPackage==null)
			return inN;
	
		return bpPackage+"."+inN;
	}
}
