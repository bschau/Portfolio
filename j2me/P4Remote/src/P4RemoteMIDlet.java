import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import javax.microedition.lcdui.*;
import javax.microedition.midlet.*;
import javax.microedition.rms.RecordStore;
import javax.microedition.rms.RecordStoreException;
import javax.microedition.io.Connector;
import javax.microedition.io.Connection;
import javax.wireless.messaging.MessageConnection;
import javax.wireless.messaging.TextMessage;

public class P4RemoteMIDlet extends MIDlet implements CommandListener {
	private static final String STORENAME="Preferences";
	private static final String SCDeactivate="120";
	private static final String SCFull="121";
	private static final String SCPartial="122";
	private static final String SCShell="123";
	protected boolean started=false;
	private Form fAbout=null, fSettings=null, fPassword=null;
	private Display display;
	private Command cmdOK, cmdBack, cmdSend;
	private List lstOptions;
	private Alert alert;
	private String cGSM, cDeactivate, cFull, cPartial, cShell, cPort;
	private String command, extra;
	private TextField tfGSM, tfDeactivate, tfFull, tfPartial, tfShell, tfPort;
	private TextField tfPIN;

	protected void startApp() {
		boolean runInit=true;

		if (!started) {
			display=Display.getDisplay(this);

			GetSettings();
			cmdOK=new Command("${TCmdOK}", Command.OK, 0);
			cmdBack=new Command("${TCmdBack}", Command.OK, 0);
			cmdSend=new Command("${TCmdSend}", Command.OK, 0);

			lstOptions=new List("", List.IMPLICIT);
			lstOptions.append("${TLstDeactivation}", null);
			lstOptions.append("${TLstFull}", null);
			lstOptions.append("${TLstPartial}", null);
			lstOptions.append("${TLstShell}", null);
			lstOptions.append("${TLstSettings}", null);
			lstOptions.append("${TLstAbout}", null);
			lstOptions.append("${TLstExit}", null);
			lstOptions.setCommandListener(this);

			started=true;
			System.gc();
			display.setCurrent(lstOptions);
		}
	}

	protected void pauseApp() {
		notifyDestroyed();
	}

	protected void destroyApp(boolean unconditional) {}

	private void GetSettings() {
		String s;

		cGSM=null;
		cDeactivate=null;
		cFull=null;
		cPartial=null;
		cShell=null;
		cPort=null;

		try {
			RecordStore r=RecordStore.openRecordStore(STORENAME, true);
			if (r.getNumRecords()>0) {
				byte[] b=r.getRecord(1);
				DataInputStream d=new DataInputStream(new ByteArrayInputStream(b));
				try {
					int i=0, c, len;
					byte[] buf=new byte[64];

					while (i<b.length) {
						c=d.read();
						len=d.read();
						if (len>buf.length)
							break;

						i+=2;
						if (len>0) {
							d.read(buf, 0, len);
							i+=len;
							s=new String(buf, 0, len);
							switch (c) {
							case 'G':
								cGSM=s;
								break;
							case 'D':
								cDeactivate=s;
								break;
							case 'F':
								cFull=s;
								break;
							case 'P':
								cPartial=s;
								break;
							case 'S':
								cShell=s;
								break;
							case 'p':
								cPort=s;
								break;
							default:
								break;
							}	
						}	
					}
				} catch (IOException ex) {}
			}

			r.closeRecordStore();
		} catch (Exception ex) {}	
	}

	private void SetSettings() {
		try {
			RecordStore r=RecordStore.openRecordStore(STORENAME, true);
			ByteArrayOutputStream b=new ByteArrayOutputStream();
			DataOutputStream d=new DataOutputStream(b);

			SetProperty(d, cGSM, 'G');
			SetProperty(d, cDeactivate, 'D');
			SetProperty(d, cFull, 'F');
			SetProperty(d, cPartial, 'P');
			SetProperty(d, cShell, 'S');
			SetProperty(d, cPort, 'p');

			byte[] a=b.toByteArray();
			if (r.getNumRecords()>0)
				r.setRecord(1, a, 0, a.length);
			else
				r.addRecord(a, 0, a.length);

			r.closeRecordStore();
		} catch (Exception ex) {}
	}

	private void SetProperty(DataOutputStream d, String s, char b) {
		try {
			d.writeByte((byte)b);
			if (s!=null) {
				d.writeByte((byte)s.length());
				d.write(s.getBytes(), 0, s.length());
			} else
				d.writeByte(0);
		} catch (Exception ex) {}
	}

