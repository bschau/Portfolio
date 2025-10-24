package dk.schau.MEDevTools;

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
 * This is main class for the MetaCreator task.
 *
 * To include the task in your build.xml do something like:
 *
 * <target name="init" description="Taskdef the MetaCreator task">
 *     <taskdef name="metabuilder"
 *              classname="dk.schau.MEDevTools.MetaCreator"
 *              classpath="jar_path" />
 * </target>
 *
 * If you copy MEDevTools.jar to the lib directory of ant you can omit
 * the classpath attribute.
 *
 * If you build a jad file you must set jadfile and jarfile attributes below.
 * If you build a manifest file you must set the manifest attribute below.
 *
 * <target name="use" description="Use" depends="init">
 *    <metabuilder
 *        buildjad="false"       <!-- build jad (true) or manifest (false) -->
 *        showversion="true"     <!-- Show version when launched -->
 *        manifest="manifest.mf" <!-- destination manifest file -->
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
 *      <jadattribute name="..." value="..." />
 *                   .
 *                   .
 *      <jadattribute name="..." value="..." />
 *      <manifestattribute name="..." value="..." />
 *                       .
 *                       .
 *      <manifestattribute name="..." value="..." />
 *    </metabuilder>
 * </target>
 */
public class MetaCreator extends Task {
	/**
	 * Vectors which holds the attributes.
	 */
	protected Vector<Attribute> attributes = new Vector<Attribute>();
	protected Vector<Attribute> jadAttributes = new Vector<Attribute>();
	protected Vector<Attribute> manifestAttributes = new Vector<Attribute>();

	/**
	 * ArrayList which holds the MidLets.
	 */
	protected ArrayList<MidLet> midlets = new ArrayList<MidLet>();

	/**
	 * Build jad (true) or manifest (false).
	 */
	protected boolean buildJad = false;

	/**
	 * Show version information when task is invoked.
	 */
	protected boolean showVersion = true;

	/**
	 * Destination manifest.mf file.
	 */
	protected String manifest = "manifest.mf";

	/**
	 * Destination .jad file.
	 */
	protected String jadFile = "output.jad";

	/**
	 * Source .jar file.
	 */
	protected String jarFile = null;

	/**
	 * MidLet Suite name.
	 */
	protected String name = "Change This!";

	/**
	 * MidLet Suite vendor.
	 */
	protected String vendor = "Change This!";

	/**
	 * MidLet Suite version.
	 */
	protected String version = "1.0";

	/**
	 * MIDP version.
	 */
	protected String midp = null;

	/**
	 * CLDC version.
	 */
	protected String cldc = null;

	/**
	 * Deployment URL.
	 */
	protected String url = null;

	/** */
	private FileWriter fw = null;
	private BufferedWriter bw = null;
	private PrintWriter pw = null;

	/**
	 * Constructor.
	 */
	public MetaCreator() {
		setMidp("2.0");
		setCldc("1.1");
	}

	/**
	 * Get the boolean value of a string.
	 *
	 * @param str String.
	 * @return true or false.
	 */
	private boolean getBoolean(String str) {
		if ((str !=null) && (str.length() > 0)) {
			if ((str.compareToIgnoreCase("true") == 0) ||
			    (str.compareToIgnoreCase("on") == 0) ||
			    (str.compareToIgnoreCase("1") == 0) ||
			    (str.compareToIgnoreCase("yes") == 0) ||
			    (str.compareToIgnoreCase("aye, mate!") == 0)) {
				return true;
			}
		}

		return false;
	}

	/**
	 * Setters (setBuildjad, setShowversion, setManifest, setJadfile,
	 * 	    setJarfile, setName, setVendor, setVersion, setMidp,
	 * 	    setCldc, setUrl).
	 */
	public void setBuildjad(String b) { buildJad = getBoolean(b); }
	public void setShowversion(String b) { showVersion = getBoolean(b); }
	public void setManifest(String n) { manifest = n; }
	public void setJadfile(String n) { jadFile = n; }
	public void setJarfile(String n) { jarFile = n; }
	public void setName(String n) { name = n; }
	public void setVendor(String n) { vendor = n; }
	public void setVersion(String n) { version = n; }
	public void setMidp(String n) { midp = "MIDP-" + n; }
	public void setCldc(String n) { cldc = "CLDC-" + n; }
	public void setUrl(String n) { url = n; }

