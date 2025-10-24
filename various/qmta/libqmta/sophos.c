#include "private.h"

/* macros */
#define SOPHOSARGLEN 256	/* sufficient, ntfydemo specifies 100 */
#define FILENAMELEN 128		/* a magic constant */
#define VIRUSNAMELEN 128

/* globals */
static struct {
	LPCOLESTR name;
	U32 type;
	LPCOLESTR value;
} sitab[]= {	/* see 'savidtk.pdf' pp. 53 */
/*	{ SOPHOS_NAMESPACE_SUPPORT, SOPHOS_TYPE_U32, "3" },		*/
	{ SOPHOS_DO_FULL_SWEEP, SOPHOS_TYPE_U32, "1" },
	{ SOPHOS_DYNAMIC_DECOMPRESSION, SOPHOS_TYPE_U32, "1" },
	{ SOPHOS_FULL_MACRO_SWEEP, SOPHOS_TYPE_U32, "1" },
	{ SOPHOS_IGNORE_TEMPLATE_BIT, SOPHOS_TYPE_U32, "1" },
	{ SOPHOS_OLE2_HANDLING, SOPHOS_TYPE_U32, "1" },
	{ SOPHOS_VBA3_HANDLING, SOPHOS_TYPE_U32, "1" },
	{ SOPHOS_VBA5_HANDLING, SOPHOS_TYPE_U32, "1" },
	{ SOPHOS_DECOMPRESS_VBA5, SOPHOS_TYPE_U32, "1" },
	{ SOPHOS_DO_EMULATION, SOPHOS_TYPE_U32, "1" },
	{ SOPHOS_PE_HANDLING, SOPHOS_TYPE_U32, "1" },
	{ SOPHOS_XF_HANDLING, SOPHOS_TYPE_U32, "1" },
	{ SOPHOS_PM97_HANDLING, SOPHOS_TYPE_U32, "1" },
	{ SOPHOS_ZIP_DECOMPRESSION, SOPHOS_TYPE_U32, "1" },
	{ SOPHOS_ARJ_DECOMPRESSION, SOPHOS_TYPE_U32, "1" },
	{ SOPHOS_RAR_DECOMPRESSION, SOPHOS_TYPE_U32, "1" },
	{ SOPHOS_UUE_DECOMPRESSION, SOPHOS_TYPE_U32, "1" },
	{ SOPHOS_GZIP_DECOMPRESSION, SOPHOS_TYPE_U32, "1" },
	{ SOPHOS_TAR_DECOMPRESSION, SOPHOS_TYPE_U32, "1" },
	{ SOPHOS_CMZ_DECOMPRESSION, SOPHOS_TYPE_U32, "1" },
	{ SOPHOS_LOOPBACK_ENABLED, SOPHOS_TYPE_U32, "1" },
	{ SOPHOS_MAX_RECURSION_DEPTH, SOPHOS_TYPE_U16, "65535" },
	{ NULL },
};
static CISavi2 *savi;
static int verbose;

/* protos */
static void listconfigs(void);
static CISavi2 *opensavi(void);
static int configuresavi(void);
static int analyze(CIEnumSweepResults *, char *);

