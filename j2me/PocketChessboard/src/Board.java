import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;

public class Board {
	public static final int WK = 1;
	public static final int WQ = 2;
	public static final int WR = 3;
	public static final int WN = 4;
	public static final int WB = 5;
	public static final int WP = 6;
	public static final int BK = 7;
	public static final int BQ = 8;
	public static final int BR = 9;
	public static final int BN = 10;
	public static final int BB = 11;
	public static final int BP = 12;
	public static final int WhiteInTurn = 1;
	public static final int BlackInTurn = 2;
	private byte[] board = new byte[64];
	private int ply;
	private String mark = Locale.get("defaultSetup");

	/**
	 * Constructor - initialize a basic "default" board.
	 */
	public Board() {
		int i;

		for (i = 0; i < 64; i++) {
			board[i] = 0;
		}
 
		board[0] = WR;
		board[1] = WN;
		board[2] = WB;
		board[3] = WQ;
		board[4] = WK;
		board[5] = WB;
		board[6] = WN;
		board[7] = WR;
 
		for (i = 0; i < 8; i++) {
			board[8 + i] = WP;
			board[48 + i] = BP;
		}

		board[56] = BR;
		board[57] = BN;
		board[58] = BB;
		board[59] = BQ;
		board[60] = BK;
		board[61] = BB;
		board[62] = BN;
		board[63] = BR;

		ply = 0;
	}

	/**
	 * Constructor - initialize the board based on another board.
	 */
	public Board(Board b) {
		for (int i = 0; i < 64; i++) {
			board[i] = b.getPiece(i);
		}

		ply = b.getPly();
		mark = b.getMark();
	}

	/**
	 * Return number of ply.
	 *
	 * @return number of ply.
	 */
	public int getPly() {
		return ply;
	}

	/**
	 * Set number of ply.
	 *
	 * @param p number of ply.
	 */
	public void setPly(int p) {
		ply = p;
	}

	/**
	 * Add one to ply.
	 */
	public void bumpPly() {
		ply++;
	}

	/**
	 * Return current turn number.
	 *
	 * @return turn number.
	 */
	public int getTurn() {
		return ((ply % 2) == 1 ? BlackInTurn : WhiteInTurn);
	}

	/**
	 * Set mark.
	 *
	 * @param m Mark.
	 */
	public void setMark(String m) {
		mark = m;
	}

	/**
	 * Return current mark.
	 *
	 * @return the mark.
	 */
	public String getMark() {
		return mark;
	}

	/**
	 * Get piece at x, y.
	 *
	 * @param x X coordinate.
	 * @param y Y coordinate.
	 * @return piece.
	 */
	public byte getPiece(int x, int y) {
		return board[(y * 8) + x];
	}

	/**
	 * Get piece at index.
	 *
	 * @param idx Index.
	 * @return piece.
	 */
	public byte getPiece(int idx) {
		return board[idx];
	}

	/**
	 * Clear square at x, y.
	 *
	 * @param x X coordinate.
	 * @param y Y coordinate.
	 */
	public void clear(int x, int y) {
		board[(y * 8) + x] = 0;
	}

	/**
	 * Set piece at x, y.
	 *
	 * @param x X coordinate.
	 * @param y Y coordinate.
	 * @param b Piece.
	 */
	public void setPiece(int x, int y, byte b) {
		board[(y * 8) + x] = b;
	}

	/**
	 * Move a piece from sX, sY to dX, dY.
	 *
	 * @param sX Source X coordinate.
	 * @param sY Source Y coordinate.
	 * @param dX Destination X coordinate.
	 * @param dY Destination Y coordinate.
	 */
	public void move(int sX, int sY, int dX, int dY) {
		int idx=(sY * 8) + sX;

		board[(dY * 8) + dX] = board[idx];
		board[idx] = 0;
	}

	/**
	 * Serialize the board.
	 *
	 * @param recPos Record position.
	 * @return data.
	 */
	public byte[] serialize(int recPos) {
		ByteArrayOutputStream baos = null;
		DataOutputStream dos = null;
		byte[] data = null;

		try {
			baos = new ByteArrayOutputStream();
			dos = new DataOutputStream(baos);

			dos.writeInt(recPos);
			for (int idx = 0; idx < 64; idx++) {
				dos.writeByte(board[idx]);
			}

			dos.writeShort(ply);
			if (mark != null) {
				dos.writeUTF(mark);
			}
			data = baos.toByteArray();
		} catch (Exception ex) {
		} finally {
			if (dos != null) {
				try {
					dos.close();
				} catch (Exception ex) {}
			}

			if (baos != null) {
				try {
					baos.close();
				} catch (Exception ex) {}
			}
		}

		return data;
	}

	/**
	 * Setup the board with values from data.
	 *
	 * @param data Data to load from.
	 * @return true if loaded succesfully, false otherwise.
	 */
	public boolean unserialize(byte[] data) {
		ByteArrayInputStream bais = null;
		DataInputStream dis = null;
		boolean ret = false;

		try {
			bais = new ByteArrayInputStream(data);
			dis = new DataInputStream(bais);

			dis.readInt();	// recPos - just discard it
			for (int idx = 0; idx < 64; idx++) {
				board[idx] = dis.readByte();
			}

			ply = (int)dis.readShort();
			if (bais.available() > 0) {
				mark = dis.readUTF();
			} else {
				mark = null;
			}

			ret = true;
		} catch (Exception ex) {
		} finally {
			if (dis != null) {
				try {
					dis.close();
				} catch (Exception ex) {}
			}

			if (bais != null) {
				try {
					bais.close();
				} catch (Exception ex) {}
			}
		}

		return ret;
	}
}
