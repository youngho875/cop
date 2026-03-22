// CDlgOverlayControler.cpp: 구현 파일
//

#include "pch.h"
#include "C4IMap.h"
#include "C4IMapView.h"
#include "DlgOverlayControler.h"
#include "afxdialogex.h"
#include "OverlayMain.h"
#include "ObjectThread.h"

#include "DlgObjectList.h"

//#include "FilePath.h"


#define DEFAULT_COLUMN_HEIGHT		20

// CDlgOverlayControler 대화 상자

IMPLEMENT_DYNAMIC(CDlgOverlayControler, CDialogEx)

CDlgOverlayControler::CDlgOverlayControler(CWnd* pParent /*=nullptr*/)
	: CDialogEx(CDlgOverlayControler::IDD, pParent)
	, m_strLayerNameForAdd(_T(""))
	, m_strSelectedLayerName(_T(""))
	, m_pMainOverlay(NULL)
	, m_pDlgObjectList(NULL)
	, m_nSelectedItemIndex(0)
{
	m_bSymbolDraw = FALSE;
	//m_piEditTool = NULL;
	//m_piMapProj = NULL;
	m_pOverlayThread = NULL;
	m_nGridDetail = 0;
	m_pMainView = pParent;
	//m_pEditOwner = new CMapViewViewEditOwner(this);
	m_bIsGrid = TRUE;

	m_bObjectEllipse = FALSE;
	m_bObjectArc = FALSE;
	m_bObjectPie = FALSE;
	m_bObjectLine = FALSE;
	m_bObjectPolyLine = FALSE;
	m_bObjectCurve = FALSE;
	m_bObjectPolygon = FALSE;
	m_bObjectLinkedLine = FALSE;
	m_bObjectMultiLine = FALSE;
	m_bObjectLinkedCurve = FALSE;
	m_bObjectMultiCurve = FALSE;
	m_bObjectText = FALSE;
	m_bObjectDiamond = FALSE;
	m_bObjectPicture = FALSE;
	m_bObjectRoundRect = FALSE;
	m_bObjectTriangle = FALSE;
	m_bObjectRect = FALSE;
	m_bObjectPentagon = FALSE;
	m_bObjectSixangle = FALSE;
	m_bObjectEightangle = FALSE;
	m_bObjectGrid = FALSE;
	m_bOverlayLayer = FALSE;
}

CDlgOverlayControler::~CDlgOverlayControler()
{
}

void CDlgOverlayControler::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BTN_OBJECT_ELLIPSE, m_btnObjectEllipse);
	DDX_Control(pDX, IDC_BTN_OBJECT_ARC, m_btnObjectArc);
	DDX_Control(pDX, IDC_BTN_OBJECT_PIE, m_btnObjectPie);
	DDX_Control(pDX, IDC_BTN_OBJECT_LINE, m_btnObjectLine);
	DDX_Control(pDX, IDC_BTN_OBJECT_POLYLINE, m_btnObjectPolyLine);
	DDX_Control(pDX, IDC_BTN_OBJECT_CURVE, m_btnObjectCurve);
	DDX_Control(pDX, IDC_BTN_OBJECT_POLYGON, m_btnObjectPolygon);
	DDX_Control(pDX, IDC_BTN_OBJECT_LINKED_LINE, m_btnObjectLinkedLine);
	DDX_Control(pDX, IDC_BTN_OBJECT_LINKED_MULTI_LINE, m_btnObjectMultiLine);
	//DDX_Control(pDX, IDC_BTN_OBJECT_LINKED_CURVE, m_btnObjectLinkedCurve);
	DDX_Control(pDX, IDC_BTN_OBJECT_LINKED_MULTI_CURVE, m_btnObjectMultiCurve);
	DDX_Control(pDX, IDC_BTN_OBJECT_TEXT, m_btnObjectText);
	DDX_Control(pDX, IDC_BTN_OBJECT_DIAMOND, m_btnObjectDiamond);
	DDX_Control(pDX, IDC_BTN_OBJECT_PICTURE, m_btnObjectPicture);
	DDX_Control(pDX, IDC_BTN_OBJECT_ROUND_RECT, m_btnObjectRoundRect);
	DDX_Control(pDX, IDC_BTN_OBJECT_TRIANGLE, m_btnObjectTriangle);
	DDX_Control(pDX, IDC_BTN_OBJECT_RECT, m_btnObjectRect);
	DDX_Control(pDX, IDC_BTN_OBJECT_PENTAGON, m_btnObjectPentagon);
	DDX_Control(pDX, IDC_BTN_OBJECT_SIXANGLE, m_btnObjectSixangle);
	DDX_Control(pDX, IDC_BTN_OBJECT_EIGHTANGLE, m_btnObjectEightangle);
	DDX_Control(pDX, IDC_BTN_OVERLAY_SHOW_GRID, m_btnObjectGrid);
	DDX_Text(pDX, IDC_EDT_ADD_LAYER_NAME, m_strLayerNameForAdd);
	DDX_Text(pDX, IDC_EDT_SELECTEDLAYER_NAME, m_strSelectedLayerName);
	DDX_Control(pDX, IDC_LIST_OVERLAY_LAYER, m_LayerList);
}


BEGIN_MESSAGE_MAP(CDlgOverlayControler, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_OBJECT_LINE, &CDlgOverlayControler::OnBnClickedBtnObjectLine)
	ON_BN_CLICKED(IDC_BTN_OBJECT_CURVE, &CDlgOverlayControler::OnBnClickedBtnObjectCurve)
	ON_BN_CLICKED(IDC_BTN_OBJECT_LINKED_LINE, &CDlgOverlayControler::OnBnClickedBtnObjectLinkedLine)
	ON_BN_CLICKED(IDC_BTN_OBJECT_LINKED_MULTI_LINE, &CDlgOverlayControler::OnBnClickedBtnObjectLinkedMultiLine)
	ON_BN_CLICKED(IDC_BTN_OBJECT_LINKED_MULTI_CURVE, &CDlgOverlayControler::OnBnClickedBtnObjectLinkedMultiCurve)
	ON_BN_CLICKED(IDC_BTN_OBJECT_POLYLINE, &CDlgOverlayControler::OnBnClickedBtnObjectPolyline)
	ON_BN_CLICKED(IDC_BTN_OBJECT_POLYGON, &CDlgOverlayControler::OnBnClickedBtnObjectPolygon)
	ON_BN_CLICKED(IDC_BTN_OBJECT_ARC, &CDlgOverlayControler::OnBnClickedBtnObjectArc)
	ON_BN_CLICKED(IDC_BTN_OBJECT_PIE, &CDlgOverlayControler::OnBnClickedBtnObjectPie)
	ON_BN_CLICKED(IDC_BTN_OBJECT_ELLIPSE, &CDlgOverlayControler::OnBnClickedBtnObjectEllipse)
	ON_BN_CLICKED(IDC_BTN_OBJECT_TRIANGLE, &CDlgOverlayControler::OnBnClickedBtnObjectTriangle)
	ON_BN_CLICKED(IDC_BTN_OBJECT_DIAMOND, &CDlgOverlayControler::OnBnClickedBtnObjectDiamond)
	ON_BN_CLICKED(IDC_BTN_OBJECT_RECT, &CDlgOverlayControler::OnBnClickedBtnObjectRect)
	ON_BN_CLICKED(IDC_BTN_OBJECT_ROUND_RECT, &CDlgOverlayControler::OnBnClickedBtnObjectRoundRect)
	ON_BN_CLICKED(IDC_BTN_OBJECT_PENTAGON, &CDlgOverlayControler::OnBnClickedBtnObjectPentagon)
	ON_BN_CLICKED(IDC_BTN_OBJECT_SIXANGLE, &CDlgOverlayControler::OnBnClickedBtnObjectSixangle)
	ON_BN_CLICKED(IDC_BTN_OBJECT_EIGHTANGLE, &CDlgOverlayControler::OnBnClickedBtnObjectEightangle)
	ON_BN_CLICKED(IDC_BTN_OBJECT_PICTURE, &CDlgOverlayControler::OnBnClickedBtnObjectPicture)
	ON_BN_CLICKED(IDC_BTN_OBJECT_TEXT, &CDlgOverlayControler::OnBnClickedBtnObjectText)
	ON_BN_CLICKED(IDC_BTN_OVERLAY_SHOW_GRID, &CDlgOverlayControler::OnBnClickedBtnShowGrid)
	ON_WM_CLOSE()
	//ON_BN_CLICKED(IDC_BUTTON1, &CDlgOverlayControler::OnBnClickedButton1)
	//ON_BN_CLICKED(IDC_BUTTON2, &CDlgOverlayControler::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BTN_LAYER_MOVE_TOP, &CDlgOverlayControler::OnBnClickedBtnLayerMoveTop)
	ON_BN_CLICKED(IDC_BTN_LAYER_MOVE_UP, &CDlgOverlayControler::OnBnClickedBtnLayerMoveUp)
	ON_BN_CLICKED(IDC_BTN_LAYER_MOVE_DOWN, &CDlgOverlayControler::OnBnClickedBtnLayerMoveDown)
	ON_BN_CLICKED(IDC_BTN_LAYER_MOVE_BOTTOM, &CDlgOverlayControler::OnBnClickedBtnLayerMoveBottom)
	ON_BN_CLICKED(IDC_BTN_ADD_LAYER, &CDlgOverlayControler::OnBnClickedBtnAddLayer)
	ON_BN_CLICKED(IDC_BTN_DELETE_LAYER, &CDlgOverlayControler::OnBnClickedBtnDeleteLayer)
	ON_NOTIFY(NM_CLICK, IDC_LIST_OVERLAY_LAYER, &CDlgOverlayControler::OnNMClickListOverlayLayer)
	ON_BN_CLICKED(IDC_BTN_LAYER_DELETE_ALL, &CDlgOverlayControler::OnBnClickedBtnLayerDeleteAll)
	//ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_BTN_LAYER_SAVE, &CDlgOverlayControler::OnBnClickedBtnLayerSave)
	ON_BN_CLICKED(IDC_BTN_LAYER_LOAD, &CDlgOverlayControler::OnBnClickedBtnLayerLoad)
	ON_BN_CLICKED(IDC_BTN_LAYER_REFRESH, &CDlgOverlayControler::OnBnClickedRefresh)
