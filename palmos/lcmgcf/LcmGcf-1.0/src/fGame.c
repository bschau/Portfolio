/*
 * fGame.c
 */
#include "Include.h"

/* protos */
static void UpdateForm(void);
static void Insert(WChar);
static void Solve(void);
static void SetupString(Char *);
static void ResetGame(void);
static void Clue(void);
static UInt16 LCM(UInt16, UInt16);
static UInt16 GCF(UInt16, UInt16);

/* globals */
static Char *strCorrect="Correct!", *strIncorrect="Incorrect!";

/*
 * fGameInit
 */
Boolean
fGameInit(void)
{
	if (session==NULL)
		SssNew();

	return true;
}

/*
 * fGameEH
 */
Boolean
fGameEH(EventType *ev)
{
	UInt16 id;
	WChar key;

	switch (ev->eType) {
	case frmOpenEvent:
		FrmDrawForm(currentForm);
		inGame=true;
	case frmUpdateEvent:
		UpdateForm();
		return true;

	case ctlSelectEvent:
		id=ev->data.ctlSelect.controlID;
		if ((id>=cGameBut0) && (id<=cGameBut9)) {
			Insert((id-cGameBut0)+'0');
			return true;
		}
	       
		switch (id) {
		case cGameButBS:
			Insert(8);
			return true;
		case cGameButC:
			Insert(1);
			return true;
		case cGameButSign:
			Clue();
			return true;
		case cGameButOK:
			Solve();
			return true;
		case cGameStop:
			if (session->round>1) {
				inGame=false;
				if (FrmAlert(aEndQuiz)==0)
					fGameEndQuiz();
				else
					inGame=true;
			} else
				fGameEndQuiz();
			return true;
		}
		break;

	case menuEvent:
		switch (ev->data.menu.itemID) {
		case mGameReset:
			if (session->round>1) {
				inGame=false;
				if (FrmAlert(aReset)==0)
					ResetGame();

				inGame=true;
			}
			return true;
		case mGameEndQuiz:
			if (session->round>1) {
				inGame=false;
				if (FrmAlert(aEndQuiz)==0)
					fGameEndQuiz();
				else
					inGame=true;
			} else
				fGameEndQuiz();

			return true;
		case mGameAbout:
			inGame=false;
			About();
			inGame=true;
			return true;
		}
		break;

	case keyDownEvent:
		key=ev->data.keyDown.chr;
		if ((key>='0') && (key<='9'))
			Insert(key);
		else if (key==8)
			Insert(key);
		else if (key==10)
			Solve();
		else if (key=='?')
			Clue();

		return true;
	default:	/* FALL-THRU */
		break;
	}

	return false;
}

/*
 * UpdateForm
 *
 * Redraw game form.
 */
static void
UpdateForm(void)
{
	FieldType *fld=UIObjectGet(cGameAnswer);
	RectangleType r;
	Char buffer[48];
	Char *str;
	UInt16 w, x;

	UIObjectHide(cGameAnswer);
	RctSetRectangle(&r, 0, 40, 160, 16);
	WinEraseRectangle(&r, 0);
	if (session->operation&PrfQuizTypeLcm)
		str="LCM";
	else
		str="GCF";

	if (session->defOperations&PrfNumbers)
		StrPrintF(buffer, "%s (%u, %u, %u):", str, session->nums[0], session->nums[1], session->nums[2]);
	else
		StrPrintF(buffer, "%s (%u, %u):", str, session->nums[0], session->nums[1]);

	FntSetFont(largeBoldFont);
	w=FntCharsWidth(buffer, StrLen(buffer));
	x=(160-AnswerFieldWidth-4-w)/2;
	WinDrawChars(buffer, StrLen(buffer), x, 40);
	FntSetFont(stdFont);

	FldGetBounds(fld, &r);
	r.topLeft.x=x+w+4;
	FldSetBounds(fld, &r);
	UIObjectShow(cGameAnswer);

	RctSetRectangle(&r, 54, 1, 91, 12);
	WinEraseRectangle(&r, 0);

	if ((session->round)>1) {
		UInt16 width;

		StrPrintF(buffer, "Score %lu out of %lu", session->score, session->round-1);
		width=FntCharsWidth(buffer, StrLen(buffer));

		WinDrawChars(buffer, StrLen(buffer), r.topLeft.x+r.extent.x-width, r.topLeft.y);
	}

	UIFieldFocus(cGameAnswer);
}

