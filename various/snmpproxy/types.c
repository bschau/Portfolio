/*
**	types.c
**
**	The various types.
*/
#include "include.h"

/* protos */
static int HToI(char *);
static void ConvertAsnOct(AsnOcts *);

/*
**	HToI
**
**	Convert two hexbytes to integer.
**
**	 -> src		Source bytes.
**
**	Returns value.
*/
static int
HToI(char *src)
{
	int v, c;

	c=(int)(*src);
	if (c>='0' && c<='9')
		v=c-'0';
	else if (c>='a' && c<='f')
		v=(c-'a')+10;
	else if (c>='A' && c<='F')
		v=(c-'A')+10;
	else {
		XLog(LOG_WARNING, "(HToI) Cannot convert '%c' to integer", c);
		v=0;
	}

	v<<=4;
	c=(int)(*(src+1));
	if (c>='0' && c<='9')
		v|=c-'0';
	else if (c>='a' && c<='f')
		v|=(c-'a')+10;
	else if (c>='A' && c<='F')
		v|=(c-'A')+10;
	else
		XLog(LOG_WARNING, "(HToI) Cannot convert '%c' to integer", c);

	return v;
}

/*
**	ConvertAsnOcts
**
**	Convert '\x??' sequences to their hexcodes.
**
**	<-> a		AsnOct.
*/
static void
ConvertAsnOct(AsnOcts *a)
{
	int srcLen=a->octetLen, len=0;
	char *s=a->octs;
	char *d=a->octs;

	for (srcLen=a->octetLen; srcLen>0; ) {
		switch (*s) {
		case '\\':
			if (srcLen>1) {
				switch (*(s+1)) {
				case '\\':
					*d++='\\';
					s++;
					srcLen--;
					break;
				case 'x':
					if (srcLen>3) {
						*d++=(unsigned char)HToI(s+2);
						s+=3;
						srcLen-=3;
					} else
						XLog(LOG_WARNING, "(ConvertAsnOcts) Missing hex digits");
					break;
				default:
					XLog(LOG_WARNING, "(ConvertAsnOcts) Unknown specifier ('%c')", *(s+1));
					*d++='\\';
					break;
				}
			} else
				*d++='\\';

			break;
		default:
			*d++=*s;
			break;
		}

		len++;
		s++;
		srcLen--;
	}

	a->octetLen=len;
}



/*
**	TypeSetSimpleString
**
**	Alloc and set a string.
**
**	 -> os		ObjectSyntax.
**	 -> src		Source string.
**	 -> srcLen	Length.
**	 -> type	SimpleSyntax type.
*/
SimpleSyntax *
TypeSetSimpleString(ObjectSyntax *os, char *src, unsigned int srcLen, int type)
{
	SimpleSyntax *s=NibbleAlloc(sizeof(SimpleSyntax)+sizeof(AsnOcts)+srcLen+1);
	AsnOcts *a;

	if (s==NULL) {
		XLog(LOG_ERR, "(TypeSetSimpleString) Cannot allocate structure");
		return NULL;
	}

	s->choiceId=type;

	a=(AsnOcts *)(((char *)s)+sizeof(SimpleSyntax));
	s->a.string=a;

	a->octetLen=srcLen;
	a->octs=((char *)a)+sizeof(AsnOcts);
	memmove(a->octs, src, srcLen);

	ConvertAsnOct(a);

	os->choiceId=OBJECTSYNTAX_SIMPLE;
	return s;
}

/*
**	TypeSetSimpleNumber
**
**	Alloc and set a number.
**
**	 -> os		ObjectSyntax.
**	 -> num		Number
*/
SimpleSyntax *
TypeSetSimpleNumber(ObjectSyntax *os, unsigned int num)
{
	SimpleSyntax *s=NibbleAlloc(sizeof(SimpleSyntax));

	if (s==NULL) {
		XLog(LOG_ERR, "(TypeSetSimpleNumber) Cannot allocate structure");
		return NULL;
	}

	s->choiceId=SIMPLESYNTAX_NUMBER;
	s->a.number=num;

	os->choiceId=OBJECTSYNTAX_SIMPLE;
	return s;
}