END_MESSAGE_MAP()


// CDlgOverlayControler 메시지 처리기
BOOL CDlgOverlayControler::OnInitDialog()
{
	CDialogEx::OnInitDialog();

/*
	m_btnObjectEllipse.SetShade(CShadeButtonST::SHS_SOFTBUMP);
	//m_btnObjectEllipse.SetIcon(IDI_OVERLAY_ELLIPSE);
	m_btnObjectEllipse.SetTooltipText(_T("타원"));
	m_btnObjectEllipse.SetBitmaps(IDB_OVERLAY_ELLIPSE_OFF, 0xffffff, -1, 0xffffff);

	m_btnObjectArc.SetShade(CShadeButtonST::SHS_SOFTBUMP);
	//m_btnObjectArc.SetIcon(IDI_OVERLAY_ARC);
	m_btnObjectArc.SetTooltipText(_T("호"));
	m_btnObjectArc.SetBitmaps(IDB_OVERLAY_ARC_OFF, 0xffffff, -1, 0xffffff);

	m_btnObjectPie.SetShade(CShadeButtonST::SHS_SOFTBUMP);
	//m_btnObjectPie.SetIcon(IDI_OVERLAY_PIE);
	m_btnObjectPie.SetTooltipText(_T("부채꼴"));
	m_btnObjectPie.SetBitmaps(IDB_OVERLAY_PIE_OFF, 0xffffff, -1, 0xffffff);

	m_btnObjectLine.SetShade(CShadeButtonST::SHS_SOFTBUMP);
	//m_btnObjectLine.SetIcon(IDI_OVERLAY_LINE);
	m_btnObjectLine.SetTooltipText(_T("선"));
	m_btnObjectLine.SetBitmaps(IDB_OVERLAY_LINE_OFF, 0xffffff, -1, 0xffffff);

	m_btnObjectPolyLine.SetShade(CShadeButtonST::SHS_SOFTBUMP);
	//m_btnObjectPolyLine.SetIcon(IDI_OVERLAY_POLYLINE);
	m_btnObjectPolyLine.SetTooltipText(_T("꺽임선"));
	m_btnObjectPolyLine.SetBitmaps(IDB_OVERLAY_POLYLINE_OFF, 0xffffff, -1, 0xffffff);

	m_btnObjectCurve.SetShade(CShadeButtonST::SHS_SOFTBUMP);
	//m_btnObjectCurve.SetIcon(IDI_OVERLAY_CURVE);
	m_btnObjectCurve.SetTooltipText(_T("곡선"));
	m_btnObjectCurve.SetBitmaps(IDB_OVERLAY_CURVELINE_OFF, 0xffffff, -1, 0xffffff);

	m_btnObjectPolygon.SetShade(CShadeButtonST::SHS_SOFTBUMP);
	//m_btnObjectPolygon.SetIcon(IDI_OVERLAY_POLYGON);
	m_btnObjectPolygon.SetTooltipText(_T("다각형"));
	m_btnObjectPolygon.SetBitmaps(IDB_OVERLAY_POLYGON_OFF, 0xffffff, -1, 0xffffff);

	m_btnObjectLinkedLine.SetShade(CShadeButtonST::SHS_SOFTBUMP);
	//m_btnObjectLinkedLine.SetIcon(IDI_OVERLAY_LINKED_LINE);
	m_btnObjectLinkedLine.SetTooltipText(_T("연결선"));
	m_btnObjectLinkedLine.SetBitmaps(IDB_OVERLAY_LINKEDLINE_OFF, 0xffffff, -1, 0xffffff);

	m_btnObjectMultiLine.SetShade(CShadeButtonST::SHS_SOFTBUMP);
	//m_btnObjectMultiLine.SetIcon(IDI_OVERLAY_LINKED_MULTI_LINE);
	m_btnObjectMultiLine.SetTooltipText(_T("연결선(꺽임선)"));
	m_btnObjectMultiLine.SetBitmaps(IDB_OVERLAY_LINKED_MULTILINE_OFF, 0xffffff, -1, 0xffffff);

	m_btnObjectMultiCurve.SetShade(CShadeButtonST::SHS_SOFTBUMP);
	//m_btnObjectMultiCurve.SetIcon(IDI_OVERLAY_LINKED_CURVE);
	m_btnObjectMultiCurve.SetTooltipText(_T("연결선(꺽임곡선)"));
	m_btnObjectMultiCurve.SetBitmaps(IDB_OVERLAY_LINKED_CURVE_OFF, 0xffffff, -1, 0xffffff);

	m_btnObjectText.SetShade(CShadeButtonST::SHS_SOFTBUMP);
	//m_btnObjectText.SetIcon(IDI_OVERLAY_TEXT);
	m_btnObjectText.SetTooltipText(_T("문자"));
	m_btnObjectText.SetBitmaps(IDB_OVERLAY_TEXT_OFF, 0xffffff, -1, 0xffffff);

	m_btnObjectDiamond.SetShade(CShadeButtonST::SHS_SOFTBUMP);
	//m_btnObjectDiamond.SetIcon(IDI_OVERLAY_DIAMOND);
	m_btnObjectDiamond.SetTooltipText(_T("마름모"));
	m_btnObjectDiamond.SetBitmaps(IDB_OVERLAY_DIAMOND_OFF, 0xffffff, -1, 0xffffff);

	m_btnObjectPicture.SetShade(CShadeButtonST::SHS_SOFTBUMP);
	//m_btnObjectPicture.SetIcon(IDI_OVERLAY_IMAGE);
	m_btnObjectPicture.SetTooltipText(_T("그림"));
	m_btnObjectPicture.SetBitmaps(IDB_OVERLAY_IMAGE_OFF, 0xffffff, -1, 0xffffff);

	m_btnObjectRoundRect.SetShade(CShadeButtonST::SHS_SOFTBUMP);
	//m_btnObjectRoundRect.SetIcon(IDI_OVERLAY_ROUND_RECT);
	m_btnObjectRoundRect.SetTooltipText(_T("둥근 사각형"));
	m_btnObjectRoundRect.SetBitmaps(IDB_OVERLAY_ROUND_RECT_OFF, 0xffffff, -1, 0xffffff);

	m_btnObjectTriangle.SetShade(CShadeButtonST::SHS_SOFTBUMP);
	//m_btnObjectTriangle.SetIcon(IDI_OVERLAY_TRIANGLE);
	m_btnObjectTriangle.SetTooltipText(_T("삼각형"));
	m_btnObjectTriangle.SetBitmaps(IDB_OVERLAY_TRIANGLE_OFF, 0xffffff, -1, 0xffffff);

	m_btnObjectRect.SetShade(CShadeButtonST::SHS_SOFTBUMP);
	//m_btnObjectRect.SetIcon(IDI_OVERLAY_RECT);
	m_btnObjectRect.SetTooltipText(_T("사각형"));
	m_btnObjectRect.SetBitmaps(IDB_OVERLAY_RECT_OFF, 0xffffff, -1, 0xffffff);

	m_btnObjectPentagon.SetShade(CShadeButtonST::SHS_SOFTBUMP);
	//m_btnObjectPentagon.SetIcon(IDI_OVERLAY_PENTAGON);
	m_btnObjectPentagon.SetTooltipText(_T("오각형"));
	m_btnObjectPentagon.SetBitmaps(IDB_OVERLAY_PENTAGON_OFF, 0xffffff, -1, 0xffffff);

	m_btnObjectSixangle.SetShade(CShadeButtonST::SHS_SOFTBUMP);
	//m_btnObjectSixangle.SetIcon(IDI_OVERLAY_SIXANGLE);
	m_btnObjectSixangle.SetTooltipText(_T("육각형"));
	m_btnObjectSixangle.SetBitmaps(IDB_OVERLAY_SIXANGLE_OFF, 0xffffff, -1, 0xffffff);

	m_btnObjectEightangle.SetShade(CShadeButtonST::SHS_SOFTBUMP);
	//m_btnObjectEightangle.SetIcon(IDI_OVERLAY_EIGHTANGLE);
	m_btnObjectEightangle.SetTooltipText(_T("팔각형"));
	m_btnObjectEightangle.SetBitmaps(IDB_OVERLAY_EIGHTANGLE_OFF, 0xffffff, -1, 0xffffff);

	m_btnObjectGrid.SetShade(CShadeButtonST::SHS_SOFTBUMP);
	//m_btnObjectGrid.SetIcon(IDI_OVERLAY_GRID);
	m_btnObjectGrid.SetTooltipText(_T("격자"));
	m_btnObjectGrid.SetBitmaps(IDB_OVERLAY_GRID_OFF, 0xffffff, -1, 0xffffff);

	//m_btnOverlayLayer.SetShade(CShadeButtonST::SHS_SOFTBUMP);
	////m_btnOverlayLayer.SetIcon(IDI_CLOSE);
	//m_btnOverlayLayer.SetTooltipText(_T("레이어 창"));
	//m_btnOverlayLayer.SetBitmaps(IDB_OVERLAY_LAYERDLG_OFF,0xffffff,-1,0xffffff);
*/

	InitList();

	return TRUE;
}

