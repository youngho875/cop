// DlgOverlayProperty.cpp : 구현 파일입니다.
//

#include "pch.h"
#include "C4IMap.h"
#include "C4IMapView.h"
#include "DlgOverlayProperty.h"
//#include "DlgOverlayLineStyle.h"
#include "OverlayView.h"
#include <windef.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

char* strLineDashStyle[]=
{
	"실선",
	"쇄선",
	"점선",
	"일점쇄선",
	"이점쇄선",
};

char* strLineStartEndKind[]=
{
	"모양1",
	"모양2",
	"모양3",
	"모양4",
	"모양5",
	"모양6",
	"모양7",
	"모양8",
	"모양9",
	"모양10",
	"모양11",
	"모양12",
	"모양13",
	"모양14",
	"모양15",
	"모양16",
	"모양17",
	"모양18",
	"모양19",
	"모양13",
	"모양13",
	"모양13",
	"모양13",
	"모양13",
	"모양13",
	"모양13",
	"모양13",
/*	"삼각",
	"사각",
	"원",
	"삼각",
	"NoAnchor",
	"SquareAnchor",
	"RoundAnchor",
	"DiamondAnchor",
	"ArrowAnchor",*/
};

IMPLEMENT_DYNAMIC(CDlgOverlayProperty, CDialogEx)

CDlgOverlayProperty::CDlgOverlayProperty(CWnd* pParent, COverlayView* pOverlayView)
	: CDialogEx(CDlgOverlayProperty::IDD, pParent)
	, m_nEdtFillBlend(0)
	, m_nEdtLineBlend(0)
	, m_nEdtLineThickness(0)
	, m_bFill(TRUE)
	, m_bLine(TRUE)	
{
	m_pMainView = (CC4IMapView*)pParent;
	m_pOverlayView = pOverlayView;
	m_pObject = NULL;
}

CDlgOverlayProperty::~CDlgOverlayProperty()
{
	//SAFE_DELETE(m_pOverlayLineStyle);
}

void CDlgOverlayProperty::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDT_OVERLAY_FILL_BLEND, m_nEdtFillBlend);
	DDX_Text(pDX, IDC_EDT_OVERLAY_LINE_BLEND, m_nEdtLineBlend);
	//DDX_Control(pDX, IDC_EDT_OVERLAY_LINE_BLEND, m_edtLineBlend);
	DDX_Text(pDX, IDC_EDT_OVERLAY_LINE_THICKNESS, m_nEdtLineThickness);
	DDX_Control(pDX, IDC_SLIDER_OVERLAY_FILL_BLEND, m_SliderFillBlend);
	DDX_Control(pDX, IDC_SLIDER_OVERLAY_LINE_BLEND, m_SliderLineBlend);
	DDX_Control(pDX, IDC_SPIN_OVERLAY_LINE_BLEND, m_SpinLineBlend);
	DDX_Control(pDX, IDC_SPIN_OVERLAY_FILL_BLEND, m_SpinFillBlend);
	//DDX_Control(pDX, IDC_BTN_OVERLAY_FILL_COLOR, m_btnFillColor);
	//DDX_Control(pDX, IDC_BTN_OVERLAY_LINE_COLOR, m_btnLineColor);
	DDV_MinMaxUInt(pDX, m_nEdtFillBlend, 0, 255);
	DDV_MinMaxUInt(pDX, m_nEdtLineBlend, 0, 255);
	DDX_Radio(pDX, IDC_RADIO_OVERLAY_NO_FILL, m_bFill);
	DDX_Radio(pDX, IDC_RADIO_OVERLAY_NO_LINE, m_bLine);
	DDX_Control(pDX, IDC_SPIN_OVERLAY_LINE_THICKNESS, m_SpinLineThickness);
	DDX_Text(pDX, IDC_STATIC_OVERLAY_FILL_COLOR, m_strFillColor);
	DDX_Text(pDX, IDC_STATIC_OVERLAY_LINE_COLOR, m_strLineColor);

	DDX_Control(pDX, IDC_STATIC_OVERLAY_FILL_COLOR, m_staticFillColor);
	DDX_Control(pDX, IDC_STATIC_OVERLAY_LINE_COLOR, m_staticLineColor);
	DDX_Control(pDX, IDC_STATIC_OVERLAY_LINE_THICK, m_staticLineThick);

	DDX_Control(pDX, IDC_CMB_OVERLAY_PROPERTY_LINE_DASH_KIND, m_cmbLineDashStyle);
	DDX_Control(pDX, IDC_CMB_OVERLAY_PROPERTY_LINE_START_KIND, m_cmbLineStartKind);
	DDX_Control(pDX, IDC_CMB_OVERLAY_PROPERTY_LINE_END_KIND, m_cmbLineEndKind);
}


