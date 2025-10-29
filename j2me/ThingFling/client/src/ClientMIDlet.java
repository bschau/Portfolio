#include "Include.j"
#if (ant:lang=="en")
#include "locale/en.j"
#elif (ant:lang=="da")
#include "locale/da.j"
#endif
import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.InputStream;
import java.lang.Thread;
import javax.microedition.lcdui.Alert;
import javax.microedition.lcdui.AlertType;
import javax.microedition.lcdui.Command;
import javax.microedition.lcdui.CommandListener;
import javax.microedition.lcdui.ChoiceGroup;
import javax.microedition.lcdui.Display;
import javax.microedition.lcdui.Displayable;
import javax.microedition.lcdui.Font;
import javax.microedition.lcdui.Form;
import javax.microedition.lcdui.Gauge;
import javax.microedition.lcdui.Item;
import javax.microedition.lcdui.List;
import javax.microedition.lcdui.Spacer;
import javax.microedition.lcdui.StringItem;
import javax.microedition.lcdui.TextField;
import javax.microedition.io.Connector;
import javax.microedition.io.HttpConnection;
import javax.microedition.midlet.MIDlet;
import javax.microedition.rms.RecordStore;

public class ClientMIDlet extends MIDlet implements CommandListener,Runnable {
	private static final String hexDigits="0123456789ABCDEF";
	private RecordStore dbPrf=null;
	private boolean softError=true, runThread=false;
	private String pin="", uid="";
	private Form fMain, fPrefs, fHTTP, fAbout, fGetPin;
	private Command cmdFling, cmdOptions, cmdBack, cmdCancel, cmdSave;
	private List lstOptions;
	private TextField flingTxt, prfPin, prfUID, tPin;
	private Thread mt;
	private Gauge httpGauge;
	private StringItem httpTxt;
	private StringBuffer httpArgs=new StringBuffer();
	private StringBuffer httpResp=new StringBuffer();
	private HttpConnection http;
	private int prfReadLen;

	/**
	 * Constructor.
	 */
	public ClientMIDlet() {}

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
		killThread();
		dbClose(dbPrf);
	}

	/**
	 * Kill the HTTP thread if it is running.
	 */
	private void killThread() {
		if (mt!=null) {
			runThread=false;

			if (mt.isAlive()) {
				try {
					Thread.yield();
				} catch (Exception e) {}
			}
		}
	}

	/**
	 * Start app. Open and load preferences and build GUI.
	 */
	public void startApp() {
		try {
			dbPrf=dbOpen("preferences", "#=LPrefName");
			loadPreferences();
#include "GUI.j"

			if ((uid==null) || (uid.trim().length()==0)) {
				setDisplayable(fPrefs);
				focus(prfUID);
			} else {
				goToMain();
			}
		} catch (Exception e) {
			softError=false;
			popup("#=LPopupUnrecErr", e.getMessage());
		}
	}

	/**
	 * Create a "standard" application logo.
	 *
	 * @return StringItem representing the logo.
	 */
	private StringItem getLogo() {
		StringItem l=new StringItem("", "Thing Fling", StringItem.PLAIN);
		l.setLayout(StringItem.LAYOUT_CENTER|StringItem.LAYOUT_NEWLINE_AFTER);
		l.setFont(Font.getFont(Font.FACE_PROPORTIONAL, Font.STYLE_BOLD|Font.STYLE_UNDERLINED, Font.SIZE_LARGE));
		return l;
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
	private void popup(String title, String msg) {
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
	private void setDisplayable(Displayable d) {
		Display.getDisplay(this).setCurrent(d);
	}

	/**
	 * Show the main from.
	 */
	private void goToMain() {
		setDisplayable(fMain);
		focus(flingTxt);
	}

	/**
	 * Handle command.
	 *
	 * @param c Command.
	 * @param d Current displayable.
	 */
	public void commandAction(Command c, Displayable d) {
		if (c==Alert.DISMISS_COMMAND) {
			if (softError) {
				goToMain();
			} else {
				notifyDestroyed();
			}
		} else if (c==cmdFling) {
			if (d==fMain) {
				if (flingTxt.size()==0) {
					return;
				}

				if (pin.trim().length()==0) {
					setDisplayable(fGetPin);
					focus(tPin);
					return;
				}
			} else if (d==fGetPin) {
				if ((pin=tPin.getString())==null) {
					pin="";
				}
			}

			httpInit();
			httpAddArg("status", flingTxt.getString());
			setDisplayable(fHTTP);
			mt=new Thread(this);
			mt.start();
		} else if (c==cmdBack) {
			goToMain();
		} else if (c==cmdOptions) {
			setDisplayable(lstOptions);
		} else if (d==lstOptions) {
			switch (lstOptions.getSelectedIndex()) {
				case 0:	// Back
					goToMain();
					break;
				case 1: // Go to mobile facebook
					try {
						platformRequest(getJADProperty("FacebookMobile"));
						notifyDestroyed();
					} catch (Exception ex) {
						popup("#=LPopupMobileFaceErr", ex.getMessage());
					}
					break;
				case 2:	// preferences
					if (pin.length()>0) {
						prfPin.setString(pin);
					}

					if (uid.length()>0) {
						prfUID.setString(uid);
					}

					setDisplayable(fPrefs);
					focus(prfUID);
					break;
				case 3: // About
					setDisplayable(fAbout);
					break;
				default: // Exit
					notifyDestroyed();
					break;
			}
		} else if (d==fPrefs) {
			if (c==cmdSave) {
				savePrefs();
			}
			goToMain();
		} else if ((d==fHTTP) && (c==cmdCancel)) {
			killThread();
			goToMain();
		}
	}

#include "DB.j"
#include "Prefs.j"

	/**
	 * Get the named propery from the JAD file.
	 *
	 * @param p JAD file entry.
	 * @return the value of the entry (or null if not found).
	 */
	public String getJADProperty(String p) {
		return getAppProperty(p);
	}

	/**
	 * Run the HTTP thread. When the thread finishes it will go to the
	 * main form.
	 * All HTTP initializations must be done before starting the thread.
	 * The response will be available in httpResp buffer.
	 */
	public void run() {
		runThread=true;

		try {
			httpGauge.setValue(1);
			httpTxt.setText("#=LRunConnect");
			httpConnect();
			if (runThread) {
				httpGauge.setValue(2);
				httpTxt.setText("#=LRunSending");
				httpSend();

				if (runThread) {
					httpGauge.setValue(3);
					httpTxt.setText("#=LRunGetting");
					httpGetResponse();

					String resp=httpResp.toString();
					if (resp.charAt(0)=='-') {
						httpResp.delete(0, httpResp.length());
						popup("#=LPopupServerError", resp.substring(1));
					} else {
						if (resp.length()>1) {
							if (resp.charAt(1)=='M') {
								int i;

								for (i=2; i<httpResp.length(); i++) {
									if (httpResp.charAt(i)=='\n') {
										break;
									}
								}
								resp=resp.substring(2, i);
								httpResp.delete(0, i);

								popup("#=LPopupMsg", resp);
							}
						} else {
							goToMain();
						}
					}
				}
			}
		} catch (Exception e) {
			popup("#=LPopupNetErr", e.getMessage());
		} finally {
			httpClose();
		}

		mt=null;
	}

#include "Net.j"

}
