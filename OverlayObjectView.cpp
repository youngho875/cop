
#include "pch.h"
#include "MainFrm.h"
#include "C4IMap.h"
#include "C4IMapView.h"
#include "OverlayObjectView.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// COverlayObjectView

COverlayObjectView::COverlayObjectView()
{
}

COverlayObjectView::~COverlayObjectView()
{
}

BEGIN_MESSAGE_MAP(COverlayObjectView, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_WM_PAINT()
	ON_WM_SETFOCUS()

	ON_COMMAND(ID_OVERLAYVIEW_OVERLAY_TOP, OnOverlayViewTop)
	ON_COMMAND(ID_OVERLAYVIEW_OVERLAY_UP, OnOverlayViewUp)
	ON_COMMAND(ID_OVERLAYVIEW_OVERLAY_DOWN, OnOverlayViewDown)
	ON_COMMAND(ID_OVERLAYVIEW_OVERLAY_BOTTOM, OnOverlayViewBottom)
	ON_COMMAND(ID_MENU_OVERLAY_NEW, OnOverlayViewNew)
	ON_COMMAND(ID_MENU_OVERLAY_OPEN, OnOverlayViewOpen)
	ON_COMMAND(ID_MENU_OVERLAY_SAVE, OnOverlayViewSave)
	ON_COMMAND(ID_OVERLAYVIEW_OVERLAY_DELETE, OnOverlayViewDelete)
	ON_WM_DESTROY()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWorkspaceBar 메시지 처리기

int COverlayObjectView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// 뷰를 만듭니다.
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS |
									TVS_CHECKBOXES | TVS_SHOWSELALWAYS | TVS_DISABLEDRAGDROP;

	if (!m_wndOverlayObjectViewTree.Create(dwViewStyle, rectDummy, this, 4))
	{
		TRACE0("파일 뷰를 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}

	// 뷰 이미지를 로드합니다.
	m_OverlayObjectViewImages.Create(IDB_OVERLAYVIEW_TREE, 15, 0, RGB(255, 0, 255));
	m_wndOverlayObjectViewTree.SetImageList(&m_OverlayObjectViewImages, TVSIL_NORMAL);

	m_wndOverlayObjectViewToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_OVERLAYVIEW_TOOLBAR);
	m_wndOverlayObjectViewToolBar.LoadToolBar(IDR_OVERLAYVIEW_TOOLBAR, 0, 0, TRUE /* 잠금 */);

	OnChangeVisualStyle();

	m_wndOverlayObjectViewToolBar.SetPaneStyle(m_wndOverlayObjectViewToolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);

	m_wndOverlayObjectViewToolBar.SetPaneStyle(m_wndOverlayObjectViewToolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));

	m_wndOverlayObjectViewToolBar.SetOwner(this);

	// 모든 명령은 부모 프레임이 아닌 이 컨트롤을 통해 라우팅됩니다.
	m_wndOverlayObjectViewToolBar.SetRouteCommandsViaFrame(FALSE);

	AdjustLayout();

	return 0;
}

void COverlayObjectView::OnDestroy()
{
	if(m_OverlayObjectViewImages)
		m_OverlayObjectViewImages.DeleteImageList();

	if(m_TreeImagesCheckBox)
		m_TreeImagesCheckBox.DeleteImageList();

	CDockablePane::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

void COverlayObjectView::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void COverlayObjectView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	return;
}

void COverlayObjectView::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	int cyTlb = m_wndOverlayObjectViewToolBar.CalcFixedLayout(FALSE, TRUE).cy;

	m_wndOverlayObjectViewToolBar.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndOverlayObjectViewTree.SetWindowPos(NULL, rectClient.left + 1, rectClient.top + cyTlb + 1, rectClient.Width() - 2, rectClient.Height() - cyTlb - 2, SWP_NOACTIVATE | SWP_NOZORDER);
}

void COverlayObjectView::OnPaint()
{
	CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

	CRect rectTree;
	m_wndOverlayObjectViewTree.GetWindowRect(rectTree);
	ScreenToClient(rectTree);

	rectTree.InflateRect(1, 1);
	dc.Draw3dRect(rectTree, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}

void COverlayObjectView::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);

	m_wndOverlayObjectViewTree.SetFocus();
}

