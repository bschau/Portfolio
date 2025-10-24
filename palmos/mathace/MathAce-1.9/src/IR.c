/*
 * IR.c
 */
#include "Include.h"

/* structs */
typedef struct {
	UInt16 version;
	Prefs prefs;
} BeamPrefsV1;

/*
 * IRBeam
 *
 * Beam the preferences.
 */
void
IRBeam(void)
{
	BeamPrefsV1 b;
	ExgSocketType sck;
	Err err;

	b.version=1;
	MemMove(&b.prefs, &prefs, sizeof(prefs));

	MemSet(&sck, sizeof(ExgSocketType), 0);
	sck.target=(UInt32)CRID;
	sck.count=1;
	sck.length=sizeof(BeamPrefsV1);
	sck.goToCreator=(UInt32)CRID;
	sck.description="MathAce Preferences";
	sck.name="Preferences";
	sck.type="mAce";
	if ((err=ExgPut(&sck))==errNone) {
		ExgSend(&sck, &b, sck.length, &err);
		err=ExgDisconnect(&sck, err);
	}

	if (err!=errNone)
		FrmAlert(aIRSendError);
}

/*
 * IRReceive
 *
 * Receive a preferences block.
 *
 *  -> sck		Socket.
 *
 * Return true if preferences was received, false otherwise.
 */
Boolean
IRReceive(ExgSocketType *sck, Prefs *p)
{
	UInt16 size=sizeof(BeamPrefsV1);
	Boolean ret=false;
	BeamPrefsV1 b;
	Err err;

	if ((err=ExgAccept(sck))==errNone) {
		if (ExgReceive(sck, &b, size, &err)==size) {
			MemMove(p, &b.prefs, sizeof(Prefs));
			ret=true;
		}

		err=ExgDisconnect(sck, err);
	}

	if (err)
		FrmAlert(aIRReceiveError);

	if (err==errNone)
		return ret;

	return false;
}
