package dk.schau.jStar;

import java.util.ArrayList;

/**
 * Player class.
 */
public class Player {
	public static final double MASS = 3000.0;
	public static final double SIZE = 7.0;
	public static final double SPEED = 6.0;
	public static final int COLLISIONFACTOR = 6;
	public Vector location = new Vector(), velocity = new Vector();
	public boolean canFire, exploding;
	public int frame;

	/** */
	private static final double piHalf = Math.PI / 180.0;
	private Vector accel = new Vector(), unit = new Vector();
	private Vector midLoc = new Vector(), step = new Vector();;
	private Vector save = new Vector();
	private int delay;
	private long noWormholeTill, noWallTill, noPaddleTill;

	/**
	 * Initialize player.
	 */
	public Player() {
		reset();
	}

	/**
	 * Reset player.
	 */
	public void reset() {
		canFire = true;
		exploding = false;
		noWormholeTill = 0;
		noWallTill = 0;
		noPaddleTill = 0;
	}

	/**
	 * Set player in motion.
	 *
	 * @param srcX Source X.
	 * @param srcY Source Y.
	 * @param dstX Destination X.
	 * @param dstY Destination Y.
	 * @return true if was set in motione, false otherwise.
	 */
	public boolean setInMotion(int srcX, int srcY, int dstX, int dstY) {
		double x = (double)srcX - (double)dstX;
		double y = ((double)srcY - (double)dstY) * -1.0;
		double angle = Math.atan(y / x) / piHalf;
		double power;

		if (x < 0.0) {
			angle += 180.0;
		}

		if ((x >= 0.0) && (y < 0.0)) {
			angle += 360.0;
		}

		if (angle >= 360.0) {
			angle -= 360.0;
		}

		power = Math.sqrt(Math.abs((x * x) + (y * y)));
		if (power < 1.0) {
			return false;
		}

		if (power > SPEED) {
			power = SPEED;
		}

		angle *= piHalf * -1.0f;
		location.x = (double)srcX;
		location.y = (double)srcY;
		velocity.x = Math.cos(angle) * power;
		velocity.y = Math.sin(angle) * power;

		noWormholeTill = 0;
		noWallTill = 0;
		noPaddleTill = 0;
		canFire = false;
		return true;
	}

	/**
	 * Set midLocation of player.
	 */
	public void setMidLocation() {
		midLoc.x = location.x + (SIZE / 2);
		midLoc.y = location.y + (SIZE / 2);
	}

	/**
	 * Move the player.
	 *
	 * @param things Array of things on this screen.
	 */
	public void move(ArrayList<Thing> things) {
		Thing t;

		setMidLocation();
		accel.zero();

		for (int i = 0; i < things.size(); i++) {
			t = things.get(i);
			if (t instanceof Planet) {
				Planet p = (Planet)t;

				attract(Planet.PlanetConstant, p.midLocation(), p.mass);
			} else if (t instanceof Wormhole) {
				if (System.currentTimeMillis() > noWormholeTill) {
					Wormhole w = (Wormhole)t;

					attract(Wormhole.WormholeConstant, w.midLocation(), w.mass);
				}
			} else if (t instanceof Deflector) {
				Deflector d = (Deflector)t;
				
				repel(Deflector.DeflectorConstant, d.midLocation(), d.mass);
			}
		}

		velocity.add(accel);
		adjustVelocity();

		save.clone(location);
		save.add(velocity);

		step.clone(velocity);
		step.divide((double)COLLISIONFACTOR);

		boolean collided = false;

		for (int i = 0; i < COLLISIONFACTOR; i++) {
			location.add(step);
			if (detectCollision(things)) {
				collided = true;
				break;
			}
		}

		if (!collided) {
			location.clone(save);
			detectCollision(things);
		}
	}

	/**
	 * Adjust velocity if outside speed range.
	 */
	private void adjustVelocity() {
		if (velocity.x > SPEED) {
			velocity.x = SPEED;
		} else if (velocity.x < -SPEED) {
			velocity.x = -SPEED;
		}

		if (velocity.y > SPEED) {
			velocity.y = SPEED;
		} else if (velocity.y < -SPEED) {
			velocity.y = -SPEED;
		}
	}

	/**
	 * Handle attractors.
	 *
	 * @param g Gravitational Constant.
	 * @param v Mid Location vector.
	 * @param m Mass.
	 */
	private void attract(double g, Vector v, double m) {
		double magnitude, factor;

		unit.clone(v);
		unit.subtract(midLoc);

		magnitude = Math.sqrt(Math.abs((unit.x * unit.x) + (unit.y * unit.y)));
		factor = (g * ((MASS * m) / (magnitude * magnitude * magnitude))) / MASS;

		unit.multiply(factor);
		
		accel.add(unit);
	}

