import java.io.*;
import java.util.*;

/**
 * Expander is the class handling expansions of strings.	It recognizes
 * ${X} and ${=X} sequences.	Detection of sequences are not fast at the
 * moment.
 *
 * @author	Brian Schau
 * @since	0.1
 */
public class Expander {
	private Properties properties;
	private PathManager pMan;
	private StringBuffer sBuf;

	/**
	 * Set up Expander
	 *
	 * @param	p	Properties to retrieve macros from.
	 * @param	m	Path Manager.
	 */
	public Expander(Properties p, PathManager m) {
		properties=p;
		pMan=m;
		sBuf=new StringBuffer(2048);
	}

	/**
	 * Find and expand any macros from the string.
	 *
	 * @param	in	String to expand.
	 * @return	Possibly expanded string.
	 */
	public String Expand(String in) {
		String v;
		int begin, end;

		if (in==null)
			return null;

		if ((begin=in.lastIndexOf("${"))==-1)
			return in;

		if ((end=in.indexOf("}", begin+2))==-1)
			return in;

		if (in.charAt(begin+2)=='=')
			v=ExpandCommand(in.substring(begin+3, end));
		else if (in.charAt(begin+2)=='@')
			v=ExpandFile(in.substring(begin+3, end));
		else if (in.charAt(begin+2)=='$')
			v=ExpandSysVariable(in.substring(begin+3, end));
		else
			v=ExpandVariable(in.substring(begin+2, end));

		if (v==null)
			return null;

		return Expand(in.substring(0, begin)+v+in.substring(end+1));
	}

	/**
	 * Handle ${X} sequence.
	 */
	private String ExpandVariable(String key) {
		String v;

		if ((v=properties.getProperty(key))==null) {
			System.err.println("Unknown property, "+key);
			return null;
		}

		return v;
	}

	/**
	 * Handle ${@X} sequence.
	 */
	private String ExpandFile(String file) {
		String s;

		sBuf.setLength(0);

		try {
			BufferedReader r=new BufferedReader(new FileReader(pMan.FullPath(file, true)));
			String term="";

			while ((s=r.readLine())!=null) {
				sBuf.append(term).append(s);
				term=pMan.lineSeparator;
			}

			r.close();
		} catch (Exception ex) {
			System.err.println("Cannot read file "+file);
			System.exit(1);
		}

		return sBuf.toString();
	}

	/**
	 * Handle ${=X} sequence.
	 */
	private String ExpandCommand(String cmd) {
		ProcessManager pp=new ProcessManager();
		String v;
		int retCode;

		if ((retCode=pp.Run(cmd))!=0) {
			System.err.println("Command returned "+retCode);
			System.err.println("Output from command: "+pp.Messages());
			return null;
		}

		v=pp.Messages();
		if (v==null || v.length()==0) {
			System.err.println("No output from command, "+cmd);
			return null;
		}

		return v;
	}

	/**
	 * Handle ${$X} sequence.
	 */
	private String ExpandSysVariable(String key) {
		String v;

		if ((v=System.getProperty(key))==null) {
			System.err.println("Unknown property, "+key);
			return null;
		}

		return v;
	}
}
