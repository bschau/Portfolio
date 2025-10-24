package dk.schau.CitiZr.CitiZrActivity;

public class Capsule {
	private String name, teaser, description;
	private int imageId, category, strengthId;
	private int[] tasting;
	
	public Capsule(int imageId, String name, int category, int strengthId, int[] tasting, String teaser, String description) {
		this.imageId = imageId;
		this.name = name;
		this.category = category;
		this.strengthId = strengthId;
		this.tasting = tasting;
		this.teaser = teaser;
		this.description = description;
	}
	
	public int getImageId() {
		return imageId;
	}

	public String getName() {
		return name;
	}
	
	public int getCategory() {
		return category;
	}

	public int getStrengthId() {
		return strengthId;
	}
	
	public int[] getTasting() {
		return tasting;
	}

	public String getTeaser() {
		return teaser;
	}

	public String getDescription() {
		return description;
	}
}
