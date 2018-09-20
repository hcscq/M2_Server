#include "stdafx.h"
#include <stdio.h>

void CenterDialog(HWND hParentWnd, HWND hWnd)
{
	RECT rcMainWnd, rcDlg;
	
	GetWindowRect(hParentWnd, &rcMainWnd);
	GetWindowRect(hWnd, &rcDlg);
	
	MoveWindow(hWnd, rcMainWnd.left + (((rcMainWnd.right - rcMainWnd.left) - (rcDlg.right - rcDlg.left)) / 2), 
				rcMainWnd.top + (((rcMainWnd.bottom - rcMainWnd.top) - (rcDlg.bottom - rcDlg.top)) / 2), 
				(rcDlg.right - rcDlg.left), (rcDlg.bottom - rcDlg.top), FALSE);
}

__int64 FileTimeToQuadWord(PFILETIME pFileTime)
{  
	__int64 qw;

	qw = pFileTime->dwHighDateTime;
	qw <<= 32;
	qw |= pFileTime->dwLowDateTime;

	return qw;
}

void QuadTimeToFileTime(__int64 qw, PFILETIME pFileTime)
{
	pFileTime->dwHighDateTime	= (DWORD)(qw >> 32);
	pFileTime->dwLowDateTime	= (DWORD)(qw & 0xFFFFFFFF);
}

int memlen(const char *str)
{
    const char *eos = str;

    while(*eos++);

    return((int)(eos - str));
}

int AnsiStrToVal(const char *nptr)
{
    int c		= (int)(unsigned char)*nptr++;
    int total	= 0;

    while (c >= '0' && c <= '9') 
	{
        total = 10 * total + (c - '0');     /* accumulate digit */
        c = (int)(unsigned char)*nptr++;    /* get next char */
    }

	return total;
}

char *ValToAnsiStr(unsigned long val, char *buf)
{
    char *p;                /* pointer to traverse string */
    char *firstdig;         /* pointer to first digit */
    char temp;              /* temp char */
	char *next;
    unsigned digval;        /* value of digit */

    p = buf;

    firstdig = p;           /* save pointer to first digit */

    do {
        digval = (unsigned) (val % 10);
        val /= 10;	       /* get next digit */

        /* convert to ascii and store */
        if (digval > 9)
            *p++ = (char) (digval - 10 + 'a');  /* a letter */
        else
            *p++ = (char) (digval + '0');       /* a digit */
    } while (val > 0);

    /* We now have the digit of the number in the buffer, but in reverse
       order.  Thus we reverse them now. */

	next = p;
    *p-- = '\0';            /* terminate string; p points to last digit */

    do {
        temp = *p;
        *p = *firstdig;
        *firstdig = temp;   /* swap *p and *firstdig */
        --p;
        ++firstdig;         /* advance to next two digits */
    } while (firstdig < p); /* repeat until halfway */

	return next;
}

void ChangeSpaceToNull(char *pszData)
{
	char *pszCheck = pszData;

	while (*pszCheck)
	{
		if (*pszCheck == 0x20)
		{
			*pszCheck = '\0';
			break;
		}

		pszCheck++;
	}
}

void GetDate(char *pszBuf)
{
	time_t	t;
	struct tm ttm;
	
	time(&t);
	memcpy(&ttm, localtime(&t), sizeof(struct tm));
	
	sprintf(pszBuf, "%02d%02d%02d", ttm.tm_year - 100, ttm.tm_mon + 1, ttm.tm_mday);
}
bool IsEmptyGuid(const GUID *guid) 
{
	const char emptyGuid[] = { "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0" };
	if (memcmp(guid, emptyGuid, sizeof(GUID))) return false;
	return true;
}

GUID *GetGuid(GUID * guid) 
{
	if (guid) 
	{
		CoInitialize(NULL);
		CoCreateGuid(guid);
		CoUninitialize();
	}
	return guid;
}
char *GetGuidSZ(char *szDest)
{
	GUID guid;
	CoInitialize(NULL);
	CoCreateGuid(&guid);
	sprintf_s(szDest, 37, "%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X",
		guid.Data1, guid.Data2, guid.Data3,
		guid.Data4[0], guid.Data4[1],
		guid.Data4[2], guid.Data4[3],
		guid.Data4[4], guid.Data4[5],
		guid.Data4[6], guid.Data4[7]);
	CoUninitialize();
	return szDest;
}
char *GetGuidSZ(char *szDest,const GUID *guid)
{
	sprintf_s(szDest, 37, "%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X",
		guid->Data1, guid->Data2, guid->Data3,
		guid->Data4[0], guid->Data4[1],
		guid->Data4[2], guid->Data4[3],
		guid->Data4[4], guid->Data4[5],
		guid->Data4[6], guid->Data4[7]);
	return szDest;
}
GUID * GetGuidTagFromString(const char *szSrc,GUID *guid)
{
	short len = memlen(szSrc)-1;
	if (len <= 0 || len > 37)
	{
		memset(guid,0,sizeof(GUID));
		return guid;
	}
	//wchar_t S[] = L"SDS";
	//wchar_t*	m_wchar = new wchar_t[MultiByteToWideChar(CP_ACP, 0, szSrc, strlen(szSrc), NULL, 0) + 1];
	//MultiByteToWideChar(CP_ACP, 0, szSrc, strlen(szSrc), m_wchar, len);
	//m_wchar[len] = '\0';
	//CLSIDFromString(m_wchar, guid);
	//delete m_wchar;
	sscanf_s(
		szSrc,
		"%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x",
		&(guid->Data1),
		&(guid->Data2),
		&(guid->Data3),
		&(guid->Data4[0]),
		&(guid->Data4[1]),
		&(guid->Data4[2]),
		&(guid->Data4[3]),
		&(guid->Data4[4]),
		&(guid->Data4[5]),
		&(guid->Data4[6]),
		&(guid->Data4[7])
	);
	return guid;
}
int GetTime()
{
	time_t	t;
	struct tm ttm;
	
	time(&t);
	memcpy(&ttm, localtime(&t), sizeof(struct tm));
	
//	wsprintf(pszBuf, _TEXT("%02d%02d%02d"), ttm.tm_year - 100, ttm.tm_mon + 1, ttm.tm_mday);

	return ttm.tm_hour;
}
