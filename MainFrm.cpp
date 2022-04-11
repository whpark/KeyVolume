// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "KeyVolume.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

//LRESULT CALLBACK KeyboardProc(int code, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK LowLevelKeyboardProc(int code, WPARAM wParam, LPARAM lParam);
LRESULT AutoShift(int nCode, WPARAM wParam, LPARAM lParam);
//void PressKey(char vk);

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

enum {
	WM_TRAY_ICON = WM_APP+1,
};

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_MESSAGE(WM_TRAY_ICON, OnTrayIcon)
	ON_COMMAND(ID_CONTEXTMENU_SHOWWINDOW, OnShowWindow)
	ON_WM_SYSCOMMAND()
END_MESSAGE_MAP()

static UINT indicators[] = {
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

HHOOK g_hHook = NULL;

// CMainFrame construction/destruction
CMainFrame::CMainFrame() {
}

CMainFrame::~CMainFrame() {
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) {
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	//if (!m_wndStatusBar.Create(this) || !m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT))) {
	//	TRACE0("Failed to create status bar\n");
	//	return -1;      // fail to create
	//}

	CMenu* pMenu = GetMenu();
	pMenu->DestroyMenu();
	SetMenu(NULL);

	NOTIFYICONDATA nid;
	memset(&nid, 0, sizeof(nid));
	nid.cbSize = sizeof(nid);
	nid.hWnd = m_hWnd;
	nid.uID = IDC_TRAY_ICON;
	nid.uCallbackMessage = WM_TRAY_ICON;
	_tcscpy_s(nid.szTip, _T("KeyVolume"));
	nid.hIcon = theApp.LoadIcon(IDR_KeyVolumeTYPE);
	nid.uFlags = NIF_ICON|NIF_TIP|NIF_MESSAGE;

	Shell_NotifyIcon(NIM_ADD, &nid);

	::DestroyIcon(nid.hIcon);

#ifndef _DEBUG
	ShowWindow(SW_HIDE);
	ModifyStyleEx(0, WS_EX_TOOLWINDOW);
#endif

	if (!g_hHook) {
		//g_hHook = SetWindowsHookEx(WH_KEYBOARD, KeyboardProc, ::GetModuleHandle(NULL), 0);
//#ifndef _DEBUG
		g_hHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, ::GetModuleHandle(NULL), 0);
//#endif
	}

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs) {
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;

	cs.x = 0;
	cs.y = 0;

	cs.style = WS_OVERLAPPED | WS_CAPTION /*| FWS_ADDTOTITLE*/ | WS_MINIMIZEBOX | WS_SYSMENU;
	cs.style &= ~WS_VISIBLE;

	return TRUE;
}

// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const { CFrameWnd::AssertValid(); }
void CMainFrame::Dump(CDumpContext& dc) const { CFrameWnd::Dump(dc); }
#endif //_DEBUG

// CMainFrame message handlers

LRESULT CMainFrame::OnTrayIcon(WPARAM wParam, LPARAM lParam) {
	switch (lParam) {
	case WM_LBUTTONDBLCLK :
		OnShowWindow();
		break;
	//case WM_LBUTTONUP :
	case WM_RBUTTONUP :
		{
			CMenu menu;
			menu.LoadMenu(IDR_MAINFRAME);
			auto pMenu = menu.GetSubMenu(2);
			auto pt = GetCurrentMessage()->pt;
			pMenu->TrackPopupMenu(TPM_LEFTALIGN, pt.x, pt.y, this);
		}
		break;
	}
	return 0L;
}

void CMainFrame::OnShowWindow() {
	ShowWindow(SW_SHOW);
	SetForegroundWindow();
	SetFocus();
}

BOOL CMainFrame::DestroyWindow() {
	if (g_hHook)
		UnhookWindowsHookEx(g_hHook);
	g_hHook = NULL;

	NOTIFYICONDATA nid;
	memset(&nid, 0, sizeof(nid));
	nid.cbSize = sizeof(nid);
	nid.hWnd = m_hWnd;
	nid.uID = IDC_TRAY_ICON;

	Shell_NotifyIcon(NIM_DELETE, &nid);

	return CFrameWnd::DestroyWindow();
}

void CMainFrame::OnSysCommand(UINT nID, LPARAM lParam) {
	switch (nID) {
	case SC_CLOSE :
#ifdef _DEBUG
		break;
#else
		ShowWindow(SW_HIDE);
		return;
#endif
	case SC_CLOSE2 :
		nID = SC_CLOSE;
		break;
	default :
		break;
	}
	CFrameWnd::OnSysCommand(nID, lParam);
}
