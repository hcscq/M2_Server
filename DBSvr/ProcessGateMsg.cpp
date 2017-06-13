#include "stdafx.h"

extern CWHList<CGateInfo*>			g_xGateInfoList;

UINT WINAPI ProcessGateMsg(LPVOID lpParameter)
{
	_TDEFAULTMESSAGE	DefaultMsg;
	char				*pszBegin, *pszEnd;
	int					nCount;
	_TCREATECHR			tCreateChr;
	_TDELCHR			tDelChr;
	PLISTNODE			pListNode;
	CGateInfo*			pGateInfo;
				
	while (TRUE)
	{
		if (g_xGateInfoList.GetCount())
		{
			pListNode = g_xGateInfoList.GetHead();

			while (pListNode)
			{
				pGateInfo = g_xGateInfoList.GetData(pListNode);
				
				if (pGateInfo)
				{
					nCount = pGateInfo->m_GateQ.GetCount();

					if (nCount)
					{
						for (int nLoop = 0; nLoop < nCount; nLoop++)
						{
							_LPTGATESENDBUFF pSendBuff = (_LPTGATESENDBUFF)pGateInfo->m_GateQ.PopQ();

							if (pSendBuff)
							{
								int nLen = memlen(pSendBuff->szData);

								if ((pszBegin = (char *)memchr(pSendBuff->szData, '#', nLen)) &&(pszEnd = (char *)memchr(pSendBuff->szData, '!', nLen)))
								{
									*pszEnd = '\0';

									fnDecodeMessageA(&DefaultMsg, (pszBegin + 1));	// 2 = "#?" ? = Check Code 

									switch (DefaultMsg.wIdent)
									{
										case CM_QUERYCHR:
											pGateInfo->QueryCharacter(pSendBuff->sock, (pszBegin + _DEFBLOCKSIZE + 1));
											break;
										case CM_NEWCHR:case CM_NEWCHRA:
											fnDecode6BitBufA((pszBegin + _DEFBLOCKSIZE + 1), (char *)&tCreateChr, sizeof(_TCREATECHR));
											pGateInfo->MakeNewCharacter(pSendBuff->sock, &tCreateChr);
											break;
										case CM_DELCHR:
											fnDecode6BitBufA((pszBegin + _DEFBLOCKSIZE + 1), (char *)&tDelChr, sizeof(_TDELCHR));
											pGateInfo->DeleteExistCharacter(pSendBuff->sock, &tDelChr);
											break;
										case CM_SELCHR:
											pGateInfo->GetSelectCharacter(pSendBuff->sock, (pszBegin + _DEFBLOCKSIZE + 2));
											break;
									}
								}

								delete pSendBuff;
							}
						}
					}
				}

				pListNode = g_xGateInfoList.GetNext(pListNode);
			}
		}

		SleepEx(1, TRUE);
	}
}