/*
 * Insert
 *
 * Insert the char into the field.
 *
 *  -> c		Char.
 */
static void
Insert(WChar c)
{
	Char buffer[16];
	Char *p;
	UInt16 l;

	MemSet(buffer, sizeof(buffer), 0);
	if ((c>='0') && (c<='9')) {
		if ((p=UIFieldGetText(cGameAnswer))==NULL)
			*buffer=c;
		else {
			if ((l=StrLen(p))>0)
				MemMove(buffer, p, l);

			buffer[l]=c;
		}
	} else if (c==8) {
		if ((p=UIFieldGetText(cGameAnswer))) {
			if ((l=StrLen(p))>0)
				MemMove(buffer, p, l-1);
		}
	}

	UIFieldSetText(cGameAnswer, buffer);
}

/*
 * Solve
 *
 * Determine if user entered the correct number, update score and move on.
 */
static void
Solve(void)
{
	Char *p=UIFieldGetText(cGameAnswer);

	if (p && (StrLen(p)>0)) {
		UInt16 user=StrAToI(p), answer;

		if (session->operation&PrfQuizTypeLcm) {
			if (session->defOperations&PrfNumbers)
				answer=LCM(session->nums[0], LCM(session->nums[1], session->nums[2]));
			else
				answer=LCM(session->nums[0], session->nums[1]);
		} else {
			if (session->defOperations&PrfNumbers)
				answer=GCF(session->nums[0], GCF(session->nums[1], session->nums[2]));
			else
				answer=GCF(session->nums[0], session->nums[1]);
		}

		if (answer==user) {
			if (!(session->defOperations&PrfScoreType)) {
				if (session->failed)
					SssUpdate(0);
				else
					SssUpdate(1);
			} else
				SssUpdate(1);

			p=strCorrect;
		} else {
			p=strIncorrect;
			if (!session->failed) {
				session->round++;
				session->failed=true;
			}
		}

		SetupString(p);

		UIFieldClear(cGameAnswer);
		UIObjectHide(cGameAnswer);
		FrmUpdateForm(fGame, frmRedrawUpdateCode);
	} else
		SetupString("You must enter a value!");
}

/*
 * SetupString
 *
 * Display string and fire off timer.
 *
 *  -> str		String to display.
 */
static void
SetupString(Char *str)
{
	FontID fID=FntSetFont(largeBoldFont);
	RectangleType r;
	UInt16 len, w;

	RctSetRectangle(&r, 0, 18, 160, 16);
	WinEraseRectangle(&r, 0);
	len=StrLen(str);
	w=FntCharsWidth(str, len);
	deleteStringDelay=2*SysTicksPerSecond();
	WinDrawChars(str, len, (160-w)/2, r.topLeft.y);
	FntSetFont(fID);
}

/*
 * ResetGame
 *
 * Reset the current game.
 */
static void
ResetGame(void)
{
	SssDestroy();
	SssNew();
	UIFieldClear(cGameAnswer);
	UIFieldFocus(cGameAnswer);
	FrmUpdateForm(fGame, frmRedrawUpdateCode);
}

/*
 * fGameEndQuiz
 *
 * End the quiz.
 */
void
fGameEndQuiz(void)
{
	inGame=false;
	if (session->round>1) {
		session->stopped=TimGetSeconds();
		FrmGotoForm(fQuizOver);
	} else {
		SssDestroy();
		FrmGotoForm(fMain);
	}
}

/*
 * Clue
 *
 * Run clue form.
 */
static void
Clue(void)
{
	fClueRun();
	UIFormRedraw();
}

/*
 * LCM
 *
 * Find Least Common Multiplier of two numbers.
 *
 *  -> a		Number #1.
 *  -> b		Number #2.
 *
 *  Returns LCM(a, b).
 */
static UInt16
LCM(UInt16 a, UInt16 b)
{
	return (a*b)/GCF(a, b);
}

/*
 * GCF
 *
 * Find Greatest Common Factor of two numbers.
 *
 *  -> a		Number #1.
 *  -> b		Number #2.
 *
 *  Returns GCF(a, b).
 */
static UInt16
GCF(UInt16 a, UInt16 b)
{
	UInt16 r;

	while ((r=a%b)>0) {
		a=b;
		b=r;
		r=a%b;
	}

	return b;
}