BEGIN_MESSAGE_MAP(CDlgOverlayProperty, CDialogEx)

	
	ON_BN_CLICKED(IDC_RADIO_OVERLAY_FILL, CDlgOverlayProperty::OnBnClickedRadioOverlayFill)
	ON_BN_CLICKED(IDC_RADIO_OVERLAY_LINE, CDlgOverlayProperty::OnBnClickedRadioOverlayLine)
	ON_BN_CLICKED(IDC_RADIO_OVERLAY_NO_FILL, CDlgOverlayProperty::OnBnClickedRadioOverlayNoFill)
	ON_BN_CLICKED(IDC_RADIO_OVERLAY_NO_LINE, CDlgOverlayProperty::OnBnClickedRadioOverlayNoLine)
	ON_WM_SHOWWINDOW()
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_OVERLAY_LINE_BLEND, CDlgOverlayProperty::OnDeltaposSpinOverlayLineBlend)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_OVERLAY_FILL_BLEND, CDlgOverlayProperty::OnDeltaposSpinOverlayFillBlend)	
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_OVERLAY_LINE_THICKNESS, CDlgOverlayProperty::OnDeltaposSpinOverlayLineThickness)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_OVERLAY_FILL_BLEND, CDlgOverlayProperty::OnNMReleasedcaptureSliderOverlayFillBlend)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_OVERLAY_LINE_BLEND, CDlgOverlayProperty::OnNMReleasedcaptureSliderOverlayLineBlend)
	
	ON_EN_CHANGE(IDC_EDT_OVERLAY_FILL_BLEND, CDlgOverlayProperty::OnEnChangeEdtOverlayFillBlend)
	ON_EN_CHANGE(IDC_EDT_OVERLAY_LINE_BLEND, CDlgOverlayProperty::OnEnChangeEdtOverlayLineBlend)
	ON_EN_CHANGE(IDC_EDT_OVERLAY_LINE_THICKNESS, CDlgOverlayProperty::OnEnChangeEdtOverlayThickness)
	ON_BN_CLICKED(IDC_BTN_OVERLAY_PROPERTY_OK, CDlgOverlayProperty::OnBnClickedBtnOverlayPropertyOk)
	ON_BN_CLICKED(IDC_BTN_OVERLAY_PROPERTY_CANCEL, CDlgOverlayProperty::OnBnClickedBtnOverlayPropertyCancel)

	//ON_MESSAGE(CPN_CLOSEUP, CDlgOverlayProperty::OnBnClickedBtnOverlayLineFillColor)

	ON_CBN_SELCHANGE(IDC_CMB_OVERLAY_PROPERTY_LINE_DASH_KIND, OnSelChangeLineDashStyle)
	ON_CBN_SELCHANGE(IDC_CMB_OVERLAY_PROPERTY_LINE_START_KIND, OnSelChangeLineStartStyle)
	ON_CBN_SELCHANGE(IDC_CMB_OVERLAY_PROPERTY_LINE_END_KIND, OnSelChangeLineEndStyle)
	ON_WM_SHOWWINDOW()
	ON_WM_TIMER()
END_MESSAGE_MAP()

BOOL CDlgOverlayProperty::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	UpdateData(FALSE);
	GetDlgItem(IDC_BTN_OVERLAY_FILL_COLOR)->EnableWindow(TRUE); 		
	GetDlgItem(IDC_BTN_OVERLAY_LINE_COLOR)->EnableWindow(TRUE);

	//m_pOverlayLineStyle = new CDlgOverlayLineStyle(this);
	//m_pOverlayLineStyle->Create(CDlgOverlayLineStyle::IDD);

	InitCombo();
	InitSlider();
	InitSpin();	

	//m_btnLineColor.SetParent(this);
	//m_btnFillColor.SetParent(this);

	return TRUE;  
}

