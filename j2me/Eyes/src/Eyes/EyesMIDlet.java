package Eyes;

import java.util.Random;
import java.lang.Math;
import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import javax.microedition.lcdui.*;
import javax.microedition.midlet.*;
import javax.microedition.rms.RecordStore;
import javax.microedition.rms.RecordStoreException;
import com.nokia.mid.ui.DirectGraphics;
import com.nokia.mid.ui.DirectUtils;
import com.nokia.mid.ui.FullCanvas;
import com.nokia.mid.sound.Sound;

public class EyesMIDlet extends MIDlet implements CommandListener {
	private static final String STORENAME="Preferences";
	private static final byte[] SndCrash={
		(byte)0x02, (byte)0x4a, (byte)0x3a, (byte)0x61,
		(byte)0x8d, (byte)0xbd, (byte)0xb5, (byte)0xc1,
		(byte)0xbd, (byte)0xcd, (byte)0x95, (byte)0xc8,
		(byte)0x04, (byte)0x00, (byte)0x15, (byte)0x1c,
		(byte)0xc4, (byte)0x16, (byte)0xa1, (byte)0xca,
		(byte)0x24, (byte)0x9d, (byte)0x10, (byte)0x72,
		(byte)0x86, (byte)0x22, (byte)0x00
	};
	private static final int nBits[]={ 1, 2, 4, 8, 16, 32, 64, 128 };
	private static final int DispW=96;
	private static final int DispH=9;
	private static final int CNextFloor=12;
	private static final int HolesMax=4;
	private static final int HoleSize=20;
	private static final int MaxDiamonds=3;
	private static final int DiamondX=9;
	private static final int DiamondY=8;
	private static final int CEyesX=11;
	private static final int CEyesY=9;
	private static final int CEyesBase=3;
	private static final int CEyesBaseLen=5;
	private static final int CEyesMaxY=((((DispH-1)*8)-CEyesY+1)*100);
	private static final int COopsW=29;
	private static final int COopsH=7;

	protected boolean started=false;
	private RecordStore prfRS=null;
	private Form fSorry=null, fInstructions=null, fAbout=null, fSound=null;
	private Canvas cIntro=null;
	private HiScore cHiScore=null;
	private Display display;
	private Command cmdBack, cmdBummer, cmdOptions;
	private Eyes eyes;
	private int hiScore, sndOptions;
	private List lstOptions;
	private ChoiceGroup cgSound;
	private Image imgDiamond, imgELeft, imgERight, imgEMiddle, imgEDown;
	private Image imgOops;
	private Font smlFont;

	protected void startApp() {
		if (!started) {
			started=true;
			display=Display.getDisplay(this);

			try {
				Class.forName("com.nokia.mid.ui.DirectGraphics");
				Class.forName("com.nokia.mid.ui.DirectUtils");
				Class.forName("com.nokia.mid.ui.FullCanvas");
				Class.forName("com.nokia.mid.sound.Sound");
				Init();
			} catch (Exception ex) {
				fSorry=new Form("Sorry ...");
				fSorry.append("\nEyes cannot be run on this device as it lacks support for the Nokia UI Toolkit.\n");

				cmdBummer=new Command("Bummer", Command.OK, 0);
				fSorry.addCommand(cmdBummer);
				fSorry.setCommandListener(this);
				display.setCurrent(fSorry);
			}
		}
	}

	public void Init() {
		PreferencesOpen();
		PreferencesGet();

		cmdBack=new Command("Back", Command.BACK, 0);
		cmdOptions=new Command("Options", Command.OK, 0);

		lstOptions=new List("", List.IMPLICIT);
		lstOptions.append("Play", null);
		lstOptions.append("High Score", null);
		lstOptions.append("Sound", null);
		lstOptions.append("Instructions", null);
		lstOptions.append("About", null);
		lstOptions.append("Exit", null);
		lstOptions.setCommandListener(this);

		cIntro=new Intro();
		cIntro.addCommand(cmdOptions);
		cIntro.setCommandListener(this);

		cHiScore=new HiScore();
		((Canvas)cHiScore).addCommand(cmdOptions);
		((Canvas)cHiScore).setCommandListener(this);

		smlFont=Font.getFont(Font.FACE_PROPORTIONAL, Font.STYLE_PLAIN, Font.SIZE_SMALL);
		try {
			imgDiamond=Image.createImage("/Eyes/diamond.png");
			imgELeft=Image.createImage("/Eyes/left.png");
			imgERight=Image.createImage("/Eyes/right.png");
			imgEMiddle=Image.createImage("/Eyes/middle.png");
			imgEDown=Image.createImage("/Eyes/down.png");
			imgOops=Image.createImage("/Eyes/oops.png");
		} catch (Exception ex) {}

		display.setCurrent(cIntro);
	}