BOOL CDlgOverlayControler::BindOverlay(COverlayThread* pOverlay, CDlgObjectList* pDlgObjectList)
{
	m_pOverlayThread = pOverlay;
	if (m_pOverlayThread == NULL)
		return FALSE;

	//m_pDlgLayerList = pDlgLayerList;
	//if(m_pDlgLayerList!=NULL)
	{
		BindObjectListDlg(m_pOverlayThread->GetOverlayMain(), pDlgObjectList);
		//return TRUE;
	}
	return TRUE;
}

// 선
void CDlgOverlayControler::OnBnClickedBtnObjectLine()
{
	if (m_pOverlayThread != NULL)
	{
		m_bSymbolDraw = FALSE;
		m_pOverlayThread->NewObject(OVERLAY_CREATE_NEW, OVERLAY::OBJECT_TYPE::LINE);
		//m_btnObjectLine.SetBitmaps(IDB_OVERLAY_LINE, 0xffffff, -1, 0xffffff);
		m_SelectObjectType = OVERLAY::OBJECT_TYPE::LINE;
		ResetItemEvent(OVERLAY::LINE);
	}

}

// 꺽임선
void CDlgOverlayControler::OnBnClickedBtnObjectCurve()
{
	if (m_pOverlayThread != NULL) {
		m_pOverlayThread->NewObject(OVERLAY_CREATE_NEW, OVERLAY::OBJECT_TYPE::CURVE);
		//m_btnObjectCurve.SetBitmaps(IDB_OVERLAY_CURVELINE, 0xffffff, -1, 0xffffff);
		m_SelectObjectType = OVERLAY::OBJECT_TYPE::CURVE;
		ResetItemEvent(OVERLAY::OBJECT_TYPE::CURVE);
	}
}

// 연결선
void CDlgOverlayControler::OnBnClickedBtnObjectLinkedLine()
{
	if (m_pOverlayThread != NULL) {
		m_pOverlayThread->NewObject(OVERLAY_CREATE_NEW, OVERLAY::OBJECT_TYPE::LINKED_LINE);
		//m_btnObjectLinkedLine.SetBitmaps(IDB_OVERLAY_LINKEDLINE, 0xffffff, -1, 0xffffff);
		m_SelectObjectType = OVERLAY::OBJECT_TYPE::LINKED_LINE;
		ResetItemEvent(OVERLAY::OBJECT_TYPE::LINKED_LINE);
	}
}

// 연결선 꺽임선
void CDlgOverlayControler::OnBnClickedBtnObjectLinkedMultiLine()
{
	if (m_pOverlayThread != NULL) {
		m_pOverlayThread->NewObject(OVERLAY_CREATE_NEW, OVERLAY::OBJECT_TYPE::LINKED_MULTI_LINE);
		//m_btnObjectMultiLine.SetBitmaps(IDB_OVERLAY_LINKED_MULTILINE, 0xffffff, -1, 0xffffff);
		m_SelectObjectType = OVERLAY::OBJECT_TYPE::LINKED_MULTI_LINE;
		ResetItemEvent(OVERLAY::OBJECT_TYPE::LINKED_MULTI_LINE);
	}

}

// 곡선
/*
void CDlgOverlayControler::OnBnClickedBtnObjectLinkedCurve()
{
	if (m_pOverlayThread != NULL) {
		m_pOverlayThread->NewObject(OVERLAY_CREATE_NEW, OVERLAY::LINKED_CURVE);
		//m_btnObjectLinkedCurve.SetBitmaps(IDB_OVERLAY_LINKED_CURVE, 0xffffff, -1, 0xffffff);
		m_SelectObjectType = OVERLAY::LINKED_CURVE;


		ResetItemEvent(OVERLAY::LINKED_CURVE);
	}

}
*/

// 연결선 꺽임 곡선
void CDlgOverlayControler::OnBnClickedBtnObjectLinkedMultiCurve()
{
	if (m_pOverlayThread != NULL) {
		m_pOverlayThread->NewObject(OVERLAY_CREATE_NEW, OVERLAY::OBJECT_TYPE::LINKED_MULTI_CURVE);
		//m_btnObjectMultiCurve.SetBitmaps(IDB_OVERLAY_LINKED_CURVE, 0xffffff, -1, 0xffffff);
		m_SelectObjectType = OVERLAY::OBJECT_TYPE::LINKED_MULTI_CURVE;
		ResetItemEvent(OVERLAY::OBJECT_TYPE::LINKED_MULTI_CURVE);
	}

}

void CDlgOverlayControler::OnBnClickedBtnObjectPolyline()
{
	if (m_pOverlayThread != NULL) {
		m_pOverlayThread->NewObject(OVERLAY_CREATE_NEW, OVERLAY::POLYLINE);
		//m_btnObjectPolyLine.SetBitmaps(IDB_OVERLAY_POLYLINE, 0xffffff, -1, 0xffffff);
		m_SelectObjectType = OVERLAY::OBJECT_TYPE::POLYLINE;
		ResetItemEvent(OVERLAY::OBJECT_TYPE::POLYLINE);
	}

}

void CDlgOverlayControler::OnBnClickedBtnObjectPolygon()
{
	if (m_pOverlayThread != NULL)
	{
		m_bSymbolDraw = FALSE;
		m_pOverlayThread->NewObject(OVERLAY_CREATE_NEW, OVERLAY::OBJECT_TYPE::POLYGON);
		//m_btnObjectPolygon.SetBitmaps(IDB_OVERLAY_POLYGON, 0xffffff, -1, 0xffffff);
		m_SelectObjectType = OVERLAY::OBJECT_TYPE::POLYGON;
		ResetItemEvent(OVERLAY::OBJECT_TYPE::POLYGON);
	}

}

void CDlgOverlayControler::OnBnClickedBtnObjectArc()
{
	if (m_pOverlayThread != NULL)
	{
		m_bSymbolDraw = FALSE;
		m_pOverlayThread->NewObject(OVERLAY_CREATE_NEW, OVERLAY::OBJECT_TYPE::ARC);
		//m_btnObjectArc.SetBitmaps(IDB_OVERLAY_ARC, 0xffffff, -1, 0xffffff);
		m_SelectObjectType = OVERLAY::OBJECT_TYPE::ARC;
		ResetItemEvent(OVERLAY::OBJECT_TYPE::ARC);
	}

}

void CDlgOverlayControler::OnBnClickedBtnObjectPie()
{
	if (m_pOverlayThread != NULL)
	{
		m_bSymbolDraw = FALSE;
		m_pOverlayThread->NewObject(OVERLAY_CREATE_NEW, OVERLAY::OBJECT_TYPE::PIE);
		//m_btnObjectPie.SetBitmaps(IDB_OVERLAY_PIE, 0xffffff, -1, 0xffffff);
		m_SelectObjectType = OVERLAY::OBJECT_TYPE::PIE;
		ResetItemEvent(OVERLAY::OBJECT_TYPE::PIE);
	}

}

void CDlgOverlayControler::OnBnClickedBtnObjectEllipse()
{
	if (m_pOverlayThread != NULL)
	{
		m_bSymbolDraw = FALSE;
		m_pOverlayThread->NewObject(OVERLAY_CREATE_NEW, OVERLAY::OBJECT_TYPE::ELLIPSE);
		//m_btnObjectEllipse.SetBitmaps(IDB_OVERLAY_ELLIPSE, 0xffffff, -1, 0xffffff);
		m_SelectObjectType = OVERLAY::OBJECT_TYPE::ELLIPSE;
		ResetItemEvent(OVERLAY::ELLIPSE);
	}

}

