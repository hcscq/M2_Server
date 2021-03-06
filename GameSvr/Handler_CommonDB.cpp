#include "stdafx.h"

void SendRDBSocket(int nCertification, char *pszData1, char *pszData2, int nData2Len);

CMapInfo* InitMapInfo(int nServerIndex)
{
	char szQuery[1024];

	CRecordset *pRec = g_pConnCommon->CreateRecordset();
	sprintf( szQuery, "SELECT COUNT(*) AS FLD_COUNT FROM TBL_MAPINFO WHERE FLD_SERVERINDEX = %d", nServerIndex );

	if (pRec->Execute( szQuery ))
	{
			if (pRec->Fetch())
				g_nNumOfMapInfo = atoi( pRec->Get( "FLD_COUNT" ) );
	}

	g_pConnCommon->DestroyRecordset( pRec );

	if (g_nNumOfMapInfo <= 0) return NULL;

	CMapInfo* pMapInfo = new CMapInfo[g_nNumOfMapInfo];

	sprintf( szQuery, "SELECT * FROM TBL_MAPINFO WHERE FLD_SERVERINDEX = %d", nServerIndex );

	pRec = g_pConnCommon->CreateRecordset();
	
	if (pRec->Execute( szQuery ))
	{
		for (int i = 0; i < g_nNumOfMapInfo; i++)
		{
			if (pRec->Fetch())
			{
				strcpy(pMapInfo[i].szMapFileName, pRec->Get( "FLD_MAPFILENAME" ) );
				ChangeSpaceToNull(pMapInfo[i].szMapFileName);
				strcpy(pMapInfo[i].szMapName, pRec->Get( "FLD_MAPNAME" ) );
				ChangeSpaceToNull(pMapInfo[i].szMapName);

				pMapInfo[i].dwAttribute			= atoi( pRec->Get( "FLD_ATTRIBUTE" ) );

				pMapInfo[i].btMapSeries			= atoi( pRec->Get( "FLD_MAPSERIES" ) );
				pMapInfo[i].btMapSeriesValue	= atoi( pRec->Get( "FLD_MAPSERIESVAL" ) );

				pMapInfo[i].mMiniMap			= atoi(pRec->Get("FLD_MINIMAP"));
			}
		}
	}

	g_pConnCommon->DestroyRecordset( pRec );

	return pMapInfo;
}

void InitStdItemEtcInfo()
{
	char szQuery[1024];

	CRecordset *pRec;
	
	pRec= g_pConnCommon->CreateRecordset();

	//sprintf( szQuery, "SELECT COUNT(*) AS FLD_COUNT FROM TBL_STDITEM_ETC");
	sprintf(szQuery, "SELECT COUNT(*) AS FLD_COUNT FROM TBL_STDITEM");

	if (pRec->Execute( szQuery ))
	{
			if (pRec->Fetch())
				g_nStdItemEtc = atoi( pRec->Get( "FLD_COUNT" ) );
	}

	g_pConnCommon->DestroyRecordset( pRec );

	if (g_nStdItemEtc < 0) return;

	g_pStdItemEtc = new CStdItem[g_nStdItemEtc];

	//sprintf( szQuery, "SELECT * FROM TBL_STDITEM_ETC ORDER BY FLD_INDEX");
	sprintf(szQuery, "SELECT FLD_TYPE,FLD_NAME,FLD_STDMODE,FLD_SHAPE,FLD_WEIGHT,FLD_LOOKS,FLD_DURAMAX,FLD_SOURCE,FLD_PRICE FROM TBL_STDITEM ORDER BY FLD_INDEX");

	pRec = g_pConnCommon->CreateRecordset();
	
	if (pRec->Execute( szQuery ))
	{
		for (int i = 0; i < g_nStdItemEtc; i++)
		{
			if (pRec->Fetch())								 
			{
				strcpy_s(g_pStdItemEtc[i].szName, pRec->Get( "FLD_NAME" ) );

				g_pStdItemEtc[i].wStdMode	= atoi( pRec->Get( "FLD_STDMODE" ) );
				g_pStdItemEtc[i].wShape		= atoi( pRec->Get( "FLD_SHAPE" ) );
				g_pStdItemEtc[i].wWeight	= atoi( pRec->Get( "FLD_WEIGHT" ) );
				g_pStdItemEtc[i].dwLooks	= atoi( pRec->Get( "FLD_LOOKS" ) );
				g_pStdItemEtc[i].wDuraMax	= atoi( pRec->Get( "FLD_DURAMAX" ) );//FLD_VAL1
				g_pStdItemEtc[i].wRSource	= atoi( pRec->Get( "FLD_SOURCE" ) );//FLD_VAL2 FLD_DURAMAX,FLD_SOURCE
				g_pStdItemEtc[i].dwPrice	= atoi( pRec->Get( "FLD_PRICE" ) );
				g_pStdItemEtc[i].btType		= (BYTE)atoi(pRec->Get("FLD_TYPE"));
			}
		}

		InsertLogMsgParam(IDS_FETCH_STDITEMETC, &g_nStdItemEtc, LOGPARAM_INT);
	}

	g_pConnCommon->DestroyRecordset( pRec );
}