	protected void pauseApp() {
		notifyDestroyed();
	}

	protected void destroyApp(boolean unconditional) {
		PreferencesClose();
	}

	private void PreferencesOpen() {
		try {
			prfRS=RecordStore.openRecordStore(STORENAME, true);
		} catch (RecordStoreException rse) {}
	}

	private void PreferencesClose() {
		if (prfRS!=null) {
			PreferencesSet();
			try {
				prfRS.closeRecordStore();
				prfRS=null;
			} catch (RecordStoreException rse) {}
		}
	}

	private void PreferencesGet() {
		if (prfRS!=null) {
			try {
				if (prfRS.getNumRecords()>0) {
					byte[] b=prfRS.getRecord(1);
					DataInputStream d=new DataInputStream(new ByteArrayInputStream(b));
					try {
						hiScore=d.readInt();
						sndOptions=d.readInt();
					} catch (IOException ex) {}
				} else {
					hiScore=0;
					sndOptions=1;
				}
			} catch (RecordStoreException rse) {}
		}
	}

	private void PreferencesSet() {
		if (prfRS!=null) {
			ByteArrayOutputStream b=new ByteArrayOutputStream();
			DataOutputStream d=new DataOutputStream(b);

			try {
				d.writeInt(hiScore);
				d.writeInt(sndOptions);
			} catch (IOException ex) {}

			byte[] a=b.toByteArray();
			try {
				if (prfRS.getNumRecords()>0)
					prfRS.setRecord(1, a, 0, a.length);
				else
					prfRS.addRecord(a, 0, a.length);
			} catch (RecordStoreException rse) {}
		}
	}

	public void commandAction(Command c, Displayable d) {
		if (c==cmdBummer) {
			notifyDestroyed();
		} else if (c==cmdOptions) {
			if (d==cIntro)
				cIntro=null;

			display.setCurrent(lstOptions);
		} else if (d==lstOptions) {
			switch (lstOptions.getSelectedIndex()) {
			case 0:
				eyes=new Eyes();
				display.setCurrent(eyes);
				break;
			case 1:
				cHiScore.ShowHiScore();
				display.setCurrent(cHiScore);
				break;
			case 2:
				if (fSound==null) {
					fSound=new Form("Sound");
					cgSound=new ChoiceGroup("Play sounds?", ChoiceGroup.EXCLUSIVE, new String[] { "No", "Yes" }, null);
					cgSound.setSelectedIndex(sndOptions, true);
					fSound.append(cgSound);
					fSound.addCommand(cmdBack);
					fSound.setCommandListener(this);
				}
				display.setCurrent(fSound);
				break;
			case 3:
				if (fInstructions==null) {
					fInstructions=new Form("Instructions");
					fInstructions.append("\nSteer the eyes left and right and attempt to escape through the holes to the next floor.\n\n");
					fInstructions.append("Avoid being smashed between a floor and the top of the screen.\n\n");
					fInstructions.append("Avoid falling out of the bottom of the screen.\n\n");
					fInstructions.append("Use left side keys to move left.\nUse right side keys to move right.\n\n");
					fInstructions.append("You only have one pair of eyes ...\n\n");
					fInstructions.addCommand(cmdOptions);
					fInstructions.setCommandListener(this);
				}
				display.setCurrent(fInstructions);
				break;
			case 4:
				if (fAbout==null) {
					fAbout=new Form("About");
					fAbout.append("Eyes 1r1\n\n");
					fAbout.append("Written by:\n  Brian Schau\n\n");
					fAbout.append("Please visit the website at:\n\n");
					fAbout.append("http://www.schau.com/midlets/eyes/\n\n");
					fAbout.addCommand(cmdOptions);
					fAbout.setCommandListener(this);
				}
				display.setCurrent(fAbout);
				break;
			default:
				notifyDestroyed();
				break;
			}
		} else if (c==cmdBack) {
			sndOptions=cgSound.getSelectedIndex();
			display.setCurrent(lstOptions);
		}
	}

class Intro extends Canvas {
	private static final String title="Eyes";
	public void paint(Graphics g) {
		int h=getHeight();
		Font f=g.getFont();
		int fS, top;
		Image img;

		fS=f.getHeight();
		top=(h-32-fS-4)/2;
		try {
			img=Image.createImage("/Eyes/eyes.png");
			g.drawImage(img, (DispW-44)/2, top, Graphics.TOP|Graphics.LEFT);
			img=null;
			top+=36+4;
		} catch (Exception ex) {}

		g.drawString(title, (DispW-f.stringWidth(title))/2, top, 0);
	}
}

class HiScore extends Canvas {
	private static final String txtHiScore="Hi Score";
	private static final String txtGameOver="* Game Over *";
	private static final String txtNewHiScore="New Hi: ";
	private static final String txtScore="Score: ";
	private boolean showHiScore=false, newHiScore=false;
	private int score;

