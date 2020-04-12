package com.schau;

import java.io.File;
import java.util.Arrays;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Properties;
import java.util.StringTokenizer;
import java.util.Vector;
import org.apache.tools.ant.Task;
import org.apache.tools.ant.Project;
import org.apache.tools.ant.BuildException;
import org.apache.tools.ant.types.FileSet;

/**
 * This is main class for the Aapp task.
 *
 * To include the task in your build.xml do something like:
 *
 * <target name="init" description="Taskdef the Aapp task">
 *     <taskdef name="aapp"
 *              classname="com.schau.Aapp"
 *              classpath="jar_path" />
 * </target>
 *
 * If you copy Aapp.jar to the lib directory of ant you can omit the classpath
 * attribute.
 *
 * <target name="use" description="Use" depends="init">
 *    <aapp showversion="true"   <!-- show version information on execute -->
 *          srcdir="src"         <!-- source directory -->
 *          dstdir="final"       <!-- where to store the processed files -->
 *          includes="."         <!-- includes paths -->
 *          ppid="#"             <!-- token start identifier -->
 *          suffixes=".java"     <!-- suffixes of files to consider -->
 *    >
 *        <variable name="var1">var1value</variable>
 *        <variable name="var2">var2value</variable>
 *                                  .
 *                                  .
 *                                  .
 *        <variable name="varX">varXvalue</variable>
 *    </aapp>
 * </target>
 */
public class Aapp extends Task {
	// must match version in common.xml!
	private static final String VERSION="1.4";

	/**
	 * Vector which holds the variables from the "command line".
	 *
	 * Use <variable name="varname">value</variable> to define a variable
	 * in the build.xml file.
	 */
	protected Vector<Variable> variables=new Vector<Variable>();

	/**
	 * List of files to be processed.
	 */
	protected ArrayList<File> files=new ArrayList<File>();

	/**
	 * Local scope variables.
	 */
	protected HashMap<String,StkVar> vars=new HashMap<String,StkVar>();

	/**
	 * Vector which holds the suffixes of the files to consider.
	 */
	protected Vector<String> suffixes=new Vector<String>();

	/**
	 * Includes file paths.
	 */
	protected ArrayList<String> includes=new ArrayList<String>();

	/**
	 * Where to store the processed files.
	 */
	protected String dstDir="final";
	
	/**
	 * Source file directory.
	 */
	protected File srcDir=new File("src");

	/**
	 * Token start identifier. When the preprocessor sees one of these
	 * it will attempt to parse and evaluate the next token.
	 */
	protected String ppid="#";

	/**
	 * Show version information when task is invoked.
	 */
	protected boolean showVersion=true;

	/**
	 * Current directory.
	 */
	protected String curDir=null;

	/**
	 * Current file.
	 */
	protected String curFile;
	
	/**
	 * Current parent directory.
	 */
	protected String curParent;

	/**
	 * Constructor.
	 */
	public Aapp() {
		includes.add(".");
		suffixes.add(".java");
	}

	/**
	 * Create the source directory class for nested elements.
	 *
	 * @return the variable.
	 */
	public Variable createVariable() {
		Variable var=new Variable();

		variables.add(var);
		return var;
	}

	/**
	 * Set the showVersion variable.
	 *
	 * @param b true/false.
	 */
	public void setShowversion(String b) {
		if ((b==null) || (b.length()==0)) {
			return;
		}

		if ((b.compareToIgnoreCase("true")==0) ||
		    (b.compareToIgnoreCase("on")==0) ||
		    (b.compareToIgnoreCase("1")==0) ||
		    (b.compareToIgnoreCase("yes")==0) ||
		    (b.compareToIgnoreCase("aye, mate!")==0)) {
			showVersion=true;
		    }
	}

	/**
	 * Setters (setSrcdir, setDstdir, setPpid).
	 */
	public void setSrcdir(File s) { srcDir=s; }
	public void setDstdir(String d) { dstDir=d; }
	public void setPpid(String p) { ppid=p; }

	/**
	 * Set suffixes.
	 *
	 * @param s suffixes.
	 */
	public void setSuffixes(String s) {
		if ((s==null) || (s.length()==0)) {
			return;
		}

		StringTokenizer st=new StringTokenizer(s, "|", false);
		String p;

		suffixes.clear();

		while (st.hasMoreTokens()) {
			p=st.nextToken().trim();
			if (p.length()>0) {
				if (p.startsWith(".")) {
					suffixes.add(p);
				} else {
					suffixes.add("."+p);
				}
			}
		}
	}

