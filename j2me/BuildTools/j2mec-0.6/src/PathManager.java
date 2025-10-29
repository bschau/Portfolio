import java.io.*;
import java.util.*;

/**
 * PathManager is the class handling directory path and file information.
 * If paths to objects are not absolute, the PathManager tries to resolve
 * the paths relative to the Base directory.
 *
 * @author	Brian Schau
 * @since	0.1
 */
public class PathManager {
	/**
	 * Base Directory
	 */
	public String pmBaseDir;

	/**
	 * Source Directory (probably relative to pmBaseDir).
	 */
	public String pmSrcDir;

	/**
	 * Temporary Directory (probably relative to pmBaseDir).
	 */
	public String pmTmpDir;

	/**
	 * Temporary Directory with Package (same as pmTmpDir if no package).
	 */
	public String pmPkgDir;

	/**
	 * Contains the line terminator sequence on this platform.
	 */
	public String lineSeparator;

	/**
	 * Constructor
	 */
	public PathManager() {
		if ((lineSeparator=System.getProperty("line.separator"))==null)
			lineSeparator="\n";
	}

	/**
	 * Resolve path - see if object exists.
	 *
	 * @param	p	Path.
	 * @param	isFile	Path points to a file if true.
	 */
	public String FullPath(String p, boolean isFile) {
		File f;
		String fullPath=null, type;

		f=new File(p);
		if (f.getParent()==null)
			f=new File(pmBaseDir+File.separator+p);

		try {
			fullPath=f.getCanonicalPath();
		} catch (Exception ex) {
			System.err.println("Cannot get canonical path of "+p);
			System.err.println(ex.getMessage());
			System.exit(1);
		}

		f=new File(fullPath);
		if (isFile) {
			if (f.isFile()==false) {
				System.err.println("The file pointed to by "+p+" does not exist");
				System.exit(1);
			}
		} else {
			if (f.isDirectory()==false) {
				System.err.println("The directory pointed to by "+p+" does not exist");
				System.exit(1);
			}
		}

		return fullPath;
	}

	/**
	 * Resolve and set some often used paths.
	 *
	 * @param	p	Properties.
	 * @param	buildFile	The supplied Build.properties file.
	 */
	public void SetPaths(Properties p, String buildFile) {
		String s;
		File f;

		f=new File(buildFile);
		try {
			pmBaseDir=f.getCanonicalPath();
		} catch (Exception ex) {
			System.err.println("Error while getting canonical path of "+buildFile);
			System.err.println(ex.getMessage());
			System.exit(1);
		}

		f=new File(pmBaseDir);
		pmBaseDir=f.getParent();

		p.setProperty("BaseDirectory", pmBaseDir);
	}

	/**
	 * Resolve paths according to Package name (if supplied).
	 *
	 * @param	p	Properties.
	 * @param	pkg	Package.
	 */
	public void PackagePaths(Properties p, String pkg) {
		String s;

		if ((s=p.getProperty("SourceDirectory"))==null)
			s="src";
	
		if (pkg==null) 
			pmSrcDir=FullPath(s, false);
		else
			pmSrcDir=FullPath(s+File.separator+pkg.replace('.', '/'), false);

		if ((s=p.getProperty("TemporaryDirectory"))==null)
			s="tmp";

		pmTmpDir=FullPath(s, false);

		if (pkg==null)
			pmPkgDir=pmTmpDir;
		else
			pmPkgDir=pmTmpDir+File.separator+pkg.replace('.', '/');
	}

	/**
	 * Quote a path if it contains whitespaces.
	 *
	 * @param	inP	In path.
	 * @return	New path.
	 */
	public String QuotedPath(String inP) {
		if (inP.indexOf(' ')!=-1)
			return "\""+inP+"\"";

		if (inP.indexOf('\t')!=-1)
			return "\""+inP+"\"";

		return inP;
	}
}
