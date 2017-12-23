#include "stdafx.h"
#include "processdbmsg.h"
#include <stdio.h>
#include "../def/dbmgr.h"

extern CWHQueue					g_DBMsgQ;

extern BOOL						g_fTerminated;

extern CWHList< GAMESERVERINFO * >	g_xGameServerList;

void SendSocket(SOCKET s, int nCertification, char *pszData1, char *pszData2, int nData2Len)
{
	int		nLen = nData2Len + DEFBLOCKSIZE + 6;
	LONG	lCert = MAKELONG(nCertification ^ 0xAA, nLen);
	char	szCert[24];
	char	szEncodeMsg[8096];

	int nCertLen = fnEncode6BitBufA((unsigned char *)&lCert, szCert, sizeof(LONG), sizeof(szCert));

	szEncodeMsg[0] = '#';
	char *pszPos = ValToAnsiStr(nCertification, &szEncodeMsg[1]);

	*pszPos++ = '/';

	memmove(pszPos, pszData1, DEFBLOCKSIZE);
	pszPos += DEFBLOCKSIZE;
	memmove(pszPos, pszData2, nData2Len);
	pszPos += nData2Len;
	memmove(pszPos, szCert, nCertLen);

	pszPos		+= nCertLen;

	*pszPos++	= '!';
	*pszPos		= '\0';

	int nSendLen = memlen(szEncodeMsg) - 1;

	send(s, szEncodeMsg, nSendLen, 0);
}

int GetHorseRcd(char *szName, _LPTHORSERCD lpTHorseRcd)
{
	char szQuery[1024];
	sprintf( szQuery, "SELECT * FROM TBL_CHARACTER_HORSE WHERE FLD_CHARNAME='%s'", szName );

	CRecordset *pRec = GetDBManager()->CreateRecordset();
	if ( pRec->Execute( szQuery ) && pRec->Fetch() )
	{
		strcpy( lpTHorseRcd->szHorseIndex, pRec->Get( "FLD_HORSEINDEX" ) );
		lpTHorseRcd->btHorseType = atoi( pRec->Get( "FLD_HORSETYPE" ) );
	}
	else
	{
		GetDBManager()->DestroyRecordset( pRec );
		return 0;
	}
	GetDBManager()->DestroyRecordset( pRec );

	return 1;
}

void GetHumanGenItemRcd(char *szName, CWHList<_LPTGENITEMRCD>	*pxUserGenItemRcdList)
{
	char szQuery[128];

	sprintf( szQuery, "SELECT * FROM TBL_CHARACTER_ITEM WHERE FLD_CHARNAME='%s'", szName );

	CRecordset *pRec = GetDBManager()->CreateRecordset();
	
	if ( pRec->Execute( szQuery ) )
	{		
		while ( pRec->Fetch() )
		{
			_LPTGENITEMRCD pItemRcd = new _TGENITEMRCD;

			if ( pItemRcd )
			{
				strcpy( pItemRcd->szItem, pRec->Get( "FLD_ITEMINDEX" ) );
				pxUserGenItemRcdList->AddNewNode( pItemRcd );
			}
		}
	}

	GetDBManager()->DestroyRecordset( pRec );
}

void GetHumanMagicRcd(char *szName, CWHList<_LPTHUMANMAGICRCD>	*pxUserMagicRcdList)
{
	char szQuery[1024];

	sprintf( szQuery, "SELECT * FROM TBL_CHARACTER_MAGIC WHERE FLD_CHARNAME='%s'", szName );

	CRecordset *pRec = GetDBManager()->CreateRecordset();
	
	if (pRec->Execute( szQuery ))
	{
		while (pRec->Fetch())
		{
			_LPTHUMANMAGICRCD	lptUserMagicRcd = new _THUMANMAGICRCD;
			
			if (lptUserMagicRcd)
			{
				lptUserMagicRcd->btMagicID	= atoi( pRec->Get( "FLD_MAGICID" ) );
				lptUserMagicRcd->btLevel	= atoi( pRec->Get( "FLD_LEVEL" ) );
				lptUserMagicRcd->btUseKey	= atoi( pRec->Get( "FLD_USEKEY" ) );
				lptUserMagicRcd->nCurrTrain	= atoi( pRec->Get( "FLD_CURRTRAIN" ) );
				
				pxUserMagicRcdList->AddNewNode(lptUserMagicRcd);
			}
		}
	}

	GetDBManager()->DestroyRecordset( pRec );
}