void InitStdItemSpecial()
{
	char szQuery[1024];

	CRecordset *pRec = g_pConnCommon->CreateRecordset();
	sprintf( szQuery, "SELECT COUNT(*) AS FLD_COUNT FROM TBL_STDITEM");
	//TEST


	//GUID guid;
	//char szQuery2[102];
	////CoCreateGuid(&guid);
	//UINT64 K= _atoi64("A50B8BC0-F0BC-4411-8E9C-3B784000F46E");
	//memcpy(&guid, &K, sizeof(guid));
	//memcpy(&K, &guid, sizeof(guid));//"PRIu64"
	//
	//sprintf(szQuery, "INSERT INTO TBL_GUID ([Guid]) VALUES(0x%x)", &guid);
	//int reC = pRec->Execute(szQuery);
	//
	//GUID guid2;
	//char szQuery1[60];
	//memcpy(&guid2, &K, sizeof(guid));
	//long s = 0;//2147483647
	//s = strtol(("0xF3F0F000000000000000000000000000"), NULL, 16);
	//sprintf(szQuery, "SELECT *  FROM TBL_GUID");
	//
	//reC = pRec->Execute(szQuery);
	//if (pRec->Fetch())
	//	s = strtol((pRec->Get("[Guid]")), NULL, 16);
	
		//memcpy(&guid2, strtol((pRec->Get("[Guid]")), NULL, 16),sizeof(guid2));
	//TEST END

	if (pRec->Execute( szQuery ))
	{
			if (pRec->Fetch())
				g_nStdItemSpecial = atoi( pRec->Get( "FLD_COUNT" ) );
	}

	g_pConnCommon->DestroyRecordset( pRec );

	if (g_nStdItemSpecial < 0) return;

	g_pStdItemSpecial = new CStdItemSpecial[g_nStdItemSpecial];

	sprintf( szQuery, "SELECT * FROM TBL_STDITEM ORDER BY FLD_INDEX");

	pRec = g_pConnCommon->CreateRecordset();
	
	if (pRec->Execute( szQuery ))
	{
		for (int i = 0; i < g_nStdItemSpecial; i++)
		{
			if (pRec->Fetch())								 
			{
				strcpy(g_pStdItemSpecial[i].szName, pRec->Get( "FLD_NAME" ) );

				g_pStdItemSpecial[i].wStdMode		= (SHORT)atoi( pRec->Get( "FLD_STDMODE" ) );
				g_pStdItemSpecial[i].btType			= ( pRec->Get( "FLD_TYPE" )[0] );
				g_pStdItemSpecial[i].Index			= (LONG)atoi(pRec->Get("FLD_Index"));
				g_pStdItemSpecial[i].wShape			= (SHORT)atoi( pRec->Get( "FLD_SHAPE" ) );
				g_pStdItemSpecial[i].wWeight		= (SHORT)atoi( pRec->Get( "FLD_WEIGHT" ) );
				g_pStdItemSpecial[i].btAniCount		= (SHORT)atoi( pRec->Get( "FLD_ANICOUNT" ) );
				g_pStdItemSpecial[i].wRSource		= (SHORT)atoi( pRec->Get( "FLD_SOURCE" ) );
				g_pStdItemSpecial[i].dwLooks		= (LONG)atoi( pRec->Get( "FLD_LOOKS" ) );
				g_pStdItemSpecial[i].wDuraMax		= (WORD)atoi( pRec->Get( "FLD_DURAMAX" ) );
				g_pStdItemSpecial[i].wAC			= (BYTE)atoi( pRec->Get( "FLD_AC" ) );
				g_pStdItemSpecial[i].wAC2			= (BYTE)atoi( pRec->Get( "FLD_AC2" ) );
				g_pStdItemSpecial[i].wMAC			= (BYTE)atoi( pRec->Get( "FLD_MAC" ) );
				g_pStdItemSpecial[i].wMAC2			= (BYTE)atoi( pRec->Get( "FLD_MAC2" ) );
				g_pStdItemSpecial[i].wDC			= (BYTE)atoi( pRec->Get( "FLD_DC" ) );
				g_pStdItemSpecial[i].wDC2			= (BYTE)atoi( pRec->Get( "FLD_DC2" ) );
				g_pStdItemSpecial[i].wMC			= (BYTE)atoi( pRec->Get( "FLD_MC" ) );
				g_pStdItemSpecial[i].wMC2			= (BYTE)atoi( pRec->Get( "FLD_MC2" ) );
				g_pStdItemSpecial[i].wSC			= (BYTE)atoi( pRec->Get( "FLD_SC" ) );
				g_pStdItemSpecial[i].wSC2			= (BYTE)atoi( pRec->Get( "FLD_SC2" ) );
				g_pStdItemSpecial[i].wNeed			= (SHORT)atoi( pRec->Get( "FLD_NEED" ) );
				g_pStdItemSpecial[i].wNeedLevel		= (SHORT)atoi( pRec->Get( "FLD_NEEDLEVEL" ) );
				g_pStdItemSpecial[i].dwPrice		= (LONG)atoi( pRec->Get( "FLD_PRICE" ) );
				g_pStdItemSpecial[i].dwStock		= (LONG)atoi( pRec->Get( "FLD_STOCK" ) );

				g_pStdItemSpecial[i].HP				= (LONG)atoi(pRec->Get("FLD_HP"));
				g_pStdItemSpecial[i].MP				= (LONG)atoi(pRec->Get("FLD_MP"));
				g_pStdItemSpecial[i].AttackSpeed	= (BYTE)atoi(pRec->Get("FLD_ATTACKSPEED"));
				g_pStdItemSpecial[i].Luck			= (BYTE)atoi(pRec->Get("FLD_LUCK"));

				g_pStdItemSpecial[i].m_btWater		= (BYTE)atoi( pRec->Get( "FLD_WATER1" ) );
				g_pStdItemSpecial[i].m_btWater2		= (BYTE)atoi( pRec->Get( "FLD_WATER2" ) );
				g_pStdItemSpecial[i].m_btFire		= (BYTE)atoi( pRec->Get( "FLD_FIRE1" ) );
				g_pStdItemSpecial[i].m_btFire2		= (BYTE)atoi( pRec->Get( "FLD_FIRE2" ) );
				g_pStdItemSpecial[i].m_btWind		= (BYTE)atoi( pRec->Get( "FLD_WIND1" ) );
				g_pStdItemSpecial[i].m_btWind2		= (BYTE)atoi( pRec->Get( "FLD_WIND2" ) );
				g_pStdItemSpecial[i].m_btLight		= (BYTE)atoi( pRec->Get( "FLD_LIGHT1" ) );
				g_pStdItemSpecial[i].m_btLight2		= (BYTE)atoi( pRec->Get( "FLD_LIGHT2" ) );
				g_pStdItemSpecial[i].m_btEarth		= (BYTE)atoi( pRec->Get( "FLD_EARTH1" ) );
				g_pStdItemSpecial[i].m_btEarth2		= (BYTE)atoi( pRec->Get( "FLD_EARTH2" ) );//ALL  34
			}
		}

		InsertLogMsgParam(IDS_FETCH_STDITEMACCESSORY, &g_nStdItemSpecial, LOGPARAM_INT);
	}

	g_pConnCommon->DestroyRecordset( pRec );
}

