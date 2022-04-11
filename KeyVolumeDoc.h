// KeyVolumeDoc.h : interface of the CKeyVolumeDoc class
//

#pragma once

class CKeyVolumeDoc : public CDocument {
protected: // create from serialization only
	CKeyVolumeDoc();
	DECLARE_DYNCREATE(CKeyVolumeDoc)

// Attributes
public:

// Operations
public:

// Overrides
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// Implementation
public:
	virtual ~CKeyVolumeDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};