void GetHumanItemRcd(const char *szGuid, CWHList<_LPTUSERITEMRCD>	*pxUserItemRcdList)
{
	char szQuery[128];

	sprintf( szQuery, "SELECT * FROM TBL_CHARACTER_ITEM WHERE FLD_OWNER='%s'", szGuid);

	CRecordset *pRec = GetDBManager()->CreateRecordset();
	
	if ( pRec->Execute( szQuery ) )
	{
		while ( pRec->Fetch() )
		{
			_LPTUSERITEMRCD	pItem = new _TUSERITEMRCD;
			/*
			AC, MAC, DC, MC, SC, Accuracy, 
			Agility, HP, MP, Strong, MagicResist, PoisonResist, 
			HealthRecovery, ManaRecovery, PoisonRecovery, 
			CriticalRate, CriticalDamage, Freezing, PoisonAttack

			public RefinedValue RefinedValue = RefinedValue.None;
			public byte RefineAdded = 0;

			public bool DuraChanged;
			public int SoulBoundId = -1;
			public bool Identified = false;
			public bool Cursed = false;

			public int WeddingRing = -1;

			public UserItem[] Slots = new UserItem[5];

			public DateTime BuybackExpiryDate;

			public ExpireInfo ExpireInfo;
			public RentalInformation RentalInformation;

			public Awake Awake = new Awake();
			*/
			pItem->szMakeIndex[0] = *(pRec->Get( "FLD_STDTYPE" ));
			memcpy( &pItem->szMakeIndex[1], pRec->Get( "FLD_MAKEDATE" ), 6 );
			memcpy( &pItem->szMakeIndex[7], pRec->Get( "FLD_MAKEINDEX" ), 36 );
			
			pItem->nStdIndex	= atoi( pRec->Get( "FLD_STDINDEX" ) );
			pItem->nDura		= atoi( pRec->Get( "FLD_DURA" ) );
			pItem->nDuraMax		= atoi( pRec->Get( "FLD_DURAMAX" ) );
			pItem->btValue[0]	= atoi( pRec->Get( "FLD_AC" ) );
			pItem->btValue[1]	= atoi( pRec->Get( "FLD_MAC" ) );
			pItem->btValue[2]	= atoi( pRec->Get( "FLD_DC" ) );
			pItem->btValue[3]	= atoi( pRec->Get( "FLD_MC" ) );
			pItem->btValue[4]	= atoi( pRec->Get( "FLD_SC" ) );
			pItem->btValue[5]	= atoi( pRec->Get( "FLD_Accuracy" ) );
			pItem->btValue[6]	= atoi( pRec->Get( "FLD_Agility" ) );
			pItem->btValue[7]	= atoi( pRec->Get( "FLD_HP" ) );
			pItem->btValue[8]	= atoi( pRec->Get( "FLD_MP" ) );
			pItem->btValue[9]	= atoi( pRec->Get( "FLD_Strong" ) );
			pItem->btValue[10]	= atoi( pRec->Get( "FLD_MagicResist" ) );
			pItem->btValue[11]	= atoi( pRec->Get( "FLD_PoisonResist" ) );
			pItem->btValue[12]	= atoi( pRec->Get( "FLD_HealthRecovery" ) );

			pItem->btValue[13] = atoi(pRec->Get("FLD_ManaRecovery"));
			pItem->btValue[14] = atoi(pRec->Get("FLD_PoisonRecovery"));
			pItem->btValue[15] = atoi(pRec->Get("FLD_CriticalRate"));
			pItem->btValue[16] = atoi(pRec->Get("FLD_CriticalDamage"));
			pItem->btValue[17] = atoi(pRec->Get("FLD_Freezing"));
			pItem->btValue[18] = atoi(pRec->Get("FLD_PoisonAttack"));
			pItem->btValue[19] = atoi(pRec->Get("FLD_RefinedValue"));
			pItem->btValue[20] = atoi(pRec->Get("FLD_RefineAdded"));
			/*FLD_DuraChanged|FLD_Identified|FLD_Cursed|FLD_WeddingRing*/
			pItem->btValue[21] = atoi(pRec->Get("FLD_Switchs"));
			memcpy(pItem->szBoundGuid, pRec->Get("FLD_SoulBoundGuid"),36);

			pItem->usCount = atoi(pRec->Get("FLD_COUNT"));

			ZeroMemory(pItem->szPrefixName, sizeof(pItem->szPrefixName));
			memcpy( pItem->szPrefixName, pRec->Get( "FLD_PREFIXNAME"),20 );

			pItem->sbtValue[0] = atoi(pRec->Get("FLD_AttackSpeed"));
			pItem->sbtValue[1] = atoi(pRec->Get("FLD_Luck"));

			pxUserItemRcdList->AddNewNode(pItem);
		}
	}
	GetDBManager()->DestroyRecordset( pRec );
}