void InitMonsterGenInfo()
{
	char szQuery[1024];

	CRecordset *pRec = g_pConnCommon->CreateRecordset();
	sprintf( szQuery, "SELECT COUNT(*) AS FLD_COUNT FROM TBL_MONGEN");

	if (pRec->Execute( szQuery ))
	{
			if (pRec->Fetch())
				g_nNumOfMonGenInfo = atoi( pRec->Get( "FLD_COUNT" ) );
	}

	g_pConnCommon->DestroyRecordset( pRec );

	if (g_nNumOfMonGenInfo < 0) return;

	g_pMonGenInfo = new CMonsterGenInfo[g_nNumOfMonGenInfo + 1];

	sprintf( szQuery, "SELECT * FROM TBL_MONGEN ORDER BY FLD_MAPNAME");

	pRec = g_pConnCommon->CreateRecordset();
	
	if (pRec->Execute( szQuery ))
	{
		for (int i = 0; i < g_nNumOfMonGenInfo; i++)
		{
			if (pRec->Fetch())								 
			{
				strcpy(g_pMonGenInfo[i].szMapName, pRec->Get( "FLD_MAPNAME" ) );
				strcpy(g_pMonGenInfo[i].szMonName, pRec->Get( "FLD_MONNAME" ) );
				
				g_pMonGenInfo[i].dwZenTime		= atoi( pRec->Get( "FLD_GENTIME" ) ) * 60000;
				g_pMonGenInfo[i].dwStartTime	= 0;

				g_pMonGenInfo[i].nMonIndex		= atoi( pRec->Get( "FLD_MONINDEX" ) );

				g_pMonGenInfo[i].nCount			= atoi( pRec->Get( "FLD_COUNT" ) );

				g_pMonGenInfo[i].nX				= atoi( pRec->Get( "FLD_X" ) );
				g_pMonGenInfo[i].nY				= atoi( pRec->Get( "FLD_Y" ) );

				g_pMonGenInfo[i].nAreaX			= atoi( pRec->Get( "FLD_AREA_X" ) );
				g_pMonGenInfo[i].nAreaY			= atoi( pRec->Get( "FLD_AREA_Y" ) );
			}
		}

		InsertLogMsgParam(IDS_FETCH_MONGEN, &g_nNumOfMonGenInfo, LOGPARAM_INT);
	}

	g_pConnCommon->DestroyRecordset( pRec );
}

