#pragma once


// CDlgPhyDistance dialog

class CDlgPhyDistance : public CDialog
{
	DECLARE_DYNAMIC(CDlgPhyDistance)

public:
	CDlgPhyDistance(CWnd* pParent ,int *pPhyDist );   // standard constructor
	virtual ~CDlgPhyDistance();

// Dialog Data
	enum { IDD = IDD_DIALOG_PHYDIST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	int m_nPhyDistance;
	int *m_pDist;
protected:
	virtual void OnOK();
};
