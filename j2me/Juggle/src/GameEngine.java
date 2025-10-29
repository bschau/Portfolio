/*
vim: ts=2 sw=2 et
*/
import java.util.Timer;
import java.util.TimerTask;
import javax.microedition.lcdui.*;
import javax.microedition.midlet.*;

class GameEngine extends Canvas {
	private static final int GameW=128;
	private static final int GameH=118;
	private static final int TimerRateEasy=300;
	private static final int TimerRateHard=200;
	private static final byte bOuterX[]={ 2, 4, 10, 20, 34, 50, 70, 86, 100, 110, 116, 118 };
	private static final byte bOuterY[]={ 56, 42, 28, 14, 4, 0, 0, 4, 14, 28, 42, 56 };
	private static final byte bMiddleX[]={ 18, 20, 26, 38, 52, 68, 82, 94, 100, 102 };
	private static final byte bMiddleY[]={ 56, 44, 32, 22, 16, 16, 22, 32, 44, 56 };
	private static final byte bInnerX[]={ 34, 39, 46, 54, 66, 74, 82, 86 };
	private static final byte bInnerY[]={ 56, 46, 38, 32, 32, 38, 46, 56 };
	private boolean dropped;
	private int gameType, manPosition, xOff, yOff, bOuter, bMiddle, bInner;
	private int bOuterAdd, bMiddleAdd, bInnerAdd, droppedBall;
	private Image gManP0, gManP1, gManP2, gBall;
	private int bOuterTouched, bMiddleTouched, bInnerTouched;
	private int score, scoreAdd, ballMove, timerRate, dropAnim;
	private Thread bThread;
	private TouchedBalls tb;
  private boolean scrClearAll;

	public void Init(int t, Canvas e) {
    scrClearAll=true;
		gameType=t;
		manPosition=1;
		bOuter=0;
		bOuterTouched=bOuter;
		bOuterAdd=1;
		bMiddle=9;
		bMiddleTouched=bMiddle;
		bMiddleAdd=-1;
		bInner=0;
		bInnerTouched=bInner;
		bInnerAdd=1;
		score=0;
		ballMove=0;
		dropped=false;
    droppedBall=0;

		if (gameType==1) {
			scoreAdd=10;
			timerRate=TimerRateHard;
		} else {
			scoreAdd=1;
			timerRate=TimerRateEasy;
		}

		try {
			gManP0=Image.createImage("/jugglel.png");
			gManP1=Image.createImage("/jugglem.png");
			gManP2=Image.createImage("/juggler.png");
			gBall=Image.createImage("/ball.png");
		} catch (Exception ex) {
			ex.printStackTrace();
		}

		if ((xOff=(getWidth()-GameW)/2)<0) {
			xOff=0;
    }

		if ((yOff=(getHeight()-GameH)/2)<0) {
			yOff=0;
    }

		tb=new TouchedBalls();
		synchronized (this) {
			if (bThread==null) {
				bThread=new Balls(e);
      }

			bThread.start();
		}
	}

	public void Destroy() {
		synchronized (this) {
			if (bThread!=null) {
				bThread=null;
      }
		}
	}

	protected void keyPressed(int keyCode) {
		boolean didMove=false;

		synchronized(this) {
			if (dropped==false) {
				switch (keyCode) {
				case Canvas.LEFT:
				case Canvas.KEY_NUM1:
				case Canvas.KEY_NUM4:
				case Canvas.KEY_NUM7:
				case Canvas.KEY_STAR:
					if (manPosition>0) {
						manPosition--;
						didMove=true;
					}
					break;
				case Canvas.RIGHT:
				case Canvas.KEY_NUM3:
				case Canvas.KEY_NUM6:
				case Canvas.KEY_NUM9:
				case Canvas.KEY_POUND:
					if (manPosition<2) {
						manPosition++;
						didMove=true;
					}
					break;
				}
	
				if (didMove) {
					repaint();
        }
		
				tb.Check();
			} else {
				manPosition=1;
				repaint();
			}
		}
	}

