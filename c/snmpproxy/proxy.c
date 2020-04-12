/*
**	proxy.c
**
**	Proxy logic. Messy file.
*/
#include "include.h"

/* protos */
static unsigned char *PPReal(Host *, int, unsigned char *, int);
static int SetPkgHeader(SBuf *);

/*
**	PPReal
**
**	Connect, Send, Recieve, Parse.
**
**	 -> h		Host.
**	 -> sck		Socket.
**	 -> pkg		Package.
**	 -> pkgLen	Package length.
**
*/
static unsigned char *
PPReal(Host *h, int sck, unsigned char *pkg, int pkgLen)
{
	struct sockaddr_in local, peer;
	int r, res, tLen;
	socklen_t sLen;
	fd_set rSet, tSet;
	struct timeval tOut;

	memset((char *)&peer, 0, sizeof(peer));
	peer.sin_family=AF_INET;
	peer.sin_addr.s_addr=h->address.s_addr;
	peer.sin_port=pPeer.port;

	memset((char *)&local, 0, sizeof(local));
	local.sin_family=AF_INET;
	local.sin_addr.s_addr=htonl(INADDR_ANY);
	local.sin_port=htons(0);

	if (bind(sck, (struct sockaddr *)&local, sizeof(local))<0) {
		XLog(LOG_ERR, "(PPReal) Cannot bind to socket");
		return NULL;
	}

	FD_ZERO(&rSet);
	FD_SET(sck, &rSet);
	r=0;
	do {
		XLog(LOG_DEBUG, "(PPReal) Sending package to %X", h->address.s_addr);
		if (sendto(sck, pkg, pkgLen, 0, (struct sockaddr *)&peer, sizeof(peer))<0) {
			XLog(LOG_ERR, "(PPReal) Cannot send package to peer");
			return NULL;
		}

		tSet=rSet;
		tOut.tv_sec=pTimeout;
		tOut.tv_usec=0;
		if ((res=select(sck+1, &tSet, NULL, NULL, &tOut))<0) {
			XLog(LOG_ERR, "(PPReal) Select error (%i)", errno);
			return NULL;
		}

		if (!res) {
			XLog(LOG_DEBUG, "(PPReal) Timeout waiting for peer (%i left)", pRetry-r);
			r++;
			continue;
		}

		sLen=sizeof(local);
		pkgLength=recvfrom(sck, pkgBuffer, PKGBUFFERLEN, 0, (struct sockaddr *)&local, &sLen);
		if (pkgLength<0) {
			if (errno==EINTR)
				continue;

			XLog(LOG_ERR, "(PPReal) Broken connection (%i)", errno);
			return NULL;
		}

		tLen=pkgLength;
		if ((pkg=VerifyPackage(pkgBuffer, &tLen)))
			return pkgBuffer;

		r++;
	} while (r<pRetry);

	return NULL;
}

/*
**	SetPkgHeader
**
**	Set package header in buffer.
**
**	 -> sBuf	SBuffer.
**
**	Returns 0 if ok, !0 in case of failure.
*/
static int
SetPkgHeader(SBuf *sBuf)
{
	int h=SBufDataLen(sBuf), size;
	unsigned char header[4];

	header[0]='0';
	if (h<0x80) {
		header[1]=(unsigned char)h;
		size=2;
	} else if (h<0xff) {
		header[1]=0x81;		/* ASN_LONG_LEN (0x80) | 1 */
		header[2]=(unsigned char)h;
		size=3;
	} else {
		header[1]=0x82;		/* ASN_LONG_LEN (0x80) | 2 */
		header[2]=(unsigned char)((h>>8)&0xff);
		header[3]=(unsigned char)(h&0xff);
		size=4;
	}

	SBufPutSegRvs(sBuf, header, size);
	if ((SBufWriteError(sBuf))) {
		XLog(LOG_ERR, "(SetPkgHeader) Attempt to write past buffer start");
		return -1;
	}

	return 0;
}

