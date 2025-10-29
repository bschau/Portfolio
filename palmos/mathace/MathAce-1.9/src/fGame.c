/*
 * fGame.c
 */
#include "Include.h"

/* protos */
static void UpdateForm(void);
static void EnableField(UInt16, UInt16);
static void Insert(WChar);
static void Solve(void);
static void SetupString(Char *);
static void ResetGame(void);

/* globals */
static Char *strAdd="+", *strSub="-", *strMul="*", *strDiv="/",  *strEqual="=";
static Char *strCorrect="Correct!", *strIncorrect="Incorrect!";
static UInt16 strAddW, strSubW, strMulW, strDivW, strEqualW;

/*
 * fGameInit
 */
Boolean
fGameInit(void)
{
	FontID fID=FntSetFont(largeBoldFont);

	strAddW=FntCharsWidth(strAdd, 1);
	strSubW=FntCharsWidth(strSub, 1);
	strMulW=FntCharsWidth(strMul, 1);
	strDivW=FntCharsWidth(strDiv, 1);
	strEqualW=FntCharsWidth(strEqual, 1);
	FntSetFont(fID);

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
			Insert('-');
			return true;
		case cGameButOK:
			Solve();
			if (FrmGlueNavIsSupported())
				FrmGlueNavObjectTakeFocus(currentForm, cGameButOK);
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
		if ((key>='0') && (key<='9')) {
			Insert(key);
			return true;
		} else if ((key=='-') || (key==8)) {
			Insert(key);
			return true;
		} else if (key==10) {
			Solve();
			if (FrmGlueNavIsSupported())
				FrmGlueNavObjectTakeFocus(currentForm, cGameButOK);
			return true;
		}
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
	UInt16 width=strEqualW+4+4, x, wl1, wl2, wr, operationW;
	FontID fID=FntSetFont(largeBoldFont);
	RectangleType r;
	Char buffer[40];
	Char *operation;

	if (session->mask&SMlhs1) {
		StrPrintF(buffer, "%ld", session->lhs1);
		wl1=FntCharsWidth(buffer, StrLen(buffer));
	} else
		wl1=AnswerFieldWidth;

	if (session->mask&SMlhs2) {
		StrPrintF(buffer, "%ld", session->lhs2);
		wl2=FntCharsWidth(buffer, StrLen(buffer));
	} else
		wl2=AnswerFieldWidth;

	if (session->mask&SMrhs) {
		StrPrintF(buffer, "%ld", session->rhs);
		wr=FntCharsWidth(buffer, StrLen(buffer));
	} else
		wr=AnswerFieldWidth;

	if (session->operation&PrfOperationAdd) {
		operationW=strAddW;
		operation=strAdd;
	} else if (session->operation&PrfOperationSub) {
		operationW=strSubW;
		operation=strSub;
	} else if (session->operation&PrfOperationMul) {
		operationW=strMulW;
		operation=strMul;
	} else {
		operationW=strDivW;
		operation=strDiv;
	}

	width+=wl1+wl2+wr+operationW;

	x=(160-width)/2;

	RctSetRectangle(&r, 0, 42, 160, 16);
	WinEraseRectangle(&r, 0);

	if (session->mask&SMlhs1) {
		StrPrintF(buffer, "%ld", session->lhs1);
		WinDrawChars(buffer, StrLen(buffer), x, r.topLeft.y);
	} else
		EnableField(x, r.topLeft.y);

	x+=2+wl1;
	WinDrawChars(operation, 1, x, r.topLeft.y);
	x+=2+operationW;

	if (session->mask&SMlhs2) {
		StrPrintF(buffer, "%ld", session->lhs2);
		WinDrawChars(buffer, StrLen(buffer), x, r.topLeft.y);
	} else
		EnableField(x, r.topLeft.y);

	x+=2+wl2;
	WinDrawChars(strEqual, 1, x, r.topLeft.y);
	x+=2+strEqualW+2;

	if (session->mask&SMrhs) {
		StrPrintF(buffer, "%ld", session->rhs);
		WinDrawChars(buffer, StrLen(buffer), x, r.topLeft.y);
	} else 
		EnableField(x, r.topLeft.y);

	FntSetFont(fID);

	RctSetRectangle(&r, 54, 1, 91, 12);
	WinEraseRectangle(&r, 0);

	if ((session->round)>1) {
		StrPrintF(buffer, "Score %lu out of %lu", session->score, session->round-1);
		width=FntCharsWidth(buffer, StrLen(buffer));

		WinDrawChars(buffer, StrLen(buffer), r.topLeft.x+r.extent.x-width, r.topLeft.y);
	}
}

/*
 * EnableField
 *
 * Turn on and reposition field.
 *
 *  -> x	X position.
 *  -> y	Y position.
 */
static void
EnableField(UInt16 x, UInt16 y)
{
	FieldType *fld=UIFormGetObject(cGameAnswer);
	RectangleType r;

	FldGetBounds(fld, &r);
	r.topLeft.x=x;
	r.topLeft.y=y;
	FldSetBounds(fld, &r);

	UIShowObject(cGameAnswer);
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
	} else if (c=='-') {
		if ((p=UIFieldGetText(cGameAnswer)))
			MemMove(buffer, p, StrLen(p));

		if (*buffer!='-') {
			for (l=StrLen(buffer); l>0; l--)
				buffer[l]=buffer[l-1];

		}

		*buffer='-';
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

	if (p && (StrLen(p)>0) && StrCompare(p, "-")!=0) {
		Int32 lh1=session->lhs1, lh2=session->lhs2, rh=session->rhs, v=StrAToI(p);
		UInt16 m=session->mask, correct=0;
		Char *p;

		if (!(m&SMlhs1))
			lh1=v;
		else if (!(m&SMlhs2))
			lh2=v;
		else
			rh=v;

		if (session->operation&PrfOperationAdd) {
			if (rh==(lh1+lh2))
				correct=1;
		} else if (session->operation&PrfOperationSub) {
			if (rh==(lh1-lh2))
				correct=1;
		} else if (session->operation&PrfOperationMul) {
			if (rh==(lh1*lh2))
				correct=1;
		} else {
			if (lh2) {
				if (rh==(lh1/lh2))
					correct=1;
			} else {
				SetupString("Cannot divide by zero");
				return;
			}
		}

		if (correct) {
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
		UIHideObject(cGameAnswer);
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

	RctSetRectangle(&r, 0, 20, 160, 16);
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