	public void ShowHiScore() {
		showHiScore=true;
		newHiScore=false;
	}

	public void ShowGameOver(int s) {
		eyes=null;
		showHiScore=false;
		score=s;
		if (s>=hiScore) {
			hiScore=s;
			newHiScore=true;
			PreferencesSet();
		} else
			newHiScore=false;
	}

	public void paint(Graphics g) {
		int h=getHeight();
		Font fntDefault=g.getFont();
		Font fntLarge=Font.getFont(Font.FACE_SYSTEM, Font.STYLE_PLAIN, Font.SIZE_LARGE);
		int fDs, fLs, top;
		String s;

		fDs=fntDefault.getHeight();
		fLs=fntLarge.getHeight();

		s=Integer.toString(hiScore);
		if (showHiScore==true) {
			top=(h-fLs-6-fLs)/2;
			g.setFont(fntLarge);
			g.drawString(txtHiScore, (DispW-fntLarge.stringWidth(txtHiScore))/2, top, 0);
			top+=6+fLs;
			g.drawString(s, (DispW-fntLarge.stringWidth(s))/2, top, 0);
		} else {
			top=(h-fLs-6-fDs)/2;
			g.setFont(fntLarge);
			g.drawString(txtGameOver, (DispW-fntLarge.stringWidth(txtGameOver))/2, top, 0);

			g.setFont(fntDefault);
			top+=fLs+6;

			if (newHiScore==true)
				s=txtNewHiScore+Integer.toString(hiScore);
			else
				s=txtScore+Integer.toString(score);

			g.setFont(fntDefault);
			g.drawString(s, (DispW-fntDefault.stringWidth(s))/2, top, 0);
		}
	}
}

class Eyes extends FullCanvas implements Runnable {
	private	Thread eyesThread;
	private DirectGraphics dg;
	private Image screen;
	private byte playField[]=new byte[DispW*DispH];
	private Random random;
	private Sound sound;
	private boolean runThread;
	private int nextFloor, refresh, holes;
	private int eyesX, eyesY, eyesXSpd, eyesYSpd;
	private boolean eyesFalling, eyesMoveLeft, eyesMoveRight;
	private boolean eyesSticky, eyesDying;
	private int[] diamonds=new int[MaxDiamonds*2];
	private int score, animCnt, superSticky;

	Eyes() {
		int i, base;

		screen=Image.createImage(DispW, DispH*8);
		dg=DirectUtils.getDirectGraphics(screen.getGraphics());

		for (i=0; i<(DispW*DispH); i++)
			playField[i]=0;

		base=((DispH-2)*DispW)+HoleSize;
		for (i=0; i<(DispW-HoleSize-HoleSize); i++) {
			playField[base]=(byte)3;
			base++;
		}

		sound=new Sound(0, 1);
		random=new Random();
		eyesX=((DispW-CEyesX)/2)*100;
		eyesY=0;
		eyesXSpd=0;
		eyesYSpd=0;
		eyesFalling=true;
		eyesMoveRight=false;
		eyesMoveLeft=false;
		eyesDying=false;
		eyesSticky=false;
		refresh=70;
		nextFloor=CNextFloor;
		holes=1;
		score=0;
		superSticky=0;

		for (i=0; i<(MaxDiamonds*2); i++)
			diamonds[i]=-100;

		runThread=true;
		eyesThread=new Thread(this);
		eyesThread.start();
	}

