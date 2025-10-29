// Copyright 2002 Nokia Corporation.
//
// THIS SOURCE CODE IS PROVIDED 'AS IS', WITH NO WARRANTIES WHATSOEVER,
// EXPRESS OR IMPLIED, INCLUDING ANY WARRANTY OF MERCHANTABILITY, FITNESS
// FOR ANY PARTICULAR PURPOSE, OR ARISING FROM A COURSE OF DEALING, USAGE
// OR TRADE PRACTICE, RELATING TO THE SOURCE CODE OR ANY WARRANTY OTHERWISE
// ARISING OUT OF ANY PROPOSAL, SPECIFICATION, OR SAMPLE AND WITH NO
// OBLIGATION OF NOKIA TO PROVIDE THE LICENSEE WITH ANY MAINTENANCE OR
// SUPPORT. FURTHERMORE, NOKIA MAKES NO WARRANTY THAT EXERCISE OF THE
// RIGHTS GRANTED HEREUNDER DOES NOT INFRINGE OR MAY NOT CAUSE INFRINGEMENT
// OF ANY PATENT OR OTHER INTELLECTUAL PROPERTY RIGHTS OWNED OR CONTROLLED
// BY THIRD PARTIES
//
// Furthermore, information provided in this source code is preliminary,
// and may be changed substantially prior to final release. Nokia Corporation
// retains the right to make changes to this source code at
// any time, without notice. This source code is provided for informational
// purposes only.
//
// Nokia and Nokia Connecting People are registered trademarks of Nokia
// Corporation.
// Java and all Java-based marks are trademarks or registered trademarks of
// Sun Microsystems, Inc.
// Other product and company names mentioned herein may be trademarks or
// trade names of their respective owners.
//
// A non-exclusive, non-transferable, worldwide, limited license is hereby
// granted to the Licensee to download, print, reproduce and modify the
// source code. The licensee has the right to market, sell, distribute and
// make available the source code in original or modified form only when
// incorporated into the programs developed by the Licensee. No other
// license, express or implied, by estoppel or otherwise, to any other
// intellectual property rights is granted herein.


package example.hello;

import java.util.Random;
import javax.microedition.lcdui.*;