/*
**	Proxy
**
**	Handle proxy logic.
**
**	 -> pkgLen	Package length.
*/
void
Proxy(void)
{
	int pkgLen=pkgLength;
	unsigned char *pkg=pkgBuffer;
	Message m;
	SBuf sBuf;

	/* Let's see what we've got ... */
	XCaptureToFile("---- Input package:\n", pCaptureFile, pkg, pkgLen);

	if ((pkg=VerifyPackage(pkg, &pkgLen))==NULL)
		return;

	/* Disect package */
	ResetNibbleMem();
	if (DisectPackage(&m, &sBuf, pkg, pkgLen)<0)
		return;

	switch (m.data->choiceId) {
	case PDUS_GET_REQUEST:
		if ((GetRequest(&m)))
			return;

		break;

	case PDUS_GET_NEXT_REQUEST:
		if ((GetNextRequest(&m)))
			return;

		break;

	default:
		XLog(LOG_DEBUG, "(Proxy) Cannot handle request (%X)", m.data->choiceId);
		return;
	}
}

/*
**	VerifyPackage
**
**	Do basic verification of package.
**
**	 -> pkg		Package.
**	<-> pkgLen	Package length.
**
**	Returns end of package header or NULL if package is invalid.
**	Updates pkgLen.
*/
unsigned char *
VerifyPackage(unsigned char *pkg, int *pkgLen)
{
	int len=0, lenBytes;
	int pLen=*pkgLen;

	if (*pkgLen<2) {
		XLog(LOG_ERR, "(VerifyPackage) Package too short");
		return NULL;
	}

	if (*pkg!='0') {	// ASN_SEQUENCE (0x10) | ASN_CONSTRUCTOR (0x20)
		XLog(LOG_ERR, "(VerifyPackage) Package not in ASN.1 format");
		return NULL;
	}

	pkg++;
	pLen--;
	if (*pkg&0x80) { /* high bit set? */
		lenBytes=*pkg&~0x80;
		if (!lenBytes) {	/* indefinite length? - forget it! */
			XLog(LOG_ERR, "(VerifyPackage) Indefinte length not supported");
			return NULL;
		}
		if (lenBytes>sizeof(int)) {
			XLog(LOG_ERR, "(VerifyPackage) Length not supported (%i>%i)", lenBytes, sizeof(int));
			return NULL;
		}
		pkg++;
		pLen--;

		while (lenBytes) {
			if (!pLen) {
				XLog(LOG_ERR, "(VerifyPackage) Packet malformed");
				return NULL;
			}

			len<<=8;
			len|=*pkg++;
			pLen--;
			lenBytes--;
		}
		if (len<0) {
			XLog(LOG_ERR, "(VerifyPackage) Length < 0");
			return NULL;
		}
	} else {
		len=*pkg;
		pkg++;
		pLen--;
	}

	if (pLen!=len) {
		XLog(LOG_DEBUG, "(VerifyPackage) Package length mismatch (%i != %i)", pLen, len);
		return NULL;
	}

	*pkgLen=pLen;
	return pkg;
}

/*
**	DisectPackage
**
**	Break down a package into a message.
**
**	 -> m		Message.
**	 -> sBuf	SBuffer.
**	 -> pkg		Package.
**	 -> pkgLen	Package length.
**
**	Return 0 if succesful, !0 otherwise.
*/
int
DisectPackage(Message *m, SBuf *sBuf, unsigned char *pkg, int pkgLen)
{
	AsnLen bD=0;
	int len;
	jmp_buf env;

	SBufInstallData(sBuf, pkg, pkgLen);

	if ((len=setjmp(env))==0)
		BDecMessageContent(sBuf, 0, pkgLen, m, &bD, env);
	else {
		XLog(LOG_WARNING, "(DisectPackage) Malformed package (%i)", len);
		return -1;
	}

	if (m->version!=0) {
		XLog(LOG_DEBUG, "(DisectPackage) Received non SNMPv1 package (version=%i)", m->version);
		return -1;
	}

	if (m->community.octetLen<1) {
		XLog(LOG_DEBUG, "(DisectPackage) Community name not set");
		return -1;
	}

	if ((strcmp(pCommunity, m->community.octs))) {
		XLog(LOG_DEBUG, "(DisectPackage) Community name invalid (got '%s', want '%s'", m->community.octs, pCommunity);
		return -1;
	}

	return 0;
}