void COverlayObjectView::OnChangeVisualStyle()
{
	m_wndOverlayObjectViewToolBar.CleanUpLockedImages();
	m_wndOverlayObjectViewToolBar.LoadBitmap(theApp.m_bHiColorIcons ? IDB_OVERLAYVIEW_TOOLBAR_24 : IDR_OVERLAYVIEW_TOOLBAR, 0, 0, TRUE /* 잠금 */);

	m_OverlayObjectViewImages.DeleteImageList();

	UINT uiBmpId = theApp.m_bHiColorIcons ? IDB_OVERLAYVIEW_TREE_24 : IDB_OVERLAYVIEW_TREE;

	CBitmap bmp;
	if (!bmp.LoadBitmap(uiBmpId))
	{
		TRACE(_T("비트맵을 로드할 수 없습니다. %x\n"), uiBmpId);
		ASSERT(FALSE);
		return;
	}

	BITMAP bmpObj;
	bmp.GetBitmap(&bmpObj);

	UINT nFlags = ILC_MASK;

	nFlags |= (theApp.m_bHiColorIcons) ? ILC_COLOR24 : ILC_COLOR4;

	m_OverlayObjectViewImages.Create(16, bmpObj.bmHeight, nFlags, 0, 0);
	m_OverlayObjectViewImages.Add(&bmp, RGB(255, 0, 255));

	m_wndOverlayObjectViewTree.SetImageList(&m_OverlayObjectViewImages, TVSIL_NORMAL);

	m_TreeImagesCheckBox.Create(IDB_TREE_CHECKBOX, 13, 4, ILC_MASK);
	m_wndOverlayObjectViewTree.SetImageList(&m_TreeImagesCheckBox, TVSIL_STATE);
}

// 맨 위로
void COverlayObjectView::OnOverlayViewTop()
{
	m_wndOverlayObjectViewTree.OnToolBarTop();
}

// 위로
void COverlayObjectView::OnOverlayViewUp()
{
	m_wndOverlayObjectViewTree.OnToolBarUp();
}

// 아래로
void COverlayObjectView::OnOverlayViewDown()
{
	m_wndOverlayObjectViewTree.OnToolBarDown();
}

// 맨 아래로
void COverlayObjectView::OnOverlayViewBottom()
{
	m_wndOverlayObjectViewTree.OnToolBarBottom();
}

// 투명도 생성
void COverlayObjectView::OnOverlayViewNew()
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();

	if(pFrame != NULL && pFrame->GetSafeHwnd() != NULL)
	{
		CC4IMapView* pView = (CC4IMapView*)pFrame->GetActiveView();
		if(pView)
		{
			pView->SendMessage(OVERLAY_VIEW_NEW_LAYER);
		}
	}
}

// 투명도 열기
void COverlayObjectView::OnOverlayViewOpen()
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();

	if(pFrame != NULL && pFrame->GetSafeHwnd() != NULL)
	{
		CC4IMapView* pView = (CC4IMapView*)pFrame->GetActiveView();
		if(pView)
		{
			pView->SendMessage(OVERLAY_VIEW_OPEN_LAYER);
		}
	}
}

// 투명도 저장
void COverlayObjectView::OnOverlayViewSave()
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();

	if(pFrame != NULL && pFrame->GetSafeHwnd() != NULL)
	{
		CC4IMapView* pView = (CC4IMapView*)pFrame->GetActiveView();
		if(pView)
		{
			pView->SendMessage(OVERLAY_VIEW_SAVE_LAYER);
		}
	}
}

// 투명도 삭제
void COverlayObjectView::OnOverlayViewDelete()
{
	m_wndOverlayObjectViewTree.OnToolBarDelete();
}

// 투명도 추가
void COverlayObjectView::AddOverlay(CString strName, CString strGuid, BOOL bCoordMap)
{
	ShowPane(TRUE, FALSE, TRUE);
	m_wndOverlayObjectViewTree.AddOverlayItem(strName, strGuid, bCoordMap);
}

// 도형 목록 추가
void COverlayObjectView::AddShapeList(CString strOverlayGuid, CString strShapeList)
{
	ShowPane(TRUE, FALSE, TRUE);
	m_wndOverlayObjectViewTree.AddShapeItems(strOverlayGuid, strShapeList);
}

// 도형 추가
void COverlayObjectView::AddShape(CString strOverlayGuid, CString strName, CString strGuid, BOOL bVisible)
{
	ShowPane(TRUE, FALSE, TRUE);
	m_wndOverlayObjectViewTree.AddShapeItem(strOverlayGuid, strName, strGuid, bVisible);
}

// 투명도 전체 삭제
void COverlayObjectView::RemoveOverlayAll()
{
	ShowPane(TRUE, FALSE, TRUE);
	m_wndOverlayObjectViewTree.RemoveAllOverlayObjItems();
}

