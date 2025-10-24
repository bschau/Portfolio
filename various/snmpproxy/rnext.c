/*
**	rnext.c
**
**	GetNext request.
*/
#include "include.h"

/* protos */
static GetNextOid *FindGetNext(AsnOcts *);
static FakeList *FindFake(AsnOcts *);
static int HandleGetNext(Message *, VarBind *);

/*
**	FindGetNext
**
**	Find an oid node given it's encoded form on the getnext list.
**
**	 -> v		Encoded OID.
**
**	Return GetNextOid or NULL if not found.
*/
static GetNextOid *
FindGetNext(AsnOcts *v)
{
	char *iOid=v->octs;
	int iOidLen=v->octetLen;
	GetNextOid *g;

	for (g=gnl; g; g=g->next) {
		if (g->encodedOid.octetLen==iOidLen && memcmp(g->encodedOid.octs, iOid, iOidLen)==0)
			return g;
	}

	if (pDebug)
		XLog(LOG_DEBUG, "(FindGetNext) OID %s not in list", OidToDottedAscii(v));

	return NULL;
}

/*
**	FindFake
**
**	Find an oid node given it's encoded form on the fake list.
**
**	 -> v		Encoded OID.
**
**	Return FakeList or NULL if not found.
*/
static FakeList *
FindFake(AsnOcts *v)
{
	char *iOid=v->octs;
	int iOidLen=v->octetLen;
	FakeList *f;

	for (f=fakeList; f; f=f->next) {
		if (f->encodedOid.octetLen==iOidLen && memcmp(f->encodedOid.octs, iOid, iOidLen)==0)
			return f;
	}

	if (pDebug)
		XLog(LOG_DEBUG, "(FindFake) OID %s not in list", OidToDottedAscii(v));

	return NULL;
}

/*
**	GetNextRequest
**
**	Handle GetNext Request.
**
**	 -> m		Message.
**
**	Return 0 if handled, !0 otherwise.
*/
int
GetNextRequest(Message *m)
{
	VarBindList *vL=m->data->a.get_next_request->variable_bindings;
	PDU *pdu=m->data->a.get_next_request;
	int idx=0;
	VarBind *v;
	char *t;
	AsnListNode *lP;

	if (vL==NULL) {
		XLog(LOG_ERR, "(GetNextRequest) VarBindList = NULL");
		return -1;
	}

	for (lP=vL->first; lP; lP=lP->next) {
		if ((v=lP->data)==NULL) {
			XLog(LOG_ERR, "(GetNextRequest) VarBind = NULL");
			return -1;
		}

		/* debugging purposes */
		if (pDebug) {
			t=OidToDottedAscii(&v->name);
			XLog(LOG_DEBUG, "(GetNextRequest) Input OID: %s", (t ? t : "out of memory"));
		}

		if ((pdu->error_status=HandleGetNext(m, v))!=NOERROR) {
			pdu->error_index=idx;
			break;
		}

		idx++;
	}

	return XmitMessage(m);
}

/*
**	HandleGetNext
**
**	Handles the actual GetNext request.
**
**	 -> m		Original message.
**	 -> dstV	Where to store value (and possible oid).
**
**	Returns a suitable pdu error or NOERROR.
*/
static int
HandleGetNext(Message *m, VarBind *dstV)
{
	VarBindList *vL;
	VarBind *v;
	SBuf sBuf;
	OidNode *n;
	GetNextOid *o;
	unsigned char *pkg;
	unsigned int pkgLen;
	FakeList *f;

	/* Ok, hunt down the OID from the GetNextList */
	if ((o=FindGetNext(&dstV->name)) && o->type==GNONormal) {
		o=o->next;
		if ((n=OidGetEncodedNode(&o->encodedOid))) {
			if (n->oidType==OTMasquerade) {
				/* _GET_REQUEST is true - we don't want to walk
				   the peers tree */
				if ((m=Masquerade(m, n, PDUS_GET_REQUEST))==NULL)
					return GENERR;

				CopyVarBind(dstV, m);
				return NOERROR;
			}

			dstV->name.octs=n->encodedOid.octs;
			dstV->name.octetLen=n->encodedOid.octetLen;
			return OidLocal(dstV->value, n);
		}

		XLog(LOG_DEBUG, "(HandleGetNext) OidGetEncodedNode failed");
		return GENERR;
	}

	/* OID not on getnext list.  What about fakes? */
	if ((f=FindFake(&dstV->name))) {
		if ((n=OidGetEncodedNode(&f->nextEncodedOid))) {
			if (n->oidType==OTMasquerade) {
				/* _GET_REQUEST is true - we don't want to walk
				   the peers tree */
				if ((m=Masquerade(m, n, PDUS_GET_REQUEST))==NULL)
					return GENERR;

				CopyVarBind(dstV, m);
				return NOERROR;
			}

			dstV->name.octs=n->encodedOid.octs;
			dstV->name.octetLen=n->encodedOid.octetLen;
			return OidLocal(dstV->value, n);
		}

		XLog(LOG_DEBUG, "(HandleGetNext) OidGetEncodedNode failed");
		return GENERR;
	}

	/* OID not present on list - proxy to peer and read response */
	if (ProxyPackage(&pPeer, pkgBuffer, pkgLength)!=0)
		return GENERR;	/* we should really notify client here :-) */

	XCaptureToFile("---- HandleGetNext return package:\n", pCaptureFile, pkgBuffer, pkgLength);

	pkgLen=pkgLength;
	if ((pkg=VerifyPackage(pkgBuffer, &pkgLen))==NULL)
		return GENERR;

	if (DisectPackage(m, &sBuf, pkg, pkgLen)!=0)
		return GENERR;

	if (m->data->a.get_request->error_status!=NOERROR)
		return m->data->a.get_request->error_status;

	if ((vL=m->data->a.get_next_request->variable_bindings)==NULL) {
		XLog(LOG_ERR, "(HandleGetNext) Proxy VarBindList = NULL");
		return GENERR;
	}

	if ((v=vL->first->data)==NULL) {
		XLog(LOG_ERR, "(HandleGetNext) Proxy VarBind = NULL");
		return GENERR;
	}

	if (vL->first->next)
		return TOOBIG;

	if ((n=OidGetEncodedNode(&v->name))) {
		if (n->oidType!=OTMasquerade)
			return OidLocal(dstV->value, n);
	}

	CopyVarBind(dstV, m);
	return NOERROR;
}
