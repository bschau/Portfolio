package dk.schau.jStar;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.MouseMotionListener;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.awt.Color;
import java.awt.Font;
import java.awt.Frame;
import java.awt.Graphics;
import java.awt.Image;
import java.awt.Insets;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.PrintWriter;

import java.util.ArrayList;
import java.util.StringTokenizer;

import javax.swing.event.PopupMenuEvent;
import javax.swing.event.PopupMenuListener;
import javax.swing.JFileChooser;
import javax.swing.JMenu;
import javax.swing.JMenuItem;
import javax.swing.JOptionPane;
import javax.swing.JPopupMenu;
import javax.swing.UIManager;

/**
 * The Gravity canvas.
 */
public class GravityCanvas extends Frame implements Runnable, MouseListener, MouseMotionListener, ActionListener, PopupMenuListener {
	/** */
	private final int WIDTH = 320;
	private final int HEIGHT = 480;

	private Color[] selAnim = { new Color(0x555555), new Color(0x888888), new Color(0xbbbbbb), new Color(0xeeeeee), new Color(0xffffff), new Color(0xdddddd), new Color(0xaaaaaa), new Color(0x777777), new Color(0x444444) };

	private Thread animThread;
	private Graphics offScreen;
	private Image offScreenImage;
	private Insets ins;
	private int srcX = -1000, srcY = -1000, dstX = -1000, dstY = -1000;
	private int popupX, popupY;
	private long nextGrowPlanet;

	private Player player = new Player();
	private ArrayList<Thing> things = new ArrayList<Thing>();
	private long started = 0, tickOMeter, lastTick;;
	private int score = 0, scoreAdd = 0;
	private StringBuffer scoreBuf = new StringBuffer();
	private Font scoreFont = new Font("Arial", Font.PLAIN, 14);
	private int selIdx = 0;

	private JFileChooser jfc = new JFileChooser();

	private JPopupMenu popupMenu = new JPopupMenu();
	private JMenuItem miReset = new JMenuItem("Reset");
	private JMenuItem miDelete = new JMenuItem("Delete");
	private JMenu miPlanet = new JMenu("Add Planet");
	private JMenuItem miAddPlanetS = new JMenuItem("Small");
	private JMenuItem miAddPlanetM = new JMenuItem("Medium");
	private JMenuItem miAddPlanetL = new JMenuItem("Large");
	private JMenuItem miAddWormhole = new JMenuItem("Add Wormhole");
	private JMenu miDeflector = new JMenu("Add Deflector");
	private JMenuItem miAddDeflectorS = new JMenuItem("Small");
	private JMenuItem miAddDeflectorM = new JMenuItem("Medium");
	private JMenuItem miAddDeflectorL = new JMenuItem("Large");
	private JMenu miWall = new JMenu("Add Wall");
	private JMenuItem miAddWallVS = new JMenuItem("Vertical Small");
	private JMenuItem miAddWallVM = new JMenuItem("Vertical Medium");
	private JMenuItem miAddWallVL = new JMenuItem("Vertical Large");
	private JMenuItem miAddWallHS = new JMenuItem("Horizontal Small");
	private JMenuItem miAddWallHM = new JMenuItem("Horizontal Medium");
	private JMenuItem miAddWallHL = new JMenuItem("Horizontal Large");
	private JMenu miPaddle = new JMenu("Add Paddle");
	private JMenuItem miAddPaddleVSS = new JMenuItem("Vertical Small Slow");
	private JMenuItem miAddPaddleVSF = new JMenuItem("Vertical Small Fast");
	private JMenuItem miAddPaddleVMS = new JMenuItem("Vertical Medium Slow");
	private JMenuItem miAddPaddleVMF = new JMenuItem("Vertical Medium Fast");
	private JMenuItem miAddPaddleVLS = new JMenuItem("Vertical Large Slow");
	private JMenuItem miAddPaddleVLF = new JMenuItem("Vertical Large Fast");
	private JMenuItem miAddPaddleHSS = new JMenuItem("Horizontal Small Slow");
	private JMenuItem miAddPaddleHSF = new JMenuItem("Horizontal Small Fast");
	private JMenuItem miAddPaddleHMS = new JMenuItem("Horizontal Medium Slow");
	private JMenuItem miAddPaddleHMF = new JMenuItem("Horizontal Medium Fast");
	private JMenuItem miAddPaddleHLS = new JMenuItem("Horizontal Large Slow");
	private JMenuItem miAddPaddleHLF = new JMenuItem("Horizontal Large Fast");
	private JMenuItem miClear = new JMenuItem("Clear");
	private JMenuItem miOpen = new JMenuItem("Open ...");
	private JMenuItem miSave = new JMenuItem("Save ...");

