#include "stdafx.h"

LPARAM OnServerSockMsg(WPARAM wParam, LPARAM lParam);
LPARAM OnClientSockMsg(WPARAM wParam, LPARAM lParam);

BOOL	jRegSetKey(LPCTSTR pSubKeyName, LPCTSTR pValueName, DWORD dwFlags, LPBYTE pValue, DWORD nValueSize);
BOOL	jRegGetKey(LPCTSTR pSubKeyName, LPCTSTR pValueName, LPBYTE pValue);

BOOL CALLBACK ConfigDlgFunc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

extern HINSTANCE		g_hInst;

extern HWND				g_hMainWnd;
extern HWND				g_hLogMsgWnd;
extern HWND				g_hToolBar;
extern HWND				g_hStatusBar;

extern HANDLE			g_hThreadForComm;

SOCKET			g_ssock = INVALID_SOCKET;
SOCKADDR_IN		g_saddr;

SOCKET			g_csock = INVALID_SOCKET;
SOCKADDR_IN		g_caddr;

extern CWHAbusive	g_xAbusive;

void SwitchMenuItem(BOOL fFlag)
{
	HMENU hMainMenu = GetMenu(g_hMainWnd);
	HMENU hMenu = GetSubMenu(hMainMenu, 0);

	if (fFlag)
	{
		EnableMenuItem(hMenu, IDM_STARTSERVICE, MF_GRAYED|MF_BYCOMMAND);
		EnableMenuItem(hMenu, IDM_STOPSERVICE, MF_ENABLED|MF_BYCOMMAND);

		SendMessage(g_hToolBar, TB_SETSTATE, (WPARAM)IDM_STARTSERVICE, (LPARAM)MAKELONG(TBSTATE_INDETERMINATE, 0));
		SendMessage(g_hToolBar, TB_SETSTATE, (WPARAM)IDM_STOPSERVICE, (LPARAM)MAKELONG(TBSTATE_ENABLED, 0));

		InsertLogMsg(IDS_STARTSERVICE);
	}
	else
	{
		EnableMenuItem(hMenu, IDM_STARTSERVICE, MF_ENABLED|MF_BYCOMMAND);
		EnableMenuItem(hMenu, IDM_STOPSERVICE, MF_GRAYED|MF_BYCOMMAND);

		SendMessage(g_hToolBar, TB_SETSTATE, (WPARAM)IDM_STARTSERVICE, (LPARAM)MAKELONG(TBSTATE_ENABLED, 0));
		SendMessage(g_hToolBar, TB_SETSTATE, (WPARAM)IDM_STOPSERVICE, (LPARAM)MAKELONG(TBSTATE_INDETERMINATE, 0));

		InsertLogMsg(IDS_STOPSERVICE);
	}
}

void OnCommand(WPARAM wParam, LPARAM lParam)
{
	switch (LOWORD(wParam))
	{
		case IDM_STARTSERVICE:
		{
			DWORD	dwIP = 0;
			int		nPort = 0;

			g_fTerminated = FALSE;

			if (!jRegGetKey(_GAMEGATE_SERVER_REGISTRY, _TEXT("LocalPort"), (LPBYTE)&nPort))
				nPort = 7200;
			
			if(InitServerSocket(g_ssock, &g_saddr, _IDM_SERVERSOCK_MSG, nPort, 2)){
				//IDS_ENABLE_PORT
				TCHAR	szSrvAddr[16];	
				_stprintf(szSrvAddr, _T("%d"),nPort);
				InsertLogMsgParam(IDS_ENABLE_PORT, szSrvAddr, LOGPARAM_STR);
				//MessageBox(NULL,_T("端口打开失败！"),_T("游戏网关"),MB_OK);
			}

			jRegGetKey(_GAMEGATE_SERVER_REGISTRY, _TEXT("RemoteIP"), (LPBYTE)&dwIP);

			if (!jRegGetKey(_GAMEGATE_SERVER_REGISTRY, _TEXT("RemotePort"), (LPBYTE)&nPort))
				nPort = 5000;

			if(ConnectToServer(g_csock, &g_caddr, _IDM_CLIENTSOCK_MSG, NULL, dwIP, nPort, FD_CONNECT|FD_READ|FD_CLOSE)){
				TCHAR	szSrvAddr[16];	
				_stprintf(szSrvAddr, _T("%d.%d.%d.%d:%d"), g_caddr.sin_addr.s_net, g_caddr.sin_addr.s_host, 
				g_caddr.sin_addr.s_lh, g_caddr.sin_addr.s_impno,nPort);
				InsertLogMsgParam(IDS_CONNECT_GAMESERVER, szSrvAddr, LOGPARAM_STR);
				
				//MessageBox(NULL,_T("服务连接失败！"),_T("游戏网关"),MB_OK);
			}

			SwitchMenuItem(TRUE);
			SendMessage(g_hStatusBar, SB_SETTEXT, MAKEWORD(0, 0), (LPARAM)_TEXT("就绪"));//左下角状态信息
			g_xAbusive.LoadAbusiveList();
			
			return;
		}
		case IDM_STOPSERVICE:
		{
			g_fTerminated = TRUE;
			SendSocketMsgS(GM_CONNECTIONCLOSE, 0, 0, 0, 0, NULL);
			ClearSocket(g_ssock);
			ClearSocket(g_csock);

			SwitchMenuItem(FALSE);

			KillTimer(g_hMainWnd, _ID_TIMER_CONNECTSERVER);//2011 11 22------20115 02 09
			SendMessage(g_hStatusBar, SB_SETTEXT, MAKEWORD(0, 0), (LPARAM)_TEXT("已停止"));
			SendMessage(g_hStatusBar, SB_SETTEXT, MAKEWORD(2, 0), (LPARAM)_TEXT(""));
			SendMessage(g_hStatusBar, SB_SETTEXT, MAKEWORD(1, 0), (LPARAM)_TEXT("连接已断开"));

			return;
		}
		case IDM_CONFIG:
		{
			DialogBox(g_hInst, MAKEINTRESOURCE(IDD_CONFIGDLG), g_hMainWnd, (DLGPROC)ConfigDlgFunc);

			return;
		}
		case IDM_EXIT:
			SendMessage(g_hMainWnd,WM_CLOSE,NULL,NULL);
			return;
					  
	}
}