void CDlgOverlayControler::OnBnClickedBtnObjectTriangle()
{
	if (m_pOverlayThread != NULL)
	{
		m_bSymbolDraw = FALSE;
		m_pOverlayThread->NewObject(OVERLAY_CREATE_NEW, OVERLAY::OBJECT_TYPE::TRIANGLE);
		//m_btnObjectTriangle.SetBitmaps(IDB_OVERLAY_TRIANGLE, 0xffffff, -1, 0xffffff);
		m_SelectObjectType = OVERLAY::OBJECT_TYPE::TRIANGLE;
		ResetItemEvent(OVERLAY::OBJECT_TYPE::TRIANGLE);
	}

}

void CDlgOverlayControler::OnBnClickedBtnObjectDiamond()
{
	if (m_pOverlayThread != NULL) {
		m_pOverlayThread->NewObject(OVERLAY_CREATE_NEW, OVERLAY::OBJECT_TYPE::DIAMOND);
		//m_btnObjectDiamond.SetBitmaps(IDB_OVERLAY_DIAMOND, 0xffffff, -1, 0xffffff);
		m_SelectObjectType = OVERLAY::OBJECT_TYPE::DIAMOND;
		ResetItemEvent(OVERLAY::OBJECT_TYPE::DIAMOND);
	}

}

void CDlgOverlayControler::OnBnClickedBtnObjectRect()
{
	if (m_pOverlayThread != NULL)
	{
		m_bSymbolDraw = FALSE;
		m_pOverlayThread->NewObject(OVERLAY_CREATE_NEW, OVERLAY::OBJECT_TYPE::RECT_TYPE);
		//m_btnObjectRect.SetBitmaps(IDB_OVERLAY_RECT, 0xffffff, -1, 0xffffff);
		m_SelectObjectType = OVERLAY::OBJECT_TYPE::RECT_TYPE;
		ResetItemEvent(OVERLAY::OBJECT_TYPE::RECT_TYPE);
	}

}

void CDlgOverlayControler::OnBnClickedBtnObjectRoundRect()
{
	if (m_pOverlayThread != NULL) {
		m_pOverlayThread->NewObject(OVERLAY_CREATE_NEW, OVERLAY::OBJECT_TYPE::ROUND_RECT);
		//m_btnObjectRoundRect.SetBitmaps(IDB_OVERLAY_ROUND_RECT, 0xffffff, -1, 0xffffff);
		m_SelectObjectType = OVERLAY::OBJECT_TYPE::ROUND_RECT;
		ResetItemEvent(OVERLAY::OBJECT_TYPE::ROUND_RECT);
	}
}

void CDlgOverlayControler::OnBnClickedBtnObjectPentagon()
{
	if (m_pOverlayThread != NULL) {
		m_pOverlayThread->NewObject(OVERLAY_CREATE_NEW, OVERLAY::OBJECT_TYPE::PENTAGON);
		//m_btnObjectPentagon.SetBitmaps(IDB_OVERLAY_PENTAGON, 0xffffff, -1, 0xffffff);
		m_SelectObjectType = OVERLAY::OBJECT_TYPE::PENTAGON;
		ResetItemEvent(OVERLAY::OBJECT_TYPE::PENTAGON);
	}
}

void CDlgOverlayControler::OnBnClickedBtnObjectSixangle()
{
	if (m_pOverlayThread != NULL) {
		m_pOverlayThread->NewObject(OVERLAY_CREATE_NEW, OVERLAY::OBJECT_TYPE::SIX_ANGLE);
		//m_btnObjectSixangle.SetBitmaps(IDB_OVERLAY_SIXANGLE, 0xffffff, -1, 0xffffff);
		m_SelectObjectType = OVERLAY::OBJECT_TYPE::SIX_ANGLE;
		ResetItemEvent(OVERLAY::OBJECT_TYPE::SIX_ANGLE);
	}

}

void CDlgOverlayControler::OnBnClickedBtnObjectEightangle()
{
	if (m_pOverlayThread != NULL) {
		m_pOverlayThread->NewObject(OVERLAY_CREATE_NEW, OVERLAY::OBJECT_TYPE::EIGHT_ANGLE);
		//m_btnObjectEightangle.SetBitmaps(IDB_OVERLAY_EIGHTANGLE, 0xffffff, -1, 0xffffff);
		m_SelectObjectType = OVERLAY::OBJECT_TYPE::EIGHT_ANGLE;
		ResetItemEvent(OVERLAY::OBJECT_TYPE::EIGHT_ANGLE);
	}

}

void CDlgOverlayControler::OnBnClickedBtnObjectPicture()
{
	if (m_pOverlayThread != NULL)
	{

		if (m_pOverlayThread == NULL)
			return;

		CString FileName;


		TCHAR szFilter[] =
			_T("jpg Files (*.jpg)|*.jpg|")
			_T("png Files (*.png)|*.png|")
			_T("bmp Files (*.bmp)|*.bmp|")
			_T("bmp Files (*.gif)|*.gif|");

		CHAR	lpPath[512] = { 0, };
		GetCurrentDirectory(512, lpPath);
		sprintf(lpPath, "%s%s", lpPath, FILEPATH_SAVE_FOLDER);

		CFileDialog dlgFile(TRUE, NULL, NULL, OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST, szFilter, this);

		CFilePath ImagePath;
		ImagePath.SetWork(FILEPATH_SAVE_FOLDER);
		dlgFile.m_ofn.lpstrInitialDir = ImagePath.GetWork();// 강제로 폴더지정?

		if (dlgFile.DoModal() == IDOK)
		{
			FileName.Format(_T("%s"), dlgFile.GetPathName());
			SetCurrentDirectory(lpPath);
		}
		else
		{
			return;
		}

		SetCurrentDirectory(lpPath);

		m_pOverlayThread->NewObject(OVERLAY_CREATE_NEW, OVERLAY::IMAGE, FileName.GetBuffer(0), FileName.GetLength());
		//m_btnObjectPicture.SetBitmaps(IDB_OVERLAY_IMAGE, 0xffffff, -1, 0xffffff);
		m_SelectObjectType = OVERLAY::IMAGE;
		ResetItemEvent(OVERLAY::IMAGE);

	}

}

void CDlgOverlayControler::OnBnClickedBtnObjectText()
{
	if (m_pOverlayThread != NULL)
	{
		m_bSymbolDraw = FALSE;
		m_pOverlayThread->NewObject(OVERLAY_CREATE_NEW, OVERLAY::TEXT);
		//m_btnObjectText.SetBitmaps(IDB_OVERLAY_TEXT, 0xffffff, -1, 0xffffff);
		m_SelectObjectType = OVERLAY::TEXT;
		ResetItemEvent(OVERLAY::TEXT);
	}

}

/*
void CDlgOverlayControler::OnBnClickedBtnShowLayerWindow()
{
	//if(m_pDlgLayerList!=NULL)
	//	m_pDlgLayerList->ShowWindow(SW_SHOW);

	m_btnOverlayLayer.SetBitmaps(IDB_OVERLAY_LAYERDLG,0xffffff,-1,0xffffff);
//
// 	if(!m_bOverlayLayer){
// 		m_btnOverlayLayer.SetBitmaps(IDB_OVERLAY_LAYERDLG,0xffffff,-1,0xffffff);
// 		m_bOverlayLayer=TRUE;
// 	}
// 	else{
// 		m_btnOverlayLayer.SetBitmaps(IDB_OVERLAY_LAYERDLG_OFF,0xffffff,-1,0xffffff);
// 		m_bOverlayLayer=FALSE;
// 	}
}
*/
void CDlgOverlayControler::OnBnClickedBtnShowGrid()
{
	if (m_pMainView != NULL)
	{
		m_nGridDetail += 20;
		m_nGridDetail %= MAX_GRID_DETAIL;

		m_pOverlayThread->SetGrid(TRUE);
		m_pOverlayThread->GetOverlayView()->ReleaseSelectObject();
		m_pOverlayThread->DrawGrid(m_pMainView->GetSafeHwnd(), m_nGridDetail);
		m_pOverlayThread->GetOverlayMain()->DrawOnScreen();
		CC4IMapView* pView = (CC4IMapView*)m_pMainView;
		//		pView->DrawSituation(FALSE,FALSE,FALSE, TRUE);
				//m_btnOverlayLayer.SetBitmaps(IDB_OVERLAY_GRID,0xffffff,-1,0xffffff);
	}
}

void CDlgOverlayControler::UpDateGridDraw()
{
	if (m_pMainView != NULL)
	{
		m_pOverlayThread->SetGrid(TRUE);
		m_pOverlayThread->GetOverlayView()->ReleaseSelectObject();
		m_pOverlayThread->DrawGrid(m_pMainView->GetSafeHwnd(), m_nGridDetail);
		m_pOverlayThread->GetOverlayMain()->DrawOnScreen();
	}
}
/*
void CDlgOverlayControler::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	if(nStatus==SW_HIDE)
	{
		m_nGridDetail -= 20;
		if(m_nGridDetail<0)
			m_nGridDetail=0;
		m_pOverlayThread->GetOverlayView()->ReleaseSelectObject();
	}
}
*/
void CDlgOverlayControler::OnClose()
{
	m_pOverlayThread->SetGrid(FALSE);
	m_pOverlayThread->GetOverlayView()->ReleaseSelectObject();

	if (m_pMainOverlay == NULL)
		return;

	m_pMainOverlay->DrawObjects();
	m_pMainOverlay->DrawEditlayer();
	m_pMainOverlay->DrawOnScreen();


	CDialogEx::OnClose();
}

