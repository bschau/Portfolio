#include "Include.j"
import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.InputStream;
import java.util.Date;
import javax.microedition.lcdui.Alert;
import javax.microedition.lcdui.AlertType;
import javax.microedition.lcdui.Canvas;
import javax.microedition.lcdui.Command;
import javax.microedition.lcdui.CommandListener;
import javax.microedition.lcdui.ChoiceGroup;
import javax.microedition.lcdui.DateField;
import javax.microedition.lcdui.Display;
import javax.microedition.lcdui.Displayable;
import javax.microedition.lcdui.Font;
import javax.microedition.lcdui.Form;
import javax.microedition.lcdui.Item;
import javax.microedition.lcdui.Image;
import javax.microedition.lcdui.ImageItem;
import javax.microedition.lcdui.List;
import javax.microedition.lcdui.Spacer;
import javax.microedition.lcdui.StringItem;
import javax.microedition.lcdui.TextField;
import javax.microedition.io.Connector;
import javax.microedition.midlet.MIDlet;
import javax.microedition.rms.RecordStore;

public class TripperLiteMIDlet extends MIDlet implements CommandListener {
	protected boolean softError=true;
	protected RecordStore db=null;
	protected Form fAbout, fEdit, fCleanUp;
	private Command cmdBack, cmdSave, cmdOptions, cmdExport, cmdDelete;
	private List lstEditOptions;
	private MainCanvas main;
	private DateField seDate, cuDate;
	private TextField seMileage;
	private TextField sePurpose;
	private int recIdx;
	private ChoiceGroup seChoices, cuChoices;

	/**
	 * Constructor.
	 */
	public TripperLiteMIDlet() {}

	/**
	 * Pause app.
	 */
	public void pauseApp() {}

	/**
	 * Destroy app.
	 *
	 * @param unconditional unused.
	 */
	public void destroyApp(boolean unconditional) {
		dbClose(db);
	}

	/**
	 * Start app. Open and load preferences and build GUI.
	 */
	public void startApp() {
		try {
			db=dbOpen("Trips", "#=LDBName");

			main=new MainCanvas();
			main.init(this);
			goToMain();
#include "GUI.j"
		} catch (Exception e) {
			softError=false;
			popup("#=LPopupUnrecErr", e.getMessage());
		}
	}

	/**
	 * Show main canvas.
	 */
	public void goToMain() {
		try {
			main.initView();
			setDisplayable(main);
		} catch (Exception ex) {
			softError=false;
			popup("#=LPopupUnrecErr", ex.getMessage());
		}
	}

	/**
	 * Show edit form.
	 * 
	 * @param idx Record index (-1 for new record).
	 */
	public void goToEdit(int idx) {
		setDisplayable(fEdit);

		recIdx=idx;
		if (idx==-1) {
			seDate.setDate(new Date());
			focus(seMileage);
		} else {
			try {
				Record r=new Record();
				r.get(db, idx);

				seDate.setDate(r.date);
				seMileage.setString(Integer.toString(r.mileage));
				if ((r.purpose!=null) && (r.purpose.length()>0)) {
					sePurpose.setString(r.purpose);
				} else {
					sePurpose.setString("");
				}

				if (r.billState==#=BSNew) {
					seChoices.setSelectedIndex(#=BSNewIdx, true);
				} else if (r.billState==#=BSSent) {
					seChoices.setSelectedIndex(#=BSSentIdx, true);
				} else {
					seChoices.setSelectedIndex(#=BSClosedIdx, true);
				}
				r=null;
				System.gc();

				focus(seChoices);
			} catch (Exception ex) {
				softError=false;
				popup("#=LExcCannotReadRecord", ex.getMessage());
			}
		}
	}

	/**
	 * Give focus to item.
	 *
	 * @param i Item (StringItem, TextField, ...)
	 */
	private void focus(Item i) {
		Display.getDisplay(this).setCurrentItem(i);
	}

	/**
	 * Popup an alert of type ERROR with the given title and message.
	 *
	 * @param title Title of popup.
	 * @param msg Message.
	 */
	public void popup(String title, String msg) {
		popup(title, msg, AlertType.ERROR);
	}

	/**
	 * Popup an alert of the given type and with the given title and
	 * message.
	 *
	 * @param title Title.
	 * @param msg Message.
	 * @param t AlertType.
	 */
	private void popup(String title, String msg, AlertType t) {
		Alert a=new Alert(title, msg, null, t);
		a.setTimeout(Alert.FOREVER);
		a.setCommandListener(this);
		setDisplayable(a);
	}
	
	/**
	 * Set the displayable as the current view.
	 *
	 * @param d Displayable (Form, List, ...)
	 */
	public void setDisplayable(Displayable d) {
		Display.getDisplay(this).setCurrent(d);
	}

	/**
	 * Handle command.
	 *
	 * @param c Command.
	 * @param d Current displayable.
	 */
	public void commandAction(Command c, Displayable d) {
		boolean[] picks;

		if (c==Alert.DISMISS_COMMAND) {
			if (softError) {
				goToMain();
			} else {
				notifyDestroyed();
			}
		} else if (c==cmdBack) {
			goToMain();
		} else if (c==cmdOptions) {
			setDisplayable(lstEditOptions);
		} else if (c==cmdSave) {
			Record r=new Record();
			byte[] b=null;

			try {
				picks=new boolean[seChoices.size()];
				seChoices.getSelectedFlags(picks);

				int idx=seChoices.getSelectedIndex();
				if (idx==#=BSNewIdx) {
					r.billState=#=BSNew;
				} else if (idx==#=BSSentIdx) {
					r.billState=#=BSSent;
				} else if (idx==#=BSClosedIdx) {
					r.billState=#=BSClosed;
				} else {
					r.billState=#=BSUnset;
				}

				r.date=seDate.getDate();
				try {
					r.mileage=Integer.parseInt(seMileage.getString());
				} catch (Exception ex) {
					r.mileage=0;
				}
				r.purpose=sePurpose.getString();

				b=r.formRecord();
				dbSave(db, b, recIdx);
				goToMain();
			} catch (Exception ex) {
				popup("#=LExcCannotStoreRecord", ex.getMessage());
			} finally {
				b=null;
				r=null;
				picks=null;
				System.gc();
			}
		} else if (c==cmdDelete) {
			Deleter del=new Deleter(db);
			picks=new boolean[cuChoices.size()];
			cuChoices.getSelectedFlags(picks);

			int bs=#=BSUnset;
			long dt=-1;

			if (cuDate.getDate()!=null) {
				dt=cuDate.getDate().getTime();
			}

			if (picks[#=BSNewIdx]) {
				bs|=#=BSNew;
			}
			if (picks[#=BSSentIdx]) {
				bs|=#=BSSent;
			}
			if (picks[#=BSClosedIdx]) {
				bs|=#=BSClosed;
			}

			del.delete(this, dt, bs);
			del=null;
			System.gc();
		} else if (d==lstEditOptions) {
			switch (lstEditOptions.getSelectedIndex()) {
				case 0:	// Cancel
					goToMain();
					break;
				case 1: // Delete
					if (recIdx>-1) {
						try {
							db.deleteRecord(recIdx);
						} catch (Exception ex) {
							popup("#=LExcCannotDelete", ex.getMessage());
						}
					}
					goToMain();
					break;
			}
		}
	}

#include "DB.j"
}
