#pragma once


// CDlgSquareParam dialog

class CDlgSquareParam : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgSquareParam)

public:
	CDlgSquareParam(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgSquareParam();
	int m_nWS4;
	int m_nWS1;
	BOOL m_bMoveSquare;
	BOOL m_bShowRSquare;
	BOOL m_bShowGSquare;
	BOOL m_bShowBSquare;
// Dialog Data
	enum { IDD = IDD_DIALOG_SETSQUARE };

public:
	void SetParamInfo(GFLSTEST_PARAMINFO *pParmaInfo,	BOOL bMoveSquare,
		BOOL bShowRSquare,	BOOL bShowGSquare,		BOOL bShowBSquare);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