BOOL GetHumanRcd(char	*szName, _LPTHUMANRCD lptHumanRcd, _LPTLOADHUMAN lpLoadHuman)
{
	char szQuery[1024];
	char *pItemMakeIndex;
	//memset(szItemMakeIndex,0,sizeof(szItemMakeIndex));

	sprintf( szQuery, "SELECT * FROM TBL_CHARACTER WHERE FLD_LOGINID='%s' AND FLD_INDEX=%d AND FLD_ISDELETED=0", lpLoadHuman->szUserID,lpLoadHuman->btCharIndex );

	CRecordset *pRec = GetDBManager()->CreateRecordset();
	CRecordset *pRec2 = GetDBManager()->CreateRecordset();

	if ( pRec->Execute( szQuery ) && pRec->Fetch() )
	{	
		memcpy(lpLoadHuman->szCharGuid, pRec->Get("FLD_GUID"),sizeof(lpLoadHuman->szCharGuid));
		strcpy(lpLoadHuman->szCharName, pRec->Get("FLD_CHARNAME"));

		strcpy(lptHumanRcd->szCharGuid,lpLoadHuman->szCharGuid);
		strcpy(lptHumanRcd->szUserID, pRec->Get( "FLD_LOGINID" ) );
		ChangeSpaceToNull(lptHumanRcd->szUserID);
		strcpy(lptHumanRcd->szCharName, pRec->Get( "FLD_CHARNAME" ) );
		ChangeSpaceToNull(lptHumanRcd->szCharName);

		memcpy(lptHumanRcd->szCharGuid, pRec->Get("FLD_GUID"), sizeof(lpLoadHuman->szCharGuid));
		lptHumanRcd->btIndex	= (BYTE)atoi(pRec->Get("FLD_INDEX"));
		lptHumanRcd->btJob		= (BYTE)atoi( pRec->Get( "FLD_JOB" ) );
		lptHumanRcd->btGender	= (BYTE)atoi( pRec->Get( "FLD_GENDER" ) );
		lptHumanRcd->szLevel	= (BYTE)atoi( pRec->Get( "FLD_LEVEL" ) );
		lptHumanRcd->nDirection	= (BYTE)atoi( pRec->Get( "FLD_DIRECTION" ) );
		lptHumanRcd->nCX		= atoi( pRec->Get( "FLD_CX" ) );
		lptHumanRcd->nCY		= atoi( pRec->Get( "FLD_CY" ) );

		lptHumanRcd->btAttackMode	= atoi( pRec->Get( "FLD_ATTACKMODE" ) );
		lptHumanRcd->nExp			= atoi( pRec->Get( "FLD_EXP" ) );

		strcpy( lptHumanRcd->szMapName, pRec->Get( "FLD_MAPNAME" ) );
		ChangeSpaceToNull( lptHumanRcd->szMapName );

		lptHumanRcd->dwGold		= atoi( pRec->Get( "FLD_GOLD" ) );
		lptHumanRcd->szHair		= atoi( pRec->Get( "FLD_HAIR" ) );

		lptHumanRcd->fIsAdmin = (BYTE)*pRec->Get("FLD_ISADMIN");

		char FLD_NAME[10][20] = { "FLD_DRESS_ID" ,"FLD_WEAPON_ID" ,"FLD_LEFTHAND_ID" ,"FLD_RIGHTHAND_ID" ,"FLD_HELMET_ID" ,"FLD_NECKLACE_ID" ,
			"FLD_ARMRINGL_ID" ,"FLD_ARMRINGR_ID" ,"FLD_RINGL_ID" ,"FLD_RINGR_ID" };
		for (int i = 0; i < sizeof(lptHumanRcd->szTakeItem)/sizeof(_TUSERITEMRCD); i++) 
		{
			lptHumanRcd->szTakeItem[i].btIsEmpty = true;
			pItemMakeIndex = pRec->Get(FLD_NAME[i]);
			if (memlen(pItemMakeIndex) < DEFGUIDLEN) continue;
			
			sprintf(szQuery, "SELECT * FROM TBL_CHARACTER_ITEM WHERE  FLD_MAKEINDEX='%s' AND FLD_ISDELETED=0", pItemMakeIndex);
			if (pRec2->Execute(szQuery)&&pRec2->Fetch()) {
				//lptHumanRcd->szTakeItem[i] = new _TUSERITEMRCD;
				lptHumanRcd->szTakeItem[i].btIsEmpty = false;
				lptHumanRcd->szTakeItem[i].szMakeIndex[0] = *(pRec2->Get("FLD_STDTYPE"));
				memcpy(&lptHumanRcd->szTakeItem[i].szMakeIndex[1], pRec2->Get("FLD_MAKEDATE"), 6);
				memcpy(&lptHumanRcd->szTakeItem[i].szMakeIndex[7], pRec2->Get("FLD_MAKEINDEX"), 36);

				lptHumanRcd->szTakeItem[i].nStdIndex = atoi(pRec2->Get("FLD_STDINDEX"));
				lptHumanRcd->szTakeItem[i].nDura = atoi(pRec2->Get("FLD_DURA"));
				lptHumanRcd->szTakeItem[i].nDuraMax = atoi(pRec2->Get("FLD_DURAMAX"));
				lptHumanRcd->szTakeItem[i].usCount = atoi(pRec2->Get("FLD_Count"));
				lptHumanRcd->szTakeItem[i].btValue[0] = atoi(pRec2->Get("FLD_AC"));
				lptHumanRcd->szTakeItem[i].btValue[1] = atoi(pRec2->Get("FLD_MAC"));
				lptHumanRcd->szTakeItem[i].btValue[2] = atoi(pRec2->Get("FLD_DC"));
				lptHumanRcd->szTakeItem[i].btValue[3] = atoi(pRec2->Get("FLD_MC"));
				lptHumanRcd->szTakeItem[i].btValue[4] = atoi(pRec2->Get("FLD_SC"));
				lptHumanRcd->szTakeItem[i].btValue[5] = atoi(pRec2->Get("FLD_Accuracy"));
				lptHumanRcd->szTakeItem[i].btValue[6] = atoi(pRec2->Get("FLD_Agility"));
				lptHumanRcd->szTakeItem[i].btValue[7] = atoi(pRec2->Get("FLD_HP"));
				lptHumanRcd->szTakeItem[i].btValue[8] = atoi(pRec2->Get("FLD_MP"));
				lptHumanRcd->szTakeItem[i].btValue[9] = atoi(pRec2->Get("FLD_Strong"));
				lptHumanRcd->szTakeItem[i].btValue[10] = atoi(pRec2->Get("FLD_MagicResist"));
				lptHumanRcd->szTakeItem[i].btValue[11] = atoi(pRec2->Get("FLD_PoisonResist"));
				lptHumanRcd->szTakeItem[i].btValue[12] = atoi(pRec2->Get("FLD_HealthRecovery"));

				lptHumanRcd->szTakeItem[i].btValue[13] = atoi(pRec2->Get("FLD_ManaRecovery"));
				lptHumanRcd->szTakeItem[i].btValue[14] = atoi(pRec2->Get("FLD_PoisonRecovery"));
				lptHumanRcd->szTakeItem[i].btValue[15] = atoi(pRec2->Get("FLD_CriticalRate"));
				lptHumanRcd->szTakeItem[i].btValue[16] = atoi(pRec2->Get("FLD_CriticalDamage"));
				lptHumanRcd->szTakeItem[i].btValue[17] = atoi(pRec2->Get("FLD_Freezing"));
				lptHumanRcd->szTakeItem[i].btValue[18] = atoi(pRec2->Get("FLD_PoisonAttack"));
				lptHumanRcd->szTakeItem[i].btValue[19] = atoi(pRec2->Get("FLD_RefinedValue"));
				lptHumanRcd->szTakeItem[i].btValue[20] = atoi(pRec2->Get("FLD_RefineAdded"));
				/*FLD_DuraChanged|FLD_Identified|FLD_Cursed|FLD_WeddingRing*/
				lptHumanRcd->szTakeItem[i].btValue[21] = atoi(pRec2->Get("FLD_Switchs"));
				strcpy(lptHumanRcd->szTakeItem[i].szBoundGuid, pRec2->Get("FLD_SoulBoundGuid"));

				ZeroMemory(lptHumanRcd->szTakeItem[i].szPrefixName, sizeof(lptHumanRcd->szTakeItem[i].szPrefixName));
				strcpy(lptHumanRcd->szTakeItem[i].szPrefixName, pRec2->Get("FLD_PREFIXNAME"));

				lptHumanRcd->szTakeItem[i].sbtValue[0] = atoi(pRec2->Get("FLD_AttackSpeed"));
				lptHumanRcd->szTakeItem[i].sbtValue[1] = atoi(pRec2->Get("FLD_Luck"));
			}
		}
	}
	else
	{
		GetDBManager()->DestroyRecordset( pRec );
		GetDBManager()->DestroyRecordset(pRec2);
		return FALSE;
	}
	
	GetDBManager()->DestroyRecordset( pRec );
	GetDBManager()->DestroyRecordset(pRec2);
	return TRUE;
}

