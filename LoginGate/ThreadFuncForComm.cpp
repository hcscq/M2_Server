#include "stdafx.h"

#define PACKET_KEEPALIVE		"%--$"

extern HWND				g_hToolBar;
extern HWND				g_hStatusBar;

extern SOCKET			g_csock;
extern SOCKADDR_IN		g_caddr;

void					SendExToServer(char *pszPacket);

BOOL					jRegGetKey(LPCTSTR pSubKeyName, LPCTSTR pValueName, LPBYTE pValue);

VOID WINAPI OnTimerProc(HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	switch (idEvent)
	{
		case _ID_TIMER_KEEPALIVE:
		{
			if (g_csock != INVALID_SOCKET)
			{
				SendExToServer(PACKET_KEEPALIVE);
				SendMessage(g_hStatusBar, SB_SETTEXT, MAKEWORD(2, 0), (LPARAM)_TEXT("Check Activity"));
			}

			break;
		}
		case _ID_TIMER_CONNECTSERVER:
		{
			if (g_csock == INVALID_SOCKET)
			{
				DWORD	dwIP = 0;
				int		nPort = 0;

				//InsertLogMsg(IDS_APPLY_RECONNECT);

				jRegGetKey(_LOGINGATE_SERVER_REGISTRY, _TEXT("RemoteIP"), (LPBYTE)&dwIP);

				if (!jRegGetKey(_LOGINGATE_SERVER_REGISTRY, _TEXT("RemotePort"), (LPBYTE)&nPort))
					nPort = 5500;

				TCHAR	szSrvAddr[16];	
				_stprintf(szSrvAddr, _T("%d.%d.%d.%d:%d"), g_caddr.sin_addr.s_net, g_caddr.sin_addr.s_host, 
				g_caddr.sin_addr.s_lh, g_caddr.sin_addr.s_impno,nPort);
				InsertLogMsgParam(IDS_APPLY_RECONNECT, szSrvAddr, LOGPARAM_STR);

				ConnectToServer(g_csock, &g_caddr, _IDM_CLIENTSOCK_MSG, NULL, dwIP, nPort, FD_CONNECT|FD_READ|FD_CLOSE);

			}

			break;
		}
	}
}
