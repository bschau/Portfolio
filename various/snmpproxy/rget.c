/*
**	rget.c
**
**	Get request.
*/
#include "include.h"

/* protos */
static int HandleGet(Message *, VarBind *, char *);

/*
**	GetRequest
**
**	Handle Get Request.
**
**	 -> m		Message.
**
**	Return 0 if handled, !0 otherwise.
*/
int
GetRequest(Message *m)
{
	VarBindList *vL=m->data->a.get_request->variable_bindings;
	PDU *pdu=m->data->a.get_request;
	int idx=0;
	VarBind *v;
	char *oid;
	AsnListNode *lP;

	if (vL==NULL) {
		XLog(LOG_ERR, "(GetRequest) VarBindList = NULL");
		return -1;
	}

	for (lP=vL->first; lP; lP=lP->next) {
		if ((v=lP->data)==NULL) {
			XLog(LOG_ERR, "(GetRequest) VarBind = NULL");
			return -1;
		}

		if ((oid=OidToDottedAscii(&v->name))==NULL) {
			XLog(LOG_ERR, "(GetRequest) Cannot convert OID to string");
			return -1;
		} else {
			if (pDebug)
				XLog(LOG_DEBUG, "(GetRequest) Input OID: %s", oid);
		}

		if ((pdu->error_status=HandleGet(m, v, oid))!=NOERROR) {
			XLog(LOG_DEBUG, "(GetRequest) HandleGet Error %i at %i", pdu->error_status, idx);
			pdu->error_index=idx;
			break;
		}

		idx++;
	}

	return XmitMessage(m);
}

/*
**	HandleGet
**
**	Handle get request.
**
**	 -> m		Original message.
**	 -> dstV	Varbind.
**	 -> oid		OID.
**
**	Returns suitable pdu error status or NOERROR.
*/
static int
HandleGet(Message *m, VarBind *dstV, char *oid)
{
	VarBindList *vL;
	VarBind *v;
	OidNode *n;
	SBuf sBuf;
	unsigned char *pkg;
	unsigned int pkgLen;

	if ((n=OidGetNode(oid))) {
		if (n->oidType==OTMasquerade) {
			if ((m=Masquerade(m, n, PDUS_GET_REQUEST))==NULL)
				return GENERR;

			if (m->data->a.get_request->error_status!=NOERROR)
				return m->data->a.get_request->error_status;

			CopyVarBind(dstV, m);
			return NOERROR;
		}

		return OidLocal(dstV->value, n);
	}

	/* OID not fake - proxy to peer and read response */
	if (ProxyPackage(&pPeer, pkgBuffer, pkgLength)!=0)
		return GENERR;	/* we should really notify client here :-) */

	XCaptureToFile("---- HandleGet return package:\n", pCaptureFile, pkgBuffer, pkgLength);

	pkgLen=pkgLength;
	if ((pkg=VerifyPackage(pkgBuffer, &pkgLen))==NULL)
		return GENERR;

	if (DisectPackage(m, &sBuf, pkg, pkgLen)!=0)
		return GENERR;

	if (m->data->a.get_request->error_status!=NOERROR)
		return m->data->a.get_request->error_status;

	if ((vL=m->data->a.get_request->variable_bindings)==NULL) {
		XLog(LOG_ERR, "(HandleGet) Proxy VarBindList = NULL");
		return GENERR;
	}

	if ((v=vL->first->data)==NULL) {
		XLog(LOG_ERR, "(HandleGet) Proxy VarBind = NULL");
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
