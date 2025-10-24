package dk.schau.CitiZr.InstructionsActivity;

public class Coffee {
	private String name, description, scale, recommendations;
	private int imageId, whipId;

	public Coffee(String name, int imageId, int whipId, String description, String scale, String recommendations) {
		this.name = name;
		this.imageId = imageId;
		this.whipId = whipId;
		this.description = description;
		this.scale = scale;
		this.recommendations = recommendations;
	}
	
	public String getName() {
		return name;
	}

	public int getImageId() {
		return imageId;
	}
	
	public int getWhipId() {
		return whipId;
	}

	public String getDescription() {
		return description;
	}

	public String getScale() {
		return scale;
	}

	public String getRecommendations() {
		return recommendations;
	}
}
