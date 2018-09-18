#ifndef _LEGENDOFMIR2_MISC_FUNC
#define _LEGENDOFMIR2_MISC_FUNC

void		CenterDialog(HWND hParentWnd, HWND hWnd);

__int64		FileTimeToQuadWord(PFILETIME pFileTime);
void		QuadTimeToFileTime(__int64 qw, PFILETIME pFileTime);

int			memlen(const char *str);
int			AnsiStrToVal(const char *nptr);
char		*ValToAnsiStr(unsigned long val, char *buf);

void		ChangeSpaceToNull(char *pszData);

void		GetDate(char *pszBuf);
bool		IsEmptyGuid(const GUID *guid);
//char		*ConvertLPWSTRToLPSTR(LPWSTR lpwszStrIn);
GUID		*GetGuid(GUID* guid=NULL);
char		*GetGuidSZ(char *szDest);
char		*GetGuidSZ(char *szDest, const GUID *guid);
GUID		*GetGuidTagFromString(const char *szSrc, GUID *guid);
int			GetTime();

#endif
