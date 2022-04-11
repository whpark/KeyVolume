// KeyVolume.cpp : Defines the class behaviors for the application.
//
#include "stdafx.h"
#include "KeyVolume.h"
#include "MainFrm.h"

#include "AboutDlg.h"

#include "KeyVolumeDoc.h"
#include "KeyVolumeView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CKeyVolumeApp

BEGIN_MESSAGE_MAP(CKeyVolumeApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CKeyVolumeApp::OnAppAbout)
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, &CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinApp::OnFileOpen)
END_MESSAGE_MAP()

// CKeyVolumeApp construction
CKeyVolumeApp::CKeyVolumeApp() {
}

// The one and only CKeyVolumeApp object

CKeyVolumeApp theApp;

// CKeyVolumeApp initialization

BOOL CKeyVolumeApp::InitInstance() {
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	// Initialize OLE libraries
	if (!AfxOleInit()) {
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();

	SetRegistryKey(_T("Biscuit-Lab"));
	LoadStdProfileSettings(0);

	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CKeyVolumeDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CKeyVolumeView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line.  Will return FALSE if
	// app was launched with /RegServer, /Register, /Unregserver or /Unregister.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The one and only window has been initialized, so show and update it
#ifdef _DEBUG
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
#else
	m_pMainWnd->UpdateWindow();
	m_pMainWnd->ShowWindow(SW_HIDE);
#endif

	// call DragAcceptFiles only if there's a suffix
	//  In an SDI app, this should occur after ProcessShellCommand
	return TRUE;
}

// App command to run the dialog
void CKeyVolumeApp::OnAppAbout() {
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CKeyVolumeApp message handlers
