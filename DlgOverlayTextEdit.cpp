// DlgOverlayTextEdit.cpp : implementation file
//

#include "pch.h"
#include "C4IMap.h"
#include "../LIBSrc/Overlay/ObjectText.h"
#include "DlgOverlayTextEdit.h"
#include "OverlayView.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgOverlayTextEdit dialog

IMPLEMENT_DYNAMIC(CDlgOverlayTextEdit, CDialogEx)

CDlgOverlayTextEdit::CDlgOverlayTextEdit(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgOverlayTextEdit::IDD, pParent)
	, m_strEdit(_T(""))
{
	m_pText = NULL;
	m_pOverlayView = NULL;
	m_TextFontBlend = 0;	
	m_OldFontSize =0;
	m_NewFontSize=0;
	m_bCheckUnderLine= FALSE;
	m_bCheckStrikeOut =FALSE;
}


void CDlgOverlayTextEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgOverlayTextEdit)
		DDX_Text(pDX, IDC_EDT_OVERLAY_TEXT_EDIT, m_strEdit);
		DDX_Text(pDX, IDC_STATIC_OVERLAY_TEXT_BLEND, m_strTextBlend);
		//DDX_Control(pDX, IDC_BTN_OVERLAY_TEXT_COLOR, m_btnObjectTextColor);
		DDX_Control(pDX, IDC_CMB_OVERLAY_TEXT_FONT_SIZE, m_cmbObjectTextFontSize);
		DDX_Control(pDX, IDC_CMB_OVERLAY_TEXT_FONT_STYLE, m_cmbObjectTextFontStyle);
		DDX_Control(pDX, IDC_CMB_OVERLAY_TEXT_FONT_STYLE2, m_cmbObjectTextFontStyle2);
		DDX_Control(pDX, IDC_CMB_OVERLAY_TEXT_FONT, m_cmbObjectTextFont);
		DDX_Control(pDX, IDC_SLIDER_OVERLAY_TEXT_TRANSPARENT, m_SliderTextBlend);
		DDX_Control(pDX, IDC_STATIC_OVERLAY_EDIT_TEXT_PREVIEW, m_staticPreview);
		DDX_Check(pDX,IDC_CHECK_UNDERLINE,m_bCheckUnderLine);
		DDX_Check(pDX,IDC_CHECK_STRIKEOUT,m_bCheckStrikeOut);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgOverlayTextEdit, CDialogEx)
	//{{AFX_MSG_MAP(CDlgOverlayTextEdit)	
	ON_BN_CLICKED(IDC_BTN_OVERLAY_TEXT_OK, OnBtnOverlayTextOk)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_OVERLAY_TEXT_TRANSPARENT, CDlgOverlayTextEdit::OnNMReleasedcaptureSliderOverlayTextBlend)	
	ON_CBN_SELCHANGE(IDC_CMB_OVERLAY_TEXT_FONT_SIZE, CDlgOverlayTextEdit::OnSelChangeOverlayFontColorFontSize) 
	ON_CBN_SELCHANGE(IDC_CMB_OVERLAY_TEXT_FONT_STYLE, CDlgOverlayTextEdit::OnSelChangeOverlayFontColorFontStyle)
	ON_CBN_SELCHANGE(IDC_CMB_OVERLAY_TEXT_FONT_STYLE2, CDlgOverlayTextEdit::OnSelChangeOverlayFontColorFontStyle2)
	ON_CBN_SELCHANGE(IDC_CMB_OVERLAY_TEXT_FONT, CDlgOverlayTextEdit::OnSelChangeOverlayFont)
	ON_CBN_EDITCHANGE(IDC_CMB_OVERLAY_TEXT_FONT_SIZE, CDlgOverlayTextEdit::OnSelEditChangeOverlayFontSize)
	ON_BN_CLICKED(IDC_CHECK_UNDERLINE,OnCheckTextUnderLine)
	ON_BN_CLICKED(IDC_CHECK_STRIKEOUT,OnCheckTextStrikeOut)
	ON_MESSAGE(CPN_CLOSEUP, CDlgOverlayTextEdit::OnBnClickedBtnOverlayFontColor)
	ON_WM_SHOWWINDOW()
	ON_WM_TIMER()

	
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgOverlayTextEdit message handlers
BOOL CDlgOverlayTextEdit::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_SliderTextBlend.SetRange(0,0xff);
	m_SliderTextBlend.SetPos(1);

	int i=0;
	char strTmmp[128];
	memset(strTmmp,0,sizeof(strTmmp));
	
	for(;i<MAX_FONT_FAMILY_NAME;i++)
	{
		WideCharToMultiByte(
							CP_ACP,
							0,
							OVERLAY::strFamilyName[i],
							wcslen(OVERLAY::strFamilyName[i]),
							strTmmp,
							sizeof(strTmmp),
							NULL,
							NULL
							);

		m_cmbObjectTextFont.InsertString(i,strTmmp);
		m_cmbObjectTextFont.SetItemData(i,i);
	}	
	m_cmbObjectTextFont.SetCurSel(0);

	i=0;
	m_cmbObjectTextFontStyle.InsertString(i, "º¸Åë");	
	m_cmbObjectTextFontStyle.SetItemData(i++,Gdiplus::FontStyleRegular);
	m_cmbObjectTextFontStyle.InsertString(i, "±½°Ô");	
	m_cmbObjectTextFontStyle.SetItemData(i++,Gdiplus::FontStyleBold);
	m_cmbObjectTextFontStyle.InsertString(i, "ÀÌÅÅ¸¯Ã¼");	
	m_cmbObjectTextFontStyle.SetItemData(i++,Gdiplus::FontStyleItalic);
	m_cmbObjectTextFontStyle.InsertString(i, "±½Àº ÀÌÅÅ¸¯");	
	m_cmbObjectTextFontStyle.SetItemData(i++,Gdiplus::FontStyleBoldItalic);
	m_cmbObjectTextFontStyle.SetCurSel(0);

	i=0;
	m_cmbObjectTextFontStyle2.InsertString(i, "---------");
	m_cmbObjectTextFontStyle2.SetItemData(i++,0);
	m_cmbObjectTextFontStyle2.InsertString(i, "¹ØÁÙ");
	m_cmbObjectTextFontStyle2.SetItemData(i++,Gdiplus::FontStyleUnderline);
	m_cmbObjectTextFontStyle2.InsertString(i, "Á¦°Å");
	m_cmbObjectTextFontStyle2.SetItemData(i++,Gdiplus::FontStyleStrikeout);
	m_cmbObjectTextFontStyle2.SetCurSel(0);

	CString str;
	char strTmp[128];
	memset(strTmp, 0 ,sizeof(strTmp));
	i=0;
	for(int j=9; j<73;)
	{
		str.Format(_T("%s"),_itoa(j, strTmp, 10));
		m_cmbObjectTextFontSize.InsertString(i,str);
		m_cmbObjectTextFontSize.SetItemData(i,j);
		i++;
		j+=3+i;
	}

	return TRUE;  
}

