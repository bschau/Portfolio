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

import javax.microedition.lcdui.*;


class TextInputScreen
    extends TextBox
    implements CommandListener
{
    private final BouncingTextMIDlet midlet;
    private final Command exitCommand;
    private final Command startCommand;


    TextInputScreen(BouncingTextMIDlet midlet, String string)
    {
        super("Enter text: ", string, 256, TextField.ANY);
        this.midlet = midlet;

        // Command area
        exitCommand = new Command("Exit", Command.EXIT, 1);
        addCommand(exitCommand);
        startCommand = new Command("Start", Command.SCREEN, 1);
        addCommand(startCommand);
        setCommandListener(this);
    }


    public void commandAction(Command c, Displayable d)
    {
        if (c == exitCommand)
        {
            midlet.exitRequested();
        }
        else if (c == startCommand)
        {
            midlet.textInputScreenDone(getString());
        }
    }
}