void CDlgOverlayProperty::InitCombo()
{
	UINT nIndex = 0;
	for(nIndex=0; nIndex<sizeof(strLineDashStyle)/4; nIndex++)
	{
		m_cmbLineDashStyle.InsertString(nIndex, strLineDashStyle[nIndex]);
		m_cmbLineDashStyle.SetItemData(nIndex, nIndex);
	}

	CString str;
	for(nIndex=0; nIndex<50; nIndex++)
	{
		str.Format(_T("크기%d"),nIndex);		
		m_cmbLineStartKind.InsertString(nIndex, str);
		m_cmbLineEndKind.InsertString(nIndex,str);
		
/*		if(nIndex>3)
		{
			m_cmbLineStartKind.SetItemData(nIndex, nIndex+(0x10-4));		
			m_cmbLineEndKind.SetItemData(nIndex, nIndex+(0x10-4));
		}
		else
		{			
			m_cmbLineStartKind.SetItemData(nIndex, nIndex);		
			m_cmbLineEndKind.SetItemData(nIndex, nIndex);
		}
*/
		m_cmbLineStartKind.SetItemData(nIndex, nIndex);		
		m_cmbLineEndKind.SetItemData(nIndex, nIndex);

	}
	
}

void CDlgOverlayProperty::InitSlider()
{
	m_SliderFillBlend.SetRange(0,0xff);
	m_SliderFillBlend.SetPos(0);
	m_SliderLineBlend.SetRange(0,0xff);
	m_SliderLineBlend.SetPos(0);
}

void CDlgOverlayProperty::InitSpin()
{
	m_SpinFillBlend.SetRange(0,0xff);
	m_SpinFillBlend.SetPos(0);
	m_SpinLineBlend.SetRange(0,0xff);
	m_SpinLineBlend.SetPos(0);
	m_SpinLineThickness.SetRange(0,0xff);
	m_SpinLineThickness.SetPos(0);
}

void CDlgOverlayProperty::ShowWindowProperty(OVERLAY::OBJECT* pObject)
{	
	CDialogEx::ShowWindow(SW_SHOW);

	UpdateData(TRUE);
	if(pObject==NULL) return;
	m_pObject = pObject;
	m_Property = m_pObject->Property;

	COLORREF FillColor = m_Property.FillColor;	
	COLORREF LineColor = m_Property.LineColor;	

	UINT R = GetBValue(FillColor);
	UINT G = GetGValue(FillColor);
	UINT B = GetRValue(FillColor);
	m_strFillColor.Format(_T("%d,%d,%d"), R,G,B);
	FillColor =  R | G<<8 | B<<16;	

	R = GetBValue(LineColor);
	G = GetGValue(LineColor);
	B = GetRValue(LineColor);
	m_strLineColor.Format(_T("%d,%d,%d"), R,G,B);
	LineColor =  R | G<<8 | B<<16;
	
	//m_btnFillColor.SetColor(FillColor);
	//m_btnLineColor.SetColor(LineColor);

	m_SpinFillBlend.SetPos((m_Property.FillColor&0xff000000)/0x1000000);
	m_SpinLineBlend.SetPos((m_Property.LineColor&0xff000000)/0x1000000);

	m_SpinLineThickness.SetPos(m_Property.dLineWidth);

	OVERLAY::OBJECT_TYPE eObjectType = m_pObject->GetType();
	if(	eObjectType!=OVERLAY::LINE &&
		eObjectType!=OVERLAY::LINKED_LINE &&
		eObjectType!=OVERLAY::LINKED_MULTI_LINE &&
		eObjectType!=OVERLAY::LINKED_CURVE &&
		eObjectType!=OVERLAY::LINKED_MULTI_CURVE &&
		eObjectType!=OVERLAY::POLYLINE )
	{
		GetDlgItem(IDC_BTN_OVERLAY_FILL_COLOR)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO_OVERLAY_FILL)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO_OVERLAY_NO_FILL)->EnableWindow(TRUE);
		GetDlgItem(IDC_SPIN_OVERLAY_FILL_BLEND)->EnableWindow(TRUE);
		GetDlgItem(IDC_SLIDER_OVERLAY_FILL_BLEND)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDT_OVERLAY_FILL_BLEND)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO_OVERLAY_NO_LINE)->EnableWindow(TRUE);

		m_bFill = m_Property.bIsFill;

		SetPreviewColor(m_staticLineColor, m_Property.LineColor);
		SetPreviewColor(m_staticFillColor, m_Property.FillColor);
		SetPreviewLine(m_staticLineThick, m_Property);


		if(m_bFill==TRUE)
		{
			OnBnClickedRadioOverlayFill();			
		}
		else
			OnBnClickedRadioOverlayNoFill();
	}
	else
	{
		GetDlgItem(IDC_BTN_OVERLAY_FILL_COLOR)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO_OVERLAY_FILL)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_OVERLAY_NO_FILL)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_OVERLAY_NO_LINE)->EnableWindow(FALSE);
		GetDlgItem(IDC_SPIN_OVERLAY_FILL_BLEND)->EnableWindow(TRUE);
		GetDlgItem(IDC_SLIDER_OVERLAY_FILL_BLEND)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDT_OVERLAY_FILL_BLEND)->EnableWindow(TRUE);
	}
	
	m_bLine = m_Property.bIsLine;
	if(m_bLine==TRUE)
	{
		OnBnClickedRadioOverlayLine();
	}
	else
		OnBnClickedRadioOverlayNoLine();

	UINT nFillBlend = m_Property.FillColor;
	UINT nLineBlend = m_Property.LineColor;

	nFillBlend = m_Property.FillColor/0x01000000;
	nLineBlend = m_Property.LineColor/0x01000000;

	m_SpinFillBlend.SetPos(nFillBlend);
	m_SpinLineBlend.SetPos(nLineBlend);

	m_SliderFillBlend.SetPos(nFillBlend);
	m_SliderLineBlend.SetPos(nLineBlend);

	m_nEdtFillBlend = nFillBlend;
	m_nEdtLineBlend = nLineBlend;

	m_nEdtLineThickness = m_Property.dLineWidth;
	m_SpinLineThickness.SetPos(m_nEdtLineThickness);

	m_cmbLineDashStyle.SetCurSel((UINT)m_Property.eLineStyle);	
