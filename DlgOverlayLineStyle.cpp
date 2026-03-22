// DlgOverlayLineStyle.cpp : 구현 파일입니다.
//

#include "pch.h"
#include "C4IMap.h"
#include "DlgOverlayLineStyle.h"
#include "DlgOverlayProperty.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif



IMPLEMENT_DYNAMIC(CDlgOverlayLineStyle, CDialog)

CDlgOverlayLineStyle::CDlgOverlayLineStyle(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgOverlayLineStyle::IDD, pParent)
{
	m_pDlgOverlayProperty = (CDlgOverlayProperty*)pParent;
}

CDlgOverlayLineStyle::~CDlgOverlayLineStyle()
{
}

void CDlgOverlayLineStyle::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BTN_OVERLAY_LINE_DASH_STYLE_SOLID, m_btnSolid);
	DDX_Control(pDX, IDC_BTN_OVERLAY_LINE_DASH_STYLE_DASH, m_btnDash);
	DDX_Control(pDX, IDC_BTN_OVERLAY_LINE_DASH_STYLE_DOT, m_btnDot);
	DDX_Control(pDX, IDC_BTN_OVERLAY_LINE_DASH_STYLE_DASHDOT, m_btnDashDot);
	DDX_Control(pDX, IDC_BTN_OVERLAY_LINE_DASH_STYLE_DASHDOTDOT, m_btnDashDotDot);
}


BEGIN_MESSAGE_MAP(CDlgOverlayLineStyle, CDialog)
	ON_BN_CLICKED(IDC_BTN_OVERLAY_LINE_DASH_STYLE_SOLID, CDlgOverlayLineStyle::OnBnClickedBtnOverlayLineDashStyleSolid)
	ON_BN_CLICKED(IDC_BTN_OVERLAY_LINE_DASH_STYLE_DASH, CDlgOverlayLineStyle::OnBnClickedBtnOverlayLineDashStyleDash)
	ON_BN_CLICKED(IDC_BTN_OVERLAY_LINE_DASH_STYLE_DOT, CDlgOverlayLineStyle::OnBnClickedBtnOverlayLineDashStyleDot)
	ON_BN_CLICKED(IDC_BTN_OVERLAY_LINE_DASH_STYLE_DASHDOT, CDlgOverlayLineStyle::OnBnClickedBtnOverlayLineDashStyleDashdot)
	ON_BN_CLICKED(IDC_BTN_OVERLAY_LINE_DASH_STYLE_DASHDOTDOT, CDlgOverlayLineStyle::OnBnClickedBtnOverlayLineDashStyleDashdotdot)
	ON_WM_CREATE()
	ON_WM_KILLFOCUS()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()


BOOL CDlgOverlayLineStyle::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_btnSolid.SetShade(CShadeButtonST::SHS_SOFTBUMP);
	m_btnSolid.SetIcon(IDI_CLOSE);
	m_btnSolid.SetTooltipText(_T("")); 

	m_btnDash.SetShade(CShadeButtonST::SHS_SOFTBUMP);
	m_btnDash.SetIcon(IDI_CLOSE);
	m_btnDash.SetTooltipText(_T("")); 

	m_btnDot.SetShade(CShadeButtonST::SHS_SOFTBUMP);
	m_btnDot.SetIcon(IDI_CLOSE);
	m_btnDot.SetTooltipText(_T("")); 

	CBitmap     bitmap;	
	if(bitmap.LoadBitmap(IDB_TRK_CIRCLE)==TRUE)
	{		
		m_btnDashDot.SetShade(CShadeButtonST::SHS_SOFTBUMP);
		m_btnDashDot.SetBitmap((HBITMAP)bitmap);
		m_btnDashDot.SetTooltipText(_T("")); 
	}

	m_btnDashDotDot.SetShade(CShadeButtonST::SHS_SOFTBUMP);
	m_btnDashDotDot.SetIcon(IDI_CLOSE);
	m_btnDashDotDot.SetTooltipText(_T("")); 

	return TRUE;
}

void CDlgOverlayLineStyle::OnBnClickedBtnOverlayLineDashStyleSolid()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void CDlgOverlayLineStyle::OnBnClickedBtnOverlayLineDashStyleDash()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void CDlgOverlayLineStyle::OnBnClickedBtnOverlayLineDashStyleDot()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void CDlgOverlayLineStyle::OnBnClickedBtnOverlayLineDashStyleDashdot()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void CDlgOverlayLineStyle::OnBnClickedBtnOverlayLineDashStyleDashdotdot()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CDlgOverlayLineStyle::OnKillFocus(CWnd* pNewWnd)
{
	CDialog::OnKillFocus(pNewWnd);
	ShowWindow(SW_HIDE);
}

void CDlgOverlayLineStyle::OnLButtonUp(UINT nFlags, CPoint point)
{
	ShowWindow(SW_HIDE);

	CDialog::OnLButtonUp(nFlags, point);
}
