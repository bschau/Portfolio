/**
 * Game.java
 */
public class Game extends Canvas implements Runnable {
	private int boomX[]=new int[${BoomFragments}];
	private int boomY[]=new int[${BoomFragments}];
	private int boomSpeed[]=new int[${BoomFragments}];
	private int scan[]=new int[screenW*2];
	private	Thread scroller;
	private int refresh;
	private boolean runThread, wormDying;
	private Image screen;
	private Graphics screenG;
	private int holeChange, holeDir, holeY, holeSize, holeNarrow;
	private Random random;
	private int barrierTop, barrierThick, barrierChange, barrierHeight;
	private int barrierRestart;
	private int starX, starY;
	private int wormX, wormY, gSpeed, delay;
	private int upKey, score, boomCnt;

	Game() {
		int i;

		random=new Random();
		screen=Image.createImage(screenW, screenH);
		screenG=screen.getGraphics();
		screenG.drawLine(0, 0, screenW, 0);
		screenG.drawLine(0, screenH-1, screenW, screenH-1);

		holeChange=${HoleChange};
		holeNarrow=${HoleNarrow};
		holeSize=screenH-4;
		holeY=1;
		SetHoleDirection();

		barrierRestart=screenW/2;
		barrierTop=0;
		barrierChange=barrierRestart;
		barrierThick=0;
		barrierHeight=3;

		switch (optTail) {
		case 0:
			wormX=screenW/8;
			break;
		case 1:
			wormX=screenW/4;
			break;
		default:
			wormX=screenW/2;
			break;
		}
		wormY=(screenH/2)*${Factor};
		gSpeed=0;
		upKey=-1;
		delay=40;
		score=0;

		starX=-${StarHeight};
		starY=0;

		switch (optSpeed) {
		case 0:
			refresh=70;
			break;
		case 1:
			refresh=50;
			break;
		default:
			refresh=30;
			break;
		}

		scroller=new Thread(this);
		scroller.start();
	}

	public void Stop() {
		runThread=false;
		try {
			scroller.join();
		} catch (Exception ex) {}
	}

	protected void keyPressed(int keyCode) {
		upKey=keyCode;
	}

	protected void keyReleased(int keyCode) {
		if (keyCode==upKey)
			upKey=-1;
	}
		
	protected void paint(Graphics g) {
		g.drawImage(screen, 0, 0, ${G});

		if (starX>-4 && starX<screenW)
			g.drawImage(imgStar, starX, starY, ${G});

		if (delay>0) {
			Font f=g.getFont();
			String s;

			if (delay>29)
				s="3!";
			else if (delay>19)
				s="2!";
			else if (delay>9)
				s="1!";
			else
				s="Go!";

			g.drawString(s, (screenW/2)+4, (screenH-f.getHeight())/2, ${G});
		}
	}

	public void run() {
		int tick=0;
		long startTime, runTime;

		runThread=true;
		wormDying=false;

		while (runThread==true) {
		try {
			startTime=System.currentTimeMillis();

			if (optSound>0 && delay>0) {
				if ((delay%10)==0) {
					if (delay>10)
						Manager.playTone(${C4}, 300, 100);
					else
						Manager.playTone(${C4}, 500, 100);
				}
			}

			if (delay>0)
				delay--;
			else
				break;


			Scroll();
			WormLogic();
			BarrierLogic();
			StarLogic();

			repaint();
			runTime=System.currentTimeMillis()-startTime;
			if (runTime<70)
				scroller.sleep(70-runTime);
		} catch (Exception ex) {}
		}

		while (runThread==true) {
		try {
			startTime=System.currentTimeMillis();

			if (wormDying==false) {
				tick++;
				if (tick>100) {
					tick=0;
					if (refresh>10)
						refresh--;
				}

				score++;
				Scroll();
				WormLogic();
				BarrierLogic();
				StarLogic();
			} else {
				screenG.setColor(0x000000);
				WormDying();
			}

			repaint();
			runTime=System.currentTimeMillis()-startTime;
			if (runTime<refresh)
				scroller.sleep(refresh-runTime);
		} catch (Exception ex) {}
		}
	}

	private void Scroll() {
// Funky!  This is how it works in the Nokia 6230 emulator ...
// screenG.copyArea(1, 1, screenW-1, screenH-2, -1, 0, Graphics.LEFT|Graphics.TOP);
//		... and this is how on the actual device!
		screenG.copyArea(1, 1, screenW-1, screenH-2, 0, 1, ${G});

		screenG.setColor(0x000000);
		screenG.drawLine(screenW-1, 0, screenW-1, screenH);
		screenG.setColor(0xffffff);
		screenG.drawLine(screenW-1, holeY, screenW-1, holeY+holeSize);

		screenG.setColor(0x000000);
		HoleLogic();
	}

