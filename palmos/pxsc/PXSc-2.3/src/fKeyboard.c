/**
 * A simple calculator layout (0-9)
 */
#include "Include.h"

/* protos */
static Boolean EH(EventType *);

/* globals */
static UInt16 gScore;

/**
 * Run the dialog.
 *
 * @param dst Where to store score.
 */
void
fKeyboardRun(UInt16 *dst)
{
	FormSave frm;

	gScore=0;
	UIFormPrologue(&frm, fKeyboard, EH);
	UIFieldFocus(cKeyboardScore);
	UIFormEpilogue(&frm, NULL, 0);
	*dst=gScore;
	return;
}

/**
 * EH
 */
static Boolean
EH(EventType *ev)
{
	switch (ev->eType) {
	case ctlSelectEvent:
		switch (ev->data.ctlSelect.controlID) {
		case cKeyboardOK:
			{
				Char *p=UIFieldGetText(cKeyboardScore);

				if (p)
					gScore=StrAToI(p);
				else
					gScore=0;
			}
			break;

		case cKeyboardBS:
			{
				FieldType *fld=UIObjectGet(cKeyboardScore);
				UInt16 len;

				if ((len=FldGetTextLength(fld)))
					FldDelete(fld, len-1, len);
			}
			return true;

		case cKeyboardClear:
			{
				FieldType *fld=UIObjectGet(cKeyboardScore);
				UInt16 len;

				len=FldGetTextLength(fld);
				FldDelete(fld, 0, len);
			}
			return true;

		default:
			{
				UInt16 idx=ev->data.ctlSelect.controlID;

				if ((idx>=cKeyboard0) && (idx<=cKeyboard9)) {
					Char buffer[2];

					buffer[0]=(idx-cKeyboard0)+'0';
					buffer[1]=0;

					FldInsert(UIObjectGet(cKeyboardScore), buffer, 1);
				}
			}
			return true;
		}
	default:
		break;
	}

	return false;
}
