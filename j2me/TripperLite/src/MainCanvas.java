#include "Include.j"
import java.lang.StringBuffer;
import java.util.Calendar;
import javax.microedition.lcdui.Canvas;
import javax.microedition.lcdui.Command;
import javax.microedition.lcdui.CommandListener;
import javax.microedition.lcdui.Displayable;
import javax.microedition.lcdui.Font;
import javax.microedition.lcdui.Graphics;
import javax.microedition.lcdui.Image;
import javax.microedition.lcdui.List;
import javax.microedition.rms.RecordComparator;
import javax.microedition.rms.RecordEnumeration;
import javax.microedition.rms.RecordFilter;

public class MainCanvas extends Canvas implements CommandListener {
	private final String NORECORDS="#=LMainNoRecords";
	private final int SPACEAFTERLOGO=6;
	private final int SPACEAFTERLINE=2;
	private final int ARROWWIDTH=11;
	private final int ARROWHEIGHT=6;
	private final int ANCHOR=Graphics.LEFT|Graphics.TOP;
	private Font defFont=Font.getDefaultFont();
	private Font lrgFont=Font.getFont(Font.FACE_PROPORTIONAL, Font.STYLE_BOLD, Font.SIZE_LARGE);
	private Font bldFont=Font.getFont(Font.FACE_PROPORTIONAL, Font.STYLE_BOLD, Font.SIZE_MEDIUM);
	private RecordEnumeration reAll=null;
	private StringBuffer sBuf=new StringBuffer();
	private TripperLiteMIDlet mid;
	private List lstOptions;
	private Command cmdOptions, cmdNew;
	private int cW, cH, records, lstLines, lstTop, barTop;
	private Image logo, arrows;
	private RecordComparator tripComparator;
	private int[] recPtr;

	/**
	 * Constructor.
	 */
	public MainCanvas() {}

	/**
	 * Initialize the Tripper Lite main canvas.
	 *
	 * @param m TripperLiteMIDlet.
	 * @exception Exception.
	 */
	public void init(TripperLiteMIDlet m) throws Exception {
		mid=m;

		logo=Image.createImage("/logo.png");
		arrows=Image.createImage("/arrows.png");

		lstOptions=new List("", List.IMPLICIT);
		lstOptions.append("#=LMainOptBack", null);
		lstOptions.append("#=LMainCleanUp", null);
		lstOptions.append("#=LMainOptAbout", null);
		lstOptions.append("#=LMainOptExit", null);
		lstOptions.setCommandListener(this);

		cmdOptions=new Command("#=LCmdOptions", Command.BACK, 0);
		this.addCommand(cmdOptions);

		cmdNew=new Command("#=LCmdNew", Command.OK, 0);
		this.addCommand(cmdNew);

		this.setCommandListener(this);

		cW=getWidth();
		cH=getHeight();

		tripComparator=new TripComparator();

		lstLines=(cH-logo.getHeight()-SPACEAFTERLOGO-bldFont.getHeight()-SPACEAFTERLINE-SPACEAFTERLINE-ARROWHEIGHT-SPACEAFTERLINE)/defFont.getHeight();
		recPtr=new int[lstLines];
		for (int i=0; i<lstLines; i++) {
			recPtr[i]=-1;
		}
		lstTop=0;
	}

	/**
	 * Initialize the main view.
	 *
	 * @exception Exception.
	 */
	public void initView() throws Exception {
		if (reAll!=null) {
			reAll=null;
			System.gc();
		}

		reAll=mid.db.enumerateRecords(null, tripComparator, false);
		records=reAll.numRecords();

		lstTop=0;
		barTop=0;
		rebuildRecPtr();
	}

	/**
	 * Rebuild the list of record "Pointers".
	 *
	 * @exception Exception.
	 */
	private void rebuildRecPtr() throws Exception {
		int idx=0;

		reAll.reset();

		while (idx<lstTop) {
			reAll.nextRecordId();
			idx++;
		}

		for (idx=0; idx<lstLines; idx++) {
			if (reAll.hasNextElement()) {
				recPtr[idx]=reAll.nextRecordId();
			} else {
				break;
			}
		}

		while (idx<lstLines) {
			recPtr[idx++]=-1;
		}
	}

