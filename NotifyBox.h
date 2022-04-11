#pragma once




// CNotifyBox

class CNotifyBox : public CWnd {
	DECLARE_DYNAMIC(CNotifyBox)

public:
	CNotifyBox();
	virtual ~CNotifyBox();

public:
	bool ShowVolume(LPCTSTR psz, std::chrono::steady_clock::duration dur);
	
protected:
	constexpr static COLORREF const crTransparent_{RGB(255, 0, 255)};
	constexpr static uint8_t alphaMax_{255};
	CString m_strNotify;
	std::chrono::steady_clock::time_point m_t;
	std::chrono::steady_clock::duration m_dur;

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	std::chrono::steady_clock::time_point m_tNotify;
	enum eTIMER { T_UPDATE_UI = 1456, T_FADE_OUT, };
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnNcPaint();
	afx_msg void OnPaint();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
};