	/**
	 * Build the GUI elements.
	 */
	private void buildGUI() {
		setTitle("jStar");
		pack();
		ins = getInsets();

		setSize(WIDTH + ins.left + ins.right, HEIGHT + ins.top + ins.bottom);
		setLocationRelativeTo(null);
		setVisible(true);
		addMouseListener(this);
		addMouseMotionListener(this);

		popupMenu.add(miReset);
		miReset.addActionListener(this);
		popupMenu.add(new JPopupMenu.Separator());
		popupMenu.add(miDelete);
		miDelete.addActionListener(this);
		popupMenu.add(new JPopupMenu.Separator());
		popupMenu.add(miPlanet);
		miPlanet.add(miAddPlanetS);
		miAddPlanetS.addActionListener(this);
		miPlanet.add(miAddPlanetM);
		miAddPlanetM.addActionListener(this);
		miPlanet.add(miAddPlanetL);
		miAddPlanetL.addActionListener(this);
		popupMenu.add(miAddWormhole);
		miAddWormhole.addActionListener(this);
		popupMenu.add(miDeflector);
		miDeflector.add(miAddDeflectorS);
		miAddDeflectorS.addActionListener(this);
		miDeflector.add(miAddDeflectorM);
		miAddDeflectorM.addActionListener(this);
		miDeflector.add(miAddDeflectorL);
		miAddDeflectorL.addActionListener(this);
		popupMenu.add(miWall);
		miWall.add(miAddWallVS);
		miAddWallVS.addActionListener(this);
		miWall.add(miAddWallVM);
		miAddWallVM.addActionListener(this);
		miWall.add(miAddWallVL);
		miAddWallVL.addActionListener(this);
		miWall.add(miAddWallHS);
		miAddWallHS.addActionListener(this);
		miWall.add(miAddWallHM);
		miAddWallHM.addActionListener(this);
		miWall.add(miAddWallHL);
		miAddWallHL.addActionListener(this);
		popupMenu.add(miPaddle);
		miPaddle.add(miAddPaddleVSS);
		miAddPaddleVSS.addActionListener(this);
		miPaddle.add(miAddPaddleVSF);
		miAddPaddleVSF.addActionListener(this);
		miPaddle.add(miAddPaddleVMS);
		miAddPaddleVMS.addActionListener(this);
		miPaddle.add(miAddPaddleVMF);
		miAddPaddleVMF.addActionListener(this);
		miPaddle.add(miAddPaddleVLS);
		miAddPaddleVLS.addActionListener(this);
		miPaddle.add(miAddPaddleVLF);
		miAddPaddleVLF.addActionListener(this);
		miPaddle.add(miAddPaddleHSS);
		miAddPaddleHSS.addActionListener(this);
		miPaddle.add(miAddPaddleHSF);
		miAddPaddleHSF.addActionListener(this);
		miPaddle.add(miAddPaddleHMS);
		miAddPaddleHMS.addActionListener(this);
		miPaddle.add(miAddPaddleHMF);
		miAddPaddleHMF.addActionListener(this);
		miPaddle.add(miAddPaddleHLS);
		miAddPaddleHLS.addActionListener(this);
		miPaddle.add(miAddPaddleHLF);
		miAddPaddleHLF.addActionListener(this);
		popupMenu.add(new JPopupMenu.Separator());
		popupMenu.add(miClear);
		miClear.addActionListener(this);
		popupMenu.add(new JPopupMenu.Separator());
		popupMenu.add(miOpen);
		miOpen.addActionListener(this);
		popupMenu.add(miSave);
		miSave.addActionListener(this);
		popupMenu.addPopupMenuListener(this);
	}