	/**
	 * Set Include path.
	 *
	 * @param s includes.
	 */
	public void setIncludes(String s) {
		if ((s==null) || (s.length()==0)) {
			return;
		}

		StringTokenizer st=new StringTokenizer(s, File.pathSeparator, false);
		String p;

		includes.clear();

		while (st.hasMoreTokens()) {
			p=st.nextToken().trim();
			if (p.length()>0) {
				includes.add(p);
			}
		}
	}

	/**
	 * Verify and setup destination folder.
	 *
	 * @exception BuildException.
	 */
	private void verifyDestination() {
		File d=new File(dstDir);

		try {
			if (!d.isDirectory()) {
				if (!d.mkdirs()) {
					throw new Exception("Cannot create destination directory: "+d.getName());
				}
			}
		} catch (Exception ex) {
			throw new BuildException(ex.getMessage());
		}
	}

	/**
	 * Add the "command-line" variables to the local scope.
	 *
	 * @exception BuildException.
	 */
	public void importVariables() throws BuildException {
		int i;

		for (Iterator it=variables.iterator(); it.hasNext(); ) {
			Variable v=(Variable)it.next();

			if (vars.get(v.getName())==null) {
				try {
					i=Integer.parseInt(v.getValue());
					vars.put(v.getName(), new StkVar(i));
				} catch (NumberFormatException n) {
					vars.put(v.getName(), new StkVar(v.getValue()));
				}
			} else {
				throw new BuildException("Cannot redefine "+v.getName());
			}
		}
	}

	/**
	 * Process the file(s).
	 *
	 * @exception BuildException.
	 */
	public void execute() throws BuildException {
		if (showVersion) {
			syslog("Aapp v"+VERSION+" (http://www.schau.com/)");
		}

		verifyDestination();
		PreProcessor.init(this, ppid);

		curDir=System.getProperty("user.dir");

		try {
			if (srcDir.isDirectory()) {
				files.clear();
				findFiles(srcDir);
				processFiles();
			} else {
				log("Not a directory: "+srcDir.getName());
			}
		} catch (Exception ex) {
			log(ex.getMessage());
			throw new BuildException("Exited with errors");
		}
	}

	/**
	 * Find files to process.
	 *
	 * @param src Source directory.
	 * @exception BuildException.
	 */
	private void findFiles(File src) throws Exception {
		File[] fl=src.listFiles();

		for (int i=0; i<fl.length; i++) {
			curFile=fl[i].getName();
			curParent=fl[i].getParent();
			if (fl[i].isDirectory()) {
				findFiles(fl[i]);
			} else if (fl[i].isFile()) {
				if (shouldProcessFile(curFile)) {
					files.add(fl[i]);
				}
			}
		}
	}

	/**
	 * Process the files.
	 *
	 * @exception Exception
	 */
	private void processFiles() throws Exception {
		int len=srcDir.getPath().length();
		String s;
		File f, p;

		for (int i=0; i<files.size(); i++) {
			f=files.get(i);
			curFile=f.getName();
			curParent=f.getParent();

			s=dstDir+f.getPath().substring(len);

			p=new File((new File(s)).getParent());

			p.mkdirs();
			vars.clear();
			importVariables();
			PreProcessor.process(p, f);
		}
	}

	/**
	 * Should this file be processed (ie. is the suffix of this file in
	 * the suffixes list?).
	 *
	 * @return true if the file should be processed, false otherwise.
	 */
	private boolean shouldProcessFile(String n) {
		for (Iterator it=suffixes.iterator(); it.hasNext(); ) {
			String s=(String)it.next();

			if (n.endsWith(s)) {
				return true;
			}
		}

		return false;
	}

	/**
	 * Log the string. Called by some of the other classes in this package.
	 */
	public void syslog(String msg) {
		log(msg);
	}

	/**
	 * Get a variable from the Ant system.
	 *
	 * @param name Property name.
	 * @return value or null.
	 */
	public String getAntVar(String name) {
		return getProject().getProperty(name);
	}
}