/*
	if((UINT)m_Property.eStartLineStyle>3)
	{
		m_cmbLineStartKind.SetCurSel((UINT)m_Property.eStartLineStyle-0x10+4);
	}
	else
	{
		m_cmbLineStartKind.SetCurSel((UINT)m_Property.eStartLineStyle);
	}

	if((UINT)m_Property.eEndLineStyle>3)
	{
		m_cmbLineEndKind.SetCurSel((UINT)m_Property.eEndLineStyle-0x10+4);
	}
	else
	{
		m_cmbLineEndKind.SetCurSel((UINT)m_Property.eEndLineStyle);
	}
*/
	m_cmbLineStartKind.SetCurSel(m_Property.nStartLineArrowStyle);
	m_cmbLineEndKind.SetCurSel(m_Property.nEndLineArrowStyle);

//	SetPreviewColor(m_staticLineColor, m_Property.LineColor);
//	SetPreviewColor(m_staticFillColor, m_Property.FillColor);
	SetPreviewLine(m_staticLineThick, m_Property);

	UpdateData(FALSE);
}

void CDlgOverlayProperty::OnBnClickedRadioOverlayFill()
{
	UpdateData(TRUE);
	if(m_pObject==NULL) return;	
	m_Property.bIsFill = m_bFill;

	if(m_bFill==FALSE)
	{
		GetDlgItem(IDC_BTN_OVERLAY_FILL_COLOR)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDT_OVERLAY_FILL_BLEND)->EnableWindow(FALSE);
		GetDlgItem(IDC_SPIN_OVERLAY_FILL_BLEND)->EnableWindow(FALSE);
		GetDlgItem(IDC_SLIDER_OVERLAY_FILL_BLEND)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDT_OVERLAY_FILL_BLEND)->EnableWindow(FALSE);
	}
	else		
	{
		GetDlgItem(IDC_BTN_OVERLAY_FILL_COLOR)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDT_OVERLAY_FILL_BLEND)->EnableWindow(TRUE);
		GetDlgItem(IDC_SPIN_OVERLAY_FILL_BLEND)->EnableWindow(TRUE);
		GetDlgItem(IDC_SLIDER_OVERLAY_FILL_BLEND)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDT_OVERLAY_FILL_BLEND)->EnableWindow(TRUE);
	}

	UpdateData(FALSE);
}