void GetLoadHumanRcd(CServerInfo* pServerInfo, _LPTLOADHUMAN lpLoadHuman, int nRecog)
{
	if (!pServerInfo) return;

	_TDEFAULTMESSAGE	DefMsg;

	char				szEncodeMsg1[24];

	_THUMANRCD					tHumanRcd;
	CWHList<_LPTUSERITEMRCD>	xUserItemRcdList;
	CWHList<_LPTHUMANMAGICRCD>	xUserMagicRcdList;
	CWHList<_LPTGENITEMRCD>		xUserGenItemRcdList;
	
	_THORSERCD					tHorseRcd;
	int							nHorse = 0;
	
	char						szEncodeMsg2[8096];
	memset(&tHumanRcd, 0, sizeof(_THUMANRCD));
	if (GetHumanRcd(lpLoadHuman->szCharName, &tHumanRcd, lpLoadHuman))		// Fetch Character Data
	{
		GetHumanItemRcd(lpLoadHuman->szCharGuid, &xUserItemRcdList);		// Fetch Item Data
		GetHumanMagicRcd(lpLoadHuman->szCharName, &xUserMagicRcdList);		// Fetch Magic Data
		GetHumanGenItemRcd(lpLoadHuman->szCharName, &xUserGenItemRcdList);	// Fetch General Item Data
		
		int nCount		= xUserItemRcdList.GetCount();
		int nMagicCount = xUserMagicRcdList.GetCount();
		int nItemCount	= xUserGenItemRcdList.GetCount();
		
		if (nCount)
			nHorse = GetHorseRcd(lpLoadHuman->szCharName, &tHorseRcd);	// Fetch Horse Data


		
		int nPos = fnEncode6BitBufA((unsigned char *)&tHumanRcd, szEncodeMsg2, sizeof(_THUMANRCD), sizeof(szEncodeMsg2));
		
		if (nItemCount)
		{
			PLISTNODE pListNode = xUserGenItemRcdList.GetHead();
			
			while (pListNode)
			{
				_LPTGENITEMRCD	lptItemRcd = xUserGenItemRcdList.GetData(pListNode);
				
				if (lptItemRcd)
				{
					nPos += fnEncode6BitBufA((unsigned char *)lptItemRcd, &szEncodeMsg2[nPos], sizeof(_TGENITEMRCD), sizeof(szEncodeMsg2) - nPos);
					
					pListNode = xUserGenItemRcdList.RemoveNode(pListNode);
					delete lptItemRcd;
				}
				else 
					pListNode = xUserGenItemRcdList.GetNext(pListNode);
			}
		}
		
		if (nMagicCount)
		{
			PLISTNODE pListNode = xUserMagicRcdList.GetHead();
			
			while (pListNode)
			{
				_LPTHUMANMAGICRCD	lptUserMagicRcd = xUserMagicRcdList.GetData(pListNode);
				
				if (lptUserMagicRcd)
				{
					nPos += fnEncode6BitBufA((unsigned char *)lptUserMagicRcd, &szEncodeMsg2[nPos], sizeof(_THUMANMAGICRCD), sizeof(szEncodeMsg2) - nPos);
					
					pListNode = xUserMagicRcdList.RemoveNode(pListNode);
					delete lptUserMagicRcd;
				}
				else 
					pListNode = xUserMagicRcdList.GetNext(pListNode);
			}
		}
		
		if (nCount)
		{
			PLISTNODE pListNode = xUserItemRcdList.GetHead();
			
			while (pListNode)
			{
				_LPTUSERITEMRCD	lptUserItemRcd = xUserItemRcdList.GetData(pListNode);
				
				if (lptUserItemRcd)
				{
					nPos += fnEncode6BitBufA((unsigned char *)lptUserItemRcd, &szEncodeMsg2[nPos], sizeof(_TUSERITEMRCD), sizeof(szEncodeMsg2) - nPos);
					
					pListNode = xUserItemRcdList.RemoveNode(pListNode);
					delete lptUserItemRcd;
				}
				else 
					pListNode = xUserItemRcdList.GetNext(pListNode);
			}
			
			if (nHorse)
				nPos += fnEncode6BitBufA((unsigned char *)&tHorseRcd, &szEncodeMsg2[nPos], sizeof(_THORSERCD), sizeof(szEncodeMsg2) - nPos);
		}
		
		szEncodeMsg2[nPos] = '\0';

		if (nRecog)
			fnMakeDefMessageA(&DefMsg, DBR_LOADHUMANRCD,DEFBLOCKSIZE+ nPos,nRecog, MAKEWORD(nCount, nItemCount), nHorse, nMagicCount);
		else
			fnMakeDefMessageA(&DefMsg, DBR_LOADHUMANRCD2, DEFBLOCKSIZE + nPos, nRecog, MAKEWORD(nCount, nItemCount), nHorse, nMagicCount);

		fnEncodeMessageA(&DefMsg, szEncodeMsg1, sizeof(szEncodeMsg1));
		
		SendSocket(pServerInfo->m_sock, lpLoadHuman->nCertification, szEncodeMsg1, szEncodeMsg2, nPos);
	}
	else
	{
		fnMakeDefMessageA(&DefMsg, DBR_FAIL,DEFBLOCKSIZE, nRecog, 0, 0, 0);
		fnEncodeMessageA(&DefMsg, szEncodeMsg1, sizeof(szEncodeMsg1));
		
		SendSocket(pServerInfo->m_sock, lpLoadHuman->nCertification, szEncodeMsg1, "Test", 4);
	}
}