// **************************************************************************************
//
//		Initializing Monster Item Generate Information
//
// **************************************************************************************

void InitMonItemInfo()
{
	int			nMonIndex;
	char		szQuery[1024];
	CRecordset *pRec = g_pConnCommon->CreateRecordset();

	sprintf( szQuery, "SELECT * FROM TBL_MONITEM");

	if (pRec->Execute( szQuery ))
	{
		while (pRec->Fetch())
		{
			nMonIndex = atoi( pRec->Get( "FLD_MONSTER" ));

			CMonItem* pMonItem = new CMonItem;

			if (pMonItem)
			{
				pMonItem->m_btItemType	= atoi( pRec->Get( "FLD_TYPE" ));
				pMonItem->m_wItemIndex	= atoi( pRec->Get( "FLD_ITEM" ));
				pMonItem->m_nPoint		= atoi( pRec->Get( "FLD_PROBABLY" ));
				pMonItem->m_wCount		= atoi( pRec->Get( "FLD_COUNT" ));
			
				if (g_pMonRaceInfo && (nMonIndex <= g_nNumOfMonRaceInfo))
					g_pMonRaceInfo[nMonIndex].m_xMonItemList.AddNewNode(pMonItem);
			}
		}	
	}

	g_pConnCommon->DestroyRecordset( pRec );
}

// **************************************************************************************
//
//		Initializing Monster Information
//
// **************************************************************************************

