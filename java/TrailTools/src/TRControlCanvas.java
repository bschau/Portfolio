package com.schau.TrailTools;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.KeyEvent;
import java.awt.GridLayout;
import java.text.NumberFormat;
import javax.swing.AbstractButton;
import javax.swing.JButton;
import javax.swing.JFormattedTextField;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.UIManager;

/**
 * The "Control" canvas used by the TrailRunner.
 */
public class TRControlCanvas extends JFrame implements ActionListener {
	private JFormattedTextField stride = new JFormattedTextField(NumberFormat.getIntegerInstance());
	private JFormattedTextField speed = new JFormattedTextField(NumberFormat.getIntegerInstance());
	private JFormattedTextField delay = new JFormattedTextField(NumberFormat.getIntegerInstance());
	private JFormattedTextField mask = new JFormattedTextField(NumberFormat.getIntegerInstance());
	private TRRunnerCanvas rc;

	/**
	 * Build the GUI elements.
	 */
	private void buildGUI() {
		JFrame frame = new JFrame("CC");
		frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		frame.setLayout(new GridLayout(5, 2));

		stride.setColumns(4);
		stride.setValue(rc.spriteStride);
		speed.setColumns(4);
		speed.setValue(rc.spriteSpeed);
		delay.setColumns(4);
		delay.setValue(rc.animationDelay);
		mask.setColumns(4);
		mask.setValue(rc.trailMask);

		frame.add(new JLabel("Sprite Stride:", JLabel.LEFT));
		frame.add(stride);
		frame.add(new JLabel("Sprite Speed:", JLabel.LEFT));
		frame.add(speed);
		frame.add(new JLabel("Animation Delay:", JLabel.LEFT));
		frame.add(delay);
		frame.add(new JLabel("Trail Mask:", JLabel.LEFT));
		frame.add(mask);

		JButton but = new JButton("Set");
		but.setVerticalTextPosition(AbstractButton.CENTER);
		but.setHorizontalTextPosition(AbstractButton.LEADING);
		but.setMnemonic(KeyEvent.VK_S);
		but.setActionCommand("set");
		but.addActionListener(this);

		frame.add(new JPanel());
		frame.add(but);
		frame.pack();

		frame.setLocationRelativeTo(rc);
		frame.setVisible(true);
	}

	/**
	 * Main entry.
	 */
	public void main() {
		try {
			UIManager.setLookAndFeel(UIManager.getSystemLookAndFeelClassName());
		} catch (Exception ex) {}

		buildGUI();
	}

	/**
	 * Set the current runner canvas.
	 */
	public void setRunnerCanvas(TRRunnerCanvas rc) {
		this.rc = rc;
	}

	/**
	 * Action handler.
	 *
	 * @param e Action Event.
	 */
	public void actionPerformed(ActionEvent e) {
		if (e.getActionCommand().equals("set")) {
			rc.animationDelay = getInt(delay.getValue(), 100);
			rc.spriteSpeed = getInt(speed.getValue(), 1);
			rc.trailMask = getInt(mask.getValue(), 1);

			try {
				rc.loadTrail();
			} catch (Exception ex) {
				System.out.println("*** "+ex.getMessage());
				System.exit(1);
			}

			rc.paintTrail();
			rc.setStride(getInt(stride.getValue(), 16));
		}
	}

	/**
	 * Get integer value from object.
	 *
	 * @param src Source object.
	 * @param def Default value.
	 * @return value or default value (in case of errors).
	 */
	private int getInt(Object src, int def) {
		try {
			String s = src.toString();
			int i = Integer.parseInt(s);

			return i;
		} catch (Exception ex) {}

		return def;
	}
}
