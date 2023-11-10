package TicTacToe;

import java.util.Random;
import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import javax.microedition.lcdui.*;
import javax.microedition.midlet.*;
import javax.microedition.rms.RecordStore;
import javax.microedition.rms.RecordStoreException;
import com.nokia.mid.ui.FullCanvas;
import com.nokia.mid.sound.Sound;

public class TicTacToeMIDlet extends MIDlet implements CommandListener {
	private static final String STORENAME="Preferences";
	private static final byte[] SndWinner={
		(byte)0x02, (byte)0x4a, (byte)0x3a, (byte)0x61,
		(byte)0x8d, (byte)0xbd, (byte)0xb5, (byte)0xc1,
		(byte)0xbd, (byte)0xcd, (byte)0x95, (byte)0xc8,
		(byte)0x04, (byte)0x00, (byte)0x15, (byte)0x1c,
		(byte)0xc4, (byte)0x16, (byte)0xa1, (byte)0xca,
		(byte)0x24, (byte)0x9d, (byte)0x10, (byte)0x72,
		(byte)0x86, (byte)0x22, (byte)0x00
	};
	private static final short[][] GameMatrix={
		{ 8, 7, 6 }, { 8, 5, 2 }, { 8, 4, 0 }, { 7, 4, 1 },
		{ 6, 3, 0 }, { 6, 4, 2 }, { 5, 4, 3 }, { 2, 1, 0 }
	};
	private static final int[] boardX={ 1, 20, 39, 1, 20, 39, 1, 20, 39 };
	private static final int[] boardY={ 1, 1, 1, 17, 17, 17, 33, 33, 33 };
	private static final int DispW=96;
	private static final int PrfUserFirst=1;
	private static final int PrfGameEasy=2;
	private static final int PrfSound=4;
	private static final short PPhone=1;
	private static final short PUser=2;
	private static final int ColYou=65;
	private static final int ColMe=84;
	private static final int StoneW=14;
	private static final int StoneH=11;

	protected boolean started=false;
	private RecordStore prfRS=null;
	private Form fSorry=null, fInstructions=null, fAbout=null, fSettings=null;
	private Canvas cIntro=null;
	private Display display;
	private Command cmdBack, cmdBummer, cmdOptions;
	private int settings;
	private List lstOptions;
	private ChoiceGroup cgFirstMove, cgLevel, cgSound;
	private Canvas ttt;
	private Image imgBoard, imgUser, imgPhone, imgYouMe;
	private Image imgIWon, imgYouWon, imgDraw;
	private Image[] imgNumbers=new Image[9];
	private Sound sound;

	protected void startApp() {
		if (!started) {
			started=true;
			display=Display.getDisplay(this);

			try {
				Class.forName("com.nokia.mid.ui.FullCanvas");
				Class.forName("com.nokia.mid.sound.Sound");
				Init();
			} catch (Exception ex) {
				fSorry=new Form("Sorry ...");
				fSorry.append("\nticTacToe cannot be run on this device as it lacks support for the Nokia UI Toolkit.\n");

				cmdBummer=new Command("Bummer", Command.OK, 0);
				fSorry.addCommand(cmdBummer);
				fSorry.setCommandListener(this);
				display.setCurrent(fSorry);
			}
		}
	}