	/**
	 * Main entry for the jStar canvas.
	 */
	public void main() {
		try {
			UIManager.setLookAndFeel(UIManager.getSystemLookAndFeelClassName());
		} catch (Exception ex) {}

		buildGUI();

		things.add(new Planet(new Vector(160.0, 240.0), Planet.PLMASS, Planet.PLSIZE));
		things.add(new Deflector(new Vector(80.0, 120.0), Deflector.DMMASS, Deflector.DMSIZE));
		things.add(new Wormhole(new Vector(120.0, 80.0), Wormhole.WHMASS, Wormhole.WHSIZE));
		things.add(new Wormhole(new Vector(200.0, 260.0), Wormhole.WHMASS, Wormhole.WHSIZE));

		things.add(new Wall(new Vector(200, 200), new Vector(Wall.WALLVSW, Wall.WALLVSH)));

		things.add(new Paddle(new Vector(250, 230), Paddle.Type.VSS));

		adjustMenu();

		animThread = new Thread(this);
		animThread.start();

		this.addWindowListener(new WindowAdapter() {
			public void windowClosing(WindowEvent e) {
				System.exit(0);
			}
		});
	}

	/**
	 * PopupMenu Listeners.
	 */
	public void popupMenuWillBecomeVisible(PopupMenuEvent e) {}
	public void popupMenuWillBecomeInvisible(PopupMenuEvent e) {}
	public void popupMenuCanceled(PopupMenuEvent e) {
		reset();
	}

	/**
	 * Thread main.
	 */
	public void run() {
		long time = System.currentTimeMillis(), t;

		while (true) {
			if ((started > 0) && (!player.exploding)) {
				t = System.currentTimeMillis();
				tickOMeter += (t - lastTick);
				lastTick = t;
				if (tickOMeter > 100) {
					tickOMeter = 0;
					score += scoreAdd;
				}

				if ((t - started) >= 10000) {
					scoreAdd = 1;
				} else if ((t - started) >= 20000) {
					scoreAdd = 2;
				} else if ((t - started) >= 30000) {
					if (t > nextGrowPlanet) {
						scoreAdd = 4;
						growPlanets();
						nextGrowPlanet = t + 10000;
					}
				}
			}

			selIdx++;
			if (selIdx >= selAnim.length) {
				selIdx = 0;
			}

			scoreBuf.delete(0, scoreBuf.length());
			scoreBuf.append(score);

			if (started > 0) {
				if (player.exploding) {
					if (player.explode()) {
						reset();
					}
				} else {
					player.move(things);
					adjustPlayerPosition();
					player.detectCollision(things);
				}
			}

			Thing h;
			for (int i = 0; i < things.size(); i++) {
				h = things.get(i);

				if (h instanceof Paddle) {
					Paddle p = (Paddle)h;

					p.move();
				}
			}

			repaint();

			try {
				time += 30;
				Thread.sleep(Math.max(0, time - System.currentTimeMillis()));
			} catch (InterruptedException ex) {
				System.out.println(ex.getMessage());
			}
		}
	}

	/**
	 * Grow planets.
	 */
	private void growPlanets() {
		Thing t;
		Planet p;

		for (int i = 0; i < things.size(); i++) {
			t = things.get(i);

			if (t instanceof Planet) {
				p = (Planet)t;
				p.grow();
			}
		}
	}

	/**
	 * Redraw canvas.
	 *
	 * @param g Graphics.
	 */
	public void update(Graphics g) {
		if (offScreen == null) {
			// Hohum, get me a double buffered display ...
			offScreenImage = createImage(WIDTH, HEIGHT);
			offScreen = offScreenImage.getGraphics();
		}

		offScreen.setColor(Color.BLACK);
		offScreen.fillRect(0, 0, 320, 480);

		offScreen.setColor(Color.WHITE);
		offScreen.setFont(scoreFont);
		offScreen.drawString(scoreBuf.toString(), 20, 20);

		drawThings(offScreen);

		if (player.canFire) {
			drawShootingAction(offScreen);
		} else {
			if (started > 0) {
				if (player.exploding) {
					drawPlayerExplosion(offScreen);
				} else {
					drawCountDown(offScreen);
					drawPlayer(offScreen);
				}
			}
		}

		g.drawImage(offScreenImage, ins.left, ins.top, this);
	}

	/**
	 * Draw the count down line.
	 */
	private void drawCountDown(Graphics g) {
		long wide = System.currentTimeMillis() - started;

		if (wide >= 10000) {
			return;
		}

		wide = 10000 - wide;
		g.setColor(Color.YELLOW);
		g.drawRect(0, HEIGHT - 7, WIDTH - 1, 6);

		int w = WIDTH - 1 - 6, l = (int)((wide * w) / 10000);

		g.fillRect(3, HEIGHT - 5, l, 3);
	}