	/**
	 * Create a attribute.
	 *
	 * @return the attribute
	 */
	public Attribute createAttribute() {
		Attribute attr = new Attribute();

		attributes.add(attr);
		return attr;
	}

	/**
	 * Create a jadfile attribute.
	 *
	 * @return the attribute
	 */
	public Attribute createJadattribute() {
		Attribute attr = new Attribute();

		jadAttributes.add(attr);
		return attr;
	}

	/**
	 * Create a manifestfile attribute.
	 *
	 * @return the attribute
	 */
	public Attribute createManifestattribute() {
		Attribute attr = new Attribute();

		manifestAttributes.add(attr);
		return attr;
	}

	/**
	 * Create a midlet.
	 *
	 * @return the midlet.
	 */
	public MidLet createMidlet() {
		MidLet mid = new MidLet();

		midlets.add(mid);
		return mid;
	}

	/**
	 * Execute.
	 *
	 * @exception BuildException.
	 */
	public void execute() throws BuildException {
		if (showVersion) {
			log("MetaBuilder v" + Version.VERSION + " (http://www.schau.com/)");
		}

		try {
			if (buildJad) {
				buildJad();
			} else {
				buildManifest();
			}
		} catch (Exception ex) {
			log(ex.getMessage());
			throw new BuildException("Exited with errors");
		}
	}

	/**
	 * Open the file for writing.
	 *
	 * @param name File name.
	 * @exception Exception.
	 */
	private void openFile(String name) throws Exception {
		fw = new FileWriter(name);
		bw = new BufferedWriter(fw);
		pw = new PrintWriter(bw);
	}

	/**
	 * Close the file.
	 */
	private void closeFile() {
		if (pw != null) {
			try {
				pw.close();
			} catch (Exception ex) {}
		}

		if (bw != null) {
			try {
				bw.close();
			} catch (Exception ex) {}
		}

		if (fw != null) {
			try {
				fw.close();
			} catch (Exception ex) {}
		}
	}

	/**
	 * Output meta information for this midlet suite.
	 */
	private void outputMeta() {
		pw.println("MIDlet-Name: " + name);
		pw.println("MIDlet-Vendor: " + vendor);
		pw.println("MIDlet-Version: " + version);
		pw.println("MicroEdition-Profile: " + midp);
		pw.println("MicroEdition-Configuration: " + cldc);
	}

	/**
	 * Output MIDlet information.
	 */
	private void outputMidLets() {
		int cnt = 1;
		MidLet m;

		for (int i = 0; i < midlets.size(); i++) {
			m = midlets.get(i);
			pw.println("MIDlet-" + cnt + ": " + m.getName() + ", " + m.getIcon() + ", " + m.getEntry());
			cnt++;
		}
	}

	/**
	 * Output attribute table.
	 *
	 * @param tbl Attribute table.
	 */
	private void outputAttributes(Vector<Attribute> tbl) {
		Attribute attr;

		for (Enumeration e = tbl.elements(); e.hasMoreElements(); ) {
			attr = (Attribute)e.nextElement();
			pw.println(attr.getName() + ": " + attr.getValue());
		}
	}

	/**
	 * Build the jad file.
	 */
	private void buildJad() throws Exception {
		if (jarFile == null) {
			throw new Exception("No JAR file given");
		}

		File f = new File(jarFile);

		if (!f.exists()) {
			throw new Exception("Cannot find JAR file: " + jarFile);
		}

		long fileSize = f.length();

		try {
			openFile(jadFile);

			String dst = url;
			String jar = f.getName();

			if (dst == null) {
				dst = jar;
			} else {
				if (dst.endsWith("/")) {
					dst = dst.substring(0, dst.length() - 1);
				}

				dst += "/" + jar;
			}

			pw.println("MIDlet-Jar-URL: " + dst);
			pw.println("MIDlet-Jar-Size: " + fileSize);
			outputMeta();
			outputMidLets();
			outputAttributes(attributes);
			outputAttributes(jadAttributes);
		} catch (Exception ex) {
			throw ex;
		} finally {
			closeFile();
		}
	}

	/**
	 * Build the manifest file.
	 */
	private void buildManifest() throws Exception {
		if (manifest == null) {
			throw new Exception("No Manifest file given");
		}

		try {
			openFile(manifest);

			outputMeta();
			outputMidLets();
			outputAttributes(attributes);
			outputAttributes(manifestAttributes);
		} catch (Exception ex) {
			throw ex;
		} finally {
			closeFile();
		}
	}
}