	public void commandAction(Command c, Displayable d) {
		String s;

		if (d==fPassword) {
			if (c==cmdSend) {
				if (tfPIN.size()!=4) {
					ShowAlert("${TAlertPINTitle}", "${TAlertPINText}", AlertType.ERROR);
					return;
				}
	
				SendSMS(tfPIN.getString());
			}
			fPassword=null;
			System.gc();
			display.setCurrent(lstOptions);
			return;
		}
	       
		if (d==fSettings) {
			if (c==cmdOK) {
				if ((cGSM=tfGSM.getString())!=null) {
					if (cGSM.length()>0) {
						if ((cGSM=ValidateField("${TFormSettingsGSM}", cGSM))==null)
							return;
					} else
						cGSM=null;
				}
				
				if ((cDeactivate=tfDeactivate.getString())!=null) {
					if (cDeactivate.length()>0) {
						if ((cDeactivate=ValidateField("${TFormSettingsDeactivate}", cDeactivate))==null)
							return;
					} else
					cDeactivate=null;
				}
			
				if ((cFull=tfFull.getString())!=null) {
					if (cFull.length()>0) {
						if ((cFull=ValidateField("${TFormSettingsDeactivate}", cFull))==null)
							return;
					} else
						cFull=null;
				}
			
				if ((cPartial=tfPartial.getString())!=null) {
					if (cPartial.length()>0) {
						if ((cPartial=ValidateField("${TFormSettingsPartial}", cPartial))==null)
							return;
					} else
						cPartial=null;
				}
			
				if ((cShell=tfShell.getString())!=null) {
					if (cShell.length()>0) {
						if ((cShell=ValidateField("${TFormSettingsShell}", cShell))==null)
							return;
					} else
						cShell=null;
				}

				cPort=tfPort.getString();
				SetSettings();
			}
			
			fSettings=null;
			System.gc();
			display.setCurrent(lstOptions);
			return;
		}

		if (d==lstOptions) {
			switch (lstOptions.getSelectedIndex()) {
			case 0:	// Deactivate
				SetupSendCommand("${TSCDeactivate}", SCDeactivate, cDeactivate);
				break;
			case 1:	// Full
				SetupSendCommand("${TSCFull}", SCFull, cFull);
				break;
			case 2:	// Partial
				SetupSendCommand("${TSCPartial}", SCPartial, cPartial);
				break;
			case 3:	// Shell
				SetupSendCommand("${TSCShell}", SCShell, cShell);
				break;
			case 4:	// Settings
				SettingsForm();
				break;
			case 5:	// About
				if (fAbout==null) {
					fAbout=new Form("${TFormAbout}");
					fAbout.append("P4Remote 1r1\n");
					fAbout.append("${TFormWritten}:\nBrian Schau\n");
					fAbout.append("${TFormVisit}:\nhttp://www.schau.com/midlets/p4remote/");
					fAbout.addCommand(cmdOK);
					fAbout.setCommandListener(this);
				}
				display.setCurrent(fAbout);
				break;
			default:
				notifyDestroyed();
				break;
			}
		} else
			display.setCurrent(lstOptions);
	}

	private String ValidateField(String fld, String v) {
		if (v.indexOf('#')==-1)
			return v;

		ShowAlert("${VFTitle}", "${VFPre} "+fld+"${VFPost}", AlertType.ERROR);
		return null;
	}

	private void SettingsForm() {
		if (fSettings!=null)
			fSettings=null;

		System.gc();
		fSettings=new Form("${TFormSettings}");

		tfGSM=new TextField("${TFormSettingsGSM}", (cGSM==null ? null : cGSM), 30, TextField.PHONENUMBER);
		fSettings.append(tfGSM);

		tfDeactivate=new TextField("${TFormSettingsDeactivate}", (cDeactivate==null ? null : cDeactivate), 43, TextField.ANY);
		fSettings.append(tfDeactivate);

		tfFull=new TextField("${TFormSettingsFull}", (cFull==null ? null : cFull), 43, TextField.ANY);
		fSettings.append(tfFull);

		tfPartial=new TextField("${TFormSettingsPartial}", (cPartial==null ? null : cPartial), 43, TextField.ANY);
		fSettings.append(tfPartial);

		tfShell=new TextField("${TFormSettingsShell}", (cShell==null ? null : cShell), 43, TextField.ANY);
		fSettings.append(tfShell);

		tfPort=new TextField("${TFormSettingsPort}", (cPort==null ? null : cPort.toString()), 5, TextField.NUMERIC);
		fSettings.append(tfPort);

		fSettings.addCommand(cmdOK);
		fSettings.setCommandListener(this);
		display.setCurrent(fSettings);
	}

	private void ShowAlert(String title, String text, AlertType type) {
		if (alert!=null)
			alert=null;

		System.gc();

		alert=new Alert(title, text, null, type);
		alert.setTimeout(Alert.FOREVER);
		display.setCurrent(alert);
	}

	private void SetupSendCommand(String desc, String cmd, String ex) {
		if (cGSM==null || cGSM.length()==0) {
			ShowAlert("${TSCTitle}", "${TSCText}", AlertType.ERROR);
			return;
		}

		command=cmd;
		extra=ex;
		if (fPassword!=null)
			fPassword=null;

		System.gc();
		fPassword=new Form(desc);
		tfPIN=new TextField("${TFormPasswordPIN}", null, 4, TextField.NUMERIC|TextField.PASSWORD);
		fPassword.append(tfPIN);
		fPassword.addCommand(cmdSend);
		fPassword.addCommand(cmdBack);
		fPassword.setCommandListener(this);
		display.setCurrent(fPassword);
	}

	private void SendSMS(String pin) {
		String c=extra+"#"+pin, addr="sms://"+cGSM;
		MessageConnection conn=null;
		TextMessage msg;

		if (cPort!=null)
			addr+=":"+cPort;

		c+=command;
		try {
			conn=(MessageConnection)Connector.open(addr);
			msg=(TextMessage)conn.newMessage(MessageConnection.TEXT_MESSAGE);
			msg.setPayloadText(c);
			conn.send(msg);
		} catch (Exception ex) {
			ShowAlert("${TAlertSendSMS}", ex.getMessage(), AlertType.ERROR);
		} finally {
			if (conn!=null) {
				try {
					conn.close();
				} catch (Exception ex) {}
			}
		}
	}	
}