	protected void paint(Graphics g) {
		String s=Integer.toString(score);
		int w=g.getFont().stringWidth(s);

		g.setColor(0xffffff);
    if (scrClearAll) {
      g.fillRect(0, 0, getWidth(), getHeight());
      scrClearAll=false;
    } else {
		  g.fillRect(xOff, yOff, GameW, GameH);
    }

		if (manPosition==0) {
			g.drawImage(gManP0, xOff, yOff+54, Graphics.TOP|Graphics.LEFT);
    } else if (manPosition==1) {
			g.drawImage(gManP1, xOff+16, yOff+54, Graphics.TOP|Graphics.LEFT);
    } else {
			g.drawImage(gManP2, xOff+28, yOff+54, Graphics.TOP|Graphics.LEFT);
    }

		if (dropped==false) {
			g.drawImage(gBall, bOuterX[bOuter]+xOff, bOuterY[bOuter]+yOff, Graphics.TOP|Graphics.LEFT);
			g.drawImage(gBall, bMiddleX[bMiddle]+xOff, bMiddleY[bMiddle]+yOff, Graphics.TOP|Graphics.LEFT);
			g.drawImage(gBall, bInnerX[bInner]+xOff, bInnerY[bInner]+yOff, Graphics.TOP|Graphics.LEFT);
		} else {
			synchronized (this) {
        if (droppedBall==1) {
			    g.drawImage(gBall, bOuterX[bOuter]+xOff, bOuterY[bOuter]+yOff, Graphics.TOP|Graphics.LEFT);
			    g.drawImage(gBall, bMiddleX[bMiddle]+xOff, bMiddleY[bMiddle]+yOff, Graphics.TOP|Graphics.LEFT);
				  if ((dropAnim&1)==0) {
			      g.drawImage(gBall, bInnerX[bInner]+xOff, bInnerY[bInner]+yOff, Graphics.TOP|Graphics.LEFT);
          }
        } else if (droppedBall==2) {
			    g.drawImage(gBall, bOuterX[bOuter]+xOff, bOuterY[bOuter]+yOff, Graphics.TOP|Graphics.LEFT);
			    g.drawImage(gBall, bInnerX[bInner]+xOff, bInnerY[bInner]+yOff, Graphics.TOP|Graphics.LEFT);
				  if ((dropAnim&1)==0) {
			      g.drawImage(gBall, bMiddleX[bMiddle]+xOff, bMiddleY[bMiddle]+yOff, Graphics.TOP|Graphics.LEFT);
          }
        } else if (droppedBall==3) {
			    g.drawImage(gBall, bInnerX[bInner]+xOff, bInnerY[bInner]+yOff, Graphics.TOP|Graphics.LEFT);
			    g.drawImage(gBall, bMiddleX[bMiddle]+xOff, bMiddleY[bMiddle]+yOff, Graphics.TOP|Graphics.LEFT);
				  if ((dropAnim&1)==0) {
			      g.drawImage(gBall, bOuterX[bOuter]+xOff, bOuterY[bOuter]+yOff, Graphics.TOP|Graphics.LEFT);
          }
        }
			}
		}

		g.setColor(0x000000);
		g.drawString(s, xOff+GameW-w, yOff, 0);
	}

class Balls extends Thread {
	private EndGameCanvas endGame;

	Balls(Canvas e) {
		endGame=(EndGameCanvas)e;
	}

  private void markBallDrop(int b) {
    dropped=true;
    droppedBall=b;
  }

	public void run() {
		while (dropped==false) {
			switch (ballMove) {
			case 0:
				bMiddle+=bMiddleAdd;
				if (bMiddle<0) {
          bMiddle=0;
          markBallDrop(2);
        } else if (bMiddle>9) {
          bMiddle=9;
          markBallDrop(2);
        } else {
					ballMove=1;
        }
				break;
			case 1:
				bInner+=bInnerAdd;
				if (bInner<0) {
          bInner=0;
          markBallDrop(1);
        } else if (bInner>7) {
          bInner=7;
          markBallDrop(1);
        } else {
					ballMove=2;
        }
				break;
			case 2:
				bOuter+=bOuterAdd;
				if (bOuter<0) {
          bOuter=0;
          markBallDrop(3);
        } else if (bOuter>11) {
          bOuter=11;
          markBallDrop(3);
        } else {
					ballMove=0;
        }
				break;
			}
	
			if (dropped==false) {
				repaint();
				if (tb.Check()) {
					if (score%100==0) {
						if (score>300) {
							timerRate=TimerRateHard;
            } else {
							timerRate=TimerRateEasy;
            }
					}

					if (timerRate>50) {
						if (gameType==0) {
							timerRate-=3;
            } else {
							timerRate-=5;
            }
					}
				}

				try {
					this.sleep(timerRate);
				} catch (Exception ex) {}
			}
		}

		dropAnim=0;

		while (dropAnim<12) {
			repaint();
			try {
				this.sleep(200);
			} catch (Exception ex) {}

			dropAnim++;
		}

		endGame.Show(gameType, score);
		Destroy();
	}
}

class TouchedBalls {
	public boolean Check() {
		boolean touched=false;

		synchronized (this) {
			if (manPosition==0) {
				if (bOuter==0) {
					if (bOuterTouched!=bOuter) {
						bOuterTouched=bOuter;
						score+=scoreAdd;
						touched=true;
					}
	
					bOuterAdd=1;
				}
		
				if (bInner==7) {
					if (bInnerTouched!=bInner) {
						bInnerTouched=bInner;
						score+=scoreAdd;
						touched=true;
					}
	
					bInnerAdd=-1;
				}
			}
	
			if (manPosition==1) {
				if (bMiddle==0) {
					if (bMiddleTouched!=bMiddle) {
						bMiddleTouched=bMiddle;
						score+=scoreAdd;
						touched=true;
					}
	
					bMiddleAdd=1;
				} else if (bMiddle==9) {
					if (bMiddleTouched!=bMiddle) {
						bMiddleTouched=bMiddle;
						score+=scoreAdd;
						touched=true;
					}
	
					bMiddleAdd=-1;
				}
			}
	
			if (manPosition==2) {
				if (bOuter==11) {
					if (bOuterTouched!=bOuter) {
						bOuterTouched=bOuter;
						score+=scoreAdd;
						touched=true;
					}
	
					bOuterAdd=-1;
				}
	
				if (bInner==0) {
					if (bInnerTouched!=bInner) {
						bInnerTouched=bInner;
						score+=scoreAdd;
						touched=true;
					}
		
					bInnerAdd=1;
				}
			}
		}
		return touched;
	}
}

}