	/**
	 * Draw things.
	 *
	 * @param g Graphics.
	 */
	private void drawThings(Graphics g) {
		Thing t;

		for (int i = 0; i < things.size(); i++) {
			t = things.get(i);
			if (t instanceof Planet) {
				Planet p = (Planet)t;

				g.setColor(Color.BLUE);
				g.fillOval((int)p.location.x, (int)p.location.y, (int)p.size, (int)p.size);
				if (p.isSelected()) {
					drawSelectedBox(g, p.location, p.size);
				}
			} else if (t instanceof Wormhole) {
				Wormhole w = (Wormhole)t;

				g.setColor(Color.GRAY);
				g.drawOval((int)w.location.x, (int)w.location.y, (int)w.size, (int)w.size);
				if (w.isSelected()) {
					drawSelectedBox(g, w.location, w.size);
				}
			} else if (t instanceof Deflector) {
				Deflector d = (Deflector)t;

				g.setColor(Color.GREEN);
				g.fillOval((int)d.location.x, (int)d.location.y, (int)d.size, (int)d.size);
				if (d.isSelected()) {
					drawSelectedBox(g, d.location, d.size);
				}
			} else if (t instanceof Wall) {
				Wall w = (Wall)t;

				g.setColor(Color.GREEN);
				g.fillRect((int)w.location.x, (int)w.location.y, (int)(w.size.x), (int)(w.size.y));

				if (w.isSelected()) {
					drawSelectedBox(g, w.location, w.size);
				}
			} else if (t instanceof Paddle) {
				Paddle p = (Paddle)t;

				g.setColor(Color.GRAY);
				g.fillRect((int)p.sledge.x, (int)p.sledge.y, (int)p.sledgeSize.x, (int)p.sledgeSize.y);

				if ((int)p.containerSize.x == 0) {
					g.drawLine((int)p.location.x, (int)p.location.y, (int)p.location.x + Paddle.WIDTH, (int)p.location.y);
					g.drawLine((int)p.location.x, (int)(p.location.y + p.containerSize.y), (int)p.location.x + Paddle.WIDTH, (int)(p.location.y + p.containerSize.y));
				} else {
					g.drawLine((int)p.location.x, (int)p.location.y, (int)p.location.x, (int)p.location.y + Paddle.WIDTH);
					g.drawLine((int)(p.location.x + p.containerSize.x), (int)p.location.y, (int)(p.location.x + p.containerSize.x), (int)p.location.y + Paddle.WIDTH);
				}

				if (p.isSelected()) {
					Vector unit = new Vector();
					unit.x = p.containerSize.x == 0 ? Paddle.WIDTH : p.containerSize.x;
					unit.y = p.containerSize.y == 0 ? Paddle.WIDTH : p.containerSize.y;
					drawSelectedBox(g, p.location, unit);
				}
			}
		}
	}

	/**
	 * Draw the selected box.
	 *
	 * @param g Graphics.
	 * @param l Vector.
	 * @param s Size.
	 */
	private void drawSelectedBox(Graphics g, Vector l, double s) {
		g.setColor(selAnim[selIdx]);

		double adj = 2.0;

		s += adj + adj;
		s -= 1.0;

		g.drawRect((int)(l.x - adj), (int)(l.y - adj), (int)s, (int)s);
	}

	/**
	 * Draw the selected box.
	 *
	 * @param g Graphics.
	 * @param l Vector.
	 * @param s Size.
	 */
	private void drawSelectedBox(Graphics g, Vector l, Vector s) {
		g.setColor(selAnim[selIdx]);

		int adj = 2;

		g.drawRect((int)l.x - adj, (int)l.y - adj, (int)s.x + adj + adj - 1, (int)s.y + adj + adj - 1);
	}

	/**
	 * Draw the player.
	 *
	 * @param g Graphics.
	 */
	private void drawPlayer(Graphics g) {
		g.setColor(Color.RED);
		g.fillOval((int)player.location.x, (int)player.location.y, (int)Player.SIZE, (int)Player.SIZE);
	}