class BouncingTextCanvas
    extends Canvas
    implements CommandListener, Runnable
{
    private final BouncingTextMIDlet midlet; // parent MIDlet
    private final Display display;           // parent MIDlet's Display
    private final Command exitCommand;       // display 'Exit' command
    private final Command reverseCommand;    // display 'Reverse' command
    private final BouncingText text;         // text string to print

    private int bgRGB;                       // background color
    private int textRGB;                     // text color
    private volatile Thread animationThread = null; // animation thread


    BouncingTextCanvas(BouncingTextMIDlet midlet, Display display,
                       String string)
    {
        this.midlet  = midlet;
        this.display = display;

        // Setup the command handling
        setCommandListener(this);
        reverseCommand = new Command("Rev", Command.SCREEN, 1);
        exitCommand = new Command("Exit", Command.EXIT, 1);
        addCommand(exitCommand);
        addCommand(reverseCommand);


        // Initialize the bouncing text message
        //
        // Create a new 'bouncing text' string whose color is 'textRGB'.
        // Its initial position is at x=0 and
        // y="middle of the canvas". the canvas width and height
        // are also used by a BouncingText object, as it manages
        // its own position update within the canvas displayable area
        // {x,y} = {0,0} to {w,h}.

        int ch = getHeight(); // canvas height
        int cw = getWidth();  // canvas width
        initColors();         // Choose fg and background colors ...
                              // ... based on display capabilities.
        text = new BouncingText(string, textRGB, 0, ch/2, cw, ch);
    }


    void startAnimation()
    {
        animationThread = new Thread(this);
        animationThread.start();
    }


    void stopAnimation()
    {
        animationThread = null;
    }


    // While this thread has not been stopped, it will perform a
    // tick() action approximately every 'millis_per_tick' milliseconds
    // (as close to that period as possible).
    public void run()
    {
        int millis_per_tick = 100; // msec

        Thread currentThread = Thread.currentThread();

        try
        {
            // This ends when animationThread is set to null, or when
            // it is subsequently set to a new Thread.  Either way, the
            // current thread should terminate.
            while (currentThread == animationThread)
            {
                long startTime = System.currentTimeMillis();
                tick();
                repaint(0, 0, getWidth(), getHeight());
                serviceRepaints();
                long elapsedTime = System.currentTimeMillis() - startTime;
                if (elapsedTime < millis_per_tick)
                {
                    synchronized(this)
                    {
                        wait(millis_per_tick - elapsedTime);
                    }
                }
                else
                {
                    currentThread.yield();
                }
            }
        }
        catch(InterruptedException e)
        {
        }
    }


    //Draw the background and the bouncing text string
    public void paint(Graphics g)
    {
        g.setColor(bgRGB);
        g.fillRect(0, 0, getWidth(), getHeight());
        text.draw(g);
    }


    // Handle the UI commands
    public void commandAction(Command c, Displayable d)
    {
        if (c == exitCommand)
        {
            stopAnimation();
            midlet.exitRequested();
        }
        else if (c == reverseCommand)
        {
            text.reverse();
        }
    }


    // What to do each run() tick
    private synchronized void tick()
    {
        text.updatePosition();
    }


    // Choose a random background color on color displays,
    // use a white background color on black-and-white displays
    private void initColors()
    {
        int white = 0xffffff;
        int black = 0x000000;
        if (display.isColor())
        {
            Random random = new Random();
            textRGB = random.nextInt() & 0xffffff;
            bgRGB = white - textRGB; // constrasting background color
        }
        else
        {
            textRGB = black;
            bgRGB = white;
        }
    }


    class BouncingText
    {
        private final int w;          // The canvas width
        private final int h;          // The canvas height
        private final int textRGB;    // Color for the string
        private final Random random;  // For generating random numbers

        private String string;        // The string to print
        private int x;                // The x anchor for the string
        private int y;                // The y anchor for the string
        private int strWidth = 0;     // The pixel width of the string
        private int strHeight = 0;    // The pixel height of the string
        private int xdir = 1;         // 1 or -1 : bounce direction
        private int ydir = 1;         // 1 or -1 : bounce direction


        BouncingText(String string, int textRGB,
                     int x, int y, int w, int h)
        {
            this.string = string;
            this.x = x;
            this.y = y;
            this.w = w;
            this.h = h;
            this.textRGB = textRGB;
            random = new Random();
        }


        // Randomly change the position of the string, the string
        // must be completely drawable within the canvas
        private void updatePosition()
        {
            // Choose a random { vx, vy } velocity.
            int vx = random.nextInt() & 0x07;  // 0-7 pixels
            int vy = random.nextInt() & 0x07;  // 0-7 pixels


            // Note: The methods maxAnchorX() and maxAnchorY()
            //       determine the upper left hand {x, y} coordinates
            //       of the string so that the string can be drawn
            //       within the canvas height and width.
            if ((xdir == 1) && ((x + vx) >= maxAnchorX()))
            {
                xdir = -xdir;  // change direction at right edge
            }
            else if ((xdir == -1) && ((x - vx) < 0))
            {
                xdir = -xdir;  // change direction at left edge
            }
            int xnew = x + (vx * xdir) ;
            if ((xnew >= 0) && (x < maxAnchorX()))
            {
                x = xnew;
            }

            if ((ydir == 1) && ((y + vy) >= maxAnchorY()))
            {
                ydir = -ydir;  // change direction at bottom edge
            }
            else if ((ydir == -1) && ((y - vy) < 0))
            {
                ydir = -ydir;  // change direction at top edge
            }
            int ynew = y + (vy * ydir) ;
            if ((ynew >= 0) && (y < maxAnchorY()))
            {
                y = ynew;
            }
        }


        // Draw the text string at the appropriate position
        private void draw(Graphics g)
        {
             // If the strHeight is still 0, then initialize the
             // strHeight and strWidth variables used in maxAnchorX()
             // and maxAnchorY(). This assumes that the font represents
             // strings as at least 1 pixel high.
             if (! (strHeight > 0))
             {
                 Font f = g.getFont();
                 strHeight = f.getHeight();
                 strWidth = f.stringWidth(string);
             }

             // draw the string
             g.setColor(textRGB);
             g.drawString(string, x, y, Graphics.TOP|Graphics.LEFT);
        }


        private int maxAnchorX()
        {
            return w - strWidth;
        }


        private int maxAnchorY()
        {
            return h - strHeight;
        }


        // Reverse the BouncingText string
        private void reverse()
        {
            // Note: another approach is to store two copies of
            //       the string, one forwards and one reversed

            char[] carray = string.toCharArray();
            for (int old = string.length()-1, nu=0;
                 old >=0;
                 old--, nu++)
            {
               carray[nu] = string.charAt(old);
            }
            string = new String(carray);
        }
    }
}

