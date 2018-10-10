// GflsScanerTestDoc.h : interface of the CGflsScanerTestDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_GFLSSCANERTESTDOC_H__4BF53E59_F248_43EF_A835_BD2AAC2CB912__INCLUDED_)
#define AFX_GFLSSCANERTESTDOC_H__4BF53E59_F248_43EF_A835_BD2AAC2CB912__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CGflsScanerTestDoc : public CDocument
{
protected: // create from serialization only
	CGflsScanerTestDoc();
	DECLARE_DYNCREATE(CGflsScanerTestDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGflsScanerTestDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CGflsScanerTestDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CGflsScanerTestDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GFLSSCANERTESTDOC_H__4BF53E59_F248_43EF_A835_BD2AAC2CB912__INCLUDED_)
