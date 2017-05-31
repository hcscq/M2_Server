#include "stdafx.h"
#include "../def/dbmgr.h"
#include "TableList.h"
LPARAM OnServerSockMsg(WPARAM wParam, LPARAM lParam);
LPARAM OnClientSockMsg(WPARAM wParam, LPARAM lParam);

BOOL		jRegSetKey(LPCTSTR pSubKeyName, LPCTSTR pValueName, DWORD dwFlags, LPBYTE pValue, DWORD nValueSize);
BOOL		jRegGetKey(LPCTSTR pSubKeyName, LPCTSTR pValueName, LPBYTE pValue);

UINT WINAPI ProcessDBMsg(LPVOID lpParameter);
UINT WINAPI ProcessGateMsg(LPVOID lpParameter);

BOOL		InitGateCommSocket(SOCKET &s, SOCKADDR_IN* addr, UINT nMsgID, int nPort, long lEvent);
LPARAM		OnGateCommSockMsg(WPARAM wParam, LPARAM lParam);

BOOL CALLBACK ConfigDlgFunc(HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

extern HINSTANCE		g_hInst;

extern HWND				g_hMainWnd;
extern HWND				g_hLogMsgWnd;
extern HWND				g_hToolBar;
extern HWND				g_hStatusBar;

BOOL					g_fTerminated = FALSE;

SOCKET					g_ssock = INVALID_SOCKET;
SOCKADDR_IN				g_saddr;

SOCKET					g_gssock = INVALID_SOCKET;
SOCKADDR_IN				g_gsaddr;


CWHList<GAMESERVERINFO*>	g_xGameServerList;

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

		SendMessage(g_hStatusBar, SB_SETTEXT, MAKEWORD(0, 0), (LPARAM)_T("就绪"));
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

void LoadCharacterRecords()
{
	InsertLogMsg(IDS_LOADACCOUNTRECORDS);

	CRecordset *pRec = GetDBManager()->CreateRecordset();
	if ( pRec->Execute( "SELECT * FROM TBL_GAMEGATEINFO" ) )
	{
		GAMESERVERINFO *pServerInfo;

		while ( pRec->Fetch() )
		{
			pServerInfo = new GAMESERVERINFO;
			if ( !pServerInfo )
				break;

			pServerInfo->index = atoi( pRec->Get( "FLD_SERVERIDX" ) );
			strcpy( pServerInfo->name, pRec->Get( "FLD_SERVERNAME" ) );
			strcpy( pServerInfo->ip,   pRec->Get( "FLD_SERVERIP" ) );
			pServerInfo->connCnt = 0;//初始化客户端连接数为0

			g_xGameServerList.AddNewNode( pServerInfo );//加入链表
		}
	}
	GetDBManager()->DestroyRecordset( pRec );
}

void OnCommand(WPARAM wParam, LPARAM lParam)
{
	int nPort;

	switch (LOWORD(wParam))
	{
		case IDM_STARTSERVICE:
		{
			TCHAR	wszDatabase[256];
			char	szDatabase[256];//数据库连接DNS

			jRegGetKey(_DB_SERVER_REGISTRY, _TEXT("Device"), (LPBYTE)wszDatabase);
			WideCharToMultiByte(CP_ACP, 0, wszDatabase, -1, szDatabase, sizeof(szDatabase), NULL, NULL);//该函数映射一个unicode字符串到一个多字节字符串。
	//szDatabase
			if(!GetDBManager()->Init( InsertLogMsg,szDatabase , "sa", "123" )){
				InsertLogMsg( _T("数据库连接失败,无法启动服务！\n") );
				//MessageBox(NULL,_T("数据库连接失败！"),_T("数据库网关"),MB_OK);
				return;
			}
			CConnection *pConn = GetDBManager()->m_dbMain.CreateConnection( szDatabase, "sa", "123" );
			if ( pConn )
			{
				if ( !GetTblStartPoint()->Init( pConn ) )
					InsertLogMsg( _T("读取 TBL_STARTPOINT 表失败\n") );//
			}
			GetDBManager()->m_dbMain.DestroyConnection( pConn );
			


			g_fTerminated = FALSE;

			if (!jRegGetKey(_DB_SERVER_REGISTRY, _TEXT("LocalPort"), (LPBYTE)&nPort))
				nPort = 5000;
	
			LoadCharacterRecords();

			if(InitServerSocket(g_ssock, &g_saddr, _IDM_GATECOMMSOCK_MSG, 6000, 1))
				InsertLogMsg( _T("端口:6000已打开.\n") );
			else InsertLogMsg( _T("端口:6000打开失败.\n") );
			if(InitGateCommSocket(g_gssock, &g_gsaddr, _IDM_GATECOMMSOCK_MSG, 5100, FD_ACCEPT|FD_READ|FD_CLOSE))
				InsertLogMsg( _T("端口:5100已打开.\n") );
			else InsertLogMsg( _T("端口:5100打开失败.\n") );

			UINT			dwThreadIDForMsg = 0;
			unsigned long	hThreadForMsg = 0;
				
//			if (hThreadForMsg = _beginthreadex(NULL, 0, ProcessUserHuman, NULL, 0, &dwThreadIDForMsg))
				hThreadForMsg = _beginthreadex(NULL, 0, ProcessDBMsg, NULL, 0, &dwThreadIDForMsg);
				hThreadForMsg = _beginthreadex(NULL, 0, ProcessGateMsg, NULL, 0, &dwThreadIDForMsg);

			SwitchMenuItem(TRUE);

			return;
		}
		case IDM_STOPSERVICE:
		{
			g_fTerminated = TRUE;
			if(ClearSocket(g_ssock))
				InsertLogMsg( _T("端口:6000已关闭.\n") );;
			if(ClearSocket(g_gssock))
				InsertLogMsg( _T("端口:5100已关闭.\n") );;
			SwitchMenuItem(FALSE);

			return;
		}
		case IDM_CONFIG:
		{
			DialogBox(g_hInst, MAKEINTRESOURCE(IDD_CONFIGDLG),g_hMainWnd, (DLGPROC)ConfigDlgFunc);

			return;
		}
		case IDM_EXIT: SendMessage(g_hMainWnd,WM_CLOSE,NULL,NULL);return;
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
		case _IDM_GATECOMMSOCK_MSG:
			return OnGateCommSockMsg(wParam, lParam);
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
				MoveWindow(g_hLogMsgWnd, 0, (rcToolBar.bottom - rcToolBar.top) - 2, (rcMain.right - rcMain.left), 
								(rcMain.bottom - rcMain.top) - (rcToolBar.bottom - rcToolBar.top) - (rcStatusBar.bottom - rcStatusBar.top) + 2, 
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

			if (MessageBox(g_hMainWnd, szMsg, szTitle,MB_YESNO) == IDYES)
			{
				if (SendMessage(g_hToolBar, TB_GETSTATE, (WPARAM)IDM_STARTSERVICE, (LPARAM)0L) == TBSTATE_INDETERMINATE)
					OnCommand(IDM_STOPSERVICE, 0L);

				ClearSocket(g_ssock);

				WSACleanup();

				CoUninitialize();

				PostQuitMessage(0);
			}

			return 0L;
		}
	}

	return (DefWindowProc(hWnd, nMsg, wParam, lParam));
}