/*******************************************************************************
*
*	listconfigs
*
*	Print config settings to stdout.
*/
static void
listconfigs(void)
{
	CIEnumEngineConfig *ce=NULL;
	CIEngineConfig *conf=NULL;
	U32 cf, type;
	HRESULT hr;
	char *name, *value;

	printf("Configuration:\n\n");
	hr=savi->pVtbl->GetConfigEnumerator(savi, (REFIID)&SOPHOS_IID_ENUM_ENGINECONFIG, (void **)&ce);
	if (SOPHOS_FAILED(hr)) {
		fprintf(stderr, "cannot get config enumerator\n");
		return;
	}

	hr=ce->pVtbl->Reset(ce);
	if (SOPHOS_FAILED(hr)) {
		fprintf(stderr, "failed to reset configenum\n");
		return;
	}

	name=line;
	value=line+SOPHOSARGLEN+2;
	for ( ;ce->pVtbl->Next(ce, 1, (void **)&conf, &cf)==SOPHOS_S_OK; conf->pVtbl->Release(conf)) {
		*name='\x00';
		*value='\x00';

		hr=conf->pVtbl->GetName(conf, SOPHOSARGLEN, name, NULL);
		if (SOPHOS_FAILED(hr)) {
			fprintf(stderr, "cannot get option name\n");
			continue;
		}

		hr=conf->pVtbl->GetType(conf, &type);
		if (SOPHOS_FAILED(hr)) {
			fprintf(stderr, "cannot get type for %s\n", name);
			continue;
		}

		hr=savi->pVtbl->GetConfigValue(savi, name, type, SOPHOSARGLEN, value, NULL);
		if (SOPHOS_FAILED(hr)) {
			fprintf(stderr, "cannot get config value for %s\n", name);
			continue;
		}

		/* all set, pretty print */
		printf("%-24s [%ld]: %s\n", name, (unsigned long)type, value);
	}

	ce->pVtbl->Release(ce);
}

/*******************************************************************************
*
*	opensavi
*
*	Open connection to SAVI.
*
*	Output:
*		s: savi handle.
*		f: NULL.
*/
static CISavi2 *
opensavi(void)
{
	CISavi2 *s=NULL;
	CISweepClassFactory2 *fac;
	HRESULT hr;

	hr=DllGetClassObject((REFIID)&SOPHOS_CLSID_SAVI2, (REFIID)&SOPHOS_IID_CLASSFACTORY2, (void **)&fac);
	if (SOPHOS_FAILED(hr)) {
		if (verbose)
			fprintf(stderr, "failed to get class factory interface\n");
		else
			qlog(QFATAL, "(opensavi) failed to get class factory interface");
		return NULL;
	}

	hr=fac->pVtbl->CreateInstance(fac, NULL, &SOPHOS_IID_SAVI2, (void **)&s);

	/* factory not needed any more */
	fac->pVtbl->Release(fac);

	/* CSavi2 gotten? */
	if (SOPHOS_FAILED(hr)) {
		if (verbose)
			fprintf(stderr, "failed to get CSavi2 interface\n");
		else
			qlog(QFATAL, "(opensavi) failed to get CSavi2 interface");
		return NULL;
	}

	/* Ask savi to initialize itself */
	hr=s->pVtbl->InitialiseWithMoniker(s, QNAME);
	if (SOPHOS_FAILED(hr)) {
		if (verbose)
			fprintf(stderr, "failed to initialize SAVI\n");
		else
			qlog(QFATAL, "(opensavi) failed to initialize SAVI");
		s->pVtbl->Release(s);
		return NULL;
	}

	/* done! */
	return s;
}

/*******************************************************************************
*
*	configuresavi
*
*	Configures the SAVI engine.
*
*	Output:
*		s: 0.
*		f: !0.
*/
static int
configuresavi(void)
{
	int ret=0, cnt;
	HRESULT hr;

	for (cnt=0; sitab[cnt].name; cnt++) {
		hr=savi->pVtbl->SetConfigValue(savi, sitab[cnt].name, sitab[cnt].type, sitab[cnt].value);
		if (SOPHOS_FAILED(hr)) {
			if (verbose) {
				fprintf(stderr, "unable to set %s: ", (char *)sitab[cnt].name);
				if (hr==SOPHOS_SAVI2_ERROR_NOT_SUPPORTED)
					fprintf(stderr, "not supported");
				fprintf(stderr, "\n");
			} else {
				qlog(QFATAL, "(configuresavi) failed to set %s", (char *)sitab[cnt].name);
				ret=1;
			}
		}
	} 
	
	return ret;
}

/*******************************************************************************
*
*	sophosopen
*
*	Open connection to sophos scan engine.
*
*	Input:
*		v - verbosity.
*	Output:
*		s: 0.
*		f: !0.
*/
int
sophosopen(int v)
{
	int err;

	verbose=v;

	elinkopen();
	savi=opensavi();
	if (!savi) 
		return -1;

	err=configuresavi();

	return 0;
}

