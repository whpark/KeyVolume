// KeyVolumeDoc.cpp : implementation of the CKeyVolumeDoc class
//

#include "stdafx.h"
#include "KeyVolume.h"

#include "KeyVolumeDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CKeyVolumeDoc

IMPLEMENT_DYNCREATE(CKeyVolumeDoc, CDocument)

BEGIN_MESSAGE_MAP(CKeyVolumeDoc, CDocument)
END_MESSAGE_MAP()

// CKeyVolumeDoc construction/destruction
CKeyVolumeDoc::CKeyVolumeDoc() {
}

CKeyVolumeDoc::~CKeyVolumeDoc() {
}

BOOL CKeyVolumeDoc::OnNewDocument() {
	if (!CDocument::OnNewDocument())
		return FALSE;

	return TRUE;
}

// CKeyVolumeDoc serialization
void CKeyVolumeDoc::Serialize(CArchive& ar) {
	if (ar.IsStoring()) {
	} else {
	}
}

// CKeyVolumeDoc diagnostics

#ifdef _DEBUG
void CKeyVolumeDoc::AssertValid() const { CDocument::AssertValid(); }
void CKeyVolumeDoc::Dump(CDumpContext& dc) const { CDocument::Dump(dc); }
#endif //_DEBUG

// CKeyVolumeDoc commands
