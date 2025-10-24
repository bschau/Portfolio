package com.schau;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.InputStreamReader;
import java.io.PrintWriter;

import java.lang.Character;

import java.util.Enumeration;
import java.util.Hashtable;
import java.util.Vector;

import org.apache.tools.ant.Task;
import org.apache.tools.ant.Project;
import org.apache.tools.ant.BuildException;
import org.apache.tools.ant.types.FileSet;

/**
 * This is main class for the TrlClassCreator task.
 *
 * To include the task in your build.xml do something like:
 *
 * <target name="init" description="Taskdef the TrlClassCreator task">
 *     <taskdef name="trl"
 *              classname="com.schau.TrlClassCreator"
 *              classpath="jar_path" />
 * </target>
 *
 * If you copy TrlClassCreator.jar to the lib directory of ant you can omit
 * the classpath attribute.
 *
 * <target name="use" description="Use" depends="init">
 *    <trl showversion="true"       <!-- show version information on execute -->
 *         strings="file"           <!-- current strings -->
 *         base="file"              <!-- base strings -->
 *         destfile="Trl.java"      <!-- output file -->
 *         package="package"        <!-- optional package name -->
 *         usespaces="false"        <!-- use spaces -->
 *         tabwidth="1"             <!-- width of tabs -->
 *         encoding="UTF-8"         <!-- encoding of strings -->
 *    >
 *        <base key="str1name">str1value</base>
 *        <base key="str2name">str2value</base>
 *                                 .
 *                                 .
 *        <base key="str1name">str1value</base>
 *        <string key="str1name">str1value</string>
 *        <string key="str2name">str2value</string>
 *                                   .
 *                                   .
 *        <string key="str1name">str1value</string>
 *    </scc>
 * </target>
 */
public class TrlClassCreator extends Task {
	// must match version in common.xml!
	private static final String VERSION="1.0";

	/**
	 * Vector which holds the base values.
	 */
	protected Vector<KeyString> vBase=new Vector<KeyString>();

	/**
	 * Hashtable which holds the base values.
	 */
	protected Hashtable<String, KeyString> htBase=new Hashtable<String, KeyString>();

	/**
	 * Vector which holds the string values.
	 */
	protected Vector<KeyString> vStrings=new Vector<KeyString>();

	/**
	 * Hashtable which holds the string values.
	 */
	protected Hashtable<String, KeyString> htStrings=new Hashtable<String, KeyString>();

	/**
	 * Name of the generated file.
	 */
	protected String dstFile="Trl.java";

	/**
	 * Base file.
	 */
	protected File baseFile=null;

	/**
	 * Overlay file.
	 */
	protected File stringsFile=null;

	/**
	 * Show version information when task is invoked.
	 */
	protected boolean showVersion=true;

	/**
	 * Package info.
	 */
	protected String pkgName=null;

	/**
	 * Use spaces instead of tabs.
	 */
	protected boolean useSpaces=false;

	/**
	 * Tab width (number of tabs or spaces).
	 */
	protected int tabWidth=1;

	/**
	 * Encoding of strings.
	 */
	protected String encoding="UTF-8";

	/**
	 * Stringbuffer used by many.
	 */
	private StringBuffer sBuf=new StringBuffer();

	/**
	 * Statistics.
	 */
	private int notInBase=0, notInStrings=0;

	/**
	 * Create a base key/string combo.
	 *
	 * @return the key/string..
	 */
	public KeyString createBase() {
		KeyString ks=new KeyString();

		vBase.add(ks);
		return ks;
	}

	/**
	 * Create a key/string combo.
	 *
	 * @return the key/string..
	 */
	public KeyString createString() {
		KeyString ks=new KeyString();

		vStrings.add(ks);
		return ks;
	}

