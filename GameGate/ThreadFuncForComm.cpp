#include "stdafx.h"

#define PACKET_KEEPALIVE		"%--$"

extern HWND				g_hToolBar;
extern HWND				g_hStatusBar;

extern SOCKET			g_csock;
extern SOCKADDR_IN		g_caddr;

extern SOCKET			g_ssock;
extern SOCKADDR_IN		g_saddr;

BOOL	jRegGetKey(LPCTSTR pSubKeyName, LPCTSTR pValueName, LPBYTE pValue);

VOID WINAPI OnTimerProc(HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)//timer proc
{
	switch (idEvent)
	{
		case _ID_TIMER_KEEPALIVE:
		{
			if (g_csock != INVALID_SOCKET)
			{
				SendSocketMsgS(GM_CHECKCLIENT, 0, 0, 0, 0, NULL);
				SendMessage(g_hStatusBar, SB_SETTEXT, MAKEWORD(2, 0), (LPARAM)_TEXT("Check Activity"));
				if (g_ssock == INVALID_SOCKET) {
					DWORD	dwIP = 0;
					int		nPort = 0;

					if (!jRegGetKey(_GAMEGATE_SERVER_REGISTRY, _TEXT("LocalPort"), (LPBYTE)&nPort))
						nPort = 7200;

					if (InitServerSocket(g_ssock, &g_saddr, _IDM_SERVERSOCK_MSG, nPort, 2)) {
						TCHAR	szSrvAddr[16];
						_stprintf(szSrvAddr, _T("%d"), nPort);
						InsertLogMsgParam(IDS_ENABLE_PORT, szSrvAddr, LOGPARAM_STR);
					}
				}
			}

			break;
		}
		case _ID_TIMER_CONNECTSERVER:
		{
			if (g_csock == INVALID_SOCKET)
			{
				DWORD	dwIP = 0;
				int		nPort = 0;

				InsertLogMsg(IDS_APPLY_RECONNECT);
				SendMessage(g_hStatusBar, SB_SETTEXT, MAKEWORD(1, 0), (LPARAM)_TEXT("Î´Á¬½Ó"));
				SendMessage(g_hStatusBar, SB_SETTEXT, MAKEWORD(2, 0), (LPARAM)_TEXT("Failed"));
				jRegGetKey(_GAMEGATE_SERVER_REGISTRY, _TEXT("RemoteIP"), (LPBYTE)&dwIP);

				if (!jRegGetKey(_GAMEGATE_SERVER_REGISTRY, _TEXT("RemotePort"), (LPBYTE)&nPort))
					nPort = 5000;

				if(ConnectToServer(g_csock, &g_caddr, _IDM_CLIENTSOCK_MSG, NULL, dwIP, nPort, FD_CONNECT|FD_READ|FD_CLOSE))
				{
					TCHAR	szSrvAddr[16];	
					_stprintf(szSrvAddr, _T("%d.%d.%d.%d:%d"), g_caddr.sin_addr.s_net, g_caddr.sin_addr.s_host, 
					g_caddr.sin_addr.s_lh, g_caddr.sin_addr.s_impno,nPort);
					InsertLogMsgParam(IDS_CONNECT_GAMESERVER, szSrvAddr, LOGPARAM_STR);
				}
				else{
				};
			}

			break;
		}
	}
}