char *SaveHumanMagicRcd(char *pszUserID, char *pszCharName, char *pszEncodeRcd, int nCount)
{
	char szTmp[1024];

	// Delete Magic Data
	CRecordset *pRec = GetDBManager()->CreateRecordset();
	sprintf(szTmp, "DELETE FROM TBL_CHARACTER_MAGIC WHERE FLD_CHARNAME = '%s'", pszCharName);
	pRec->Execute(szTmp);
	GetDBManager()->DestroyRecordset( pRec );

	// Update Magic Data
	char *pszEncode = pszEncodeRcd;
	_THUMANMAGICRCD	tUserMagicRcd;

	for (int i = 0; i < nCount; i++)
	{
		if ( memlen( pszEncode ) >= MAGICRCDBLOCKSIZE )
		{
			pRec = GetDBManager()->CreateRecordset();

			fnDecode6BitBufA( pszEncode, (char *) &tUserMagicRcd, sizeof( _THUMANMAGICRCD ) );
			//done 2012/6/29
			sprintf(szTmp, "INSERT TBL_CHARACTER_MAGIC(FLD_LOGINID, FLD_CHARNAME, FLD_MAGICID, FLD_LEVEL, FLD_USEKEY, FLD_CURRTRAIN) VALUES "
							"( '%s', '%s', %d, %d, %d, %d )",
							pszUserID, pszCharName, tUserMagicRcd.btMagicID, tUserMagicRcd.btLevel, tUserMagicRcd.btUseKey, tUserMagicRcd.nCurrTrain);
			
			if ( !pRec->Execute( szTmp ) || pRec->GetRowCount() <= 0 )
				InsertLogMsg(_T("SaveHumanMagicRcd °»½Å ¿À·ù"));
			
			GetDBManager()->DestroyRecordset( pRec );

			pszEncode += MAGICRCDBLOCKSIZE + 1;
		}
	}
		
	return pszEncode;
}