	/**
	 * Draw the shooting line and endpoint elements.
	 *
	 * @param g Graphics.
	 */
	private void drawShootingAction(Graphics g) {
		if (srcX > -999) {
			int s = (int)(Player.SIZE / 2.0);

			g.setColor(Color.RED);
			g.fillOval(srcX - s, srcY - s, (int)Player.SIZE, (int)Player.SIZE);

			if (dstX > -999) {
				g.drawLine(srcX, srcY, dstX, dstY);

				g.fillOval(dstX - s , dstY - s, (int)Player.SIZE, (int)Player.SIZE);
			}
		}
	}

	/**
	 * Draw player explosion animation.
	 *
	 * @param g Graphics.
	 */
	private void drawPlayerExplosion(Graphics g) {
		drawPlayer(g);

		int j = 0, w;

		for (int i = 0; i < player.frame; i++) {
			w = j + j;
			g.drawOval((int)player.location.x - j, (int)player.location.y - j, w, w);
			j += (i * 2);
		}
	}

	/**
	 * Redraw canvas.
	 *
	 * @param g Graphics.
	 */
	public void paint(Graphics g) {
		update(g);
	}

	/**
	 * Reset game.
	 */
	private void reset() {
		dstX = -1000;
		dstY = -1000;
		srcX = -1000;
		srcY = -1000;
		started = 0;
		player.reset();
	}

	/**
	 * Show the popup menu.
	 *
	 * @param e MouseEvent.
	 * @return true if menu was shown.
	 */
	private void showPopupMenu(MouseEvent e) {
		if (e.isPopupTrigger()) {
			player.canFire = false;
			popupX = e.getX();
			popupY = e.getY();
			popupMenu.show(e.getComponent(), e.getX(), e.getY());
		}
	}

	/**
	 * Mouse events.
	 */
	public void mousePressed(MouseEvent e) {
		showPopupMenu(e);
	}

	public void mouseReleased(MouseEvent e) {
		showPopupMenu(e);

		if ((player.canFire) && (srcX > -999) && (dstX > -999)) {
			setInMotion();
		}
	}

	public void mouseEntered(MouseEvent e) {}
	public void mouseExited(MouseEvent e) {}
	public void mouseClicked(MouseEvent e) {
		showPopupMenu(e);

		if (!select(e)) {
			if (player.canFire) {
				if (srcX < -999) {
					srcX = e.getX() - ins.left;
					srcY = e.getY() - ins.top;
				} else if (dstX > -999) {
					srcX = e.getX() - ins.left;
					srcY = e.getY() - ins.top;
					dstX = -1000;
					dstY = -1000;
				} else {
					dstX = e.getX() - ins.left;
					dstY = e.getY() - ins.top;
					setInMotion();
				}
			}
		}
	}

	public void mouseMoved(MouseEvent e) {
		mouseDragged(e);
	}

	public void mouseDragged(MouseEvent e) {
		showPopupMenu(e);

		if ((player.canFire) && (srcX > -1000)) {
			dstX = e.getX() - ins.left;
			dstY = e.getY() - ins.top;
		}
	}

	/**
	 * Select/unselect.
	 *
	 * @param e MouseEvent.
	 * @return true if something was selected, false otherwise.
	 */
	private boolean select(MouseEvent e) {
		boolean ret = false;
		int x = e.getX() - ins.left, y = e.getY() - ins.top;
		Thing t;

		for (int i = 0; i < things.size(); i++) {
			t = things.get(i);
			if (t.hitTest(x, y)) {
				if (t.isSelected()) {
					t.deselect();
				} else {
					t.select();
				}

				return true;
			}
		}

		adjustMenu();
		return ret;
	}

