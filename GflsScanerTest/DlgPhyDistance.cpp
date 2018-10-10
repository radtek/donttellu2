// DlgPhyDistance.cpp : implementation file
//

#include "stdafx.h"
#include "GflsScanerTest.h"
#include "DlgPhyDistance.h"


// CDlgPhyDistance dialog

IMPLEMENT_DYNAMIC(CDlgPhyDistance, CDialog)

CDlgPhyDistance::CDlgPhyDistance(CWnd* pParent , int *pPhyDist )
	: CDialog(CDlgPhyDistance::IDD, pParent)
	, m_nPhyDistance(1)
{
	m_pDist = NULL;
	m_pDist = pPhyDist;

}

CDlgPhyDistance::~CDlgPhyDistance()
{
	m_pDist = NULL;
}

void CDlgPhyDistance::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_DISTANCE, m_nPhyDistance);
	DDV_MinMaxInt(pDX, m_nPhyDistance, 0, 5000000);
}


BEGIN_MESSAGE_MAP(CDlgPhyDistance, CDialog)
END_MESSAGE_MAP()


// CDlgPhyDistance message handlers

void CDlgPhyDistance::OnOK()
{
	UpdateData();
	*m_pDist= m_nPhyDistance;
	CDialog::OnOK();
}