	protected void keyPressed(int keyCode) {
		switch (keyCode) {
		case Canvas.KEY_NUM1:
		case Canvas.KEY_NUM4:
		case Canvas.KEY_NUM7:
		case Canvas.KEY_STAR:
		case Canvas.LEFT:
			eyesMoveRight=false;
			eyesMoveLeft=true;
			break;
		case Canvas.KEY_NUM3:
		case Canvas.KEY_NUM6:
		case Canvas.KEY_NUM9:
		case Canvas.KEY_POUND:
		case Canvas.RIGHT:
			eyesMoveRight=true;
			eyesMoveLeft=false;
			break;
		default:
			eyesMoveRight=false;
			eyesMoveLeft=false;
		}
	}

	protected void keyReleased(int keyCode) {
		switch (keyCode) {
		case Canvas.KEY_NUM1:
		case Canvas.KEY_NUM4:
		case Canvas.KEY_NUM7:
		case Canvas.KEY_STAR:
		case Canvas.LEFT:
			eyesMoveLeft=false;
			break;
		case Canvas.KEY_NUM3:
		case Canvas.KEY_NUM6:
		case Canvas.KEY_NUM9:
		case Canvas.KEY_POUND:
		case Canvas.RIGHT:
			eyesMoveRight=false;
			break;
		}
	}
		
	protected void paint(Graphics g) {
		int i, oX, oY;
		Image p;
		String s;

		g.drawImage(screen, 0, 0, 0);

		g.setFont(smlFont);
		s=Integer.toString(score);
		g.drawString(s, (DispW-smlFont.stringWidth(s)), 0, 0);

		for (i=0; i<(MaxDiamonds*2); i+=2) {
			if (diamonds[i]>=0 && diamonds[i+1]>-DiamondY)
				g.drawImage(imgDiamond, diamonds[i], diamonds[i+1], Graphics.TOP|Graphics.LEFT);
		}

		if (eyesDying==false) {
			if (eyesMoveLeft==true)
				p=imgELeft;
			else if (eyesMoveRight==true)
				p=imgERight;
			else if (eyesFalling==true)
				p=imgEDown;
			else
				p=imgEMiddle;

			g.drawImage(p, (eyesX/100), (eyesY/100), Graphics.LEFT|Graphics.TOP);
		} else {
			g.drawImage(imgEMiddle, (eyesX/100), (eyesY/100), Graphics.LEFT|Graphics.TOP);

			oX=(eyesX/100)+(CEyesX/2)+1-(COopsW/2);
			if (oX<0)
				oX=0;
			else if (oX>(DispW-COopsW))
				oX=DispW-COopsW;

			if (eyesY<0)
				oY=(eyesY/100)+CEyesY+1;
			else
				oY=(eyesY/100)-COopsH-1;

			if ((animCnt&1)==0) {
				g.setColor(0xffffff);
				g.fillRect(oX, oY, COopsW, COopsH);
			} else
				g.drawImage(imgOops, oX, oY, Graphics.TOP|Graphics.LEFT);
		} 
	}

	public void run() {
		long startTime, runTime;

		while (runThread==true) {
		try {
			startTime=System.currentTimeMillis();

			if (score>800) {
				holes=4;
				superSticky=5;
				refresh=30;
			} else if (score>400) {
				holes=3;
				superSticky=3;
				refresh=45;
			} else if (score>100) {
				holes=2;
				superSticky=1;
				refresh=60;
			}

			EyesMovement();
			if (eyesDying==false) {
				eyesY=(eyesY/100)*100;
				PickUpDiamonds();
				ScrollUp();
			}

			dg.drawPixels(playField, null, 0, DispW, 0, 0, DispW, DispH*8, 0, DirectGraphics.TYPE_BYTE_1_GRAY_VERTICAL);
			repaint();
			runTime=System.currentTimeMillis()-startTime;
			if (runTime<refresh)
				eyesThread.sleep(refresh-runTime);
		} catch (InterruptedException ex) {}
		}

		if (sndOptions==1) {
			sound.init(SndCrash, Sound.FORMAT_TONE);
			sound.play(1);
		}

		for (animCnt=0; animCnt<12; animCnt++) {
			animCnt++;
			repaint();

			try {
			eyesThread.sleep(200);
			} catch (InterruptedException ex) {}
		}

		cHiScore.ShowGameOver(score);
		display.setCurrent(cHiScore);
	}

	private int EyesRandom(int limit) {
		int r=random.nextInt();

		if (r<0)
			r=Math.abs(r);

		return r%limit;
	}

