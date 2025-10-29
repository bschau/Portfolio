import java.io.*;
import java.util.*;

/**
 * Main entry class.
 *
 * @author	Brian Schau
 * @since	0.1
 */
public class j2mec {
	/**
	 * main
	 */
	public static void main(String[] args) {
		Variables v=new Variables();
		String buildFile=null, midlets, m;
		File f;

		if (args.length>0)
			buildFile=args[0];

		try {
			v.Initialize(buildFile);
		} catch (IOException ex) {
			System.err.println("Trouble reading the Build Properties file");
			System.err.println(ex.getMessage());
			ex.printStackTrace();
			System.exit(1);
		}

		if ((midlets=v.GetProperty("MIDlets"))==null)
			midlets="midlet1";

		StringTokenizer sToken=new StringTokenizer(midlets, " \t");
		while (sToken.hasMoreTokens()) {
			m=sToken.nextToken();
			if (v.GetProperty(m+"Sources")==null) {
				System.err.println("No defined sources for "+m);
				System.exit(1);
			}
		}

		try {
			PathManager pMan=v.GetPathManager();

			f=new File(pMan.pmTmpDir);
			EmptyDirectory(f);
		} catch (Exception ex) {
			System.err.println(ex.getMessage());
			System.exit(1);
		}

		PreProcessor pp=new PreProcessor(v, midlets);
		pp.PreProcess();

		Compiler c=new Compiler(v, midlets);
		c.Compile();

		JarManager jm=new JarManager(v, midlets);
		jm.CreateManifest();
		jm.CreateJar();
		jm.CreateJad();

		System.exit(0);
	}

	/**
	 * Delete the contents of a directory.
	 *
	 * @param	d	Directory to delete (given as a file).
	 * should be removed.
	 */
	private static void EmptyDirectory(File d) throws IOException {
		File[] dc=d.listFiles();

		for (int i=0; i<dc.length; i++) {
			if (dc[i].isDirectory()) {
				EmptyDirectory(dc[i]);
				if (dc[i].delete()==false)
					throw new IOException("Cannot delete directory "+dc[i].getAbsolutePath());
			} else {
				if (dc[i].delete()==false)
					throw new IOException("Cannot delete file "+dc[i].getAbsolutePath());
			}
		}
	}
}