void InitMonRaceInfo()
{
	char szQuery[1024];

	CRecordset *pRec = g_pConnCommon->CreateRecordset();
	sprintf( szQuery, "SELECT COUNT(*) AS FLD_COUNT FROM TBL_MONSTER");

	if (pRec->Execute( szQuery ))
	{
			if (pRec->Fetch())
				g_nNumOfMonRaceInfo = atoi( pRec->Get( "FLD_COUNT" ) );
	}

	g_pConnCommon->DestroyRecordset( pRec );

	if (g_nNumOfMonRaceInfo < 0) return;

	g_pMonRaceInfo = new CMonRaceInfo[g_nNumOfMonRaceInfo];

	sprintf( szQuery, "SELECT * FROM TBL_MONSTER ORDER BY FLD_INDEX");

	pRec = g_pConnCommon->CreateRecordset();
	
	if (pRec->Execute( szQuery ))
	{
		for (int i = 0; i < g_nNumOfMonRaceInfo; i++)
		{
			if (pRec->Fetch())								 
			{
				g_pMonRaceInfo[i].nIndex	= i;

				strcpy(g_pMonRaceInfo[i].szMonName, pRec->Get( "FLD_NAME" ) );
				ChangeSpaceToNull(g_pMonRaceInfo[i].szMonName);

				g_pMonRaceInfo[i].nMonRace		= atoi( pRec->Get( "FLD_RACE" ) );
				g_pMonRaceInfo[i].nAppear		= atoi( pRec->Get( "FLD_APPR" ) );

				g_pMonRaceInfo[i].btUndead		= (BYTE)atoi( pRec->Get( "FLD_UNDEAD" ) );
				g_pMonRaceInfo[i].Level			= (BYTE)atoi( pRec->Get( "FLD_LEVEL" ) );

				g_pMonRaceInfo[i].HP			= atoi( pRec->Get( "FLD_HP" ) );
				g_pMonRaceInfo[i].MP			= atoi( pRec->Get( "FLD_MP" ) );

				g_pMonRaceInfo[i].AC			= atoi( pRec->Get( "FLD_AC" ) );
				g_pMonRaceInfo[i].MAC			= atoi( pRec->Get( "FLD_MAC" ) );
				g_pMonRaceInfo[i].DC			= MAKEWORD((BYTE)atoi( pRec->Get( "FLD_DC" ) ), (BYTE)atoi( pRec->Get( "FLD_MAXDC" ) ));

				g_pMonRaceInfo[i].dwAttackSpeed = atoi( pRec->Get( "FLD_ATTACKSPEED" ) );
				g_pMonRaceInfo[i].dwWalkSpeed	= atoi( pRec->Get( "FLD_WALKSPEED" ) );

				g_pMonRaceInfo[i].Speed			= atoi( pRec->Get( "FLD_SPEED" ) );
				g_pMonRaceInfo[i].Hit			= atoi( pRec->Get( "FLD_HIT" ) );

				g_pMonRaceInfo[i].ViewRange		= (BYTE)atoi( pRec->Get( "FLD_VIEWRANGE" ) );

				g_pMonRaceInfo[i].wRaceIndex	= atoi( pRec->Get( "FLD_RACEINDEX" ) );

				g_pMonRaceInfo[i].Exp			= atoi( pRec->Get( "FLD_EXP" ) );

				g_pMonRaceInfo[i].wEscape		= atoi( pRec->Get( "FLD_ESCAPE" ) );

				g_pMonRaceInfo[i].m_wWater		= MAKEWORD((BYTE)atoi( pRec->Get( "FLD_WATER1" ) ), (BYTE)atoi( pRec->Get( "FLD_WATER2" ) ));
				g_pMonRaceInfo[i].m_wFire		= MAKEWORD((BYTE)atoi( pRec->Get( "FLD_FIRE1" ) ), (BYTE)atoi( pRec->Get( "FLD_FIRE2" ) ));
				g_pMonRaceInfo[i].m_wWind		= MAKEWORD((BYTE)atoi( pRec->Get( "FLD_WIND1" ) ), (BYTE)atoi( pRec->Get( "FLD_WIND2" ) ));
				g_pMonRaceInfo[i].m_wLight		= MAKEWORD((BYTE)atoi( pRec->Get( "FLD_LIGHT1" ) ), (BYTE)atoi( pRec->Get( "FLD_LIGHT2" ) ));
				g_pMonRaceInfo[i].m_wEarth		= MAKEWORD((BYTE)atoi( pRec->Get( "FLD_EARTH1" ) ), (BYTE)atoi( pRec->Get( "FLD_EARTH2" ) ));
			}
		}

		InsertLogMsgParam(IDS_FETCH_MONSTER, &g_nNumOfMonRaceInfo, LOGPARAM_INT);
	}

	g_pConnCommon->DestroyRecordset( pRec );

	InitMonItemInfo();
}

