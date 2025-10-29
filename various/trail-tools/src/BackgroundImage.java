package com.schau.TrailTools;

import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.image.BufferedImage;
import java.awt.image.WritableRaster;
import java.util.Hashtable;

/**
 * A background image used by the TrailRunner.
 */
public class BackgroundImage {
	/**
	 * Width and height of image.
	 */
	public int width, height;

	/** */
	private BufferedImage image, current;

	/**
	 * Constructor.
	 *
	 * @param img BufferedImage.
	 */
	public BackgroundImage(BufferedImage img) {	
		image = img;
		width = image.getWidth();
		height = image.getHeight();
		current = getImage();
	}

	/**
	 * Draw the current image at (0, 0).
	 *
	 * @param g Graphics to draw on.
	 */
	public void drawCurrentImage(Graphics g) {
		((Graphics2D)g).drawImage(current, null, 0, 0);
	}

	/**
	 * Get (a copy of) the image (as a buffered image).
	 *
	 * @return image.
	 */
	public BufferedImage getImage() {
		String[] pNames = image.getPropertyNames();
		Hashtable<String, Object> cProps = new Hashtable<String, Object>();
		if (pNames != null) {
			for (int i = 0; i < pNames.length; i++) {
				cProps.put(pNames[i], image.getProperty(pNames[i]));
			}
		}

		WritableRaster wr = image.getRaster();
		WritableRaster cwr = wr.createCompatibleWritableRaster();
		cwr.setRect(wr);

		BufferedImage copy = new BufferedImage(image.getColorModel(), cwr, image.isAlphaPremultiplied(), cProps);
		return copy;
	}

	/**
	 * Set the current image.
	 * 
	 * @param img BufferedImage.
	 */
	public void setCurrentImage(BufferedImage img) {
		current = img;
	}
}