void CDlgOverlayTextEdit::BindOverlayView(COverlayView* pOverlayView)
{
	m_pOverlayView = pOverlayView;
}

void CDlgOverlayTextEdit::OnBtnOverlayTextOk() 
{
	UpdateData(TRUE);	
	WCHAR* pText = m_pText->GetText();
	memset(pText,0,m_pText->GetSize());

	if(m_strEdit.GetLength()>MAX_TEXT_STRING)
	{
		m_strEdit.GetBufferSetLength(MAX_TEXT_STRING-1);
	}

	MultiByteToWideChar(CP_ACP, 0, m_strEdit.GetBuffer(0), m_strEdit.GetLength(), m_pText->GetText(), m_strEdit.GetLength());
	
	UpdateData(FALSE);

	if(m_pOverlayView!=NULL)
	{
		m_pOverlayView->ReleaseSelectObject();
		m_pOverlayView->DrawEditlayer();
	}

	CDialogEx::OnOK();
}

void CDlgOverlayTextEdit::OnNMReleasedcaptureSliderOverlayTextBlend(NMHDR *pNMHDR, LRESULT *pResult)
{
	if(m_pText==NULL)
		return;

	UpdateData(TRUE);	

	UINT nFillBlend = m_SliderTextBlend.GetPos();
	m_strTextBlend.Format(_T("%d"),nFillBlend);
	m_TextFontBlend = (COLORREF)(nFillBlend*0x01000000);
	UpdateData(FALSE);
	*pResult = 0;
	
	COLORREF temColor;
	temColor = m_TextFontBlend+(m_pText->GetFontColor()&0x00ffffff);
	m_pText->SetColor(temColor);

	PreviewTextStyle(m_staticPreview);
}