void CDlgOverlayProperty::OnBnClickedRadioOverlayLine()
{
	//UpdateData(TRUE);
	if(m_pObject==NULL) return;
	m_Property.bIsLine = m_bLine;

	if(m_bLine==FALSE)
	{
		GetDlgItem(IDC_BTN_OVERLAY_LINE_COLOR)->EnableWindow(FALSE);
		GetDlgItem(IDC_BTN_OVERLAY_LINE_COLOR)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDT_OVERLAY_LINE_BLEND)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDT_OVERLAY_LINE_THICKNESS)->EnableWindow(FALSE);
		GetDlgItem(IDC_SPIN_OVERLAY_LINE_BLEND)->EnableWindow(FALSE);
		GetDlgItem(IDC_SPIN_OVERLAY_LINE_THICKNESS)->EnableWindow(FALSE);
		GetDlgItem(IDC_SLIDER_OVERLAY_LINE_BLEND)->EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_BTN_OVERLAY_LINE_COLOR)->EnableWindow(TRUE);
		GetDlgItem(IDC_BTN_OVERLAY_LINE_COLOR)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDT_OVERLAY_LINE_BLEND)->EnableWindow(TRUE);		
		GetDlgItem(IDC_EDT_OVERLAY_LINE_THICKNESS)->EnableWindow(TRUE);
		GetDlgItem(IDC_SPIN_OVERLAY_LINE_BLEND)->EnableWindow(TRUE);
		GetDlgItem(IDC_SPIN_OVERLAY_LINE_THICKNESS)->EnableWindow(TRUE);
		GetDlgItem(IDC_SLIDER_OVERLAY_LINE_BLEND)->EnableWindow(TRUE);
	}

	UpdateData(FALSE);
}

void CDlgOverlayProperty::OnBnClickedRadioOverlayNoFill()
{
	UpdateData(TRUE);
	if(m_pObject==NULL) return;
	m_Property.bIsFill = m_bFill;

	if(m_bFill==FALSE)
	{
		GetDlgItem(IDC_BTN_OVERLAY_FILL_COLOR)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDT_OVERLAY_FILL_BLEND)->EnableWindow(FALSE);
		GetDlgItem(IDC_SPIN_OVERLAY_FILL_BLEND)->EnableWindow(FALSE);
		GetDlgItem(IDC_SLIDER_OVERLAY_FILL_BLEND)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDT_OVERLAY_FILL_BLEND)->EnableWindow(FALSE);
	}
	else		
	{
		GetDlgItem(IDC_BTN_OVERLAY_FILL_COLOR)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDT_OVERLAY_FILL_BLEND)->EnableWindow(TRUE);
		GetDlgItem(IDC_SPIN_OVERLAY_FILL_BLEND)->EnableWindow(TRUE);
		GetDlgItem(IDC_SLIDER_OVERLAY_FILL_BLEND)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDT_OVERLAY_FILL_BLEND)->EnableWindow(TRUE);
	}

	UpdateData(FALSE);
}

void CDlgOverlayProperty::OnBnClickedRadioOverlayNoLine()
{
	UpdateData(TRUE);
	if(m_pObject==NULL) return;
	m_Property.bIsLine = m_bLine;

	if(m_bLine==FALSE)
	{
		GetDlgItem(IDC_BTN_OVERLAY_LINE_COLOR)->EnableWindow(FALSE);
		GetDlgItem(IDC_BTN_OVERLAY_LINE_COLOR)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDT_OVERLAY_LINE_BLEND)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDT_OVERLAY_LINE_THICKNESS)->EnableWindow(FALSE);
		GetDlgItem(IDC_SPIN_OVERLAY_LINE_BLEND)->EnableWindow(FALSE);
		GetDlgItem(IDC_SLIDER_OVERLAY_LINE_BLEND)->EnableWindow(FALSE);
		GetDlgItem(IDC_SPIN_OVERLAY_LINE_THICKNESS)->EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_BTN_OVERLAY_LINE_COLOR)->EnableWindow(TRUE);
		GetDlgItem(IDC_BTN_OVERLAY_LINE_COLOR)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDT_OVERLAY_LINE_BLEND)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDT_OVERLAY_LINE_THICKNESS)->EnableWindow(TRUE);
		GetDlgItem(IDC_SPIN_OVERLAY_LINE_BLEND)->EnableWindow(TRUE);
		GetDlgItem(IDC_SLIDER_OVERLAY_LINE_BLEND)->EnableWindow(TRUE);
		GetDlgItem(IDC_SPIN_OVERLAY_LINE_THICKNESS)->EnableWindow(TRUE);
	}

	UpdateData(FALSE);
}
/*
void CDlgOverlayProperty::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	if( bShow==FALSE)
	{
		m_pObject = NULL;
	}
}
*/
void CDlgOverlayProperty::OnNMReleasedcaptureSliderOverlayFillBlend(NMHDR *pNMHDR, LRESULT *pResult)
{
	UpdateData(TRUE);

	UINT nFillBlend = m_SliderFillBlend.GetPos();	

	m_SpinFillBlend.SetPos(nFillBlend);
	m_nEdtFillBlend = nFillBlend;
	m_Property.FillColor = (COLORREF)((nFillBlend*0x01000000) + (m_Property.FillColor&0x00ffffff));

	SetPreviewColor(m_staticFillColor, m_Property.FillColor);
	UpdateData(FALSE);
	*pResult = 0;
}