/*
**	ProxyPackage
**
**	Send package to peer. Retrieve answer.
*/ 
int
ProxyPackage(Host *h, unsigned char *pkg, int pkgLen)
{
	int sck;

	XLog(LOG_DEBUG, "(ProxyPackage) Sending to %s", h->name);
	if ((h->tick+pCacheName)<time(NULL)) {
		XLog(LOG_DEBUG, "(ProxyPackage) Cache timeout - looking up host name");
		XLookup(h, 1);
	}

	if ((sck=socket(AF_INET, SOCK_DGRAM, 0))<0) {
		XLog(LOG_ERR, "(ProxyPackage) Cannot create socket");
		return -1;
	}

	pkg=PPReal(h, sck, pkg, pkgLen);
	close(sck);
	if (pkg) {
		XCaptureToFile("---- Reply proxy package:\n", pCaptureFile, pkgBuffer, pkgLength);
		return 0;
	}

	return -1;
}

/*
**	Masquerade
**
**	Send masqueraded oid to peer and return message.
**
**	 -> m		Original message.
**	 -> n		OID Node to use.
**	 -> pduType	Which type of request.
**
*/
Message *
Masquerade(Message *m, OidNode *n, int pduType)
{
	Message *new;
	SBuf sBuf;
	int l, pkgLen;
	jmp_buf env;
	Message rM;
	unsigned char *pkg;
	
	if ((new=CopyMessage(m, n, 0, 1))==NULL)
		return NULL;

	new->data->choiceId=pduType;

	SBufInit(&sBuf, masqBuffer, MASQBUFFERLEN);
	SBufResetInWriteRvsMode(&sBuf);

	if ((l=setjmp(env))==0)
		BEncMessageContent(&sBuf, new);
	else {
		XLog(LOG_ERR, "(Masquerade) Cannot encode response (%i)", l);
		return NULL;
	}

	if (SetPkgHeader(&sBuf)<0)
		return NULL;

	XCaptureToFile("---- Masqurade package:\n", pCaptureFile, SBufDataPtr(&sBuf), SBufDataLen(&sBuf));

	if (ProxyPackage(&pPeer, SBufDataPtr(&sBuf), SBufDataLen(&sBuf))<0)
		return NULL;

	pkgLen=pkgLength;
	if ((pkg=VerifyPackage(pkgBuffer, &pkgLen))==NULL)
		return NULL;

	SBufInit(&sBuf, pkg, pkgLen);
	SBufResetInWriteRvsMode(&sBuf);

	if (DisectPackage(&rM, &sBuf, pkg, pkgLen)<0)
		return NULL;

	if ((new=CopyMessage(&rM, n, 1, 0))==NULL)
		return NULL;

	new->data->choiceId=PDUS_GET_RESPONSE;
	return new;
}

