import java.io.*;
import java.util.*;

/**
 * PreProcessor is the class handling the pre-processing stage.	Pre-processing
 * directives is one of ${X}, ${=X} or ${@X}.
 *
 * @author	Brian Schau
 * @since	0.1
 */
class PreProcessor {
	private String midlets;
	private Variables v;
	private PathManager pMan;
	private Properties p;
	private Expander exp;
	private boolean inComment;

	/**
	 * Set up PreProcessor.
	 *
	 * @param	vars	Variables block.
	 * @param	m	Midlets to compile separated by whitespace.
	 */
	public PreProcessor(Variables vars, String m) {
		v=vars;
		midlets=m;

		pMan=v.GetPathManager();
	}

	/**
	 * Pre-process midlet source files.
	 */
	public void PreProcess() {
		if (v.runPreProcessor==false) {
			v.gSrcDir=pMan.pmSrcDir;
			return;
		}

		StringTokenizer sToken=new StringTokenizer(midlets, " \t");
		exp=v.GetExpander();
		while (sToken.hasMoreTokens()) 
			PreProcessMidlet(sToken.nextToken());

		v.gSrcDir=pMan.pmPkgDir;
	}

	/**
	 * Pre-process this midlets source files.
	 */
	private void PreProcessMidlet(String midlet) {
		String sources=v.GetProperty(midlet+"Sources");
		StringTokenizer sToken=new StringTokenizer(sources, " \t");

		while (sToken.hasMoreTokens())
			PreProcessFile(sToken.nextToken());
	}

	/**
	 * Pre-process a source file.
	 *
	 * @param	file	Source file to preprocess.	Input is taken
	 * from the SourceDirectory, output is written to the
	 * TemporaryDirectory.
	 */
	private void PreProcessFile(String file) {
		StringBuffer buf=new StringBuffer();
		boolean inComment=false;
		BufferedReader r;
		BufferedWriter w;
		String s;

		try {
			r=new BufferedReader(new FileReader(pMan.pmSrcDir+File.separator+file+v.extension));

			while ((s=r.readLine())!=null) {
				buf.append(s);
				buf.append('\n');
			}

			r.close();
			PreProcessBuffer(file+v.extension, buf);

			w=new BufferedWriter(new FileWriter(pMan.pmPkgDir+File.separator+file+".java"));
			w.write(buf.toString(), 0, buf.length());
			w.close();
		} catch (Exception ex) {
			System.err.println("Read error");
			System.err.println(ex.getMessage());
			System.exit(1);
		}
	}

	/**
	 * Swallow comments and expand tokens.
	 *
	 * @param	name	File name.
	 * @param	inBuf	String buffer to preprocess.
	 */
	private void PreProcessBuffer(String name, StringBuffer inBuf) {
		StringBuffer outBuf=new StringBuffer();
		int bufLen=inBuf.length(), ptr=0;
		boolean swallow=false, inQuote=false;
		char ch=0, depth=0, lastCh;
		int line=1, quote=-1;

		while (ptr<bufLen) {
			lastCh=ch;
			if ((ch=inBuf.charAt(ptr++))=='\n')
				line++;

			if (swallow) {
				if (ch=='\n') {
					swallow=false;
					outBuf.append(pMan.lineSeparator);
				}
			} else {
				if (depth>0) {
					if (lastCh=='*' && ch=='/') {
						if (depth>0)
							depth--;
					} else if (lastCh=='/' && ch=='*') {
						System.err.println(name+": embedded comment detected on line "+Integer.toString(line));
						depth++;
					}
				} else {
					if (ch=='"' || ch=='\'') {
						if (inQuote) {
						       if (quote==ch)
							       inQuote=false;
						} else {
							quote=ch;
							inQuote=true;
						}
						outBuf.append(ch);
						continue;
					}

					if (ch=='/' && lastCh=='/' && inQuote==false) {
						swallow=true;
						outBuf.setLength(outBuf.length()-1);
					} else if (ch=='*' && lastCh=='/' && inQuote==false) {
						depth++;
						outBuf.setLength(outBuf.length()-1);
					}

					if (swallow==false && depth==0) {
						if (ch=='\n')
							outBuf.append(pMan.lineSeparator);
						else
							outBuf.append(ch);
					}
				}
			}
		}

		inBuf.setLength(0);
		inBuf.append(exp.Expand(outBuf.toString()));
	}
}