void CDlgOverlayProperty::OnNMReleasedcaptureSliderOverlayLineBlend(NMHDR *pNMHDR, LRESULT *pResult)
{
	UpdateData(TRUE);

	UINT nLineBlend = m_SliderLineBlend.GetPos();	
	m_SpinLineBlend.SetPos(nLineBlend);
	m_nEdtLineBlend = nLineBlend;
	m_Property.LineColor = (COLORREF)((nLineBlend*0x01000000) + (m_Property.LineColor&0x00ffffff));

	SetPreviewColor(m_staticLineColor, m_Property.LineColor);

	UpdateData(FALSE);
	*pResult = 0;
}

void CDlgOverlayProperty::OnDeltaposSpinOverlayLineBlend(NMHDR *pNMHDR, LRESULT *pResult)
{
	UpdateData(TRUE);

	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);		

	if(pNMUpDown->iDelta>0)
		pNMUpDown->iPos++;
	else if(pNMUpDown->iDelta<0)
		pNMUpDown->iPos--;

	if(pNMUpDown->iPos<0)
		pNMUpDown->iPos = 0;
	else if(pNMUpDown->iPos>255)
		pNMUpDown->iPos = 255;

	UINT nLineBlend = pNMUpDown->iPos;

	m_SliderLineBlend.SetPos(nLineBlend);
	m_nEdtLineBlend = nLineBlend;
	m_Property.LineColor = (COLORREF)((nLineBlend*0x01000000) + (m_Property.LineColor&0x00ffffff));

	SetPreviewColor(m_staticLineColor, m_Property.LineColor);

	UpdateData(FALSE);
	*pResult = 0;
}

void CDlgOverlayProperty::OnDeltaposSpinOverlayFillBlend(NMHDR *pNMHDR, LRESULT *pResult)
{
	UpdateData(TRUE);

	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);		
	
	if(pNMUpDown->iDelta>0)
		pNMUpDown->iPos++;
	else if(pNMUpDown->iDelta<0)
		pNMUpDown->iPos--;

	if(pNMUpDown->iPos<0)
		pNMUpDown->iPos = 0;
	else if(pNMUpDown->iPos>255)
		pNMUpDown->iPos = 255;

	UINT nFillBlend = pNMUpDown->iPos;

	m_SliderFillBlend.SetPos(nFillBlend);
	m_nEdtFillBlend = nFillBlend;
	m_Property.FillColor = (COLORREF)((nFillBlend*0x01000000) + (m_Property.FillColor&0x00ffffff));

	SetPreviewColor(m_staticFillColor, m_Property.FillColor);
	
	UpdateData(FALSE);
	*pResult = 0;
}

void CDlgOverlayProperty::OnDeltaposSpinOverlayLineThickness(NMHDR *pNMHDR, LRESULT *pResult)
{
	UpdateData(TRUE);
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	
	if(pNMUpDown->iDelta>0)
		pNMUpDown->iPos++;
	else if(pNMUpDown->iDelta<0)
		pNMUpDown->iPos--;

	if(pNMUpDown->iPos<0)
		pNMUpDown->iPos = 0;
	else if(pNMUpDown->iPos>255)
		pNMUpDown->iPos = 255;

	UINT nLineThickness = pNMUpDown->iPos;

	m_nEdtLineThickness = nLineThickness;
	m_Property.dLineWidth = (double)nLineThickness;

	UpdateData(FALSE);
	
	SetPreviewLine(m_staticLineThick, m_Property);
	*pResult = 0;
}

void CDlgOverlayProperty::OnEnChangeEdtOverlayFillBlend()
{
	UpdateData(TRUE);	
	if(m_pObject==NULL) return;

	if(m_nEdtFillBlend>255)
		m_nEdtFillBlend = 255;

	m_Property.FillColor = (m_nEdtFillBlend*0x01000000)|(m_Property.FillColor&0xffffff);

	m_SliderFillBlend.SetPos(m_nEdtFillBlend);
	m_SpinFillBlend.SetPos(m_nEdtFillBlend);

	SetPreviewColor(m_staticFillColor, m_Property.FillColor);
	
	UpdateData(FALSE);
}