// **************************************************************************************
//
//			
//
// **************************************************************************************

LPARAM APIENTRY MainWndProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	switch (nMsg)
	{
#ifdef _SOCKET_ASYNC_IO
		case _IDM_SERVERSOCK_MSG:
			return OnServerSockMsg(wParam, lParam);
#endif
		case _IDM_CLIENTSOCK_MSG:
			return OnClientSockMsg(wParam, lParam);
		case WM_COMMAND:
			OnCommand(wParam, lParam);
			break;
		case WM_SIZE:
		{
			if (g_hToolBar && g_hMainWnd && g_hStatusBar) 
			{
				RECT rcToolBar, rcMain, rcStatusBar;

				GetWindowRect(g_hToolBar, &rcToolBar);
				GetClientRect(g_hMainWnd, &rcMain);
				GetWindowRect(g_hStatusBar, &rcStatusBar);

				MoveWindow(g_hToolBar, 0, 0, LOWORD(lParam), (rcToolBar.bottom - rcToolBar.top), TRUE);
				MoveWindow(g_hStatusBar, 0, rcMain.bottom - (rcStatusBar.bottom - rcStatusBar.top), 
								LOWORD(lParam), (rcStatusBar.bottom - rcStatusBar.top), TRUE);
				MoveWindow(g_hLogMsgWnd, 0, (rcToolBar.bottom - rcToolBar.top), (rcMain.right - rcMain.left), 
								(rcMain.bottom - rcMain.top) - (rcToolBar.bottom - rcToolBar.top) - (rcStatusBar.bottom - rcStatusBar.top), 
								TRUE);

				int	nStatusPartsWidths[_NUMOFMAX_STATUS_PARTS];
				int nCnt = 0;

				for (int i = _NUMOFMAX_STATUS_PARTS - 1; i >= 0 ; i--)
					nStatusPartsWidths[nCnt++] = (rcStatusBar.right - rcStatusBar.left) - (90 * i);

				SendMessage(g_hStatusBar, SB_SETPARTS, _NUMOFMAX_STATUS_PARTS, (LPARAM)nStatusPartsWidths);
			}

			break;
		}
		case WM_CLOSE:
		{
			TCHAR	szMsg[128];
			TCHAR	szTitle[128];

			LoadString(g_hInst, IDS_PROGRAM_QUIT, szMsg, sizeof(szMsg));
			LoadString(g_hInst, IDS_PROGRAM_TITLE, szTitle, sizeof(szTitle));

			if (MessageBox(g_hMainWnd, szMsg, szTitle, MB_YESNO) == IDYES)
			{
				if (SendMessage(g_hToolBar, TB_GETSTATE, (WPARAM)IDM_STARTSERVICE, (LPARAM)0L) == TBSTATE_INDETERMINATE)
					OnCommand(IDM_STOPSERVICE, 0L);//MessageBox(g_hMainWnd, szMsg, szTitle, MB_ICONINFORMATION|MB_YESNO) == IDYES

				WSACleanup();

				PostQuitMessage(0);
			}

			return 0L;
		}
	}

	return (DefWindowProc(hWnd, nMsg, wParam, lParam));
}