	private void ScrollUp() {
		int r1, r2, r3, r4, r5, r6, r7, r8, r9, x;
		byte b;

		score++;

		r1=0;
		r2=r1+DispW;
		r3=r2+DispW;
		r4=r3+DispW;
		r5=r4+DispW;
		r6=r5+DispW;
		r7=r6+DispW;
		r8=r7+DispW;
		r9=r8+DispW;
	
		for (x=0; x<DispW; x++) {
			playField[r1+x]>>=1;
			if ((playField[r2+x]&1)==1)
				playField[r1+x]|=(byte)128;
			else
				playField[r1+x]&=(byte)127;
			
			playField[r2+x]>>=1;
			if ((playField[r3+x]&1)==1)
				playField[r2+x]|=(byte)128;
			else
				playField[r2+x]&=(byte)127;
			
			playField[r3+x]>>=1;
			if ((playField[r4+x]&1)==1)
				playField[r3+x]|=(byte)128;
			else
				playField[r3+x]&=(byte)127;
			
			playField[r4+x]>>=1;
			if ((playField[r5+x]&1)==1)
				playField[r4+x]|=(byte)128;
			else
				playField[r4+x]&=(byte)127;

			playField[r5+x]>>=1;
			if ((playField[r6+x]&1)==1)
				playField[r5+x]|=(byte)128;
			else
				playField[r5+x]&=(byte)127;

			playField[r6+x]>>=1;
			if ((playField[r7+x]&1)==1)
				playField[r6+x]|=(byte)128;
			else
				playField[r6+x]&=(byte)127;

			playField[r7+x]>>=1;
			if ((playField[r8+x]&1)==1)
				playField[r7+x]|=(byte)128;
			else
				playField[r7+x]&=(byte)127;

			playField[r8+x]>>=1;
			if ((playField[r9+x]&1)==1)
				playField[r8+x]|=(byte)128;
			else
				playField[r8+x]&=(byte)127;

			playField[r9+x]>>=1;
			playField[r9+x]&=(byte)127;
		}

		if (eyesFalling==false)
			eyesY-=100;

		r3=0;
		for (x=0; x<MaxDiamonds; x++) {
			if (diamonds[r3+1]>-DiamondY)
				diamonds[r3+1]--;
			else {
				diamonds[r3]=-100;
				diamonds[r3+1]=-100;
			}

			r3+=2;
		}

		if (nextFloor==0) {
			if (EyesRandom(10)<=superSticky) {
				for (x=0; x<DispW; x+=2) {
					playField[r9+x]=(byte)128;
					playField[r9+1+x]=(byte)64;
				}
			} else {
				for (x=0; x<DispW; x++)
					playField[r9+x]=(byte)192;
			}

			for (x=0; x<holes; x++) {
				r1=EyesRandom(DispW-HoleSize);
				for (r2=r1; r2<(r1+HoleSize); r2++)
					playField[r9+r2]=0;
			}

			PopupDiamond();

			nextFloor=CNextFloor;
		} else
			nextFloor--;
	}

	private void PopupDiamond() {
		int base=DispW*(DispH-1), x, attempts, bLine, tryX, i;

		if (EyesRandom(4)!=0)
			return;

		for (x=0; x<MaxDiamonds; x++) {
			if (diamonds[x*2]==-100) {
				for (attempts=0; attempts<4; attempts++) {
					tryX=EyesRandom(DispW-DiamondX);
					bLine=0;
					for (i=tryX; i<(tryX+DiamondX); i++) {
						if (playField[base+i]!=0)
							bLine++;
					}

					if (bLine>(DiamondX/2)) {
						diamonds[x*2]=tryX;
						diamonds[(x*2)+1]=((DispH-1)*8)-1;
						return;
					}
				}
			}
		}
	}

