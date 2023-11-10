import java.io.*;
import java.util.*;
/**
 * Compiler is the class handling the actual compiling and preverification
 * of the sources in the midlet suite.
 * Basic checks are done with regards to file locations.
 * Currently, the compiler to use has been hardcoded as the well as the
 * arguments - this is expected to change in the future.
 * Preverifying is somewhat configurable - this will also change in the
 * forthcoming releases.
 *
 * @author	Brian Schau
 * @since	0.1
 */
public class Compiler {
	private String midlets, classPath;
	private Variables v;
	private PathManager pMan;

	/**
	 * Set up Compiler
	 *
	 * @param	vars	Variables block.
	 * @param	m	Midlets to compile separated by whitespace.
	 */
	public Compiler(Variables vars, String m) {
		v=vars;
		midlets=m;
		pMan=v.GetPathManager();
	}

	/**
	 * Get classpath from environment.
	 */
	private void SetClassPath() {
		if ((classPath=v.GetProperty("ClassPath"))==null) {
			System.err.println("Cannot find ClassPath");
			System.exit(1);
		}
	}

	/**
	 * Perform compilation. 
	 */
	public void Compile() {
		StringTokenizer sToken=new StringTokenizer(midlets, " \t");

		SetClassPath();

		while (sToken.hasMoreTokens())
			CompileMidlet(sToken.nextToken());

		PreVerifyMidlets();
	}

	/**
	 * Compile a midlet.
	 *
	 * @param	midlet	Midlet to compile.
	 */
	private void CompileMidlet(String midlet) {
		String cmd="javac ", sources=v.GetProperty(midlet+"Sources"), s;
		StringTokenizer sToken;

		if ((s=v.GetProperty(midlet+"JFlags"))!=null)
			cmd+=s+" ";

		cmd+="-bootclasspath "+classPath+" -d "+pMan.QuotedPath(pMan.pmTmpDir);

		sToken=new StringTokenizer(sources, " \t");
		while (sToken.hasMoreTokens()) 
			cmd+=" "+pMan.QuotedPath(v.gSrcDir+File.separator+sToken.nextToken()+".java");

		ProcessManager pp=new ProcessManager();
		pp.RunCommand(cmd);
	}

	/**
	 * Preverify midlets.
	 */
	private void PreVerifyMidlets() {
		String cmd, s;
		File f;
		int idx;
		File[] sources;

		if ((cmd=v.GetProperty("Preverify"))==null)
			return;

		cmd+=" ";

		if ((s=v.GetProperty("PreverifyFlags"))!=null)
			cmd+=s+" ";

		cmd+=" -classpath "+classPath+File.pathSeparator+pMan.QuotedPath(pMan.pmTmpDir);
		cmd+=" -d "+pMan.QuotedPath(pMan.pmTmpDir);

		f=new File(pMan.pmPkgDir);
		FileFilter ff=new FileFilter() {
			public boolean accept(File file) {
				return file.getName().endsWith(".class");
			}
		};

		if ((sources=f.listFiles(ff))==null) {
			System.err.println("No class files found?!?!");
			System.exit(1);
		}

		for (int i=0; i<sources.length; i++) {
			s=sources[i].getName();
			idx=s.lastIndexOf(".class");
			cmd+=" "+v.GetPackagedName(s.substring(0, idx));
		}

		ProcessManager pp=new ProcessManager();
		pp.RunCommand(cmd);
	}
}