void CDlgOverlayTextEdit::ShowWindowProperty(OVERLAY::OBJECT* pObject) 
{	
	CDialogEx::ShowWindow(SW_SHOW);
	if(pObject==NULL)
	{
		CDialogEx::ShowWindow(SW_HIDE);
		return;
	}	

	char *pText;//[1024];
	pText = new char[1024];
	//memset(pText, 0, sizeof(pText));

	m_pText = (OVERLAY::ObjectText*)pObject;
	if(m_pText == NULL)
		return;

	//m_strEdit = (CString)m_pText->GetText();
	WCHAR* KKK = new WCHAR;
	KKK = m_pText->GetText();
	USES_CONVERSION;
		pText = W2A(KKK);
	//pText = m_pText->GetText();
	//WCHAR* KKK = m_pText->GetText();
	//int KKK2 = m_pText->GetSize();
	//int KLL =sizeof(pText);
	//WideCharToMultiByte(
	//	CP_ACP,
	//	0,
	//	KKK,
	//	KKK2,
	//	pText,
	//	KLL,
	//	NULL,
	//	NULL
	//	);//°¡²û¾¿ Á×´Â Çö»ó È®ÀÎ¿ä¸Á 
	if(m_pText!=NULL)
		m_strEdit.Format(_T("%s"),pText);

// 	m_bCheckUnderLine= FALSE;
// 	m_bCheckStrikeOut =FALSE;
	COLORREF FontColor = m_pText->GetFontColor();
	m_TextFontBlend = FontColor & 0xff000000;

	FontColor = ((m_pText->GetFontColor()&0xff0000)/0x10000) + (m_pText->GetFontColor()&0xff00) + ((m_pText->GetFontColor()&0xff)*0x10000);
	//FontColor = 0x32CD32  //Å×½ºÆ®¿ë ;
	//m_btnObjectTextColor.SetColor(FontColor);
	m_SliderTextBlend.SetPos(m_TextFontBlend/0x1000000);
	
	m_cmbObjectTextFont.SetCurSel((UINT)m_pText->GetFamilyName());
	m_cmbObjectTextFontStyle.SetCurSel((UINT)((UINT)m_pText->GetFontStyle()&0x03));
	m_cmbObjectTextFontStyle2.SetCurSel((UINT)((m_pText->GetFontStyle()&0x8)>>1) );
	
	CString strTextSize;
	strTextSize.Format(_T("%f"),m_pText->GetFontSize());
	//m_cmbObjectTextFontSize.SetWindowText(strTextSize);
	//m_cmbObjectTextFontSize.SetWindowText("0");
	m_strTextBlend.Format(_T("%d"),m_TextFontBlend/0x1000000);

	PreviewTextStyle(m_staticPreview);

	if(m_pText->GetFontStyle() == Gdiplus::FontStyleUnderline){
		//UpdateData(TRUE);
		m_bCheckUnderLine= TRUE;
		m_bCheckStrikeOut =FALSE;
		//UpdateData(FALSE);
	}
	else if(m_pText->GetFontStyle() == Gdiplus::FontStyleStrikeout){
		m_bCheckUnderLine= FALSE;
		m_bCheckStrikeOut =TRUE;
	}
	else{
		m_bCheckUnderLine= FALSE;
		m_bCheckStrikeOut =FALSE;
	}

	UpdateData(FALSE);	
}

void CDlgOverlayTextEdit::PreviewTextStyle(CStatic& staticWindow)
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

	//------------------------------------------------
	WCHAR PreviewText[]=L"°¡Aa12";
	
	m_pText->SetSize((double)abs(m_pText->pt[1].Y() - m_pText->pt[0].Y())*FONT_RESOLUTION);

	Gdiplus::Font genericMonoFont(
		OVERLAY::strFamilyName[(UINT)m_pText->GetFamilyName()],
		(Gdiplus::REAL)m_pText->GetFontSize(),
		m_pText->GetFontStyle(),
		m_pText->GetUnit()
		);

	Gdiplus::SolidBrush solidbrush(m_pText->GetFontColor());
	UINT nStrLength = wcslen(PreviewText);// ¼öÁ¤ ÇÊ¿ä.
	
	g.DrawString(
			PreviewText, 
			nStrLength,
			&genericMonoFont, 
			Gdiplus::PointF(0.0,0.0), 
			&solidbrush
		);
}

void CDlgOverlayTextEdit::OnSelChangeOverlayFontColorFontSize()
{
	UpdateData(TRUE);	

//	char strSize[11];
//	memset(strSize,0,sizeof(strSize));
	int nSize = m_cmbObjectTextFontSize.GetItemData(m_cmbObjectTextFontSize.GetCurSel());//.GetWindowText(strSize,10);
	m_pText->SetSize(nSize);

	UpdateData(FALSE);
	PreviewTextStyle(m_staticPreview);
}