	private void SetHoleDirection() {
		int r=(random.nextInt()>>>1)%8;

		switch (r) {
		case 0:
		case 2:
		case 6:
			holeDir=-1;
			break;
		case 1:
		case 5:
		case 7:
			holeDir=1;
			break;
		default:
			holeDir=0;
			break;
		}
	}

	private void HoleLogic() {
		if (holeChange<1) {
			holeChange=${HoleChange};
			SetHoleDirection();
		} else
			holeChange--;

		if (holeNarrow<1) {
			holeNarrow=${HoleNarrow};
			if (holeSize>${MinHoleSize}) {
				holeSize--;
				if ((holeSize&1)==1)
					holeY++;
			}
		} else
			holeNarrow--;

		if (holeDir==-1) {
			if (holeY>1)
				holeY--;
			else
				holeY=1;
		} else if (holeDir==1) {
			if (holeY<(screenH-2-holeSize))
				holeY++;
			else
				holeY=screenH-2-holeSize;
		}
	}

	private void BarrierLogic() {
		int b, i, bT;

		if (barrierThick==0) {
			if (barrierChange-->0)
				return;

			barrierChange=barrierRestart;
			barrierThick=3;

			barrierHeight=holeSize/2;
			barrierTop=holeY+((random.nextInt()>>>1)%(holeSize-barrierHeight));
		} else
			barrierThick--;

		screenG.drawLine(screenW-1, barrierTop, screenW-1, barrierTop+barrierHeight);
	}

	private void StarLogic() {
		int i;

		if (imgStar==null)
			return;

		if (starX>-${StarHeight})
			starX--;
		else {
			if (barrierChange<(barrierRestart/2)) {
				starX=screenW;
				starY=holeY+((random.nextInt()>>>1)%(holeSize-${StarHeight}));
			}
		}
	}

	private void WormLogic() {
		int oldWormY=wormY, w;

		if (delay<1) {
			// Gravity sucks!
			if (upKey==-1)
				gSpeed+=${Gravity};
			else
				gSpeed-=${Gravity};

			if (gSpeed>${MaxSpeed})
				gSpeed=${MaxSpeed};

			if (gSpeed<-${MaxSpeed})
				gSpeed=-${MaxSpeed};

			wormY+=gSpeed;

			w=wormY/${Factor};
			if (w<1)
				SetupWormDying();
			else if (w>(screenH-1))
				SetupWormDying();
			else {
				screen.getRGB(scan, 0, screenW, wormX, w, 1, 1);
				if ((scan[0]&0x00ffffff)==0)
					SetupWormDying();
			}

			if (starX>-4 && starX<screenW) {
				if ((wormX>=starX) && (wormX<=(starX+${StarSize})) && (w>=starY) && (w<=(starY+${StarSize}))) {
					starX=screenW*2;
					score+=${StarScore};
					if (optSound>0) {
						try {
							Manager.playTone(${C4}, 300, 100);
						} catch (Exception ex) {}
					}
				}
			}
		}

		screenG.drawLine(wormX, oldWormY/${Factor}, wormX, wormY/${Factor});
	}

	private void SetupWormDying() {
		wormDying=true;

		refresh=200;

		int bY=wormY/${Factor};
		for (int i=0; i<${BoomFragments}; i++) {
			boomY[i]=bY;
			boomX[i]=wormX;
			boomSpeed[i]=((random.nextInt()>>>1)%3)+1;
		}
		boomCnt=0;
	}

	private void WormDying() {
		int s, bX, bY;

		if (optSound>0) {
			try {
				Manager.playTone(${C4}, 200, 70+((random.nextInt()>>>1)%30));
			} catch (Exception ex) {}
		}

		for (int i=0; i<${BoomFragments}; i++) {
			s=boomSpeed[i]*(((random.nextInt()>>>1)%3)+1);
			bX=boomX[i];
			bY=boomY[i];
			boomX[i]+=(BoomDX[i]*s);
			boomY[i]+=(BoomDY[i]*s);

			if (boomX[i]>=0 && boomX[i]<=(screenW-1) && boomY[i]>=0 && boomY[i]<=(screenH-1))
				screenG.drawLine(bX, bY, boomX[i], boomY[i]);
		}

		boomCnt++;
		if (boomCnt>12) {
			runThread=false;
			cHiScore.ShowGameOver(score);
			display.setCurrent(cHiScore);
		}
	}
}