	/**
	 * Paint display.
	 *
	 * @param g Graphics object.
	 */
	public void paint(Graphics g) {
		g.setColor(0xffffff);
		g.fillRect(0, 0, cW, cH);

		g.setColor(0x000000);
		g.drawImage(logo, (cW-logo.getWidth())/2, 0, ANCHOR);

		if (records>0) {
			Record r=new Record();
			Calendar cal=Calendar.getInstance();
			int y=logo.getHeight()+SPACEAFTERLOGO, idx=0, h=defFont.getHeight(), i, d;
			int c2, c3;

			g.drawString("#=LMainTableDate", 0, y, ANCHOR);
			c2=defFont.stringWidth("888888")+4;
			g.drawString("#=LMainTableDist", c2, y, ANCHOR);
			c3=defFont.stringWidth("8888")+c2+4;
			g.drawString("#=LMainTablePurpose", c3, y, ANCHOR);

			y+=bldFont.getHeight();
			g.drawLine(0, y, cW, y);
			y+=SPACEAFTERLINE;

			i=barTop*h;
			g.fillRect(0, y+i, cW, h);

			g.setFont(defFont);

			for (i=0; i<lstLines; i++) {
				if (recPtr[i]==-1) {
					y+=h;
					continue;
				}

				try {
					r.get(mid.db, recPtr[i]);

					if (i==barTop) {
						g.setColor(0xffffff);
					} else {
						if (r.billState==#=BSSent) {
							g.setColor(0xf65c7d);
							g.fillRect(0, y, cW, h);
						} else if (r.billState==#=BSClosed) {
							g.setColor(0x86ff68);
							g.fillRect(0, y, cW, h);
						}
						g.setColor(0x000000);
					}

					cal.setTime(r.date);

					sBuf.delete(0, sBuf.length());
					d=cal.get(Calendar.YEAR);
					d%=100;
					if (d<10) {
						sBuf.append('0');
					}
					sBuf.append(d);
					d=cal.get(Calendar.MONTH)+1;
					if (d<10) {
						sBuf.append('0');
					}
					sBuf.append(d);
					d=cal.get(Calendar.DAY_OF_MONTH);
					if (d<10) {
						sBuf.append('0');
					}
					sBuf.append(d);
					g.drawString(sBuf.toString(), 0, y, ANCHOR);

					g.drawString(Integer.toString(r.mileage), c2, y, ANCHOR);
					if ((r.purpose!=null) && (r.purpose.length()>0)) {
						g.drawString(r.purpose, c3, y, ANCHOR);
					}

					y+=h;
				} catch (Exception ex) {
					mid.softError=false;
					mid.popup("#=LExcCannotReadRecord", ex.getMessage());
					break;
				}
			}

			y+=SPACEAFTERLINE;

			h=cW-(2*ARROWWIDTH)-2;
			if (lstTop>0) {
				g.drawRegion(arrows, 0, 0, ARROWWIDTH, ARROWHEIGHT, 0, h, y, ANCHOR);
			} else {
				g.drawRegion(arrows, ARROWWIDTH*2, 0, ARROWWIDTH, ARROWHEIGHT, 0, h, y, ANCHOR);
			}

			h+=2+ARROWWIDTH;
			if ((lstTop+lstLines)<records) {
				g.drawRegion(arrows, ARROWWIDTH, 0, ARROWWIDTH, ARROWHEIGHT, 0, h, y, ANCHOR);
			} else {
				g.drawRegion(arrows, ARROWWIDTH*3, 0, ARROWWIDTH, ARROWHEIGHT, 0, h, y, ANCHOR);
			}
		} else {
			int w=defFont.stringWidth(NORECORDS);

			g.setFont(defFont);
			g.drawString(NORECORDS, (cW-w)/2, (cH-defFont.getHeight())/2, ANCHOR);
		}
	}

	/**
	 * Handle commands etc.
	 *
	 * @param c Current command.
	 * @param d Displayable.
	 */
	public void commandAction(Command c, Displayable d) {
		if (c==cmdOptions) {
			mid.setDisplayable(lstOptions);
		} else if (c==cmdNew) {
			mid.goToEdit(-1);
		} else if (d==lstOptions) {
			switch (lstOptions.getSelectedIndex()) {
				case 0:		// Back
					mid.goToMain();
					break;
				case 1:		// Clean Up
					mid.setDisplayable(mid.fCleanUp);
					break;
				case 2:		// About
					mid.setDisplayable(mid.fAbout);
					break;
				default:	// Exit
					mid.notifyDestroyed();
					break;
			}
		}
	}

	/**
	 * Handle keypresses.
	 *
	 * @param keyCode Current key press.
	 */
	protected void keyPressed(int keyCode) {
		int ga=getGameAction(keyCode);

		if ((ga==Canvas.UP) ||
		    (keyCode==Canvas.KEY_NUM2)) {
			if (barTop>0) {
				barTop--;
				repaint();
			} else if (lstTop>0) {
				lstTop--;
				reworkList();
			}
		} else if ((ga==Canvas.DOWN) ||
			   (keyCode==Canvas.KEY_NUM8)) {
			if (records<=lstLines) {
				if (barTop<(records-1)) {
					barTop++;
					repaint();
				}
			} else {
				if (barTop<(lstLines-1)) {
					barTop++;
					repaint();
				} else if ((lstTop+lstLines)<records) {
					lstTop++;
					reworkList();
				}
			}
		} else if ((ga==Canvas.FIRE) ||
			   (keyCode==Canvas.KEY_NUM5)) {
			mid.goToEdit(recPtr[barTop]);
		}
	}

	/**
	 * Rework and repaint the main table.
	 */
	private void reworkList() {
		try {
			rebuildRecPtr();
			repaint();
		} catch (Exception ex) {
			mid.softError=false;
			mid.popup("#=LExcCannotRebuildList", ex.getMessage());
		}
	}
}
