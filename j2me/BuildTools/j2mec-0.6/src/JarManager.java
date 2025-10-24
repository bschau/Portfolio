import java.io.*;
import java.util.*;

/**
 * JarManager is the class handling the creation of the Manifest, jar and jad
 * files.
 * Currently the jar command used is somewhat hardcoded - 'jar' needs to be
 * in the users path.
 *
 * @author	Brian Schau
 * @since	0.1
 */
public class JarManager {
	private String midlets, suiteName, manifest, cldc, midp, jarFile;
	private Variables v;
	private PathManager pMan;

	/**
	 * Set up JarManager.
	 *
	 * @param	vars	Variables block.
	 * @param	m	Midlets to compile separated by whitespace.
	 */
	public JarManager(Variables vars, String m) {
		v=vars;
		midlets=m;
		pMan=v.GetPathManager();

		if ((suiteName=v.GetProperty("SuiteName"))==null)
			suiteName="midlet";
	}

	/**
	 * Get name of midlet from Build property file.	In case of failure,
	 * autogenerate name.
	 */
	private String GetMidletName(String m, int idx) {
		String s;

		if ((s=v.GetProperty(m+"Name"))==null)
			s="midlet"+Integer.toString(idx);

		return s;
	}

	/**
	 * Create Manifest file based on users preference.
	 */
	public void CreateManifest() {
		String mContent;
		File f;
		String s, m, fC;
		BufferedReader r;
		BufferedWriter b;
		StringTokenizer sToken, mToken;
		int idx;

		if ((cldc=v.GetProperty("CLDCVersion"))==null)
			cldc="CLDC-1.0";

		if ((midp=v.GetProperty("MIDPVersion"))==null)
			midp="MIDP-1.0";

		mContent="";
		if ((manifest=v.GetProperty("Manifest"))!=null) {
			manifest=pMan.FullPath(manifest, true);
			f=new File(manifest);

			try {
				r=new BufferedReader(new FileReader(manifest));
				while ((s=r.readLine())!=null)
					mContent+=s+"\n";
				r.close();
			} catch (Exception ex) {
				System.err.println("Cannot read Manifest template");
				System.exit(1);
			}

			mContent=mContent.trim()+"\n";
		} else
			mContent="Manifest-Version: 1.0\nMicroEdition-Configuration: "+cldc+"\nMicroEdition-Profile: "+midp+"\n";

		mContent+="MIDlet-Name: "+suiteName+"\nMIDlet-Version: "+v.bpVersion+"\nMIDlet-Vendor: "+v.bpVendor+"\n";
		sToken=new StringTokenizer(midlets, " \t");
		idx=1;
		while (sToken.hasMoreTokens()) {
			m=sToken.nextToken();
			s=GetMidletName(m, idx);
			mToken=new StringTokenizer(v.GetProperty(m+"Sources"), " \t");
			mContent+="MIDlet-"+Integer.toString(idx)+": "+s+", , "+v.GetPackagedName(mToken.nextToken())+"\n";
			idx++;
		}

		manifest=pMan.pmTmpDir+File.separator+"MANIFEST";
		try {
			b=new BufferedWriter(new FileWriter(manifest));
				
			b.write(mContent, 0, mContent.length());
			b.close();
		} catch (Exception ex) {
			System.err.println("Cannot update Manifest file");
			System.err.println(ex.getMessage());
			System.exit(1);
		}
	}

	/**
	 * Create Jar File using the jar command.
	 */
	public void CreateJar() {
		StringTokenizer sToken, mToken;
		String m, src, classes, s, sources, resources, pkg;
		String cmd;
		File f;
		File[] sourceList;
		int i;

		f=new File(pMan.pmPkgDir);
		FileFilter ff=new FileFilter() {
			public boolean accept(File file) {
				return file.getName().endsWith(".class");
			}
		};

		if ((sourceList=f.listFiles(ff))==null) {
			System.err.println("No class files?!?");
			System.exit(1);
		}

		sToken=new StringTokenizer(midlets, " \t");
		classes=new String();
		if (v.bpPackage==null)
			pkg="";
		else
			pkg=v.bpPackage.replace('.', '/')+"/";

		while (sToken.hasMoreTokens()) {
			m=sToken.nextToken();

			sources=v.GetProperty(m+"Sources");
			mToken=new StringTokenizer(sources, " \t");
			while (mToken.hasMoreTokens()) {
				src=mToken.nextToken();
				for (i=0; i<sourceList.length; i++) {
					s=sourceList[i].getName();
					if (s.startsWith(src+".") || s.startsWith(src+"$"))
						classes+=" -C "+pMan.QuotedPath(pMan.pmTmpDir)+" "+pkg+s;
				}
			}
		}

		if ((resources=v.GetProperty("Resources"))!=null) {
			resources=pMan.FullPath(resources, false);
			f=new File(resources);
			sourceList=f.listFiles();
			for (i=0; i<sourceList.length; i++)
				classes+=" -C "+pMan.QuotedPath(resources)+" "+sourceList[i].getName();
		}

		if ((jarFile=v.GetProperty("JarFile"))==null)
			jarFile=suiteName.replaceAll("\\s", "");

		if (jarFile.endsWith(".jar")==false)
			jarFile+=".jar";

		cmd="jar cfm "+pMan.QuotedPath(jarFile)+" "+pMan.QuotedPath(manifest)+" "+classes;

		ProcessManager pp=new ProcessManager();
		pp.RunCommand(cmd);
	}

	/**
	 * Create the corresponding jad file.
	 */
	public void CreateJad() {
		String jadContent="MicroEdition-Configuration: "+cldc+"\nMicroEdition-Profile: "+midp+"\nMIDlet-Name: "+suiteName+"\nMIDlet-Version: "+v.bpVersion+"\nMIDlet-Vendor: "+v.bpVendor+"\nMIDlet-Jar-URL: "+jarFile+"\nMIDlet-Jar-Size: ";
		File f;
		StringTokenizer sToken, mToken;
		String m, jInfo, jCont, s;
		int idx;
		BufferedWriter b;
		BufferedReader r;

		f=new File(jarFile);
		jadContent+=Integer.toString((int)f.length())+"\n";
		
		sToken=new StringTokenizer(midlets, " \t");
		idx=1;
		while (sToken.hasMoreTokens()) {
			m=sToken.nextToken();
			s=GetMidletName(m, idx);

			mToken=new StringTokenizer(v.GetProperty(m+"Sources"), " \t");
			jadContent+="MIDlet-"+Integer.toString(idx)+": "+s+", , "+v.GetPackagedName(mToken.nextToken())+"\n";

			if ((jInfo=v.GetProperty(m+"JadInfo"))!=null) {
				if (jInfo.charAt(0)=='@') {
					jInfo=pMan.FullPath(jInfo.substring(1), true);
					jCont="";
					try {
						r=new BufferedReader(new FileReader(jInfo));
						while ((s=r.readLine())!=null)
							jCont+=s+"\n";

						r.close();
					} catch (Exception ex) {
						System.err.println("Cannot read Jad Info for "+m);
						System.err.println(ex.getMessage());
						System.exit(1);
					}

					jadContent+=jCont.trim()+"\n";
				} else
					jadContent+=jInfo+"\n";
			}

			idx++;
		}

		try {
			String p=jarFile.substring(0, jarFile.length()-1)+"d";
			b=new BufferedWriter(new FileWriter(p));
				
			b.write(jadContent, 0, jadContent.length());
			b.close();
		} catch (Exception ex) {
			System.err.println("Cannot write .jad file");
			System.err.println(ex.getMessage());
			System.exit(1);
		}
	}
}
