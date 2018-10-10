// GfRectTracker.h: interface for the CGfRectTracker class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GFRECTTRACKER_H__2C0CE2A4_63AB_400D_8A62_E3ABD34ADC9B__INCLUDED_)
#define AFX_GFRECTTRACKER_H__2C0CE2A4_63AB_400D_8A62_E3ABD34ADC9B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define GFP_RTTRACKER_HANDSIZE				4

// �����Tracker֧�ֵ� move/resize����
#define GFRT_RESIZE_FLAG_NULL				0x0000
#define GFRT_RESIZE_FLAG_TOPLEFT			0x1
#define GFRT_RESIZE_FLAG_TOPRIGHT			0x2
#define GFRT_RESIZE_FLAG_BOTTOMRIGHT		0x4
#define GFRT_RESIZE_FLAG_BOTTOMLEFT			0x8
#define GFRT_RESIZE_FLAG_TOP				0x10
#define GFRT_RESIZE_FLAG_RIGHT				0x20
#define GFRT_RESIZE_FLAG_BOTTOM				0x40
#define GFRT_RESIZE_FLAG_LEFT				0x80
#define GFRT_RESIZE_FLAG_MOVE				0x100
#define GFRT_RESIZE_FLAG_ALL				0x1ff


typedef struct tagGfRectTrackInfo
{
	POINT	ptVertex[4];	// �ĸ�����, ���ϡ����ϡ����¡�����

	POINT	ptBorderMid[4];	// �����ߵ��е�, �ϱߡ��ұߡ��±ߡ���ߵ��е�

	POINT	ptVector;		// ��ת����
	double  fVector;		// ���ȱ�ʾ����ת����

	int		nHSplit;		// ˮƽ����ֱ�ָ���룬����ת����Ϊ(1,0)ʱ��
	int		nVSplit;		// rect���ϱߺ���ߵľ���������

	POINT	ptHSplit[2];	// ˮƽ�ָ��ߵ������յ�; (����ָ����Ƶ�������)
	POINT	ptHsplitMiddle;

	POINT	ptVSplit[2];	// ��ֱ�ָ��ߵ������յ㣻(����ָ����Ƶ�Ĵָ��)
	POINT	ptVsplitMiddle;

	POINT	ptOrigin;		// ��תԭ��
}GFRECTTRACKINFO;

// struct for tenprint scan
typedef struct tagGTscan_InputBox
{
	UCHAR		bPalm;		// palm or not
	UCHAR		bleft;		// left palm or not when bpalm = 1
	UCHAR		bCut;		// ignore when bpalm = 1;
	UCHAR		bRes1[5];

	int			nwChar;		// TEXTMETRIC.tmAveCharWidth
	int			nhChar;		// TEXTMETRIC.tmHeight
	
	char		szDiscript[16];	//

	CPen*		pBoxpen;
	CPen*		pCutpen;
#if	POINTER_SIZE == 4
	UCHAR		uPtrRes1[8];
#endif

	COLORREF	clrText;
	COLORREF	clrCut;

	// only for plam
	UCHAR		bRes2[6];
	WORD		fPalmflag;		 // Palm sub area cut flag

	char		szPalmArea[16];
	char		szFourArea[16];
	char		szUpThumb[16];
	char		szDwThumb[16];
}GTSCAN_INPUTBOX;	// 128 bytes

class CGfRectTracker  
{
public:
// Constructors
	CGfRectTracker();
	CGfRectTracker(LPRECT lprect, POINT ptVector, LPPOINT pptOrigin);
	CGfRectTracker(LPRECT lprect, double fVector, LPPOINT pptOrigin);

// Attributes
	GFRECTTRACKINFO	m_stTrackInfo;

	CSize	m_sizeMin;    // minimum X and Y size during track operation
	
	UINT	m_nresizeFlag;	// ������ƶ������Ĵ�С����
	int		m_nHandleSize;

	BOOL	m_bSplit;		// �Ƿ�ָ�rectTracker
	BOOL	m_bNoFinger;	//!< �Ƿ�ȱָ

	CSize	m_sizeMinSplit;

// Hit-Test codes
	enum Gf_TrackerHit
	{
		gf_hitNothing = -1,
		gf_hitTopLeft = 0, gf_hitTopRight = 1, gf_hitBottomRight = 2, gf_hitBottomLeft = 3,
		gf_hitTop = 4, gf_hitRight = 5, gf_hitBottom = 6, gf_hitLeft = 7, gf_hitMiddle = 8,
		gf_hitHsplitTop = 9, gf_hitVsplitLeft = 10
	};

	
// Operations
	void	SetTopflagColor(COLORREF clrTop);
	void	SetResizeFlage(UINT nresizeFlag);
	void	SetSplitValue(BOOL bSplit, int hSplit, int vSplit);
	void	SetRectTrackerVector(POINT ptVector);
	void	SetRectTrackerVector(double fVector);
	void	DrawTracker(CDC *pDC);
	void	SetTrackedRect(LPRECT lprect, LPPOINT pptOrigin = NULL);
	void	GetTrackerRect(LPRECT lprect, int &nHsplit, int &nVsplit);
	void	ChangeRotateOrigin(POINT ptOrigin);

	int		HitTest(CPoint point);
	void	AboutOnMouseMove(CPoint pt, CWnd* pWnd);
	BOOL	Gf_Track(CWnd* pWnd, CPoint point, CWnd* pWndClipTo = NULL);

	void	SetClipRectArea(BOOL bNeedClip, LPRECT rtClip);

// Overridables
	virtual void DrawTrackerRect(GFRECTTRACKINFO* pstRectInfo, CWnd* pWndClipTo,
								 CDC* pDC, CWnd* pWnd, LPRECT rect);
	virtual void AdjustRect(int nHandle, LPRECT lpRect);
	virtual void OnChangedRect(const GFRECTTRACKINFO& stRectInfo);

// Implementation
public:
	virtual ~CGfRectTracker();


protected:
	COLORREF m_clrTop; 
	HPEN	m_hpenTop, m_hpenBlackDotted;

	CRect	m_rect;				// current position (always in pixels and ptvector = (1, 0))
	CRect	m_rectLast;
	CSize	m_sizeLast;
	BOOL	m_bErase;			// TRUE if DrawTrackerRect is called for erasing
	BOOL	m_bFinalErase;		// TRUE if DragTrackerRect called for final erase

	POINT	m_ptSplit;			// ptSplit.yˮƽ�ָ�����m_rect��ߵĽ��㣻
								// ptSplit.x��ֱ�ָ�����m_rect�ϱߵĽ��㣻

	GFRECTTRACKINFO	m_stTrackInfoLast;

	BOOL	m_bNeedClip;
	CRect	m_rtClipArea;		// Clip region for dc

	// implementation helpers
	void	Initialize();
	double	Point2Angle(POINT &ptVector);
	void    Angle2Point(POINT &ptVector, double fAngle);
	void	RotatePointByVector(POINT ptSrc, POINT &ptRotate, double fVector, POINT &ptOrigin);
	void	RotateRectByVector(CRect &rect, GFRECTTRACKINFO &stTrackInfo);
	void	RotateSplitByVector(CRect &rect, GFRECTTRACKINFO &stTrackInfo);

	BOOL	Gf_TrackHandle(int nHandle, CWnd* pWnd, CPoint point, CWnd* pWndClipTo);
	void	GetModifyPointers(int nHandle, int** ppx, int** ppy, int* px, int* py);
};

#endif // !defined(AFX_GFRECTTRACKER_H__2C0CE2A4_63AB_400D_8A62_E3ABD34ADC9B__INCLUDED_)