// 투명도 선택 삭제
void COverlayObjectView::RemoveOverlay(CString strGuid)
{
	ShowPane(TRUE, FALSE, TRUE);
	m_wndOverlayObjectViewTree.RemoveOverlayItem(strGuid);
}

// 도형 전체 삭제
void COverlayObjectView::RemoveShapeAll(CString strOverlayGuid)
{
	ShowPane(TRUE, FALSE, TRUE);
	m_wndOverlayObjectViewTree.RemoveShapeAllItems(strOverlayGuid);
}

// 도형 선택 삭제
void COverlayObjectView::RemoveShape(CString strOverlayGuid, CString strShapeGuid)
{
	ShowPane(TRUE, FALSE, TRUE);
	m_wndOverlayObjectViewTree.RemoveShapeItem(strOverlayGuid, strShapeGuid);
}

// 투명도 선택
void COverlayObjectView::SelectOverlay(CString strOverlayGuid)
{
	ShowPane(TRUE, FALSE, TRUE);
	m_wndOverlayObjectViewTree.SelectOverlayItem(strOverlayGuid);
}

// 도형 선택
void COverlayObjectView::SelectShape(CString strOverlayGuid, CString strShapeGuid)
{
	ShowPane(TRUE, FALSE, TRUE);
	m_wndOverlayObjectViewTree.SelectShapeItem(strOverlayGuid, strShapeGuid);
}

// 투명도 체크 설정
void COverlayObjectView::SetCheckOverlay(CString strOverlayGuid, BOOL bCheck)
{
	m_wndOverlayObjectViewTree.SetCheckOverlayItem(strOverlayGuid, bCheck);
}

// 도형 체크 설정
void COverlayObjectView::SetCheckShape(CString strOverlayGuid, CString strShapeGuid, BOOL bCheck)
{
	m_wndOverlayObjectViewTree.SetCheckShapeItem(strOverlayGuid, strShapeGuid, bCheck);
}

// 도형 앞으로
void COverlayObjectView::MoveForwardShape(CString strOverlayGuid, CString strShapeGuid)
{
	m_wndOverlayObjectViewTree.MoveForwardShapeItem(strOverlayGuid, strShapeGuid);
}

// 도형 뒤로
void COverlayObjectView::MoveBackwardShape(CString strOverlayGuid, CString strShapeGuid)
{
	m_wndOverlayObjectViewTree.MoveBackwardShapeItem(strOverlayGuid, strShapeGuid);
}

// 도형 맨 위로
void COverlayObjectView::MoveTopShape(CString strOverlayGuid, CString strShapeGuid)
{
	m_wndOverlayObjectViewTree.MoveTopShapeItem(strOverlayGuid, strShapeGuid);
}

// 도형 맨 아래로
void COverlayObjectView::MoveBottomShape(CString strOverlayGuid, CString strShapeGuid)
{
	m_wndOverlayObjectViewTree.MoveBottomShapeItem(strOverlayGuid, strShapeGuid);
}

// 투명도 앞으로
void COverlayObjectView::MoveForwardOverlay(CString strOverlayGuid)
{
	m_wndOverlayObjectViewTree.MoveForwardOverlayItem(strOverlayGuid);
}

// 투명도 뒤로
void COverlayObjectView::MoveBackwardOverlay(CString strOverlayGuid)
{
	m_wndOverlayObjectViewTree.MoveBackwardOverlayItem(strOverlayGuid);
}

// 투명도 맨 위로
void COverlayObjectView::MoveTopOverlay(CString strOverlayGuid)
{
	m_wndOverlayObjectViewTree.MoveTopOverlayItem(strOverlayGuid);
}

// 투명도 맨 아래로
void COverlayObjectView::MoveBottomOverlay(CString strOverlayGuid)
{
	m_wndOverlayObjectViewTree.MoveBottomOverlayItem(strOverlayGuid);
}

// 투명도 좌표계 설정
void COverlayObjectView::SetOverlayCoordMap(CString strOverlayGuid, BOOL bCoordMap)
{
	m_wndOverlayObjectViewTree.SetOverlayCoordMap(strOverlayGuid, bCoordMap);
}

// 도형 명칭 변경
void COverlayObjectView::SetShapeName(CString strOverlayGuid, CString strShapeGuid, CString strShapeName)
{
	m_wndOverlayObjectViewTree.SetShapeName(strOverlayGuid, strShapeGuid, strShapeName);
}

// 투명도 명칭 변경
void COverlayObjectView::SetLayerName(CString strOverlayGuid, CString strLayerName)
{
	m_wndOverlayObjectViewTree.SetLayerName(strOverlayGuid, strLayerName);
}