void InitMerchantInfo()
{
	char szQuery[1024];

	CRecordset *pRec = g_pConnCommon->CreateRecordset();
	sprintf( szQuery, "SELECT COUNT(*) AS FLD_COUNT FROM TBL_MERCHANT");

	if (pRec->Execute( szQuery ))
	{
			if (pRec->Fetch())
				g_nNumOfMurchantInfo = atoi( pRec->Get( "FLD_COUNT" ) );
	}

	g_pConnCommon->DestroyRecordset( pRec );

	if (g_nNumOfMurchantInfo < 0) return;

	g_pMerchantInfo = new CMerchantInfo[g_nNumOfMurchantInfo];

	sprintf( szQuery, "SELECT * FROM TBL_MERCHANT ORDER BY FLD_ID");

	pRec = g_pConnCommon->CreateRecordset();
	
	if (pRec->Execute( szQuery ))
	{
		for (int i = 0; i < g_nNumOfMurchantInfo; i++)
		{
			if (pRec->Fetch())								 
			{
				strcpy(g_pMerchantInfo[i].szNPCName, pRec->Get( "FLD_NPCNAME" ) );
				ChangeSpaceToNull(g_pMerchantInfo[i].szNPCName);
				strcpy(g_pMerchantInfo[i].szMapName, pRec->Get( "FLD_MAPNAME" ) );
				ChangeSpaceToNull(g_pMerchantInfo[i].szMapName);

				g_pMerchantInfo[i].nPosX = atoi( pRec->Get( "FLD_POSX" ) );
				g_pMerchantInfo[i].nPosY = atoi( pRec->Get( "FLD_POSY" ) );
				g_pMerchantInfo[i].sFace = atoi( pRec->Get( "FLD_FACE" ) );
				g_pMerchantInfo[i].sBody = atoi( pRec->Get( "FLD_BODY" ) );
				g_pMerchantInfo[i].sGender = atoi( pRec->Get( "FLD_GENDER" ) );
			}
		}

		InsertLogMsgParam(IDS_FETCH_MERCHANT, &g_nNumOfMurchantInfo, LOGPARAM_INT);
	}

	g_pConnCommon->DestroyRecordset( pRec );
}

void InitMagicInfo()
{
	char szQuery[1024];

	CRecordset *pRec = g_pConnCommon->CreateRecordset();
	sprintf( szQuery, "SELECT COUNT(*) AS FLD_COUNT FROM TBL_MAGIC");

	if (pRec->Execute( szQuery ))
	{
			if (pRec->Fetch())
				g_nNumOfMagicInfo = atoi( pRec->Get( "FLD_COUNT" ) );
	}

	g_pConnCommon->DestroyRecordset( pRec );

	if (g_nNumOfMagicInfo < 0) return;

	g_pMagicInfo = new CMagicInfo[g_nNumOfMagicInfo];

	sprintf( szQuery, "SELECT * FROM TBL_MAGIC ORDER BY FLD_INDEX");

	pRec = g_pConnCommon->CreateRecordset();
	
	if (pRec->Execute( szQuery ))
	{
		for (int i = 0; i < g_nNumOfMagicInfo; i++)
		{
			if (pRec->Fetch())								 
			{
				g_pMagicInfo[i].nIndex			= atoi( pRec->Get( "FLD_INDEX" ) );
				
				strcpy(g_pMagicInfo[i].szName, pRec->Get( "FLD_NAME" ) );
				ChangeSpaceToNull(g_pMagicInfo[i].szName);

				g_pMagicInfo[i].sEffectType		= atoi( pRec->Get( "FLD_EFFECTTYPE" ) );
				g_pMagicInfo[i].sEffect			= atoi( pRec->Get( "FLD_EFFECT" ) );
				g_pMagicInfo[i].sSpell			= atoi( pRec->Get( "FLD_SPELL" ) );
				g_pMagicInfo[i].sPower			= atoi( pRec->Get( "FLD_POWER" ) );
				g_pMagicInfo[i].sMaxPower		= atoi( pRec->Get( "FLD_MAXPOWER" ) );
				g_pMagicInfo[i].sDefSpell		= atoi( pRec->Get( "FLD_DEFSPELL" ) );
				g_pMagicInfo[i].sDefPower		= atoi( pRec->Get( "FLD_DEFPOWER" ) );
				g_pMagicInfo[i].sDefMaxPower	= atoi( pRec->Get( "FLD_DEFMAXPOWER" ) );
				g_pMagicInfo[i].sJob			= atoi( pRec->Get( "FLD_JOB" ) );
				g_pMagicInfo[i].sNeed[0]		= atoi( pRec->Get( "FLD_NEEDL1" ) );
				g_pMagicInfo[i].sTrain[0]		= atoi( pRec->Get( "FLD_L1TRAIN" ) );
				g_pMagicInfo[i].sNeed[1]		= atoi( pRec->Get( "FLD_NEEDL2" ) );
				g_pMagicInfo[i].sTrain[1]		= atoi( pRec->Get( "FLD_L2TRAIN" ) );
				g_pMagicInfo[i].sNeed[2]		= atoi( pRec->Get( "FLD_NEEDL3" ) );
				g_pMagicInfo[i].sTrain[2]		= atoi( pRec->Get( "FLD_L3TRAIN" ) );
				g_pMagicInfo[i].sDelay			= atoi( pRec->Get( "FLD_DELAY" ) );
			}
		}

		InsertLogMsgParam(IDS_FETCH_MAGICDATA, &g_nNumOfMagicInfo, LOGPARAM_INT);
	}

	g_pConnCommon->DestroyRecordset( pRec );
}