void CDlgOverlayProperty::OnEnChangeEdtOverlayLineBlend()
{
	UpdateData(TRUE);
	if(m_pObject==NULL) return;

	if(m_nEdtLineBlend>255)
		m_nEdtLineBlend = 255;

	m_Property.LineColor = (m_nEdtLineBlend*0x01000000)|(m_Property.LineColor&0xffffff);
	m_SliderLineBlend.SetPos(m_nEdtLineBlend);
	m_SpinLineBlend.SetPos(m_nEdtLineBlend);
	UpdateData(FALSE);

	SetPreviewColor(m_staticLineColor, m_Property.LineColor);
}

void CDlgOverlayProperty::OnEnChangeEdtOverlayThickness()
{
	UpdateData(TRUE);
	if(m_pObject==NULL) return;

	m_Property.dLineWidth = m_nEdtLineThickness;
	m_SpinLineThickness.SetPos(m_nEdtLineThickness);

	SetPreviewLine(m_staticLineThick, m_Property);
	UpdateData(FALSE);
}

void CDlgOverlayProperty::OnBnClickedBtnOverlayPropertyOk()
{
	UpdateData(TRUE);

	if(m_pObject==NULL) return;	

	UpdateData(FALSE);
	memcpy(&m_pObject->Property ,&m_Property,sizeof(m_Property));
	if(m_pMainView!=NULL)
//		m_pMainView->DrawSituation(FALSE,FALSE,FALSE, TRUE);

	if(m_pOverlayView!=NULL)
	{
		m_pOverlayView->ReleaseSelectObject();
		m_pOverlayView->DrawEditlayer();
	}
	
	CDialogEx::OnOK();
}

void CDlgOverlayProperty::OnBnClickedBtnOverlayPropertyCancel()
{
	CDialogEx::OnCancel();
}

LRESULT CDlgOverlayProperty::OnBnClickedBtnOverlayLineFillColor(WPARAM DlgCtrlID, LPARAM lParam)
{
	COLORREF Color =0;
	UINT R =0;
	UINT G =0;
	UINT B =0;
/*
	if(DlgCtrlID==IDC_BTN_OVERLAY_FILL_COLOR)
	{
		Color = m_btnFillColor.GetColor();	

		R = GetRValue(Color);
		G = GetGValue(Color);
		B = GetBValue(Color);
		m_strFillColor.Format(_T("%d,%d,%d"), R,G,B);
		m_Property.FillColor = (m_Property.FillColor& 0xff000000) | R<<16 | G<<8 | B;

		SetPreviewColor(m_staticFillColor, m_Property.FillColor);
	}
	else if(DlgCtrlID==IDC_BTN_OVERLAY_LINE_COLOR)
	{
		Color = m_btnLineColor.GetColor();	

		R = GetRValue(Color);
		G = GetGValue(Color);
		B = GetBValue(Color);
		m_strLineColor.Format(_T("%d,%d,%d"), R,G,B);
		m_Property.LineColor = (m_Property.LineColor& 0xff000000) | R<<16 | G<<8 | B;

		SetPreviewColor(m_staticLineColor, m_Property.LineColor);
		SetPreviewLine(m_staticLineThick,m_Property);
	}
*/
	return 0L;
}

void CDlgOverlayProperty::OnSelChangeLineDashStyle()
{	//Line Style
	m_Property.eLineStyle = (OVERLAY::LINE_STYLE)m_cmbLineDashStyle.GetItemData(m_cmbLineDashStyle.GetCurSel());
	SetPreviewLine(m_staticLineThick,m_Property);
}

void CDlgOverlayProperty::OnSelChangeLineStartStyle()
{
	//Line Cap Style
	//m_Property.eStartLineStyle = (OVERLAY_OBJECTS::LineCap)m_cmbLineStartKind.GetItemData(m_cmbLineStartKind.GetCurSel());	
	m_Property.nStartLineArrowStyle = (OVERLAY::LineCap)m_cmbLineStartKind.GetItemData(m_cmbLineStartKind.GetCurSel());
	SetPreviewLine(m_staticLineThick,m_Property);
}

void CDlgOverlayProperty::OnSelChangeLineEndStyle()
{
	//Line Cap Style
	//m_Property.eEndLineStyle = (OVERLAY_OBJECTS::LineCap)m_cmbLineEndKind.GetItemData(m_cmbLineEndKind.GetCurSel());
	m_Property.nEndLineArrowStyle = (OVERLAY::LineCap)m_cmbLineEndKind.GetItemData(m_cmbLineEndKind.GetCurSel());
	SetPreviewLine(m_staticLineThick,m_Property);
}

