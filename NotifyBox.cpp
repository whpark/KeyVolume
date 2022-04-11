// NotifyBox.cpp : implementation file
//

#include "stdafx.h"
#include "KeyVolume.h"
#include "NotifyBox.h"


// NotifyBox

IMPLEMENT_DYNAMIC(CNotifyBox, CWnd)

CNotifyBox::CNotifyBox() {
}

CNotifyBox::~CNotifyBox() {
}

BEGIN_MESSAGE_MAP(CNotifyBox, CWnd)
	ON_WM_CREATE()
	ON_WM_TIMER()
	//ON_WM_NCPAINT()
	ON_WM_PAINT()
	ON_WM_SYSCOMMAND()
END_MESSAGE_MAP()



// CNotifyBox message handlers

int CNotifyBox::OnCreate(LPCREATESTRUCT lpCreateStruct) {
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	SetLayeredWindowAttributes(crTransparent_, 255, LWA_ALPHA|LWA_COLORKEY);

	return 0;
}


void CNotifyBox::OnTimer(UINT_PTR nIDEvent) {
	switch (nIDEvent) {
	case T_UPDATE_UI :
		{
			auto t = std::chrono::steady_clock::now();
			if (t > m_t + m_dur) {
				ShowWindow(SW_HIDE);
				KillTimer(T_UPDATE_UI);
				return;
			}
			else {

				auto per = (m_t+m_dur - t) * 100 / m_dur;
				auto alpha = std::clamp<uint8_t>(alphaMax_ * per / 100, 0, alphaMax_);
				SetLayeredWindowAttributes(crTransparent_, alpha, LWA_ALPHA|LWA_COLORKEY);
				//Invalidate(false);
			}
		}
		return;
	}
	CWnd::OnTimer(nIDEvent);
}


void CNotifyBox::OnNcPaint() {
}


void CNotifyBox::OnPaint() {
	CPaintDC dc(this);

	CRect rectClient;
	GetClientRect(rectClient);

	constexpr static COLORREF crText{RGB(255, 255, 255)};
	constexpr static COLORREF crOut{RGB(0, 0, 92)};
	static CBrush brushBkgnd(crTransparent_);
	static CPen penBkgnd(PS_SOLID, 1, crTransparent_);
	static CFont font;
	if (!font.m_hObject) {
		constexpr static LOGFONT lf{ .lfHeight = -32, .lfQuality = NONANTIALIASED_QUALITY, .lfFaceName = _T("Segoe UI")};
		font.CreateFontIndirect(&lf);
	}
	auto* pOldBrush = dc.SelectObject(&brushBkgnd);
	auto* pOldPen = dc.SelectObject(penBkgnd);
	auto* pOldFont = dc.SelectObject(&font);

	dc.Rectangle(rectClient);

	//dc.SetTextColor(crText);
	//dc.SetBkColor(RGB(96, 96, 192));
	dc.SetBkMode(TRANSPARENT);

	dc.SetTextColor(crOut);
	for (auto shift : std::array<CPoint, 4>{{{0,1}, {0,-1}, {1, 0}, {-1, 0}}}) {
		auto rectO = rectClient + shift;
		dc.DrawText(m_strNotify, rectO, DT_CENTER|DT_SINGLELINE|DT_VCENTER);
	}
	dc.SetTextColor(crText);
	dc.DrawText(m_strNotify, rectClient, DT_CENTER|DT_SINGLELINE|DT_VCENTER);

	dc.SelectObject(pOldFont);
	dc.SelectObject(pOldPen);
	dc.SelectObject(pOldBrush);
}

bool CNotifyBox::ShowVolume(LPCTSTR psz, std::chrono::steady_clock::duration dur) {
	KillTimer(T_UPDATE_UI);

	m_strNotify = psz;
	m_t = std::chrono::steady_clock::now();
	m_dur = dur;

	if (!IsWindowVisible())
		ShowWindow(SW_SHOW);
	Invalidate(false);

	SetLayeredWindowAttributes(crTransparent_, alphaMax_, LWA_ALPHA|LWA_COLORKEY);
	SetTimer(T_UPDATE_UI, 20, nullptr);

	return false;
}


void CNotifyBox::OnSysCommand(UINT nID, LPARAM lParam) {
	if (nID == SC_CLOSE)
		return;

	CWnd::OnSysCommand(nID, lParam);
}