	/**
	 * Handle deflectors.
	 *
	 * @param g Gravitational Constant.
	 * @param v Mid Location vector.
	 * @param m Mass.
	 */
	private void repel(double g, Vector v, double m) {
		double magnitude, factor;

		unit.clone(v);
		unit.subtract(midLoc);

		magnitude = Math.sqrt(Math.abs((unit.x * unit.x) + (unit.y * unit.y)));
		factor = -((g * ((MASS * m) / (magnitude * magnitude * magnitude))) / MASS);

		unit.multiply(factor);
		
		accel.add(unit);
	}

	/**
	 * Detect if player collides with any thing on this screen.
	 *
	 * @param things Array of things on this screen.
	 * @return true if player collided with something, false otherwise.
	 */
	public boolean detectCollision(ArrayList<Thing> things) {
		Thing t;

		setMidLocation();
		for (int i = 0; i < things.size(); i++) {
			t = things.get(i);
			if (t instanceof Planet) {
				Planet p = (Planet)t;

				if (isInRange(p.midLocation(), p.size)) {
					exploding = true;
					frame = 0;
					delay = 0;
					return true;
				}
			} else if (t instanceof Wormhole) {
				if (System.currentTimeMillis() < noWormholeTill) {
					continue;
				}

				Wormhole w = (Wormhole)t;

				if (isInRange(w.midLocation(), w.size)) {
					for (int j = 0; j < things.size(); j++) {
						Thing n;

						n = things.get(j);
						if (n instanceof Wormhole) {
							if (i != j) {
								Wormhole h = (Wormhole)n;
								useWormhole(h.midLocation());
								return true;
							}
						}
					}
				}
			} else if (t instanceof Wall) {
				if (System.currentTimeMillis() < noWallTill) {
					continue;
				}

				Wall w = (Wall)t;

				if (boxCollision(w.location, w.size)) {
					noWallTill = System.currentTimeMillis() + 500;
					return true;
				}
			} else if (t instanceof Paddle) {
				if (System.currentTimeMillis() < noPaddleTill) {
					continue;
				}

				Paddle p = (Paddle)t;

				if (boxCollision(p.sledge, p.sledgeSize)) {
					noPaddleTill = System.currentTimeMillis() + 500;
					return true;
				}
			}
		}

		return false;
	}

	/**
	 * Detect if player has collided with box (wall/paddle).
	 *
	 * @param l Location vector.
	 * @param s Size vector.
	 * @return true if collision detected, false otherwise.
	 */
	private boolean boxCollision(Vector l, Vector s) {
		double sz = SIZE / 2.0;
		double uX = l.x - sz, uY = l.y - sz;
		double lX = l.x + s.x + sz, lY = l.y + s.y + sz;

		if ((midLoc.x >= uX) && (midLoc.x <= lX) &&
		    (midLoc.y >= uY) && (midLoc.y <= lY)) {
			uX = l.x;
			uY = l.y - sz;
			lX = uX + s.x;
			lY = l.y;
			if ((midLoc.x >= uX) && (midLoc.x <= lX) && (midLoc.y >= uY) && (midLoc.y <= lY)) {
				velocity.y = -velocity.y;
				return true;
			}

			uY = l.y + s.y;
			lY = uY + sz;
			if ((midLoc.x >= uX) && (midLoc.x <= lX) && (midLoc.y >= uY) && (midLoc.y <= lY)) {
				velocity.y = -velocity.y;
				return true;
			}

			uX = l.x - sz;
			uY = l.y;
			lX = l.x;
			lY = l.y + s.y;
			if ((midLoc.x >= uX) && (midLoc.x <= lX) && (midLoc.y >= uY) && (midLoc.y <= lY)) {
				velocity.x = -velocity.x;
				return true;
			}

			uX = l.x;
			lX = uX + sz;
			if ((midLoc.x >= uX) && (midLoc.x <= lX) && (midLoc.y >= uY) && (midLoc.y <= lY)) {
				velocity.x = -velocity.x;
				return true;
			}

			velocity.x = -velocity.x;
			velocity.y = -velocity.y;
			return true;
		}

		return false;
	}

	/**
	 * Use the wormhole.
	 *
	 * @param v Exit vector.
	 */
	private void useWormhole(Vector v) {
		location.clone(v);

		velocity.multiply(2.0);
		adjustVelocity();
		noWormholeTill = System.currentTimeMillis() + 500;
	}

	/**
	 * Detect if the object with the given dimensions is "in range".
	 *
	 * @param v Location vector.
	 * @param s Size of object.
	 */
	private boolean isInRange(Vector v, double s) {
		double x = midLoc.x - v.x;
		double y = midLoc.y - v.y;
		double l = Math.sqrt(Math.abs((x * x) + (y * y)));

		if ((l - (SIZE / 2) - (s / 2)) <= -1.0) {
			return true;
		}

		return false;
	}

	/**
	 * Explode player.
	 *
	 * @return true if explosion ended, false otherwise.
	 */
	public boolean explode() {
		if (delay == 3) {
			delay = 0;
			frame++;
			if (frame > 7) {
				return true;
			}
		} else {
			delay++;
		}

		return false;
	}
}