void SaveGenItemRcd(char *pszUserID, char *pszCharName, char *pszEncodeRcd, int nCount)
{
	char szTmp[1024];

	// Delete Magic Data
	CRecordset *pRec = GetDBManager()->CreateRecordset();
	sprintf(szTmp, "DELETE FROM TBL_CHARACTER_GENITEM WHERE FLD_CHARNAME = '%s'", pszCharName);
	pRec->Execute(szTmp);
	GetDBManager()->DestroyRecordset( pRec );

	// Update General Item Data
	sprintf( szTmp, "FLD_CHARNAME='%s'", pszCharName );

	CQueryManager query;

	char *pszEncode = pszEncodeRcd;
	_TGENITEMRCD tItemRcd;

	for (int i = 0; i < nCount; i++)
	{
		while ( memlen( pszEncode ) >= GENITEMRCDBLOCKSIZE )
		{
			pRec = GetDBManager()->CreateRecordset();
		
			ZeroMemory(&tItemRcd, sizeof(_TGENITEMRCD));

			fnDecode6BitBufA( pszEncode, (char *) &tItemRcd, sizeof( _TGENITEMRCD ) );

			sprintf(szTmp, "INSERT TBL_CHARACTER_GENITEM (FLD_LOGINID, FLD_CHARNAME, FLD_ITEMINDEX) VALUES "//DONE 2012 /6/29
							"( '%s', '%s', '%s' )", pszUserID, pszCharName, tItemRcd.szItem);

			if ( !pRec->Execute( szTmp ) || pRec->GetRowCount() <= 0 )
				InsertLogMsg(_T("SaveGenItemRcd Ê§°Ü."));

			GetDBManager()->DestroyRecordset( pRec );

			pszEncode += (GENITEMRCDBLOCKSIZE + 1);
		}
	}
}

BOOL SaveHumanRcd(CServerInfo* pServerInfo, _LPTLOADHUMAN lpLoadHuman, _LPTHUMANRCD lptHumanRcd, int nRecog)
{
	char szSQL[1024];
	char szEquipC[10][10] = { {"'%s'"},{"'%s'"} ,{"'%s'"}, {"'%s'"}, {" '%s'"}, {"'%s' "},{"'%s'"}, {" '%s'"} ,{" '%s'"}, {" '%s'"} };

	char szEquip[10][66];

	for (int i = 0; i < 10; i++) {
		/*0:STDType,1-6:MakeDate*/
		if (memlen(lptHumanRcd->szTakeItem[i].szMakeIndex) > 7)
			sprintf(szEquip[i], szEquipC[i], &lptHumanRcd->szTakeItem[i].szMakeIndex[7]);
		else
			memcpy(szEquip[i], "NULL", sizeof("NULL"));
	}
	
	sprintf(szSQL, "UPDATE TBL_CHARACTER SET FLD_JOB=%d, FLD_GENDER=%d, FLD_LEVEL=%d, FLD_DIRECTION=%d, FLD_CX=%d, FLD_CY=%d, "
						"FLD_MAPNAME='%s', FLD_GOLD=%d, FLD_HAIR=%d, FLD_DRESS_ID=%s, FLD_WEAPON_ID=%s, "
						"FLD_LEFTHAND_ID=%s, FLD_RIGHTHAND_ID=%s, FLD_HELMET_ID=%s, FLD_NECKLACE_ID=%s, "
						"FLD_ARMRINGL_ID=%s, FLD_ARMRINGR_ID=%s, FLD_RINGL_ID=%s, FLD_RINGR_ID=%s, FLD_EXP=%d,FLD_ATTACKMODE=%d "
						"WHERE FLD_GUID='%s'",
						lptHumanRcd->btJob, lptHumanRcd->btGender, lptHumanRcd->szLevel, lptHumanRcd->nDirection,
						lptHumanRcd->nCX, lptHumanRcd->nCY, lptHumanRcd->szMapName, lptHumanRcd->dwGold,
						lptHumanRcd->szHair, szEquip[0], szEquip[1], szEquip[2], szEquip[3], szEquip[4],
						szEquip[5], szEquip[6], szEquip[7], szEquip[8], szEquip[9], lptHumanRcd->nExp, lptHumanRcd->btAttackMode,
						lptHumanRcd->szCharGuid);

	CRecordset *pRec = GetDBManager()->CreateRecordset();

	if (!pRec->Execute( szSQL ) || pRec->GetRowCount() <= 0 )
	{
		InsertLogMsg(_T("SaveHumanRcd Ê§°Ü."));
		GetDBManager()->DestroyRecordset( pRec );
		return FALSE;		
	}
	
	GetDBManager()->DestroyRecordset( pRec );

	return TRUE;
}

