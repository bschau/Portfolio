package dk.schau.OSkoleMio.vos;

public class Document {
	public String fullPath = "";
	public String mimeType = "";
	
	public Document(String fullPath, String mimeType) {
		this.fullPath = fullPath;
		this.mimeType = mimeType;
	}
}