/*******************************************************************************
*
*	sophosclose
*
*	Close connection to sophos scan engine.
*/
void
sophosclose(void)
{
	if (savi) {
		savi->pVtbl->Terminate(savi);
		savi->pVtbl->Release(savi);
		savi=NULL;
	}

	elinkclose();
	verbose=0;
}

/*******************************************************************************
*
*	sophoslist
*
*	Print sophos characteristica.
*/
void
sophoslist(void)
{
	CIEnumIDEDetails *idelist=NULL;
	HRESULT hr;
	U32 enginever, viras;
	SYSTEMTIME date;
	int err;

	err=sophosopen(1);
	if (err)
		return;

	/* header */
	hr=savi->pVtbl->GetVirusEngineVersion(savi, &enginever, line, SOPHOSARGLEN, &date, &viras, NULL, (REFIID)&SOPHOS_IID_ENUM_IDEDETAILS, (void **)&idelist);
	if (SOPHOS_FAILED(hr))
		fprintf(stderr, "failed to get header\n\n");
	else {
		printf("Virus engine version: %u.%u\n", (unsigned int)((enginever>>16)&0xffff), (unsigned int)(enginever&0xffff));
		printf("Virus data version:   %s\n", line);
		printf("Number of vira:       %i\n\n", viras);
	}

	listconfigs();
}

/*******************************************************************************
*
*	sophos
*
*	Virus scan file.
*
*	Input:
*		file - file to scan.
*	Output:
*		s: SCLEAN, SINFECTED.
*		f: -1.
*/
int
sophos(char *file)
{
	int ret=-1;
	CIEnumSweepResults *esr=NULL;
	HRESULT hr;

	/* scan file */
	hr=savi->pVtbl->SweepFile(savi, file, (REFIID)&SOPHOS_IID_ENUM_SWEEPRESULTS, (void **)&esr);
	if (SOPHOS_FAILED(hr)) {
		qlog(QFATAL, "(sophos) SweepFile failed on %s (%08x)", file, hr);
		ret=-1;
	} else {
		if (hr!=SOPHOS_S_OK)
			ret=analyze(esr, file);
		else
			ret=SCLEAN;

		if (esr)
			esr->pVtbl->Release(esr);
	}

	return ret;
}

/*******************************************************************************
*
*	analyze
*
*	Analyzes the result from SweepFile.
*
*	Input:
*		esr - result enumerator.
*		file - file name.
*	Ouput:
*		see 'sophos'.
*/
static int
analyze(CIEnumSweepResults *esr,
        char *file)
{
	int clean=SCLEAN, err;
	CISweepResults *res=NULL;
	SOPHOS_ULONG f;
	HRESULT hr;
	U32 vt;

	hr=esr->pVtbl->Reset(esr);
	if (SOPHOS_FAILED(hr)) {
		qlog(QFATAL, "(analyze) cannot reset enum");
		return -1;
	}

	for (; esr->pVtbl->Next(esr, 1, (void **)&res, &f)==SOPHOS_S_OK; ) {
		hr=res->pVtbl->GetVirusType(res, &vt);
		if (SOPHOS_FAILED(hr)) {
			clean=-1;
			break;
		}

		if (vt==SOPHOS_NO_VIRUS)
			continue;

		clean=SINFECTED;
		memset(line, 0, HUGELINELEN);
		strncpy(line, file, FILENAMELEN);
		strcat(line, " contained the virus ");
		err=strlen(line);
		
		hr=res->pVtbl->GetVirusName(res, VIRUSNAMELEN, line+err, NULL);
		if (SOPHOS_FAILED(hr))
			err=elinkadd("one or more vira detected");
		else
			err=elinkadd(line);

		if (err)
			qlog(QWARN, "(analyze) cannot add elink message");

		res->pVtbl->Release(res);
		res=NULL;
	}

	return clean;
}
