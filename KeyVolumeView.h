// KeyVolumeView.h : interface of the CKeyVolumeView class
//

#pragma once


#include "NotifyBox.h"

class CKeyVolumeView : public CFormView {
protected: // create from serialization only
	CKeyVolumeView();
	DECLARE_DYNCREATE(CKeyVolumeView)

public:
	enum{ IDD = IDD_KEYVOLUME_FORM };

// Attributes
public:
	CKeyVolumeDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitialUpdate(); // called first time after construct

// Implementation
public:
	virtual ~CKeyVolumeView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

	CNotifyBox m_box;
	CComboBox m_cmbZone;

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	// Volume Up/Down Signal (VK_VOLUME_UP/DOWN)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	enum eTIMER { T_UPDATE_UI = 3901, T_GENERATE_KEY, };
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnVolumeUp();
	afx_msg void OnVolumeDown();
	afx_msg void OnCbSendSpeakerVolumeKey();
	afx_msg void OnCbAutoShift();
	afx_msg void OnCbMouseJump();
	afx_msg void OnContextmenuAutoShift();
	afx_msg void OnUpdateAutoShift(CCmdUI* pCmdUI);
	afx_msg void OnCbReceiverVolume();
	afx_msg void OnQuit();
	afx_msg void OnDeviceList();
	afx_msg void OnRawInput(UINT nInputcode, HRAWINPUT hRawInput);
	afx_msg void OnSelchangeReceiverZone();
	afx_msg void OnBnClickedReceiverPowerOn();
	afx_msg void OnBnClickedReceiverPowerOff();
	afx_msg void OnBnClickedWakeUpServer();
	afx_msg void OnBnClickedCbStart();
};

#ifndef _DEBUG  // debug version in KeyVolumeView.cpp
inline CKeyVolumeDoc* CKeyVolumeView::GetDocument() const { return reinterpret_cast<CKeyVolumeDoc*>(m_pDocument); }
#endif