	public void Init() {
		int i;

		PreferencesOpen();
		PreferencesGet();

		try {
			imgBoard=Image.createImage("/TicTacToe/board.png");
			imgUser=Image.createImage("/TicTacToe/white.png");
			imgPhone=Image.createImage("/TicTacToe/black.png");
			imgYouMe=Image.createImage("/TicTacToe/youme.png");
			imgIWon=Image.createImage("/TicTacToe/iwon.png");
			imgYouWon=Image.createImage("/TicTacToe/youwon.png");
			imgDraw=Image.createImage("/TicTacToe/itsadraw.png");

			for (i=1; i<10; i++)
				imgNumbers[i-1]=Image.createImage("/TicTacToe/"+i+".png");
			
		} catch (Exception ex) {}

		if (imgBoard==null)
			return;

		cmdBack=new Command("Back", Command.BACK, 0);
		cmdOptions=new Command("Options", Command.OK, 0);

		lstOptions=new List("", List.IMPLICIT);
		lstOptions.append("Play", null);
		lstOptions.append("Settings", null);
		lstOptions.append("Instructions", null);
		lstOptions.append("About", null);
		lstOptions.append("Exit", null);
		lstOptions.setCommandListener(this);

		cIntro=new Intro();
		cIntro.addCommand(cmdOptions);
		cIntro.setCommandListener(this);

		sound=new Sound(0, 1);

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
						settings=d.readInt();
					} catch (IOException ex) {}
				} else {
					settings=PrfUserFirst|PrfGameEasy|PrfSound;
				}
			} catch (RecordStoreException rse) {}
		}
	}

	private void PreferencesSet() {
		if (prfRS!=null) {
			ByteArrayOutputStream b=new ByteArrayOutputStream();
			DataOutputStream d=new DataOutputStream(b);

			try {
				d.writeInt(settings);
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
				ttt=new TTT();
				display.setCurrent(ttt);
				break;
			case 1:
				if (fSettings==null) {
					fSettings=new Form("Settings");
					cgFirstMove=new ChoiceGroup("First mover is", ChoiceGroup.EXCLUSIVE, new String[] { "You", "Phone" }, null);
					if ((settings&PrfUserFirst)==PrfUserFirst)
						cgFirstMove.setSelectedIndex(0, true);
					else
						cgFirstMove.setSelectedIndex(1, true);

					cgLevel=new ChoiceGroup("Game is", ChoiceGroup.EXCLUSIVE, new String[] { "Easy", "Hard" }, null);
					if ((settings&PrfGameEasy)==PrfGameEasy)
						cgLevel.setSelectedIndex(0, true);
					else
						cgLevel.setSelectedIndex(1, true);
					
					cgSound=new ChoiceGroup("Sound is", ChoiceGroup.EXCLUSIVE, new String[] { "On", "Off" }, null);
					if ((settings&PrfSound)==PrfSound)
						cgSound.setSelectedIndex(0, true);
					else
						cgSound.setSelectedIndex(1, true);

					fSettings.append(cgFirstMove);
					fSettings.append(cgLevel);
					fSettings.append(cgSound);
					fSettings.addCommand(cmdBack);
					fSettings.setCommandListener(this);
				}
				display.setCurrent(fSettings);
				break;
			case 2:
				if (fInstructions==null) {
					fInstructions=new Form("Instructions");
					fInstructions.append("\nPlay the game of Tic Tac Toe against your Mobile Phone.\n\n");
					fInstructions.append("Try to get 3 in a row by placing stones in empty pockets.\n\n");
					fInstructions.append("Press keys 1 to 9 to place stone in pocket.\n\n");
					fInstructions.addCommand(cmdOptions);
					fInstructions.setCommandListener(this);
				}
				display.setCurrent(fInstructions);
				break;
			case 3:
				if (fAbout==null) {
					fAbout=new Form("About");
					fAbout.append("TicTacToe 1r1\n\n");
					fAbout.append("Written by:\n  Brian Schau\n\n");
					fAbout.append("Please visit the website at:\n\n");
					fAbout.append("http://www.schau.com/midlets/tictactoe/\n\n");
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
			settings=0;
			if (cgFirstMove.getSelectedIndex()==0)
				settings|=PrfUserFirst;

			if (cgLevel.getSelectedIndex()==0)
				settings|=PrfGameEasy;

			if (cgSound.getSelectedIndex()==0)
				settings|=PrfSound;

			display.setCurrent(lstOptions);
		}
	}

class Intro extends Canvas {
	public void paint(Graphics g) {
		int h=getHeight();
		Image img;

		try {
			img=Image.createImage("/TicTacToe/tictactoe.png");
			g.drawImage(img, (DispW-71)/2, (h-45)/2, Graphics.TOP|Graphics.LEFT);
			img=null;
		} catch (Exception ex) {}
	}
}

class TTT extends FullCanvas implements Runnable {
	private Thread myself;
	private boolean runThread;
	private Random random;
	private int[] brainLine={ 4, 8, 6, 2, 0, 7, 5, 4, 1 };
	private int[] board=new int[9];
	private int[] moves=new int[9];
	private boolean userMayMove, gameOver;
	private int winner, winLine, animCnt;

	TTT() {
		int i, x, y, t;

		random=new Random();

		for (i=0; i<9; i++) {
			moves[i]=-1;
			board[i]=0;
		}

		if ((settings&PrfGameEasy)==PrfGameEasy) {
			for (i=0; i<100; i++) {
				x=(random.nextInt()&7)+1;
				y=(random.nextInt()&7)+1;
				t=brainLine[x];
				brainLine[x]=brainLine[y];
				brainLine[y]=t;
			}

			x=(random.nextInt()&7)+1;
			brainLine[0]=brainLine[x];
			brainLine[x]=4;
		} else {
			for (i=0; i<100; i++) {
				x=(random.nextInt()&3)+1;
				y=(random.nextInt()&3)+1;
				t=brainLine[x];
				brainLine[x]=brainLine[y];
				brainLine[y]=t;

				x=(random.nextInt()&3)+5;
				y=(random.nextInt()&3)+5;
				t=brainLine[x];
				brainLine[x]=brainLine[y];
				brainLine[y]=t;
			}
		}

		if ((settings&PrfUserFirst)!=PrfUserFirst)
			PhoneMove();

		winner=0;
		animCnt=0;
		userMayMove=true;
		gameOver=false;
		runThread=true;

		myself=new Thread(this);
		myself.start();
	}

	private int FindHole(int c) {
		int i, x, y, z;

		for (i=0; i<8; i++) {
			x=GameMatrix[i][0];
			y=GameMatrix[i][1];
			z=GameMatrix[i][2];

			if (board[x]==c && board[y]==c && board[z]==0)
				return z;

			if (board[x]==c && board[y]==0 && board[z]==c)
				return y;

			if (board[x]==0 && board[y]==c && board[z]==c)
				return x;
		}

		return -1;
	}

	private void PhoneMove() {
		int myMove, i;

		// Can I win?
		if ((myMove=FindHole(PPhone))!=-1) {
			RecordMove(myMove, PPhone, true);
			return;
		}

		// Can I block?
		if ((myMove=FindHole(PUser))!=-1) {
			RecordMove(myMove, PPhone, true);
			return;
		}

		// Select random move
		for (i=0; i<9; i++) {
			myMove=brainLine[i];
			if (board[myMove]==0) {
				RecordMove(myMove, PPhone, true);
				return;
			}
		}

		// No valid moves? How come we're still here????
		winner=0;
		winLine=-1;
		gameOver=true;
		userMayMove=false;
		System.out.println("No valid moves but game is still open???");
		runThread=false;
	}

	private void RecordMove(int move, int c, boolean uM) {
		int i;

		board[move]=c;
		for (i=0; i<9; i++) {
			if (moves[i]==-1) {
				moves[i]=move;
				break;
			}
		}

		repaint();
		if (GameOver()==false) {
			userMayMove=uM;

			if (userMayMove==false)
				PhoneMove();
		} else {
			runThread=false;
			userMayMove=false;
			gameOver=true;
		}
	}

	private boolean GameOver() {
		int i, x, y, z;

		for (i=0; i<8; i++) {
			x=GameMatrix[i][0];
			y=GameMatrix[i][1];
			z=GameMatrix[i][2];

			if (board[x]==PPhone && board[y]==PPhone && board[z]==PPhone) {
				winner=PPhone;
				winLine=i;
				return true;
			}

			if (board[x]==PUser && board[y]==PUser && board[z]==PUser) {
				winner=PUser;
				winLine=i;
				return true;
			}
		}

		for (i=0; i<9; i++) {
			if (board[i]==0)
				return false;
		}

		winner=0;
		winLine=-1;
		return true;
	}
		

	protected void keyPressed(int keyCode) {
		int i;

		if (userMayMove==true) {
			if (keyCode>=Canvas.KEY_NUM1 && keyCode<=Canvas.KEY_NUM9) {
				i=(int)(keyCode-Canvas.KEY_NUM1);
				if (board[i]==0) {
					if ((settings&PrfSound)==PrfSound) {
						sound.init(622, 75);
						sound.play(1);
					}
					RecordMove(i, PUser, false);
				}
			}
		}
	}

	protected void paint(Graphics g) {
		int moveY=0, moveX, i, x, y, z;
		Image p, t;

		g.drawImage(imgBoard, 0, 0, Graphics.TOP|Graphics.LEFT);
		g.drawImage(imgYouMe, 60, 0, Graphics.TOP|Graphics.LEFT);

		if ((settings&PrfUserFirst)==PrfUserFirst)
			moveX=ColYou;
		else
			moveX=ColMe;

		moveY+=7;

		for (i=0; i<9; i++) {
			switch (board[i]) {
			case 1:
				p=imgPhone;
				break;
			case 2:
				p=imgUser;
				break;
			default:
				p=null;
				break;
			}

			if (p!=null)
				g.drawImage(p, boardX[i], boardY[i], Graphics.TOP|Graphics.LEFT);
			if (moves[i]>-1)
				g.drawImage(imgNumbers[moves[i]], moveX, moveY, Graphics.TOP|Graphics.LEFT);

			if (moveX==ColYou)
				moveX=ColMe;
			else
				moveX=ColYou;

			if (moveX==ColYou)
				moveY+=7;
		}

		if (gameOver==true) {
			switch (winner) {
			case PPhone:
				p=imgIWon;
				t=imgPhone;
				moveX=31;
				break;
			case PUser:
				p=imgYouWon;
				t=imgUser;
				moveX=43;
				break;
			default:
				p=imgDraw;
				t=null;
				moveX=54;
				break;
			}
	
			if ((animCnt&1)==0) {
				g.setColor(0xffffff);
				g.fillRect((DispW-moveX)/2, 54, moveX, 5);
			} else
				g.drawImage(p, (DispW-moveX)/2, 54, Graphics.LEFT|Graphics.TOP);

			if (winLine!=-1) {
				x=GameMatrix[winLine][0];
				y=GameMatrix[winLine][1];
				z=GameMatrix[winLine][2];
			
				if ((animCnt&1)==1) {
					g.setColor(0xffffff);
					g.fillRect(boardX[x], boardY[x], StoneW, StoneH);
					g.fillRect(boardX[y], boardY[y], StoneW, StoneH);
					g.fillRect(boardX[z], boardY[z], StoneW, StoneH);
				} else {
					g.drawImage(t, boardX[x], boardY[x], Graphics.TOP|Graphics.LEFT);
					g.drawImage(t, boardX[y], boardY[y], Graphics.TOP|Graphics.LEFT);
					g.drawImage(t, boardX[z], boardY[z], Graphics.TOP|Graphics.LEFT);
				}
			}
		}
	}

	public void run() {
		long startTime, runTime;
		
		while (runThread==true) {
		try {
			startTime=System.currentTimeMillis();

			repaint();
			runTime=System.currentTimeMillis()-startTime;
			if (runTime<70)
				myself.sleep(70-runTime);
		} catch (InterruptedException ex) {}
		}

		if ((settings&PrfSound)==PrfSound) {
			sound.init(SndWinner, Sound.FORMAT_TONE);
			sound.play(1);
		}

		try {
		while (animCnt<16) {
			startTime=System.currentTimeMillis();

			repaint();
			animCnt++;
			runTime=System.currentTimeMillis()-startTime;
			if (runTime<200)
				myself.sleep(200-runTime);
		}
		} catch (InterruptedException ex) {}

		display.setCurrent(lstOptions);
	}
}

}