	/**
	 * Handler.
	 *
	 * @param ev Event.
	 */
	public void actionPerformed(ActionEvent ev) {
		Object src = ev.getSource();

		if (src == miReset) {
			reset();
		} else if (src == miDelete) {
			if (JOptionPane.showConfirmDialog(null, "Do you want to delete the selected items?", "Delete?", JOptionPane.YES_NO_OPTION) == JOptionPane.YES_OPTION) {
				for (int i = 0; i < things.size(); i++) {
					Thing t = things.get(i);

					if (t.isSelected()) {
						things.remove(i);
					}
				}
				adjustMenu();
				reset();
			}
		} else if (src == miAddPlanetS) {
			things.add(new Planet(new Vector((double)popupX, (double)popupY), Planet.PSMASS, Planet.PSSIZE));
			reset();
		} else if (src == miAddPlanetM) {
			things.add(new Planet(new Vector((double)popupX, (double)popupY), Planet.PMMASS, Planet.PMSIZE));
			reset();
		} else if (src == miAddPlanetL) {
			things.add(new Planet(new Vector((double)popupX, (double)popupY), Planet.PLMASS, Planet.PLSIZE));
			reset();
		} else if (src == miAddWormhole) {
			if (getNumOfWormholes() < 2) {
				things.add(new Wormhole(new Vector((double)popupX, (double)popupY), Wormhole.WHMASS, Wormhole.WHSIZE));
				adjustMenu();
			}
			reset();
		} else if (src == miAddDeflectorS) {
			things.add(new Deflector(new Vector((double)popupX, (double)popupY), Deflector.DSMASS, Deflector.DSSIZE));
			reset();
		} else if (src == miAddDeflectorM) {
			things.add(new Deflector(new Vector((double)popupX, (double)popupY), Deflector.DMMASS, Deflector.DMSIZE));
			reset();
		} else if (src == miAddDeflectorL) {
			things.add(new Deflector(new Vector((double)popupX, (double)popupY), Deflector.DLMASS, Deflector.DLSIZE));
			reset();
		} else if (src == miAddWallVS) {
			things.add(new Wall(new Vector((double)popupX, (double)popupY), new Vector(Wall.WALLVSW, Wall.WALLVSH)));
			reset();
		} else if (src == miAddWallVM) {
			things.add(new Wall(new Vector((double)popupX, (double)popupY), new Vector(Wall.WALLVMW, Wall.WALLVMH)));
			reset();
		} else if (src == miAddWallVL) {
			things.add(new Wall(new Vector((double)popupX, (double)popupY), new Vector(Wall.WALLVLW, Wall.WALLVLH)));
			reset();
		} else if (src == miAddWallHS) {
			things.add(new Wall(new Vector((double)popupX, (double)popupY), new Vector(Wall.WALLHSW, Wall.WALLHSH)));
			reset();
		} else if (src == miAddWallHM) {
			things.add(new Wall(new Vector((double)popupX, (double)popupY), new Vector(Wall.WALLHMW, Wall.WALLHMH)));
			reset();
		} else if (src == miAddWallHL) {
			things.add(new Wall(new Vector((double)popupX, (double)popupY), new Vector(Wall.WALLHLW, Wall.WALLHLH)));
			reset();
		} else if (src == miAddPaddleVSS) {
			things.add(new Paddle(new Vector((double)popupX, (double)popupY), Paddle.Type.VSS));
			reset();
		} else if (src == miAddPaddleVSF) {
			things.add(new Paddle(new Vector((double)popupX, (double)popupY), Paddle.Type.VSF));
			reset();
		} else if (src == miAddPaddleVMS) {
			things.add(new Paddle(new Vector((double)popupX, (double)popupY), Paddle.Type.VMS));
			reset();
		} else if (src == miAddPaddleVMF) {
			things.add(new Paddle(new Vector((double)popupX, (double)popupY), Paddle.Type.VMF));
			reset();
		} else if (src == miAddPaddleVLS) {
			things.add(new Paddle(new Vector((double)popupX, (double)popupY), Paddle.Type.VLS));
			reset();
		} else if (src == miAddPaddleVLF) {
			things.add(new Paddle(new Vector((double)popupX, (double)popupY), Paddle.Type.VLF));
			reset();
		} else if (src == miAddPaddleHSS) {
			things.add(new Paddle(new Vector((double)popupX, (double)popupY), Paddle.Type.HSS));
			reset();
		} else if (src == miAddPaddleHSF) {
			things.add(new Paddle(new Vector((double)popupX, (double)popupY), Paddle.Type.HSF));
			reset();
		} else if (src == miAddPaddleHMS) {
			things.add(new Paddle(new Vector((double)popupX, (double)popupY), Paddle.Type.HMS));
			reset();
		} else if (src == miAddPaddleHMF) {
			things.add(new Paddle(new Vector((double)popupX, (double)popupY), Paddle.Type.HMF));
			reset();
		} else if (src == miAddPaddleHLS) {
			things.add(new Paddle(new Vector((double)popupX, (double)popupY), Paddle.Type.HLS));
			reset();
		} else if (src == miAddPaddleHLF) {
			things.add(new Paddle(new Vector((double)popupX, (double)popupY), Paddle.Type.HLF));
			reset();
		} else if (src == miClear) {
			if (JOptionPane.showConfirmDialog(null, "Do you want to clear the playfield?", "Clear?", JOptionPane.YES_NO_OPTION) == JOptionPane.YES_OPTION) {
				things.clear();
				adjustMenu();
				reset();
			}
		} else if (src == miOpen) {

			if (jfc.showOpenDialog(this) == JFileChooser.APPROVE_OPTION) {
				loadLevel();
			}
		} else if (src == miSave) {
			if (jfc.showSaveDialog(this) == JFileChooser.APPROVE_OPTION) {
				saveLevel();
			}
		}
	}