void CDlgOverlayTextEdit::OnSelChangeOverlayFontColorFontStyle()
{
	m_pText->SetFontStyle((Gdiplus::FontStyle)(m_cmbObjectTextFontStyle2.GetItemData(m_cmbObjectTextFontStyle2.GetCurSel()) | m_cmbObjectTextFontStyle.GetItemData(m_cmbObjectTextFontStyle.GetCurSel())) );
	PreviewTextStyle(m_staticPreview);
}

void CDlgOverlayTextEdit::OnSelChangeOverlayFontColorFontStyle2()
{
	m_pText->SetFontStyle((Gdiplus::FontStyle)(m_cmbObjectTextFontStyle2.GetItemData(m_cmbObjectTextFontStyle2.GetCurSel()) | m_cmbObjectTextFontStyle.GetItemData(m_cmbObjectTextFontStyle.GetCurSel())) );
	PreviewTextStyle(m_staticPreview);
}

void CDlgOverlayTextEdit::OnSelChangeOverlayFont()
{
	m_pText->SetFontFamilyName((OVERLAY::E_FAMILY_NAME_)(m_cmbObjectTextFont.GetItemData(m_cmbObjectTextFont.GetCurSel())));
	PreviewTextStyle(m_staticPreview);
}

LRESULT CDlgOverlayTextEdit::OnBnClickedBtnOverlayFontColor(WPARAM DlgCtrlID, LPARAM lParam)
{
	COLORREF Color =0;
	UINT R =0;
	UINT G =0;
	UINT B =0;
	if(DlgCtrlID==IDC_BTN_OVERLAY_TEXT_COLOR)
	{
		//Color = m_btnObjectTextColor.GetColor();//50 205 50	

		//R = GetRValue(Color);
		//G = GetGValue(Color);
		//B = GetBValue(Color);		

		m_pText->SetColor((m_pText->GetFontColor()& 0xff000000) | R<<16 | G<<8 | B);		
		PreviewTextStyle(m_staticPreview);
	}	
	return 0L;
}

void CDlgOverlayTextEdit::OnSelEditChangeOverlayFontSize()
{
	UpdateData(TRUE);
	char strSize[11];
	memset(strSize,0,sizeof(strSize));
	m_cmbObjectTextFontSize.GetWindowText(strSize,10);
	int nSize = atoi(strSize);
	m_pText->SetSize(nSize);

	PreviewTextStyle(m_staticPreview);
}
void CDlgOverlayTextEdit::OnCheckTextUnderLine()
{
	m_bCheckStrikeOut = FALSE;
	if(!m_bCheckUnderLine){
		m_pText->SetFontStyle(Gdiplus::FontStyleUnderline);
		PreviewTextStyle(m_staticPreview);
		m_bCheckUnderLine = TRUE;
	}
	else{
		m_pText->SetFontStyle(Gdiplus::FontStyleRegular);
		PreviewTextStyle(m_staticPreview);
		m_bCheckUnderLine = FALSE;
	}
	UpdateData(FALSE);


}
void CDlgOverlayTextEdit::OnCheckTextStrikeOut()
{
	m_bCheckUnderLine = FALSE;
	if(!m_bCheckStrikeOut){
		m_pText->SetFontStyle(Gdiplus::FontStyleStrikeout);
		PreviewTextStyle(m_staticPreview);
		m_bCheckStrikeOut = TRUE;
	}
	else{
		m_pText->SetFontStyle(Gdiplus::FontStyleRegular);
		PreviewTextStyle(m_staticPreview);
		m_bCheckStrikeOut = FALSE;
	}
	UpdateData(FALSE);

}
void CDlgOverlayTextEdit::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);
	
	SetTimer(TIMER_SHOW_PREVIEW_TEXT,100,NULL);
}

void CDlgOverlayTextEdit::OnTimer(UINT_PTR nIDEvent)
{
	//TRACE("%d\n", 888);
	// TODO: Add your message handler code here and/or call default
	if(nIDEvent == TIMER_SHOW_PREVIEW_TEXT){
		PreviewTextStyle(m_staticPreview);
		KillTimer(TIMER_SHOW_PREVIEW_TEXT);
	}
	
	CDialogEx::OnTimer(nIDEvent);
}