/*
**	TypeSetSimpleNull
**
**	Alloc and set a NULL value.
**
**	 -> os		ObjectSyntax.
*/
SimpleSyntax *
TypeSetSimpleNull(ObjectSyntax *os)
{
	SimpleSyntax *s=NibbleAlloc(sizeof(SimpleSyntax));

	if (s==NULL) {
		XLog(LOG_ERR, "(TypeSetSimpleNull) Cannot allocate structure");
		return NULL;
	}

	s->choiceId=SIMPLESYNTAX_EMPTY;
	s->a.empty='\x00';

	os->choiceId=OBJECTSYNTAX_SIMPLE;
	return s;
}

/*
**	TypeSetApplicationString
**
**	Alloc and set an application string.
**
**	 -> os		ObjectSyntax.
**	 -> src		Source string.
**	 -> srcLen	Length.
**	 -> type	ApplicationSyntax type.
*/
ApplicationSyntax *
TypeSetApplicationString(ObjectSyntax *os, char *src, unsigned int srcLen, int type)
{
	ApplicationSyntax *s=NibbleAlloc(sizeof(ApplicationSyntax)+sizeof(AsnOcts)+srcLen+1);
	AsnOcts *a;

	if (s==NULL) {
		XLog(LOG_ERR, "(TypeSetApplicationString) Cannot allocate structure");
		return NULL;
	}

	s->choiceId=type;

	a=(AsnOcts *)(((char *)s)+sizeof(ApplicationSyntax));
	switch (type) {
	case APPLICATIONSYNTAX_ARBITRARY:
		s->a.arbitrary=a;
		break;
	default:
		XLog(LOG_ERR, "(TypeSetApplicationString) Hein?! Unknown type %i", type);
		return NULL;
	}

	a->octetLen=srcLen;
	a->octs=((char *)a)+sizeof(AsnOcts);
	memmove(a->octs, src, srcLen);

	ConvertAsnOct(a);

	os->choiceId=OBJECTSYNTAX_APPLICATION_WIDE;
	return s;
}

/*
**	TypeSetApplicationNumber
**
**	Alloc and set an application number.
**
**	 -> os		ObjectSyntax.
**	 -> num		Number
**	 -> type	ApplicationSyntax type.
**
*/
ApplicationSyntax *
TypeSetApplicationNumber(ObjectSyntax *os, unsigned int num, int type)
{
	ApplicationSyntax *s=NibbleAlloc(sizeof(ApplicationSyntax));

	if (s==NULL) {
		XLog(LOG_ERR, "(TypeSetApplicationNumber) Cannot allocate structure");
		return NULL;
	}

	s->choiceId=type;
	switch (type) {
	case APPLICATIONSYNTAX_COUNTER:
		s->a.counter=num;
		break;
	case APPLICATIONSYNTAX_GAUGE:
		s->a.gauge=num;
		break;
	case APPLICATIONSYNTAX_TICKS:
		s->a.ticks=num;
		break;
	default:
		XLog(LOG_ERR, "(TypeSetApplicationNumber) Hein?! Unknown type %i", type);
		return NULL;
	}
		
	os->choiceId=OBJECTSYNTAX_APPLICATION_WIDE;
	return s;
}

/*
**	TypeSetApplicationIp
**
**	Set IP Address.
**
**	 -> os		ObjectSyntax.
**	 -> ip		IP Address.
*/
ApplicationSyntax *
TypeSetApplicationIp(ObjectSyntax *os, char *ip)
{
	ApplicationSyntax *s=NibbleAlloc(sizeof(ApplicationSyntax)+sizeof(NetworkAddress)+sizeof(IpAddress)+4);
	AsnOcts *a;
	NetworkAddress *n;

	if (s==NULL) {
		XLog(LOG_ERR, "(TypeSetApplicationIp) Cannot allocate structure");
		return NULL;
	}

	s->choiceId=APPLICATIONSYNTAX_ADDRESS;

	n=(NetworkAddress *)(((char *)s)+sizeof(ApplicationSyntax));
	a=(AsnOcts *)(((char *)n)+sizeof(NetworkAddress));

	s->a.address=n;
	n->choiceId=NETWORKADDRESS_INTERNET;
	n->a.internet=a;

	a->octetLen=4;
	a->octs=((char *)a)+sizeof(AsnOcts);
	memmove(a->octs, ip, 4);

	os->choiceId=OBJECTSYNTAX_APPLICATION_WIDE;
	return s;
}