void InitMoveMapEventInfo()
{
	char szQuery[1024];

	CRecordset *pRec = g_pConnCommon->CreateRecordset();
	sprintf( szQuery, "SELECT COUNT(*) AS FLD_COUNT FROM TBL_MOVEMAPEVENT");

	if (pRec->Execute( szQuery ))
	{
			if (pRec->Fetch())
				g_nNumOfMoveMapEventInfo = atoi( pRec->Get( "FLD_COUNT" ) );
	}

	g_pConnCommon->DestroyRecordset( pRec );

	if (g_nNumOfMoveMapEventInfo < 0) return;

	g_pMoveMapEventInfo = new CMoveMapEventInfo[g_nNumOfMoveMapEventInfo];

	sprintf( szQuery, "SELECT * FROM TBL_MOVEMAPEVENT ORDER BY FLD_INDEX");

	pRec = g_pConnCommon->CreateRecordset();
	
	if (pRec->Execute( szQuery ))
	{
		for (int i = 0; i < g_nNumOfMoveMapEventInfo; i++)
		{
			if (pRec->Fetch())								 
			{
				strcpy(g_pMoveMapEventInfo[i].szSMapFileName, pRec->Get( "FLD_SMAPFILENAME" ) );
				strcpy(g_pMoveMapEventInfo[i].szDMapFileName, pRec->Get( "FLD_DMAPFILENAME" ) );

				g_pMoveMapEventInfo[i].nSX = atoi( pRec->Get( "FLD_SX" ) );
				g_pMoveMapEventInfo[i].nSY = atoi( pRec->Get( "FLD_SY" ) );
				g_pMoveMapEventInfo[i].nDX = atoi( pRec->Get( "FLD_DX" ) );
				g_pMoveMapEventInfo[i].nDY = atoi( pRec->Get( "FLD_DY" ) );

				g_pMoveMapEventInfo[i].fIsOpen = FALSE;
			}
		}

		InsertLogMsgParam(IDS_FETCH_MOVEMAPEVENT, &g_nNumOfMoveMapEventInfo, LOGPARAM_INT);
	}

	g_pConnCommon->DestroyRecordset( pRec );
}

CMagicInfo*	GetMagicInfo(int nMagicID)
{
 	if (g_pMagicInfo[nMagicID].nIndex == nMagicID)
		return &g_pMagicInfo[nMagicID];

	for (int i = 0; i < g_nNumOfMagicInfo; i++)
	{
		if (g_pMagicInfo[i].nIndex == nMagicID)
			return &g_pMagicInfo[i];
	}

	return NULL;
}

CMonsterGenInfo* GetMonGenInfo(char *pszMonName)
{
	int nCmpLen;
	int nLen = memlen(pszMonName) - 1;

	for (int i = 0; i < g_nNumOfMonGenInfo; i++)
	{
		nCmpLen = memlen(g_pMonGenInfo[i].szMonName) - 1;

		if (nCmpLen == nLen)
		{
			if (memcmp(g_pMonGenInfo[i].szMonName, pszMonName, nLen) == 0)
				return &g_pMonGenInfo[i];
		}
	}

	return NULL;
}

CMonRaceInfo* GetMonRaceInfo(char *pszMonName)
{
	int nCmpLen;
	int nLen = memlen(pszMonName) - 1;

	for (int i = 0; i < g_nNumOfMonRaceInfo; i++)
	{
		nCmpLen = memlen(g_pMonRaceInfo[i].szMonName) - 1;

		if (nCmpLen == nLen)
		{
			if (memcmp(g_pMonRaceInfo[i].szMonName, pszMonName, nLen) == 0)
				return &g_pMonRaceInfo[i];
		}
	}

	return NULL;
}