BOOL MakeNewItem(CServerInfo* pServerInfo, _LPTLOADHUMAN lpHumanLoad, _LPTMAKEITEMRCD lpMakeItemRcd, int nRecog)
{
	static char  g_szYesterDay[24];
	//static UINT  g_nItemIndexCnt = 0;

	CRecordset *pRec;

	char szQuery[1024];
	char szDate[24];
	char szMakeIndex[43];
	GetDate( szDate );
//	if (strcmp(szDate, g_szYesterDay) != 0)
//	{
	/* use Guid 2017.06.23
		sprintf( szQuery, 
			"SELECT FLD_MAKEINDEX FROM TBL_CHARACTER_ITEM WHERE FLD_STDTYPE = '%c' AND "
			"FLD_MAKEDATE = '%s' ORDER BY FLD_MAKEINDEX DESC",
			lpMakeItemRcd->szStdType, szDate );
		
		pRec = GetDBManager()->CreateRecordset();
		if ( pRec->Execute( szQuery ) && pRec->Fetch() )
		{
			g_nItemIndexCnt = atoi( pRec->Get( "FLD_MAKEINDEX" ) ) + 1;
		}
		GetDBManager()->DestroyRecordset( pRec );
	*/
	GetGuidSZ(szMakeIndex);
	strcpy(g_szYesterDay, szDate);
//	}
//	else
//		g_nItemIndexCnt++;

	char szUserID[32];
	char szCharName[32];
	byte btCharIndex;
	char szGuid[37];
	if (lpHumanLoad)
	{
		strcpy(szUserID, lpHumanLoad->szUserID);
		strcpy(szCharName, lpHumanLoad->szCharName);
		btCharIndex = lpHumanLoad->btCharIndex;
		strcpy(szGuid,lpHumanLoad->szCharGuid);
		//wcscpy(szCharName, lpHumanLoad->szCharName);
	}
	else
	{
		strcpy(szUserID, "0");
		strcpy(szCharName, "WEMADE");
		btCharIndex = 0;
		memset(szGuid, 0, sizeof(szGuid));
	}
	//DOWN 2012/6/29
	/*
	lptHumanRcd->szTakeItem[i].btValue[0] = atoi(pRec->Get("FLD_AC"));
	lptHumanRcd->szTakeItem[i].btValue[1] = atoi(pRec->Get("FLD_MAC"));
	lptHumanRcd->szTakeItem[i].btValue[2] = atoi(pRec->Get("FLD_DC"));
	lptHumanRcd->szTakeItem[i].btValue[3] = atoi(pRec->Get("FLD_MC"));
	lptHumanRcd->szTakeItem[i].btValue[4] = atoi(pRec->Get("FLD_SC"));
	lptHumanRcd->szTakeItem[i].btValue[5] = atoi(pRec->Get("FLD_Accuracy"));
	lptHumanRcd->szTakeItem[i].btValue[6] = atoi(pRec->Get("FLD_Agility"));
	lptHumanRcd->szTakeItem[i].btValue[7] = atoi(pRec->Get("FLD_HP"));
	lptHumanRcd->szTakeItem[i].btValue[8] = atoi(pRec->Get("FLD_MP"));
	lptHumanRcd->szTakeItem[i].btValue[9] = atoi(pRec->Get("FLD_Strong"));
	lptHumanRcd->szTakeItem[i].btValue[10] = atoi(pRec->Get("FLD_MagicResist"));
	lptHumanRcd->szTakeItem[i].btValue[11] = atoi(pRec->Get("FLD_PoisonResist"));
	lptHumanRcd->szTakeItem[i].btValue[12] = atoi(pRec->Get("FLD_HealthRecovery"));

	lptHumanRcd->szTakeItem[i].btValue[13] = atoi(pRec->Get("FLD_ManaRecovery"));
	lptHumanRcd->szTakeItem[i].btValue[14] = atoi(pRec->Get("FLD_PoisonRecovery"));
	lptHumanRcd->szTakeItem[i].btValue[15] = atoi(pRec->Get("FLD_CriticalRate"));
	lptHumanRcd->szTakeItem[i].btValue[16] = atoi(pRec->Get("FLD_CriticalDamage"));
	lptHumanRcd->szTakeItem[i].btValue[17] = atoi(pRec->Get("FLD_Freezing"));
	lptHumanRcd->szTakeItem[i].btValue[18] = atoi(pRec->Get("FLD_PoisonAttack"));
	lptHumanRcd->szTakeItem[i].btValue[19] = atoi(pRec->Get("FLD_RefinedValue"));
	lptHumanRcd->szTakeItem[i].btValue[20] = atoi(pRec->Get("FLD_RefineAdded"));
	/*FLD_DuraChanged|FLD_Identified|FLD_Cursed|FLD_WeddingRing
	lptHumanRcd->szTakeItem[i].btValue[21] = atoi(pRec->Get("FLD_Switchs"));
	*/
	sprintf(szQuery, "INSERT TBL_CHARACTER_ITEM (FLD_LOGINID, FLD_CHARINDEX, FLD_STDTYPE, FLD_MAKEDATE, FLD_MAKEINDEX, "
						"FLD_STDINDEX, FLD_DURA, FLD_DURAMAX,FLD_COUNT, FLD_AC, FLD_MAC, FLD_DC, FLD_MC, FLD_SC, "//
						"FLD_Accuracy, FLD_Agility, FLD_HP, FLD_MP, FLD_Strong, FLD_MagicResist, FLD_PoisonResist, FLD_HealthRecovery, "
						"FLD_ManaRecovery, FLD_PoisonRecovery, FLD_CriticalRate,FLD_CriticalDamage,FLD_Freezing,"
						"FLD_PoisonAttack,FLD_RefinedValue,FLD_RefineAdded,FLD_SWITCHS,FLD_PREFIXNAME,FLD_LASTOWNER,FLD_LASTACTION,FLD_OWNER) "
						"VALUES( '%s', '%d', '%c', '%s', '%s', %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, '%d', %d," 
						"'%d' ,'%d' ,'%d' ,'%d' ,'%d' ,'%d' ,'%d' ,'%s','%s' ,'%d','%s')", 
						szUserID, btCharIndex, lpMakeItemRcd->szStdType, g_szYesterDay, szMakeIndex,
						lpMakeItemRcd->nStdIndex, lpMakeItemRcd->nDura,	lpMakeItemRcd->nDuraMax, 1,
						lpMakeItemRcd->btValue[0], lpMakeItemRcd->btValue[1], lpMakeItemRcd->btValue[2], lpMakeItemRcd->btValue[3], 
						lpMakeItemRcd->btValue[5], lpMakeItemRcd->btValue[5], lpMakeItemRcd->btValue[6], lpMakeItemRcd->btValue[7], 
						lpMakeItemRcd->btValue[8], lpMakeItemRcd->btValue[9], lpMakeItemRcd->btValue[10], lpMakeItemRcd->btValue[11], 
						lpMakeItemRcd->btValue[12], lpMakeItemRcd->btValue[13], lpMakeItemRcd->btValue[14], lpMakeItemRcd->btValue[15],
						lpMakeItemRcd->btValue[16], lpMakeItemRcd->btValue[17], lpMakeItemRcd->btValue[18], lpMakeItemRcd->btValue[19], 
						lpMakeItemRcd->btValue[20], lpMakeItemRcd->btValue[21],"", szGuid, 1,szGuid);
	
	pRec = GetDBManager()->CreateRecordset();
	if ( !pRec->Execute( szQuery ) || pRec->GetRowCount() <= 0 )
	{
		GetDBManager()->DestroyRecordset( pRec );
		return FALSE;
	}
	GetDBManager()->DestroyRecordset( pRec );
	
	if ( pServerInfo )
	{
		// Send Packet to Game Server
		_TDEFAULTMESSAGE	DefMsg;
		_TUSERITEMRCD		UserItemRcd;
		
		char				szEncodeMsg1[24];
		char				szEncodeMsg2[128];
		int					nPos = 0;
		

		
		UserItemRcd.szMakeIndex[0] = lpMakeItemRcd->szStdType;
		
		memcpy( &UserItemRcd.szMakeIndex[1], g_szYesterDay, 6 );
		sprintf( &UserItemRcd.szMakeIndex[7], "%s", szMakeIndex );
		
		memcpy(&UserItemRcd.nStdIndex, &lpMakeItemRcd->nStdIndex, sizeof(_TUSERITEMRCD) - sizeof(UserItemRcd.szMakeIndex));
		
		nPos = fnEncode6BitBufA((unsigned char *)&UserItemRcd, szEncodeMsg2, sizeof(_TUSERITEMRCD), sizeof(szEncodeMsg2));

		if (lpHumanLoad)
			fnMakeDefMessageA(&DefMsg, DBR_MAKEITEMRCD, DEFBLOCKSIZE+nPos,nRecog, 0, 0, 0);
		else
			fnMakeDefMessageA(&DefMsg, DBR_MAKEITEMRCD2, DEFBLOCKSIZE + nPos,nRecog, 0, 0, 0);

		fnEncodeMessageA(&DefMsg, szEncodeMsg1, sizeof(szEncodeMsg1));

		SendSocket(pServerInfo->m_sock, 2, szEncodeMsg1, szEncodeMsg2, nPos);		
	}

	return TRUE;
}

