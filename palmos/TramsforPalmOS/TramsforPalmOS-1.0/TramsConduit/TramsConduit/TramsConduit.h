#ifndef  TRAMSCONDUIT
#define  TRAMSCONDUIT
#include <condapi.h>
#include "TramsSync.h"

#ifndef ExportFunc
#define ExportFunc __declspec(dllexport)
#endif

extern "C" {
typedef long (*PROGRESSFN) (char*);
ExportFunc long OpenConduit(PROGRESSFN, CSyncProperties&);
ExportFunc long GetConduitName(char*, WORD);
ExportFunc DWORD GetConduitVersion();
ExportFunc long ConfigureConduit(CSyncPreference& pref);
ExportFunc long GetConduitInfo(ConduitInfoEnum, void *, void *, DWORD *);
}

#endif