void CDlgOverlayProperty::SetPreviewColor(CStatic& staticWindow, COLORREF color)
{
	CDC* pDC = staticWindow.GetDC();
	if(pDC==NULL)
		return;
	if(pDC->GetSafeHdc()==NULL)
		return;

	CRect rect;
	staticWindow.GetClientRect(&rect);

	Gdiplus::Graphics g(pDC->GetSafeHdc());
	Gdiplus::PointF		pt[4];

	pt[0].X = 0;
	pt[0].Y = 0;
	pt[1].X = rect.right;
	pt[1].Y = 0;
	pt[2].X = rect.right;
	pt[2].Y = rect.bottom;
	pt[3].X = 0;
	pt[3].Y = rect.bottom;

	Gdiplus::SolidBrush	brush(0xffffffff);
	g.FillPolygon(&brush,pt,4);
	brush.SetColor(color);
	g.FillPolygon(&brush,pt,4);	
}

void CDlgOverlayProperty::SetPreviewLine(CStatic& staticWindow, OVERLAY::ObjectProperty& Property)
{
	CDC* pDC = staticWindow.GetDC();
	if(pDC==NULL)
		return;
	if(pDC->GetSafeHdc()==NULL)
		return;
	
	CRect rect;
	staticWindow.GetClientRect(&rect);	
	Gdiplus::Graphics g(pDC->GetSafeHdc());
	Gdiplus::Pen ppen(Gdiplus::Color(0xff,0xff,0xff,0xff));	

	/////////////////////////////////
	Gdiplus::PointF		pt[4];
	Gdiplus::SolidBrush	brush(0xffffffff);

	pt[0].X = 0;
	pt[0].Y = 0;
	pt[1].X = rect.right;
	pt[1].Y = 0;
	pt[2].X = rect.right;
	pt[2].Y = rect.bottom;
	pt[3].X = 0;
	pt[3].Y = rect.bottom;	
	
	g.FillPolygon(&brush,pt,4);
	/////////////////////////////////

	ppen.SetWidth(Property.dLineWidth);
	ppen.SetDashStyle((Gdiplus::DashStyle)Property.eLineStyle);
/*	ppen.SetLineCap(
		(Gdiplus::LineCap)Property.eStartLineStyle,
		(Gdiplus::LineCap)Property.eEndLineStyle,
		(Gdiplus::DashCap)Property.eDashStyle
		);*/

	///////////////////////////////////////////////////////////
	// 도형(선) 화살표 설정.
	AdjustableArrowCap StartArrow(0, 0, true);
	AdjustableArrowCap EndArrow(0, 0, true);
	
	StartArrow.SetHeight(Property.nStartLineArrowStyle);
	StartArrow.SetWidth(Property.nStartLineArrowStyle);	

	EndArrow.SetHeight(Property.nEndLineArrowStyle);
	EndArrow.SetWidth(Property.nEndLineArrowStyle);
	
	ppen.SetCustomStartCap(&StartArrow);
	ppen.SetCustomEndCap(&EndArrow);
	///////////////////////////////////////////////////////////

	g.DrawLine(&ppen,	(INT)Property.dLineWidth, (INT)((rect.bottom)/2),	(INT)rect.right-Property.dLineWidth, (INT)((rect.bottom)/2));
	ppen.SetColor(Property.LineColor);	
	g.DrawLine(&ppen,	(INT)Property.dLineWidth, (INT)((rect.bottom)/2),	(INT)rect.right-Property.dLineWidth, (INT)((rect.bottom)/2));	
}
void CDlgOverlayProperty::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);
// 	UpdateData(TRUE);
// 	SetPreviewColor(m_staticLineColor, m_Property.LineColor);
// 	SetPreviewColor(m_staticFillColor, m_Property.FillColor);
// 	SetPreviewLine(m_staticLineThick, m_Property);
// 	UpdateData(FALSE);
	SetTimer(TIMER_SHOW_PREVIEW,100,NULL);
}

void CDlgOverlayProperty::OnTimer(UINT_PTR nIDEvent)
{
	//TRACE("%d\n", 777);
	// TODO: Add your message handler code here and/or call default
	if(nIDEvent == TIMER_SHOW_PREVIEW){
		SetPreviewColor(m_staticLineColor, m_Property.LineColor);
		SetPreviewColor(m_staticFillColor, m_Property.FillColor);
		SetPreviewLine(m_staticLineThick, m_Property);
		KillTimer(TIMER_SHOW_PREVIEW);
	}
	
	CDialogEx::OnTimer(nIDEvent);
}

