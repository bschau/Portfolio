package com.schau;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.PrintWriter;
import java.util.ArrayList;
import java.util.Enumeration;
import java.util.Vector;
import org.apache.tools.ant.Task;
import org.apache.tools.ant.Project;
import org.apache.tools.ant.BuildException;
import org.apache.tools.ant.types.FileSet;

/**
 * This is main class for the JadBuilder task.
 *
 * To include the task in your build.xml do something like:
 *
 * <target name="init" description="Taskdef the JadBuilder task">
 *     <taskdef name="jadbuilder"
 *              classname="com.schau.JadBuilder"
 *              classpath="jar_path" />
 * </target>
 *
 * If you copy JadBuilder.jar to the lib directory of ant you can omit
 * the classpath attribute.
 *
 * <target name="use" description="Use" depends="init">
 *    <jadbuilder
 *        showversion="true"     <!-- Show version when launched -->
 *        jadfile="output.jad"   <!-- destination jad file -->
 *        jarfile=".."           <!-- jar file -->
 *        name="Change This!"    <!-- MidLet-Name -->
 *        vendor="Change This!"  <!-- MidLet-Vendor -->
 *        version="1.0"          <!-- MidLet-Version -->
 *        midp="2.0"             <!-- MicroEdition-Profile -->
 *        cldc="1.1"             <!-- MicroEdition-Configuration -->
 *        url=".."               <!-- Target URL where jad/jar file will
 *                                    be deployed -->
 *    >
 *      <midlet name=".." entry=".." icon=".." />
 *                .
 *                .
 *      <midlet name=".." entry=".." icon=".." />
 *      <attribute name="..." value="..." />
 *                .
 *                .
 *      <attribute name="..." value="..." />
 *    </jadbuilder>
 * </target>
 */
public class JadBuilder extends Task {
	// must match version in common.xml!
	private static final String VERSION="1.0";

	/**
	 * Vector which holds the attributes.
	 */
	protected Vector<Attribute> attributes=new Vector<Attribute>();

	/**
	 * ArrayList which holds the MidLets.
	 */
	protected ArrayList<MidLet> midlets=new ArrayList<MidLet>();

	/**
	 * Show version information when task is invoked.
	 */
	protected boolean showVersion=true;

	/**
	 * Destination .jad file.
	 */
	protected String jadFile="output.jad";

	/**
	 * Source .jar file.
	 */
	protected String jarFile=null;

	/**
	 * MidLet Suite name.
	 */
	protected String name="Change This!";

	/**
	 * MidLet Suite vendor.
	 */
	protected String vendor="Change This!";

	/**
	 * MidLet Suite version.
	 */
	protected String version="1.0";

	/**
	 * MIDP version.
	 */
	protected String midp=null;

	/**
	 * CLDC version.
	 */
	protected String cldc=null;

	/**
	 * Deployment URL.
	 */
	protected String url=null;

	/**
	 * Constructor.
	 */
	public JadBuilder() {
		setMidp("2.0");
		setCldc("1.1");
	}

	/**
	 * Setters (setShowversion, setJadfile, setJarfile, setName, setVendor,
	 *          setVersion, setMidp, setCldc, setUrl).
	 */
	public void setShowversion(String b) {
		if ((b==null) || (b.length()==0)) {
			return; }

		if ((b.compareToIgnoreCase("true")==0) ||
		    (b.compareToIgnoreCase("on")==0) ||
		    (b.compareToIgnoreCase("1")==0) ||
		    (b.compareToIgnoreCase("yes")==0) ||
		    (b.compareToIgnoreCase("aye, mate!")==0)) {
			showVersion=true;
		    }
	}

	public void setJadfile(String n) { jadFile=n; }
	public void setJarfile(String n) { jarFile=n; }
	public void setName(String n) { name=n; }
	public void setVendor(String n) { vendor=n; }
	public void setVersion(String n) { version=n; }
	public void setMidp(String n) { midp="MIDP-"+n; }
	public void setCldc(String n) { cldc="CLDC-"+n; }
	public void setUrl(String n) { url=n; }

	/**
	 * Create an attribute.
	 *
	 * @return the attribute
	 */
	public Attribute createAttribute() {
		Attribute attr=new Attribute();

		attributes.add(attr);
		return attr;
	}

	/**
	 * Create a midlet.
	 *
	 * @return the midlet.
	 */
	public MidLet createMidlet() {
		MidLet mid=new MidLet();

		midlets.add(mid);
		return mid;
	}

	/**
	 * Build Jad.
	 *
	 * @exception BuildException.
	 */
	public void execute() throws BuildException {
		if (showVersion) {
			log("JadBuilder v"+VERSION+" (http://www.schau.com/)");
		}

		try {
			buildJad();
		} catch (Exception ex) {
			log(ex.getMessage());
			throw new BuildException("Exited with errors");
		}
	}

	/**
	 * Build the jad file.
	 */
	private void buildJad() throws Exception {
		if (jarFile==null) {
			throw new Exception("No JAR file given");
		}

		File f=new File(jarFile);
		if (!f.exists()) {
			throw new Exception("Cannot find JAR file: "+jarFile);
		}

		long fileSize=f.length();

		FileWriter fw=null;
		BufferedWriter bw=null;
		PrintWriter pw=null;

		try {
			fw=new FileWriter(jadFile);
			bw=new BufferedWriter(fw);
			pw=new PrintWriter(bw);

			String dst=url;
			String jar=f.getName();

			if (dst==null) {
				dst=jar;
			} else {
				if (dst.endsWith("/")) {
					dst=dst.substring(0, dst.length()-1);
				}

				dst+="/"+jar;
			}

			pw.println("MIDlet-Jar-URL: "+dst);
			pw.println("MIDlet-Jar-Size: "+fileSize);
			pw.println("MIDlet-Name: "+name);
			pw.println("MIDlet-Vendor: "+vendor);
			pw.println("MIDlet-Version: "+version);
			pw.println("MicroEdition-Profile: "+midp);
			pw.println("MicroEdition-Configuration: "+cldc);

			int cnt=1;
			MidLet m;
			for (int i=0; i<midlets.size(); i++) {
				m=midlets.get(i);
				pw.println("MIDlet-"+cnt+": "+m.getName()+", "+m.getIcon()+", "+m.getEntry());
				cnt++;
			}

			Attribute attr;
			for (Enumeration e=attributes.elements(); e.hasMoreElements(); ) {
				attr=(Attribute)e.nextElement();
				pw.println(attr.getName()+": "+attr.getValue());
			}
		} catch (Exception ex) {
			throw ex;
		} finally {
			if (pw!=null) {
				try {
					pw.close();
				} catch (Exception ex) {}
			}

			if (bw!=null) {
				try {
					bw.close();
				} catch (Exception ex) {}
			}

			if (fw!=null) {
				try {
					fw.close();
				} catch (Exception ex) {}
			}
		}
	}
}