/*
**	CopyMessage
**
**	Clone/copy message but set new oid.
**
**	 -> m		Message to copy.
**	 -> n		OidNode.
**	 -> cO		Copy OID (1) or masqueraded oid (0).
**	 -> blank	Blank value (1) or copy (0).
**
**	Returns new message or NULL.
*/
Message *
CopyMessage(Message *m, OidNode *n, int cO, int blank)
{
	Message *sMessage;
	PDUs *sPduData;
	PDU *sPdu;
	VarBindList *sVBL;
	VarBind **sVBD;
	VarBind *mVarBind, *sVarBind;
	SimpleSyntax *s;
	ObjectSyntax *os;
	
	if ((sMessage=NibbleAlloc(sizeof(Message)+sizeof(struct PDUs)+sizeof(struct PDU)+sizeof(VarBind)))==NULL) {
		XLog(LOG_ERR, "(Masquerade) Cannot allocate structure");
		return NULL;
	}

	sMessage->version=m->version;
	sMessage->community.octetLen=m->community.octetLen;
	sMessage->community.octs=m->community.octs;

	sPduData=(PDUs *)(((char *)sMessage)+sizeof(Message));
	sMessage->data=sPduData;

	sPduData->choiceId=m->data->choiceId;
	sPdu=(PDU *)(((char *)sPduData)+sizeof(PDUs));
	sVarBind=(VarBind *)(((char *)sPdu)+sizeof(PDU));

	/* Treat all (valid) PDUs as get_request PDUs */
	switch (m->data->choiceId) {
	case PDUS_GET_REQUEST:
	case PDUS_GET_NEXT_REQUEST:
	case PDUS_GET_RESPONSE:
		sPduData->a.get_request=sPdu;
		sPdu->request_id=m->data->a.get_request->request_id;
		mVarBind=m->data->a.get_request->variable_bindings->first->data;
		sPdu->error_status=m->data->a.get_request->error_status;
		sPdu->error_index=m->data->a.get_request->error_index;
		break;
	default:	/* PDUS_SET_REQUEST | PDUS_TRAP */
		XLog(LOG_ERR, "(Masquerade) Cannot handle PDU type %i", m->data->choiceId);
		return NULL;
	}

	if ((sVBL=AsnListNew(0))==NULL) {
		XLog(LOG_ERR, "(Masquerade) Cannot allocate AsnList");
		return NULL;
	}

	sPdu->variable_bindings=sVBL;

	if ((sVBD=AsnListPrepend(sVBL))==NULL) {
		XLog(LOG_ERR, "(Masquerade) Cannot allocate AsnNode");
		return NULL;
	}

	/* This is sooo nasty!   The pointer returned from AsnListPrepend and
	   AsnListAppend is a pointer to where the data segment pointer must
	   be stored.   Nasty - indirection. */
	*sVBD=sVarBind;
	if (blank) {
		if ((s=NibbleAlloc(sizeof(SimpleSyntax)+sizeof(ObjectSyntax)))==NULL) {
			XLog(LOG_ERR, "(Masquerade) Cannot allocate structure");
			return NULL;
		}
		s->choiceId=SIMPLESYNTAX_EMPTY;
		s->a.empty='\x00';
		
		os=(ObjectSyntax *)(((char *)s)+sizeof(SimpleSyntax));
		os->choiceId=OBJECTSYNTAX_SIMPLE;
		os->a.simple=s;
		sVarBind->value=os;
	} else
		sVarBind->value=mVarBind->value;

	if (cO) {
		sVarBind->name.octs=n->encodedOid.octs;
		sVarBind->name.octetLen=n->encodedOid.octetLen;
	} else {
		sVarBind->name.octs=n->o.masquerade.octs;
		sVarBind->name.octetLen=n->o.masquerade.octetLen;
	}

	return sMessage;
}

/*
**	XmitMessage
**
**	Send message to client.
**
**	 -> m		Message.
**
**	Returns 0 if successful, !0 otherwise.
*/
int
XmitMessage(Message *m)
{
	struct SBuf sBuf;
	jmp_buf env;
	int l;

	switch (m->data->a.get_request->error_status) {
	case NOERROR:
	case TOOBIG:
		m->data->a.get_request->error_index=0;
		break;
	default:
		m->data->a.get_request->error_index=1;
		break;
	}

	m->data->choiceId=PDUS_GET_RESPONSE;

	SBufInit(&sBuf, pkgBuffer, PKGBUFFERLEN);
	SBufResetInWriteRvsMode(&sBuf);
	if ((l=setjmp(env))==0)
		BEncMessageContent(&sBuf, m);
	else {
		XLog(LOG_ERR, "(XmitMessage) Cannot encode response (%i)", l);
		return -1;
	}

	if (SetPkgHeader(&sBuf)<0)
		return -1;

	XCaptureToFile("---- Reply package:\n", pCaptureFile, SBufDataPtr(&sBuf), SBufDataLen(&sBuf));

	if (sendto(sFd, SBufDataPtr(&sBuf), SBufDataLen(&sBuf), 0, (struct sockaddr *)&client, clientLen)<0) {
		XLog(LOG_WARNING, "(XmitMessage) Cannot send reply (%i)", errno);
		return -1;
	}

	return 0;
}

/*
**	CopyVarBind
**
**	Copy varbind info from the first varbind in message to dstV.
**
**	 -> dstV	Where to store varbind info.
**	 -> m		Message to lift varbind from.
**
**	Note. This function does not do an actual copy the content - it merely
**	moves the pointers ...
*/
void
CopyVarBind(VarBind *dstV, Message *m)
{
	VarBind *v=m->data->a.get_next_request->variable_bindings->first->data;

	dstV->name=v->name;
	dstV->value=v->value;
}
