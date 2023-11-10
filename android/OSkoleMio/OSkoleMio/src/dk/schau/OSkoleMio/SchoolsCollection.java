package dk.schau.OSkoleMio;

import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.PrintWriter;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Iterator;

import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.NodeList;

import dk.schau.OSkoleMio.comparators.SchoolComparator;
import dk.schau.OSkoleMio.comparators.StringComparator;
import dk.schau.OSkoleMio.vos.School;

import android.app.Activity;
import android.os.Environment;

public class SchoolsCollection {
	public static ArrayList<School> schools = new ArrayList<School>();
	private static ArrayList<String> _allNames = new ArrayList<String>();

	public static boolean init(Activity activity) {
		String xml;

		try {
			xml = getXml(activity);
		} catch (Exception ex) {
			return false;
		}
		
		Document doc = XMLFunctions.xmlFromString(xml);
		if (doc == null) {
			return false;
		}

		dispose();

		NodeList nodes = doc.getElementsByTagName("school");

		for (int index = 0; index < nodes.getLength(); index++) {
			Element element = (Element) nodes.item(index);

			schools.add(new School(XMLFunctions.getValue(element, "name"), XMLFunctions.getValue(element, "url")));
		}

		Collections.sort(schools, new SchoolComparator());
		return true;
	}

	public static void dispose() {
		_allNames.clear();
		schools.clear();
	}
	
	public static File getExternalFilesDir(Activity activity) {
		String packageName = activity.getBaseContext().getPackageName();
		File externalPath = Environment.getExternalStorageDirectory();
		return new File(externalPath.getAbsolutePath() + "/Android/data/" + packageName + "/files");
	}
	
	public static File getSchoolsFile(Activity activity) {
		return new File(getExternalFilesDir(activity), activity.getString(R.string.schoolsfile));
	}
	
	private static String getXml(Activity activity) throws IOException {
		File file = getSchoolsFile(activity);
		FileInputStream fileInputStream = null;
		
		try {
			fileInputStream = new FileInputStream(file);

			return XMLFunctions.readXmlFile(fileInputStream);
		} finally {
			if (fileInputStream != null) {
				fileInputStream.close();
			}
		}
	}

	public static void setRemoteXml(String xml, Activity activity) throws IOException {
		PrintWriter printWriter = null;

		try {
			File file = getSchoolsFile(activity);
			FileOutputStream fileOutputStream = new FileOutputStream(file);
			BufferedOutputStream bufferedOutputStream = new BufferedOutputStream(fileOutputStream);
			printWriter = new PrintWriter(bufferedOutputStream);

			printWriter.write(xml);
		} finally {
			if (printWriter != null) {
				try {
					printWriter.close();
				} catch (Exception exception) { }
			}
		}
	}

	public static ArrayList<String> getAllNames(Activity parent) {
		if (_allNames.size() != 0) {
			return _allNames;
		}
		
		if (schools.size() == 0) {
			if (init(parent)) {
				return _allNames;
			}
		}

		Iterator<School> iterator = schools.iterator();

		while (iterator.hasNext()) {
			School school = iterator.next();
			_allNames.add(school.getName());
		}

		Collections.sort(_allNames, new StringComparator());
		return _allNames;
	}
}