void CDlgOverlayControler::ResetItemEvent(int KindObject)
{
/*
	if (m_SelectObjectType != OVERLAY::LINE)
		m_btnObjectLine.SetBitmaps(IDB_OVERLAY_LINE_OFF, 0xffffff, -1, 0xffffff);
	else
		m_btnObjectLine.SetBitmaps(IDB_OVERLAY_LINE, 0xffffff, -1, 0xffffff);

	if (m_SelectObjectType != OVERLAY::CURVE)
		m_btnObjectCurve.SetBitmaps(IDB_OVERLAY_CURVELINE_OFF, 0xffffff, -1, 0xffffff);
	else
		m_btnObjectCurve.SetBitmaps(IDB_OVERLAY_CURVELINE, 0xffffff, -1, 0xffffff);

	if (m_SelectObjectType != OVERLAY::LINKED_LINE)
		m_btnObjectLinkedLine.SetBitmaps(IDB_OVERLAY_LINKEDLINE_OFF, 0xffffff, -1, 0xffffff);
	else
		m_btnObjectLinkedLine.SetBitmaps(IDB_OVERLAY_LINKEDLINE, 0xffffff, -1, 0xffffff);

	if (m_SelectObjectType != OVERLAY::LINKED_MULTI_LINE)
		m_btnObjectMultiLine.SetBitmaps(IDB_OVERLAY_LINKED_MULTILINE_OFF, 0xffffff, -1, 0xffffff);
	else
		m_btnObjectMultiLine.SetBitmaps(IDB_OVERLAY_LINKED_MULTILINE, 0xffffff, -1, 0xffffff);

	if (m_SelectObjectType != OVERLAY::LINKED_MULTI_CURVE)
		m_btnObjectMultiCurve.SetBitmaps(IDB_OVERLAY_LINKED_CURVE_OFF, 0xffffff, -1, 0xffffff);
	else
		m_btnObjectMultiCurve.SetBitmaps(IDB_OVERLAY_LINKED_CURVE, 0xffffff, -1, 0xffffff);

	if (m_SelectObjectType != OVERLAY::POLYLINE)
		m_btnObjectPolyLine.SetBitmaps(IDB_OVERLAY_POLYLINE_OFF, 0xffffff, -1, 0xffffff);
	else
		m_btnObjectPolyLine.SetBitmaps(IDB_OVERLAY_POLYLINE, 0xffffff, -1, 0xffffff);

	if (m_SelectObjectType != OVERLAY::POLYGON)
		m_btnObjectPolygon.SetBitmaps(IDB_OVERLAY_POLYGON_OFF, 0xffffff, -1, 0xffffff);
	else
		m_btnObjectPolygon.SetBitmaps(IDB_OVERLAY_POLYGON, 0xffffff, -1, 0xffffff);

	if (m_SelectObjectType != OVERLAY::ARC)
		m_btnObjectArc.SetBitmaps(IDB_OVERLAY_ARC_OFF, 0xffffff, -1, 0xffffff);
	else
		m_btnObjectArc.SetBitmaps(IDB_OVERLAY_ARC, 0xffffff, -1, 0xffffff);

	if (m_SelectObjectType != OVERLAY::PIE)
		m_btnObjectPie.SetBitmaps(IDB_OVERLAY_PIE_OFF, 0xffffff, -1, 0xffffff);
	else
		m_btnObjectPie.SetBitmaps(IDB_OVERLAY_PIE, 0xffffff, -1, 0xffffff);

	if (m_SelectObjectType != OVERLAY::ELLIPSE)
		m_btnObjectEllipse.SetBitmaps(IDB_OVERLAY_ELLIPSE_OFF, 0xffffff, -1, 0xffffff);
	else
		m_btnObjectEllipse.SetBitmaps(IDB_OVERLAY_ELLIPSE, 0xffffff, -1, 0xffffff);

	if (m_SelectObjectType != OVERLAY::TRIANGLE)
		m_btnObjectTriangle.SetBitmaps(IDB_OVERLAY_TRIANGLE_OFF, 0xffffff, -1, 0xffffff);
	else
		m_btnObjectTriangle.SetBitmaps(IDB_OVERLAY_TRIANGLE, 0xffffff, -1, 0xffffff);

	if (m_SelectObjectType != OVERLAY::DIAMOND)
		m_btnObjectDiamond.SetBitmaps(IDB_OVERLAY_DIAMOND_OFF, 0xffffff, -1, 0xffffff);
	else
		m_btnObjectDiamond.SetBitmaps(IDB_OVERLAY_DIAMOND, 0xffffff, -1, 0xffffff);

	if (m_SelectObjectType != OVERLAY::RECT_TYPE)
		m_btnObjectRect.SetBitmaps(IDB_OVERLAY_RECT_OFF, 0xffffff, -1, 0xffffff);
	else
		m_btnObjectRect.SetBitmaps(IDB_OVERLAY_RECT, 0xffffff, -1, 0xffffff);

	if (m_SelectObjectType != OVERLAY::ROUND_RECT)
		m_btnObjectRoundRect.SetBitmaps(IDB_OVERLAY_ROUND_RECT_OFF, 0xffffff, -1, 0xffffff);
	else
		m_btnObjectRoundRect.SetBitmaps(IDB_OVERLAY_ROUND_RECT, 0xffffff, -1, 0xffffff);

	if (m_SelectObjectType != OVERLAY::PENTAGON)
		m_btnObjectPentagon.SetBitmaps(IDB_OVERLAY_PENTAGON_OFF, 0xffffff, -1, 0xffffff);
	else
		m_btnObjectPentagon.SetBitmaps(IDB_OVERLAY_PENTAGON, 0xffffff, -1, 0xffffff);

	if (m_SelectObjectType != OVERLAY::SIX_ANGLE)
		m_btnObjectSixangle.SetBitmaps(IDB_OVERLAY_SIXANGLE_OFF, 0xffffff, -1, 0xffffff);
	else
		m_btnObjectSixangle.SetBitmaps(IDB_OVERLAY_SIXANGLE, 0xffffff, -1, 0xffffff);

	if (m_SelectObjectType != OVERLAY::EIGHT_ANGLE)
		m_btnObjectEightangle.SetBitmaps(IDB_OVERLAY_EIGHTANGLE_OFF, 0xffffff, -1, 0xffffff);
	else
		m_btnObjectEightangle.SetBitmaps(IDB_OVERLAY_EIGHTANGLE, 0xffffff, -1, 0xffffff);

	if (m_SelectObjectType != OVERLAY::IMAGE)
		m_btnObjectPicture.SetBitmaps(IDB_OVERLAY_IMAGE_OFF, 0xffffff, -1, 0xffffff);
	else
		m_btnObjectPicture.SetBitmaps(IDB_OVERLAY_IMAGE, 0xffffff, -1, 0xffffff);

	if (m_SelectObjectType != OVERLAY::TEXT)
		m_btnObjectText.SetBitmaps(IDB_OVERLAY_TEXT_OFF, 0xffffff, -1, 0xffffff);
	else
		m_btnObjectText.SetBitmaps(IDB_OVERLAY_TEXT, 0xffffff, -1, 0xffffff);
*/
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
void CDlgOverlayControler::OnBnClickedBtnLayerMoveTop()
{
	if (m_pMainOverlay == NULL)
		return;
	m_pMainOverlay->MoveLayerTop(m_nSelectedItemIndex);
	AddRawWithLayerList();
}

void CDlgOverlayControler::OnBnClickedBtnLayerMoveUp()
{
	if (m_pMainOverlay == NULL)
		return;
	m_pMainOverlay->MoveLayerUp(m_nSelectedItemIndex);
	AddRawWithLayerList();
}

void CDlgOverlayControler::OnBnClickedBtnLayerMoveDown()
{
	if (m_pMainOverlay == NULL)
		return;
	m_pMainOverlay->MoveLayerDown(m_nSelectedItemIndex);
	AddRawWithLayerList();
}

void CDlgOverlayControler::OnBnClickedBtnLayerMoveBottom()
{
	if (m_pMainOverlay == NULL)
		return;
	m_pMainOverlay->MoveLayerBottom(m_nSelectedItemIndex);
	AddRawWithLayerList();
}

void CDlgOverlayControler::OnBnClickedBtnAddLayer()
{
	if (m_pMainOverlay == NULL)
		return;
	UpdateData(TRUE);

	if (m_strLayerNameForAdd.GetLength() < 1)
	{
		AfxMessageBox(_T("레이어 이름을 입력하세요."));
		return;
	}

	if (m_pMainOverlay->AddLayer(m_strLayerNameForAdd.GetBuffer(0), m_strLayerNameForAdd.GetLength()))
	{
		UINT nIndex = m_LayerList.GetItemCount();
		if ((nIndex + 1) >= MAX_LAYER)
			return;
	}
	else
	{
		AfxMessageBox(_T("더이상 레이어를 추가할수 없습니다."));
	}
	AddRawWithLayerList();
}

void CDlgOverlayControler::OnBnClickedBtnDeleteLayer()
{
	if (m_pMainOverlay == NULL)
		return;
	m_pMainOverlay->DeleteLayer(m_nSelectedItemIndex);
	AddRawWithLayerList();
}

void CDlgOverlayControler::OnBnClickedBtnLayerDeleteAll()
{
	if (m_pMainOverlay == NULL)
		return;
	if (MessageBox(_T("전체 레이어를 삭제 하시겠습니까?"), _T("확인"), MB_YESNO) == IDYES)
	{
		//m_piEditTool->DeleteAll();	// 전술부호도 전체 삭제
		m_pMainOverlay->ClearLayer();
		AddRawWithLayerList();
		UpdateData(FALSE);
	}
}
/*
void CDlgOverlayControler::OnNMDblclkListOverlayLayer(NMHDR *pNMHDR, LRESULT *pResult)
{
	return;
	UpdateData(TRUE);

	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	m_nSelectedItemIndex = pNMItemActivate->iItem;

	m_pDlgObjectList->SetLayer(m_nSelectedItemIndex );
	CLayerStructure* pLayer = m_pMainOverlay->GetLayer(m_nSelectedItemIndex);
	if(pLayer!=NULL)
	{
		OVERLAY::OBJECT* pObject;
		pObject = pLayer->get_first_object();
		m_strSelectedLayerName.Format(_T("%s"),pLayer->GetLayerName());
		for(INT j=0; j<pLayer->NumberOfObjectInLayer(); j++)
		{
			if(pObject!=NULL)
			{
				m_pDlgObjectList->AddRaw(pObject);
				pObject = pLayer->get_next_object();
			}
		}
	}
	m_pDlgObjectList->ShowWindow(SW_SHOW);
	*pResult = 0;

	UpdateData(FALSE);
}*/

void CDlgOverlayControler::OnNMClickListOverlayLayer(NMHDR* pNMHDR, LRESULT* pResult)
{
	UpdateData(TRUE);

	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	m_nSelectedItemIndex = pNMItemActivate->iItem;

	CLayerStructure* pLayer = m_pMainOverlay->GetLayer(m_nSelectedItemIndex);

	BOOL bIsVisable = FALSE;
	if (pLayer != NULL)
	{
		bIsVisable = pLayer->IsDisplay();
		m_strSelectedLayerName = pLayer->GetLayerName();
		////////////////////////////////////
		if (pNMItemActivate->ptAction.x < 20 && pNMItemActivate->ptAction.y < 20 * m_nSelectedItemIndex + DEFAULT_COLUMN_HEIGHT + 20)
		{
			bIsVisable = m_LayerList.GetCheck(m_nSelectedItemIndex);
			pLayer->SetDisplay(!bIsVisable);

			m_pMainOverlay->DrawObjects();
			m_pMainOverlay->DrawEditlayer();
			m_pMainOverlay->DrawOnScreen();

			//-------------------------
			CC4IMapView* pView = (CC4IMapView*)m_pMainView;
			if (pView == NULL)
				return;
		}
		else
		{
			m_LayerList.SetCheck(m_nSelectedItemIndex, m_LayerList.GetCheck(m_nSelectedItemIndex));
		}
	}

	*pResult = 0;
	UpdateData(FALSE);
}

void CDlgOverlayControler::BindObjectListDlg(COverlayMain* pOverlay, CDlgObjectList* pDlgObjectList)
{
	m_pMainOverlay = pOverlay;
	m_pDlgObjectList = pDlgObjectList;
	m_pDlgObjectList->BindOverlay(pOverlay);
}

void CDlgOverlayControler::Clear()
{
	m_LayerList.DeleteAllItems();
}

void CDlgOverlayControler::InitList()
{
	InitColumn();
}

void CDlgOverlayControler::InitColumn()
{
	int nIndex = 0;
	m_LayerList.InsertColumn(nIndex, _T("Index"));
	m_LayerList.SetColumnWidth(nIndex++, 80);

	m_LayerList.InsertColumn(nIndex, _T("Layer Name"));
	m_LayerList.SetColumnWidth(nIndex++, 190);

	m_LayerList.InsertColumn(nIndex, _T("도형갯수"));
	m_LayerList.SetColumnWidth(nIndex++, 80);

	m_LayerList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES | LVS_EX_GRIDLINES);
}

