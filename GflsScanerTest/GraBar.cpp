// GraBar.cpp : implementation file
//

#include "stdafx.h"
#include "gflsscanertest.h"
#include "GraBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGraBar

CGraBar::CGraBar()
{
	m_bhor = 1;
}

CGraBar::~CGraBar()
{
}


BEGIN_MESSAGE_MAP(CGraBar, CDialogBar)
	//{{AFX_MSG_MAP(CGraBar)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CGraBar message handlers

BOOL CGraBar::Create(CWnd* pParentWnd,UINT nIDTemplate, UINT nStyle,UINT nID) 
{
	// TODO: Add your specialized code here and/or call the base class
	return CDialogBar::Create( pParentWnd,nIDTemplate,nStyle,nID );
	  //CDialogBar::Create(pParentWnd, IDD_DIALOG_GRADUATIONX, nStyle, IDD_DIALOG_GRADUATIONX);
	 //return CDialogBar::Create(pParentWnd, IDD_DIALOG_GRADUATIONY, CBRS_LEFT | CBRS_TOOLTIPS | CBRS_FLYBY, IDD_DIALOG_GRADUATIONY);
	 
}

void CGraBar::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	CRect rt;
	int num=100;
	GetClientRect(&rt);
	CBrush bgBrush;
	bgBrush.CreateSolidBrush(RGB(255,255,255));
	dc.SelectObject(bgBrush);
	dc.Rectangle(rt);
	CPen PenBlack(PS_SOLID, 1, RGB(0, 0, 0));
	dc.SelectObject(PenBlack);
	COLORREF clText, clOld;
	clText = RGB(0, 0, 0);
	clOld = dc.SetTextColor(clText);
	CFont font,*pOldFont; 
	font.CreateFont(10, // 字体高度 
	5, // 字体宽度 
	0, // nEscapement 
	0, // nOrientation 
	FW_BOLD, // nWeight 
	FALSE, // bItalic 
	FALSE, // bUnderline 
	0, // cStrikeOut 
	ANSI_CHARSET, // nCharSet 
	OUT_DEFAULT_PRECIS, // nOutPrecision 
	CLIP_DEFAULT_PRECIS, // nClipPrecision 
	DEFAULT_QUALITY, // nQuality 
	DEFAULT_PITCH | FF_SWISS, // nPitchAndFamily 
	_T("Arial")); // lpszFac 
	

	int		x, y;

	if(m_bhor==1)
	{
		for( x = 20; x < rt.Width(); x += 10)
		{
			dc.MoveTo(x, 0);
			dc.LineTo(x, 5);
		}
		for( x =120; x < rt.Width(); x+=100)
		{
			dc.MoveTo(x,0);
			dc.LineTo(x,10);
		}


		for( x = 125, num = 100; x < rt.Width(); x += 100,num += 100)
		{
			char buffer[20] ={0};
			int j;
			j = sprintf( buffer, "%d\n", num );
			
			//_itoa( num, buffer, 10 );
			pOldFont=dc.SelectObject(&font); 
			dc.TextOut(x, 5, buffer);
			dc.SelectObject(pOldFont);
				//dc.DrawText(buffer, -1, rt, DT_SINGLELINE | DT_BOTTOM | DT_LEFT);
		}
	}

	else
	{
		
		for( y = 0; y < rt.Height(); y += 10)
		{
		dc.MoveTo(0, y);
		dc.LineTo(5, y);
		}
		for( y = 100; y < rt.Height(); y+=100)
		{
			dc.MoveTo(0,y);
			dc.LineTo(10,y);
		}


		for( y = 105, num = 100; y < rt.Height(); y += 100,num += 100)
		{

			char buffer[20] ={0};
			_itoa( num, buffer, 10 );
			pOldFont=dc.SelectObject(&font); 
			//dc.DrawText(buffer, -1, rt, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			dc.TextOut(5, y, buffer);
			dc.SelectObject(pOldFont);
		}

	}
		dc.SetTextColor(clOld);
		PenBlack.DeleteObject();
		bgBrush.DeleteObject();

	

	// TODO: Add your message handler code here
	
	// Do not call CDialogBar::OnPaint() for painting messages
}