	/**
	 * Evaluate string to boolean.
	 *
	 * @param b Boolean string.
	 * @param def Default value.
	 * @return true/false.
	 */
	private boolean getBoolean(String b, boolean def) {
		if ((b==null) || (b.length()==0)) {
			return def;
		}

		if ((b.compareToIgnoreCase("true")==0) ||
		    (b.compareToIgnoreCase("on")==0) ||
		    (b.compareToIgnoreCase("1")==0) ||
		    (b.compareToIgnoreCase("yes")==0) ||
		    (b.compareToIgnoreCase("aye, mate!")==0)) {
			return true;
		}

		return false;
	}

	/**
	 * Setters (setShowversion, setDestfile, setBase, setOverlay
	 *          setPackage, setUsetabs, setTabwidth, setEncoding).
	 */
	public void setShowversion(String b) { showVersion=getBoolean(b, true); }
	public void setDestfile(String d) { dstFile=d; }
	public void setBase(File f) { baseFile=f; }
	public void setStrings(File f) { stringsFile=f; }
	public void setPackage(String p) { pkgName=p; }
	public void setUsespaces(String b) { useSpaces=getBoolean(b, true); }
	public void setTabwidth(String i) { try { tabWidth=Integer.parseInt(i); } catch (Exception ex) { tabWidth=0; } }
	public void setEncoding(String e) { encoding=e; }

	/**
	 * Process the file(s).
	 *
	 * @exception BuildException.
	 */
	public void execute() throws BuildException {
		if (showVersion) {
			syslog("TrlClassCreator v"+VERSION+" (http://www.schau.com/)");
		}

		try {
			if (baseFile!=null) {
				if (!baseFile.exists()) {
					throw new Exception("Failed to find base file");
				}

				loadStrings(baseFile, htBase);
			}

			if (stringsFile!=null) {
				if (!stringsFile.exists()) {
					throw new Exception("Failed to find strings file");
				}

				loadStrings(stringsFile, htStrings);
			}

			KeyString ks;
			for (int i=0; i<vBase.size(); i++) {
				ks=vBase.elementAt(i);
				htBase.put(ks.getKey(), ks);
			}

			for (int i=0; i<vStrings.size(); i++) {
				ks=vStrings.elementAt(i);
				htStrings.put(ks.getKey(), ks);
			}

			generateStringsFile();

			if ((notInBase>0) || (notInStrings>0)) {
				syslog("Not in BASE: "+notInBase);
				syslog("Not in STRINGS: "+notInStrings);
			}
		} catch (Exception ex) {
			throw new BuildException(ex.getMessage());
		}
	}

	/**
	 * Log the string. Called by some of the other classes in this package.
	 */
	public void syslog(String msg) {
		log(msg);
	}

	/**
	 * Load strings from file into hashtable.
	 *
	 * @param file File.
	 * @param tbl Hashtable.
	 * @exception Exception.
	 */
	private void loadStrings(File file, Hashtable<String, KeyString> tbl) throws Exception {
		FileInputStream fis=null;
		InputStreamReader isr=null;
		BufferedReader br=null;
		int line=0;

		try {
			fis=new FileInputStream(file);
			isr=new InputStreamReader(fis, encoding);
			br=new BufferedReader(isr);

			KeyString ks;
			String s;
			int pos, len;
			while ((s=br.readLine())!=null) {
				line++;
				s=s.trim();

				len=s.length();
				if ((len==0) || (s.charAt(0)=='#')) {
					continue;
				}

				sBuf.delete(0, sBuf.length());
				if (Character.isJavaIdentifierStart(s.charAt(0))) {
					sBuf.append(s.charAt(0));
					pos=1;
					while ((Character.isJavaIdentifierPart(s.charAt(pos))) && (pos<len)) {
						sBuf.append(s.charAt(pos));
						pos++;
					}

					ks=new KeyString();
					ks.setKey(sBuf.toString());

					while ((Character.isWhitespace(s.charAt(pos))) && (pos<len)) {
						pos++;
					}

					if (pos<len) {
						sBuf.delete(0, sBuf.length());

						if (s.charAt(len-1)=='\\') {
							sBuf.append(s.substring(pos, len-1));
							while ((s=br.readLine())!=null) {
								line++;
								s=s.trim();
								len=s.length();
								if ((len==0) || (s.charAt(0)=='#')) {
									continue;
								}

								if (s.charAt(len-1)=='\\') {
									sBuf.append(s.substring(0, len-1));
								} else {
									sBuf.append(s);
									break;
								}
							}

							ks.addText(sBuf.toString());
						} else {
							ks.addText(s.substring(pos));
						}

						s=ks.getKey();
						if (tbl.containsKey(s)) {
							throw new Exception("Attempt to redefine '"+s+"' on line "+line+" in "+file.getName());
						}

						tbl.put(s, ks);
					} else {
						throw new Exception("Key with no string on line "+line+" in "+file.getName());
					}
				} else {
					throw new Exception("Invalid key on line "+line+" in "+file.getName());
				}
			}
		} catch (Exception ex) {
			throw ex;
		} finally {
			if (br!=null) {
				try {
					br.close();
				} catch (Exception ex) {}
			}

			if (isr!=null) {
				try {
					isr.close();
				} catch (Exception ex) {}
			}

			if (fis!=null) {
				try {
					fis.close();
				} catch (Exception ex) {}
			}
		}
	}