void CDlgOverlayControler::AddRawWithLayerList()
{
	m_LayerList.DeleteAllItems();
	CLayerStructure* pLayer = NULL;
	for (INT i = 0; i < m_pMainOverlay->NumberOfLayer(); i++)
	{
		pLayer = m_pMainOverlay->GetLayer(i);
		if (pLayer != NULL)
			AddRaw(pLayer);
	}
}

void CDlgOverlayControler::AddRaw(CLayerStructure* pLayer)
{
	CString strTemp;
	UINT nIndex = m_LayerList.GetItemCount();
	strTemp.Format(_T("%d"), nIndex);
	m_LayerList.InsertItem(nIndex, strTemp);
	m_LayerList.SetItemText(nIndex, 1, pLayer->GetLayerName());

	UINT nNumberOfObject = pLayer->NumberOfObjectInLayer();
	strTemp.Format(_T("%d"), nNumberOfObject);
	m_LayerList.SetItemText(nIndex, 2, strTemp);
	m_LayerList.SetCheck(nIndex, pLayer->IsDisplay());


	UpdateData(FALSE);
}

/*
void CDlgOverlayControler::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	if (nStatus == SW_HIDE)
	{
		m_nGridDetail -= 20;
		if (m_nGridDetail < 0)
			m_nGridDetail = 0;
		m_pOverlayThread->GetOverlayView()->ReleaseSelectObject();
	}

	if (m_pMainOverlay == NULL)
		return;
	if (bShow == TRUE)
	{
		AddRawWithLayerList();
	}
}
*/

void CDlgOverlayControler::OnBnClickedBtnLayerSave()
{
	if (m_pMainOverlay == NULL)
		return;

	if (m_nSelectedItemIndex < 0 || m_nSelectedItemIndex >= m_pMainOverlay->NumberOfLayer())
	{
		AfxMessageBox(_T("저장될 레이어를 선택해 주십시오."));
		return;
	}
	if (m_pMainOverlay->Save(m_strSelectedLayerName.GetBuffer(0), m_strSelectedLayerName.GetLength(), m_nSelectedItemIndex) == FALSE)
	{
		//AfxMessageBox(_T("저장에 실패하였습니다."));
		return;
	}
	else
	{
		AfxMessageBox(_T("저장하였습니다."));
	}

}



void CDlgOverlayControler::OnBnClickedRefresh()
{
	AddRawWithLayerList();
}


void CDlgOverlayControler::OnBnClickedBtnLayerLoad()
{
	if (m_pMainOverlay == NULL)
		return;

	CString FileName;

	CHAR	lpPath[512] = { 0, };
	GetCurrentDirectory(512, lpPath);
	sprintf(lpPath, "%s%s", lpPath, FILEPATH_SAVE_FOLDER);

	CFileDialog dlgFile(TRUE, _T("*.xml"), NULL, OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR, _T("투명도 저장파일(*.xml)|*.xml|"), this);
	dlgFile.m_ofn.lpstrInitialDir = lpPath;// 강제로 폴더지정?

	if (dlgFile.DoModal() == IDOK)
	{
		FileName.Format(_T("%s"), dlgFile.GetPathName());
	}

	SetCurrentDirectory(lpPath);

	m_pMainOverlay->Load(FileName.GetBuffer(0), FileName.GetLength());
	// 	if(m_pMainOverlay->Load(FileName.GetBuffer(0),FileName.GetLength())==FALSE)
	//  	{		
	//  		AfxMessageBox(_T("실패하였습니다."));	
	//  	}	

	AddRawWithLayerList();
}

void CDlgOverlayControler::Refresh()
{
	Invalidate();
}
CWnd* CDlgOverlayControler::GetWnd()
{
	return (CWnd*)this;
}

/*
void CDlgOverlayControler::OnClose()
{
	if(m_pMainOverlay==NULL)
		return;

	m_pMainOverlay->DrawObjects();
	m_pMainOverlay->DrawEditlayer();
	m_pMainOverlay->DrawOnScreen();
	//-------------------------
	CC4IMapView *pView=(CC4IMapView*)m_pMainView;
	if(pView==NULL)
		return;
	pView->m_pOverlayThread->GetOverlayView()->ReleaseSelectObject();
//	pView->DrawSituation(FALSE,FALSE,FALSE, TRUE);
	CDialogEx::OnClose();
}
*/