UINT WINAPI ProcessDBMsg(LPVOID lpParameter)
{
	while (TRUE)
	{
		if (g_fTerminated) return 0L;

		_LPTSENDBUFF pSendBuff = (_LPTSENDBUFF)g_DBMsgQ.PopQ();

		if (pSendBuff)
		{
			switch (pSendBuff->DefMsg.wIdent)
			{
				case DB_LOADHUMANRCD:
					GetLoadHumanRcd(pSendBuff->pServerInfo, &pSendBuff->HumanLoad, pSendBuff->DefMsg.nRecog);
					break;
				case DB_SAVEHUMANRCD:
				{
					SaveHumanRcd(pSendBuff->pServerInfo, &pSendBuff->HumanLoad, (_LPTHUMANRCD)pSendBuff->lpbtAddData, pSendBuff->DefMsg.nRecog);
					char *pszData = SaveHumanMagicRcd(pSendBuff->HumanLoad.szUserID, pSendBuff->HumanLoad.szCharName, (char *)pSendBuff->lpbtAddData2, pSendBuff->DefMsg.wParam);
					SaveGenItemRcd(pSendBuff->HumanLoad.szUserID, pSendBuff->HumanLoad.szCharName, pszData, pSendBuff->DefMsg.wTag);
					break;
				}
				case DB_MAKEITEMRCD:
					MakeNewItem(pSendBuff->pServerInfo, &pSendBuff->HumanLoad, (_LPTMAKEITEMRCD)pSendBuff->lpbtAddData, pSendBuff->DefMsg.nRecog);
					break;
				case DB_MAKEITEMRCD2:
				{
					_LPTUSERITEMRCD lptUserItemRcd = (_LPTUSERITEMRCD)pSendBuff->lpbtAddData;
					_TMAKEITEMRCD	tMakeItemRcd;

					tMakeItemRcd.szStdType	= lptUserItemRcd->szMakeIndex[0];
					tMakeItemRcd.nStdIndex	= lptUserItemRcd->nStdIndex;
					tMakeItemRcd.nDura		= lptUserItemRcd->nDura;
					tMakeItemRcd.nDuraMax	= lptUserItemRcd->nDuraMax;
					memcpy(tMakeItemRcd.btValue, lptUserItemRcd->btValue, sizeof(lptUserItemRcd->btValue));

					MakeNewItem(pSendBuff->pServerInfo, NULL, &tMakeItemRcd, pSendBuff->DefMsg.nRecog);
					break;
				}
			} // switch

			delete pSendBuff;
		} // if (pSendBuff)

		SleepEx(1, TRUE);
	}

	return 0L;
}