	private void EyesMovement() {
		int spdMax=400, x, base, fSpd, t, bS, bit, i;

		if (eyesY<0 || eyesY>CEyesMaxY) {
			eyesDying=true;
			runThread=false;
			return;
		}

		eyesY=(eyesY/100)*100;
		if (eyesFalling==true)
			eyesSticky=false;

		if (eyesSticky==true) {
			if (eyesXSpd<-200)
				eyesXSpd=-200;
			else if (eyesXSpd>200)
				eyesXSpd=200;

			spdMax=200;
		}

		if (eyesMoveLeft==true) {
			if (eyesX>0) {
				if (eyesXSpd>-spdMax)
					eyesXSpd-=50;
			}
		} else if (eyesMoveRight==true) {
			if (eyesX<(DispW*100)) {
				if (eyesXSpd<spdMax)
					eyesXSpd+=50;
			}
		} else {
			if (eyesSticky==false) {
				if (eyesXSpd<0) {
					eyesXSpd+=100;
					if (eyesXSpd>0)
						eyesXSpd=0;
				} else if (eyesXSpd>0) {
					eyesXSpd-=100;
					if (eyesXSpd<0)
						eyesXSpd=0;
				}
			} else
				eyesXSpd=0;
		}

		if (eyesXSpd!=0) {
			eyesX+=eyesXSpd;
			if (eyesX<0) {
				eyesX=0;
				eyesXSpd=0;
			} else if (eyesX>((DispW-CEyesX)*100)) {
				eyesX=(DispW-CEyesX)*100;
				eyesXSpd=0;
			}
		}

		if (eyesFalling==true) {
			if (eyesYSpd<400)
				eyesYSpd+=50;

			fSpd=eyesYSpd;
			fSpd/=100;
			fSpd*=100;
			while (fSpd>0) {
				if (eyesY>CEyesMaxY) {
					eyesDying=true;
					runThread=true;
					return;
				}

				base=(eyesY/100)+CEyesY;

				bit=base%8;
				base/=8;
				base*=DispW;
				base+=(eyesX/100)+CEyesBase;

				bS=0;
				for (i=0; i<CEyesBaseLen; i++)
					bS|=playField[base+i]&nBits[bit];

				if (bS==0) {
					eyesY+=100;
					fSpd-=100;
				} else {
					bS=0;
					for (i=0; i<CEyesBaseLen; i++) {
						bS|=GetBottomBits(base, bit);
						base++;
					}

					eyesFalling=false;
					if (bS==1)
						eyesSticky=true;
					else if (bS==2)
						eyesSticky=false;

					return;
				}
			}
		} else {
			base=(eyesY/100)+CEyesY;
			bit=base%8;
			base/=8;
			base*=DispW;
			base+=(eyesX/100)+CEyesBase;
			bS=0;
			for (i=0; i<CEyesBaseLen; i++)
				bS|=GetBottomBits(base+i, bit);

			if (bS==0) {
				eyesFalling=true;
				eyesSticky=false;
				eyesYSpd=0;
			} else if (bS==1)
				eyesSticky=true;
			else if (bS==2)
				eyesSticky=false;
		}
	}

	private int GetBottomBits(int base, int bit) {
		int b=0;

		if ((playField[base]&nBits[bit])>0)
			b=1;

		if (bit>6) {
			bit=0;
			base+=DispW;
		} else
			bit++;

		if ((playField[base]&nBits[bit])>0)
			b|=2;

		if (b==1 || b==2)
			return 1;

		if (b==3)
			return 2;

		return 0;
	}

	private void PickUpDiamonds() {
		int i;

		for (i=0; i<(MaxDiamonds*2); i+=2) {
			if (diamonds[i]>=0 && diamonds[i+1]>-DiamondY) {
				if (DiamondOverlap(diamonds[i], diamonds[i+1])==true) {
					diamonds[i]=-100;
					diamonds[i+1]=-100;
					score+=5;
					if (sndOptions==1) {
						sound.init(622, 75);
						sound.play(1);
					}
				}
			}
		}
	}

	private boolean DiamondOverlap(int dX, int dY) {
		int eX=eyesX/100, eY=eyesY/100;
		int diaX=dX+DiamondX, diaY=dY+DiamondY;
		int eW=CEyesX, eH=CEyesY;

		eW+=eX;
		eH+=eY;

		//    dddd
		// EEEEddd
		// EEEEddd
		// EEEEddd
		// EEEE
		if (eW>=dX && eW<=diaX && eY>=dY && eY<=diaY)
			return true;

		// dddd
		// dddEEEE
		// dddEEEE
		// dddEEEE
		//    EEEE
		if (eX>=dX && eX<=diaX && eY>=dY && eY<=diaY)
			return true;

		// EEEE
		// EEEEddd
		// EEEEddd
		// EEEEddd
		//    dddd
		if (eW>=dX && eW<=diaX && eH>=dY && eH<=diaY)
			return true;

		//    EEEE
		// dddEEEE
		// dddEEEE
		// dddEEEE
		// dddd
		if (eX>=dX && eX<=diaX && eH>=dY && eH<=diaY)
			return true;

		return false;
	}
}

}