/////////////////////////////////
// 군대부호는 임시로 막고 진행
// 유영호
/////////////////////////////////
/*
////////////////////////////////////////////////////////////////////////////////////////////
// 전술부호 연결
////////////////////////////////////////////////////////////////////////////////////////////
void CDlgOverlayControler::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
	//CC4IMapView *pView = (CC4IMapView *)m_pMainView; 
	//pView->OnSetSymbol();
	//m_bSymbolDraw = true;//!m_bSymbolDraw;
	////pView->m_bNewSymbol = true;
	//pView->OnMilitary();
	//pView->OnMenu80Ads();
	if (m_pOverlayThread != NULL)
	{
		m_bSymbolDraw = true;
		m_pOverlayThread->NewObject(OVERLAY_CREATE_NEW, OVERLAY::SYMBOL);
		SelectSymbol();
	}
}

// 특정 전술부호 연결
void CDlgOverlayControler::OnBnClickedButton2()
{
	// TODO: Add your control notification handler code here
	//CC4IMapView *pView = (CC4IMapView *)m_pMainView; 
	SpecialSymbol();
}



void CDlgOverlayControler::GetEditables(ICOPS::CPtrArrayIWarSymEditable& ret)
{
	BOOL		bRes;
	INT_PTR		inx, count;
	CTest_OverItem** ppItems;
	ICOPS::IWarSymEditablePtr	piEditable;
	
	CC4IMapDoc* pDoc = (CC4IMapDoc*)((CMainFrame*)AfxGetMainWnd())->GetActiveDocument();

	count = pDoc->m_parOverItems.GetSize();
	if (count <= 0) return;

	ppItems = (CTest_OverItem**)pDoc->m_parOverItems.GetData();
	for (inx = 0; inx < count; inx++)
	{
		piEditable = NULL;
		bRes = ppItems[inx]->QueryInterface(_T("IWarSymEditable"), (void**)&piEditable);
		if (bRes)
		{
			ret.Add(piEditable);
		}
	}
}


void CDlgOverlayControler::AddEditable(ICOPS::IWarSymEditable* piEditable, BOOL bCompleted)
{
	CTest_OverItemPtr	pItem;
	CC4IMapView* pView = (CC4IMapView*)m_pMainView;
	CC4IMapDoc* pDoc = (CC4IMapDoc*)((CMainFrame*)AfxGetMainWnd())->GetActiveDocument();

	INT_PTR		count;
	BOOL		bRes;

	pItem = NULL;
	bRes = piEditable->QueryInterface(_T("CTest_OverItem"), (void**)&pItem);

	if (bRes && pItem != NULL)
	{
		count = m_piEditTool->GetCoordinateCount();
		if (count > 0)
		{
			if (pItem->m_piSym != NULL) pItem->m_piSym->CacheSymbolImage(m_bCacheSymImg);
			pDoc->m_parOverItems.Add(pItem);
			pDoc->SetModifiedFlag(TRUE);
			pItem->AddRef();

			m_piEditTool->OnChangeEditableList();
		}
	}

	if (piEditable != NULL)  piEditable->Release();
}


void CDlgOverlayControler::DeleteEditable(IWarSymEditable* piEditable)
{
	BOOL		bRes;
	INT_PTR		inx, count;
	CTest_OverItem* pDel;
	CTest_OverItem* pItem;
	CC4IMapDoc* pDoc = (CC4IMapDoc*)((CMainFrame*)AfxGetMainWnd())->GetActiveDocument();

	ASSERT(piEditable != NULL);
	if (piEditable == NULL) return;

	pDel = NULL;
	bRes = piEditable->QueryInterface(_T("CTest_OverItem"), (void**)&pDel);
	if (!bRes || pDel == NULL)
	{
		ASSERT(FALSE);
		return;
	}

	count = pDoc->m_parOverItems.GetSize();
	for (inx = 0; inx < count; inx++)
	{
		pItem = pDoc->m_parOverItems.GetAt(inx);
		if (pItem == pDel)
		{
			pDoc->m_parOverItems.RemoveAt(inx);
			pDoc->SetModifiedFlag(TRUE);
			pDel->Release();
			pItem->Release();
			return;
		}
	}
	pDel->Release();
}


void CDlgOverlayControler::DoContextMenu(Gdiplus::Graphics* pGrpx, UINT nFlags, const CPoint& point, CPtrArrayIWarSymEditable* pSelected)
{
	//BOOL	bRes;
	//CMenu	menu, *pSubMenu;
	//CPoint	ptPop;

	//if (pSelected->GetSize()<=0) return;

	//bRes = menu.LoadMenu(IDR_POPUP);
	//if (!bRes) return;	// 메뉴 색성 오류

	//pSubMenu = menu.GetSubMenu(0);
	//ASSERT(pSubMenu!=NULL);
	//if (pSubMenu==NULL) return;

	//ptPop = point;
	//ClientToScreen(&ptPop);
	//pSubMenu->TrackPopupMenu(0, ptPop.x, ptPop.y, this);
}


void CDlgOverlayControler::Refresh()
{
	Invalidate();
}


CWnd* CDlgOverlayControler::GetWnd()
{
	return (CWnd*)this;
}


////////////////////////////////////////////////////////////////////////////////
// Editor operations

void CDlgOverlayControler::InitProjection()
{
	CC4IMapApp* pApp = (CC4IMapApp*) ::AfxGetApp();
	CC4IMapView* pView = (CC4IMapView*)m_pMainView;
	ICOPS::CAppContext* pAppCxt = pApp->m_pAppCxt;
	ICOPS::SGeoCoord2	gpC;

	BOOL	bRes;
	bRes = pAppCxt->CreateObject(_T("ICOPS_Framework"), _T(".dll"), _T("CMapProjectionNone"), _T("IMapProjection"), (void**)&m_piMapProj);
	if (!bRes)
	{
		ICOPS::CLog::AddLog(__FILE__, __LINE__, ICOPS::CLog::Log_Error, _T("좌표투영 객채 생성 실패 : CMapProjectionNone / IMapProjection"));
		return;
	}

	// 지도 중심좌표 설정
	gpC.lon = 128.0;
	gpC.lat = 37.0;

	m_piMapProj->SetRadius(EARTH_RADIUS);
	m_piMapProj->SetMapScale(1000000);
	m_piMapProj->SetZoomFactor(1.0f);
	m_piMapProj->SetCenter(&gpC);
}


void CDlgOverlayControler::InitSymbolEditTool()
{
	CC4IMapView* pView = (CC4IMapView*)m_pMainView;
	CC4IMapApp* pApp = (CC4IMapApp*) ::AfxGetApp();
	ICOPS::CAppContext* pAppCxt = pApp->m_pAppCxt;
	BOOL	bRes;

	bRes = pAppCxt->CreateObject(_T("IWarSymEditTool"), (void**)&m_piEditTool);
	if (!bRes)
	{
		ICOPS::CLog::AddLog(__FILE__, __LINE__, ICOPS::CLog::Log_Error, _T("IWarSymEditTool 인터페이스를 구현하는 사용자정의 객체 생성 실패"));
	}
	else
	{
		m_piMapProj->SetZoomFactor(1.0f);
		m_piMapProj->SetMapScale(1.0f);
		m_rMPW = 1.0f;

		// Initialize IWarSymEditTool component object
		m_piEditTool->SetEditOwner(dynamic_cast<ICOPS::IWarSymEditOwner*>(m_pEditOwner));
		m_piEditTool->SetMapProjection(m_piMapProj);
		m_piEditTool->SetMeterPerPixel(m_rMPW);
		m_piEditTool->SnapToGrid(FALSE);

		m_piEditTool->SetEditingMode(RefreshOnChange);	// 부호 작도중 부호 변경시마다 화면 갱신
		//m_piEditTool->SetEditingMode(UseInverseDraw);	// 부호 작도중 부호 변경시 역상 도시 사용하여 rubber banding 기능 수행
	}
}


void CDlgOverlayControler::DrawOverItems(Gdiplus::Graphics* pGrpx)
{
	CCoord Coord;
	CCoordMgr* CoordMgr = GP_COORDMGR;

	INT_PTR		inx, count, bufSize;
	CTest_OverItem** ppItems, * pItem;
	IMapProjection* piMapProj = nullptr;
	CC4IMapDoc* pDoc = (CC4IMapDoc*)((CMainFrame*)AfxGetMainWnd())->GetActiveDocument();
	CC4IMapView* pView = (CC4IMapView*)m_pMainView;
		
	Gdiplus::PointF* pWp;
	const ICOPS::SGeoCoord2* pGp;
	ICOPS::CTypedBuffer<Gdiplus::PointF>	bufWp;
	Gdiplus::Color	clrCtrl(70, 0, 0, 0);

	count = pDoc->m_parOverItems.GetSize();
	if (count <= 0) return;

	ppItems = (CTest_OverItem**)pDoc->m_parOverItems.GetData();
	for (inx = 0; inx < count; inx++)
	{
		pItem = ppItems[inx];

		bufSize = pItem->GetCoordCount();
		if (bufSize > 0)
		{
			pWp = bufWp.SetCount(bufSize);
			pGp = pItem->GetCoordData(piMapProj);
			m_piMapProj->Forward(pGp, pWp, bufSize);
			pItem->m_piSym->Draw(pGrpx, pWp, bufSize, m_rMPW);
		}
	}
}


void CDlgOverlayControler::ApplyDrawModeToAll()
{
	INT_PTR		inx, count;
	CTest_OverItem** ppItems;
	
	CC4IMapDoc* pDoc = (CC4IMapDoc*)((CMainFrame*)AfxGetMainWnd())->GetActiveDocument();
	CC4IMapView* pView = (CC4IMapView*)m_pMainView;
	SWarSymDrawMode* pSymDrawMode;

	count = pDoc->m_parOverItems.GetSize();
	if (count <= 0) return;

	ppItems = (CTest_OverItem**)pDoc->m_parOverItems.GetData();
	for (inx = 0; inx < count; inx++)
	{
		pSymDrawMode = ppItems[inx]->m_piSym->GetDrawMode();
		*pSymDrawMode = pView->m_sDrawMode;
		ppItems[inx]->m_piSym->InvalidateSymbolImage();
	}
}


// command process

void CDlgOverlayControler::SelectSymbol()
{
	BOOL	bRes;
	CTest_OverItem* pItem;
	CC4IMapApp* pApp = (CC4IMapApp*) ::AfxGetApp();

	ICOPS::CAppContext* pAppCxt = pApp->m_pAppCxt;
	ICOPS::IWarSymFactoryPtr	piSymFac;
	ICOPS::IWarSymbolPtr		piSym;
	ICOPS::IWarSymEditablePtr	piSymE;
	ICOPS::IWarSymPropertyEditorPtr		piPrpEditor;
	CC4IMapView* pView = (CC4IMapView*)m_pMainView;

	bRes = pAppCxt->GetStockObject(_T("IWarSymFactory"), (void**)&piSymFac);
	if (!bRes)
	{
		ICOPS::CLog::AddLog(__FILE__, __LINE__, ICOPS::CLog::Log_Error, _T("군대부호관리 객체 얻기 실패"));
		return;
	}

	bRes = piSymFac->CreatePropertyEditor(&piPrpEditor);
	if (!bRes)
	{
		ICOPS::CLog::AddLog(__FILE__, __LINE__, ICOPS::CLog::Log_Error, _T("군대부호 편집 컴포넌트 객체 얻기 실패"));
		return;
	}


	piPrpEditor->SetWarSymFactory(piSymFac);
	//piPrpEditor->NeedModifier(FALSE);	// 수식정보 설정 탭을 사용하지 않으려면 FALSE로 설정
	bRes = piPrpEditor->OpenSymbolSelector(m_strLastSymCode, this, &piSym);
	if (bRes)
	{
		m_strLastSymCode = piSym->GetSymbolCode();
		pItem = new CTest_OverItem;
		pItem->AddRef();
		pItem->m_piSym = piSym;

		ICOPS::IWarSymView* piSymView;
		ICOPS::CwsColorset* pColorset;
		piSymView = piSym->GetSymbolView();
		if (piSymView != NULL)
		{
			pColorset = piSymView->GetColorset();
			if (pColorset != NULL)
			{
				//pColorset->SetIconColors(&Gdiplus::Color(255, 255, 255, 255));
				pColorset->SetNormalTextColor(Gdiplus::Color(255, 0, 0, 0));		// 레이블 문자 색상
				pColorset->SetInverseTextColor(Gdiplus::Color(0, 255, 255, 255));	// 레이블 문자 배경을 투명하게
			}
			//piSymView->SetFontSizeScale(1.0f);	// 레이블 문자 크기
			//piSymView->SetLineWidthScale(2.0f);	// 선 굵기
		}

		bRes = pItem->QueryInterface(_T("IWarSymEditable"), (void**)&piSymE);
		if (bRes)
			m_piEditTool->SetAddingSymbol(piSymE);
	}
}

void CDlgOverlayControler::OnEditProperty()
{
	CString		strSymCode;
	BOOL		bRes;
	CTest_OverItem* pItem;
	ICOPS::IWarSymEditable** ppSelected;
	ICOPS::IWarSymPropertyEditor* piPrpEditor = NULL;

	CC4IMapApp* pApp = (CC4IMapApp*) ::AfxGetApp();
	ICOPS::CAppContext* pAppCxt = pApp->m_pAppCxt;
	ICOPS::IWarSymFactory* piSymFac;

	// 선택된 도형 얻기
	ppSelected = m_piEditTool->GetSelected();
	if (ppSelected == NULL) return;	// 선택된 도형 없음

	pItem = NULL;
	//pItem = dynamic_cast<CTest_OverItem *>(ppSelected[0]);
	bRes = ppSelected[0]->QueryInterface(_T("CTest_OverItem"), (void**)&pItem);
	if (!bRes || pItem == NULL)
	{
		ASSERT(FALSE);
		return;	// 선택된 도형의 자료형이 다름
	}

	bRes = pAppCxt->GetStockObject(_T("IWarSymFactory"), (void**)&piSymFac);
	if (!bRes)
	{
		pItem->Release();
		return;
	}

	bRes = piSymFac->CreatePropertyEditor(&piPrpEditor);
	if (!bRes)
	{
		piSymFac->Release();
		return;
	}

	piPrpEditor->SetWarSymFactory(piSymFac);
	piPrpEditor->SetTargetSymbol(pItem->m_piSym);
	//piPrpEditor->NeedModifier(FALSE);	// 수식정보 설정 탭을 사용하지 않으려면 FALSE로 설정
	piPrpEditor->OpenEditor(_T("부호속성 설정"), this);

	pItem->Release();
	piPrpEditor->Release();
	piSymFac->Release();

	Invalidate();
}

void CDlgOverlayControler::OnEditDelete()
{
	// 선택된 도형 삭제
	m_piEditTool->DeleteSelected(TRUE);
	Invalidate();
}

void CDlgOverlayControler::SpecialSymbol()
{
	m_bSymbolDraw = TRUE;
	BOOL	bRes;
	CString strCode;
	CTest_OverItem* pItem;
	CC4IMapApp* pApp = (CC4IMapApp*) ::AfxGetApp();

	ICOPS::CAppContext* pAppCxt = pApp->m_pAppCxt;
	ICOPS::IWarSymFactoryPtr	piSymFac;
	ICOPS::IWarSymbolPtr		piSym;
	ICOPS::IWarSymEditablePtr	piSymE;
	ICOPS::IWarSymPropertyEditorPtr		piPrpEditor;
	ICOPS::IWarSymPrpTacSymbolPtr	piPrpSym;
	ICOPS::SWarSymDrawMode* pDM;

	bRes = pAppCxt->GetStockObject(_T("IWarSymFactory"), (void**)&piSymFac);
	if (!bRes)
	{
		ICOPS::CLog::AddLog(__FILE__, __LINE__, ICOPS::CLog::Log_Error, _T("군대부호관리 객체 얻기 실패"));
		return;
	}

	bRes = piSymFac->CreatePropertyEditor(&piPrpEditor);
	if (!bRes)
	{
		ICOPS::CLog::AddLog(__FILE__, __LINE__, ICOPS::CLog::Log_Error, _T("군대부호 편집 컴포넌트 객체 얻기 실패"));
		return;
	}

	piPrpEditor->SetWarSymFactory(piSymFac);
	strCode = "S-G-USAB-------";
	//piPrpEditor->NeedModifier(FALSE);	// 수식정보 설정 탭을 사용하지 않으려면 FALSE로 설정
	//bRes = piPrpEditor->OpenSymbolSelector(strCode, this, &piSym);
	bRes = piSymFac->CreateSymbol(strCode, &piSym);
	//pDM = piSym->GetDrawMode();
	if (bRes)
	{
		piSym->SetSymbolCode(strCode);
		pDM = piSym->GetDrawMode();
		piSym->GetPropInterface(_T("IWarSymPrpTacSymbol"), (void**)&piPrpSym);
		pItem = new CTest_OverItem;
		pItem->AddRef();
		pItem->m_piSym = piSym;

		ICOPS::IWarSymView* piSymView;
		ICOPS::CwsColorset* pColorset;
		piSymView = piSym->GetSymbolView();
		if (piSymView != NULL)
		{
			pColorset = piSymView->GetColorset();
			if (pColorset != NULL)
			{
				pDM->fillFrame = true;
				pDM->showFrame = true;
				piPrpSym->SetAffiliation('F');
				piPrpSym->SetFillFrame(true);

				pColorset->SetAffiliationColor(0, Gdiplus::Color(255, 255, 255, 255));
				//pColorset->SetIconColors(&Gdiplus::Color(255, 255, 255, 255));
				pColorset->SetNormalTextColor(Gdiplus::Color(255, 0, 0, 0));	// 레이블 문자 색상
				pColorset->SetInverseTextColor(Gdiplus::Color(0, 0, 0, 0));	// 레이블 문자 배경을 투명하게
			}
			//piSymView->SetFontSizeScale(1.0f);	// 레이블 문자 크기
			//piSymView->SetLineWidthScale(2.0f);	// 선 굵기
		}

		bRes = pItem->QueryInterface(_T("IWarSymEditable"), (void**)&piSymE);
		if (bRes)
			m_piEditTool->SetAddingSymbol(piSymE);
	}
}
*/


