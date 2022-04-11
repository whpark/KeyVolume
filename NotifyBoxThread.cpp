// CNotifyBoxThread.cpp : implementation file
//

#include "stdafx.h"
#include "KeyVolume.h"
#include "NotifyBoxThread.h"


// CNotifyBoxThread

IMPLEMENT_DYNCREATE(CNotifyBoxThread, CWinThread)

CNotifyBoxThread::CNotifyBoxThread() {
}

CNotifyBoxThread::~CNotifyBoxThread() {
}

BOOL CNotifyBoxThread::InitInstance() {
	return TRUE;
}

int CNotifyBoxThread::ExitInstance() {
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CNotifyBoxThread, CWinThread)
END_MESSAGE_MAP()


// CNotifyBoxThread message handlers