	/**
	 * Generate the file.
	 *
	 * @exception Exception.
	 */
	private void generateStringsFile() throws Exception {
		PrintWriter pw=null;
		int i;

		try {
			sBuf.delete(0, sBuf.length());
			for (i=0; i<tabWidth; i++) {
				sBuf.append((useSpaces ? ' ' : '\t'));
			}
			String prefix=sBuf.toString();

			pw=new PrintWriter(dstFile, encoding);

			if (pkgName!=null) {
				pw.print("package ");
				pw.print(pkgName);
				pw.println(";");
				pw.println();
			}

			File f=new File(dstFile);
			String s=f.getName();
			int l=s.lastIndexOf('.');

			if (l>-1) {
				s=s.substring(0, l);
			}

			pw.print("class ");
			pw.print(s);
			pw.println(" {");

			Enumeration en=htStrings.elements();
			KeyString ks;
			while (en.hasMoreElements()) {
				ks=(KeyString)en.nextElement();

				if (htBase.get(ks.getKey())==null) {
					pw.println();
					pw.print(prefix);
					pw.println("// Not in BASE:");
					outputKey(pw, ks, prefix);
					pw.print(prefix);
					pw.println("// ============");
					pw.println();
					notInBase++;
				} else {
					outputKey(pw, ks, prefix);
				}
			}

			diff();

			if (htBase.size()>0) {
				pw.println();
				pw.print(prefix);
				pw.println("// Not in STRINGS:");
				en=htBase.elements();

				while (en.hasMoreElements()) {
					outputKey(pw, (KeyString)en.nextElement(), prefix);
					notInStrings++;
				}
				pw.print(prefix);
				pw.println("// ===============");
			}

			pw.println("}");
		} catch (Exception ex) {
			throw ex;
		} finally {
			if (pw!=null) {
				try {
					pw.close();
				} catch (Exception ex) {}
			}
		}
	}

	/**
	 * Output a key.
	 *
	 * @param pw PrintWriter.
	 * @param ks KeyString.
	 * @param prefix Prefix string.
	 */
	private void outputKey(PrintWriter pw, KeyString ks, String prefix) {
		pw.print(prefix);
		pw.print("public static final String ");
		pw.print(ks.getKey());
		pw.print(" = \"");

		sBuf.delete(0, sBuf.length());
		String s=ks.getString();
				
		int i=0, pos;
		while (true) {
			pos=s.indexOf('"', i);
			if (pos>-1) {
				sBuf.append(s.substring(i, pos-1));
				sBuf.append("\\");
				i=pos+1;
			} else {
				sBuf.append(s.substring(i));
				break;
			}
		}

		pw.print(sBuf.toString());
		pw.println("\";");
	}

	/**
	 * Remove all entries from base which are present in strings.
	 */
	private void diff() {
		Enumeration en=htStrings.elements();
		KeyString ks;

		while (en.hasMoreElements()) {
			ks=(KeyString)en.nextElement();

			htBase.remove(ks.getKey());
		}
	}
}
