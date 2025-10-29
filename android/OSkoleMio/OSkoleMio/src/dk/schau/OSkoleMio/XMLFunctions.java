package dk.schau.OSkoleMio;

import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.InputStreamReader;
import java.io.StringReader;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;

import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;
import org.xml.sax.InputSource;

public class XMLFunctions {
	public final static Document xmlFromString(String xml) {
		DocumentBuilderFactory documentBuilderFactory = DocumentBuilderFactory.newInstance();
		Document document = null;

		try {
			DocumentBuilder documentBuilder = documentBuilderFactory.newDocumentBuilder();

			InputSource inputSource = new InputSource();
			inputSource.setCharacterStream(new StringReader(xml));
			document = documentBuilder.parse(inputSource);
		} catch (Exception exception) {
			return null;
		}

		return document;
	}

	public final static String getElementValue(Node element) {
		if (element == null) {
			return "";
		}
		
		if (!element.hasChildNodes()) {
			return "";
		}
		
		for (Node kid = element.getFirstChild(); kid != null; kid = kid.getNextSibling()) {
			if (kid.getNodeType() == Node.TEXT_NODE) {
				return kid.getNodeValue();
			}
		}

		return "";
	}

	public static String getValue(Element item, String tag) {
		NodeList nodeList = item.getElementsByTagName(tag);

		return XMLFunctions.getElementValue(nodeList.item(0));
	}

	public static String readXmlFile(FileInputStream inputStream) {
		StringBuilder builder = new StringBuilder();
		BufferedReader bufferedReader = null;
		String returnXml = "";

		try {
			InputStreamReader inputStreamReader = new InputStreamReader(inputStream);
			bufferedReader = new BufferedReader(inputStreamReader);

			String line;
			while ((line = bufferedReader.readLine()) != null) {
				builder.append(line);
				builder.append("\r\n");
			}

			returnXml = builder.toString();
		} catch (Exception exception) {
		} finally {
			if (bufferedReader != null) {
				try {
					bufferedReader.close();
				} catch (Exception exception) { }
			}
		}

		return returnXml;
	}
}