	/**
	 * Adjust menu.
	 */
	private void adjustMenu() {
		int selected = 0;

		for (int i = 0; i < things.size(); i++) {
			if (things.get(i).isSelected()) {
				selected++;
			}
		}

		if (selected > 0) {
			miDelete.setEnabled(true);
		} else {
			miDelete.setEnabled(false);
		}

		if (getNumOfWormholes() < 2) {
			miAddWormhole.setEnabled(true);
		} else {
			miAddWormhole.setEnabled(false);
		}
	}

	/**
	 * Find number of allocated wormholes.
	 *
	 * @return count.
	 */
	private int getNumOfWormholes() {
		int cnt = 0;

		for (int i = 0; i < things.size(); i++) {
			if (things.get(i) instanceof Wormhole) {
				cnt++;
			}
		}

		return cnt;
	}

	/**
	 * Set player in motion.
	 */
	private void setInMotion() {
		if (player.setInMotion(srcX, srcY, dstX, dstY)) {
			score = 0;
			scoreAdd = 0;
			tickOMeter = 0;
			nextGrowPlanet = 0;
			started = System.currentTimeMillis();
			lastTick = started;
		} else {
			reset();
		}
	}

	/**
	 * Adjust the player position if it reaches the boundaries.
	 */
	private void adjustPlayerPosition() {
		if (player.location.x <= 0.0) {
			player.velocity.x = -player.velocity.x;
			player.location.x = 0.1;
		} else if (player.location.x >= 320.0) {
			player.velocity.x = -player.velocity.x;
			player.location.x = 319.9;
		}

		if (player.location.y <= 0.0) {
			player.velocity.y = -player.velocity.y;
			player.location.y = 0.1;
		} else if (player.location.y >= 480.0) {
			player.velocity.y = -player.velocity.y;
			player.location.y = 479.9;
		}
	}

	/**
	 * Save this level.
	 */
	private void saveLevel() {
		FileWriter fw = null;
		BufferedWriter bw = null;
		PrintWriter pw = null;

		try {
			fw = new FileWriter(jfc.getSelectedFile());
			bw = new BufferedWriter(fw);
			pw = new PrintWriter(bw);
			StringBuffer sBuf = new StringBuffer();

			Thing t;

			for (int i = 0; i < things.size(); i++) {
				sBuf.delete(0, sBuf.length());
				t = things.get(i);
				if (t instanceof Planet) {
					Planet p = (Planet)t;

					sBuf.append(Thing.Type.toString(Thing.Type.PLANET));
					sBuf.append(' ');
					sBuf.append(p.location.x);
					sBuf.append(' ');
					sBuf.append(p.location.y);
					sBuf.append(' ');
					sBuf.append(p.mass);
					sBuf.append(' ');
					sBuf.append(p.size);
				} else if (t instanceof Wormhole) {
					Wormhole w = (Wormhole)t;

					sBuf.append(Thing.Type.toString(Thing.Type.WORMHOLE));
					sBuf.append(' ');
					sBuf.append(w.location.x);
					sBuf.append(' ');
					sBuf.append(w.location.y);
					sBuf.append(' ');
					sBuf.append(w.mass);
					sBuf.append(' ');
					sBuf.append(w.size);
				} else if (t instanceof Deflector) {
					Deflector d = (Deflector)t;

					sBuf.append(Thing.Type.toString(Thing.Type.DEFLECTOR));
					sBuf.append(' ');
					sBuf.append(d.location.x);
					sBuf.append(' ');
					sBuf.append(d.location.y);
					sBuf.append(' ');
					sBuf.append(d.mass);
					sBuf.append(' ');
					sBuf.append(d.size);
				} else if (t instanceof Wall) {
					Wall w = (Wall)t;

					sBuf.append(Thing.Type.toString(Thing.Type.WALL));
					sBuf.append(' ');
					sBuf.append(w.location.x);
					sBuf.append(' ');
					sBuf.append(w.location.y);
					sBuf.append(' ');
					sBuf.append(w.size.x);
					sBuf.append(' ');
					sBuf.append(w.size.y);
				} else if (t instanceof Paddle) {
					Paddle p = (Paddle)t;

					sBuf.append(Thing.Type.toString(Thing.Type.PADDLE));
					sBuf.append(' ');
					sBuf.append(Paddle.Type.toString(p.type));
					sBuf.append(' ');
					sBuf.append(p.location.x);
					sBuf.append(' ');
					sBuf.append(p.location.y);
				}

				pw.println(sBuf.toString());
			}
			JOptionPane.showMessageDialog(null, "Level saved!");
		} catch (Exception ex) {
			JOptionPane.showMessageDialog(null, ex.getMessage());
		} finally {
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
	}

	/**
	 * Load level.
	 */
	private void loadLevel() {
		things.clear();
		reset();

		BufferedReader in = null;
		FileReader fr = null;

		try {
			fr = new FileReader(jfc.getSelectedFile());
			in = new BufferedReader(fr);
			int line = 0, wormholes = 0, tokCnt;
			String s, token;
			StringTokenizer st;
			double x, y, mass, size, width, height;

			while ((s = in.readLine()) != null) {
				line++;

				s = s.trim();
				if (s.length() == 0) {
					continue;
				}

				st = new StringTokenizer(s);
				tokCnt = st.countTokens() - 1;
				token = st.nextToken();

				if (token.equals("TypePlanet")) {
					if (tokCnt != 4) {
						throw new Exception("TypePlanet needs 4 arguments on line " + line);
					}

					x = Double.parseDouble(st.nextToken());
					y = Double.parseDouble(st.nextToken());
					mass = Double.parseDouble(st.nextToken());
					size = Double.parseDouble(st.nextToken());
					things.add(new Planet(new Vector(x, y), mass, size));
				} else if (token.equals("TypeWormhole")) {
					if (wormholes > 2) {
						throw new Exception("No more Wormholes allowed on line " + line);
					}

					if (tokCnt != 4) {
						throw new Exception("TypeWormhole needs 4 arguments on line " + line);
					}

					x = Double.parseDouble(st.nextToken());
					y = Double.parseDouble(st.nextToken());
					mass = Double.parseDouble(st.nextToken());
					size = Double.parseDouble(st.nextToken());
					things.add(new Wormhole(new Vector(x, y), mass, size));
				} else if (token.equals("TypeDeflector")) {
					if (tokCnt != 4) {
						throw new Exception("TypeDeflector needs 4 arguments on line " + line);
					}

					x = Double.parseDouble(st.nextToken());
					y = Double.parseDouble(st.nextToken());
					mass = Double.parseDouble(st.nextToken());
					size = Double.parseDouble(st.nextToken());
					things.add(new Deflector(new Vector(x, y), mass, size));
				} else if (token.equals("TypeWall")) {
					if (tokCnt != 4) {
						throw new Exception("TypeWall needs 4 arguments on line " + line);
					}

					x = Double.parseDouble(st.nextToken());
					y = Double.parseDouble(st.nextToken());
					width = Double.parseDouble(st.nextToken());
					height = Double.parseDouble(st.nextToken());
					things.add(new Wall(new Vector(x, y), new Vector(width, height)));
				} else if (token.equals("TypePaddle")) {
					if (tokCnt != 3) {
						throw new Exception("TypePaddle needs 3 arguments on line " + line);
					}

					Paddle.Type pt = Paddle.Type.fromString(st.nextToken());
					x = Double.parseDouble(st.nextToken());
					y = Double.parseDouble(st.nextToken());
					things.add(new Paddle(new Vector(x, y), pt));
				} else {
					throw new Exception("Unknown object type: '" + token + "' on line " + line);
				}
			}
		} catch (Exception ex) {
			JOptionPane.showMessageDialog(null, ex.getMessage());
		} finally {
			if (in != null) {
				try {
					in.close();
				} catch (Exception e) {}
			}

			if (fr != null) {
				try {
					fr.close();
				} catch (Exception e) {}
			}
		}

		adjustMenu();
	}
}
