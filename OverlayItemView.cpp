// 이 MFC 샘플 소스 코드는 MFC Microsoft Office Fluent 사용자 인터페이스("Fluent UI")를 
// 사용하는 방법을 보여 주며, MFC C++ 라이브러리 소프트웨어에 포함된 
// Microsoft Foundation Classes Reference 및 관련 전자 문서에 대해 
// 추가적으로 제공되는 내용입니다.  
// Fluent UI를 복사, 사용 또는 배포하는 데 대한 사용 약관은 별도로 제공됩니다.  
// Fluent UI 라이선싱 프로그램에 대한 자세한 내용은 
// http://msdn.microsoft.com/officeui를 참조하십시오.
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

#include "pch.h"
#include "MainFrm.h"
#include "OverlayItemView.h"
#include "Resource.h"
#include "C4IMap.h"
#include "C4IMapDoc.h"
#include "C4IMapView.h"

class COverlayItemViewMenuButton : public CMFCToolBarMenuButton
{
	friend class COverlayItemView;

	DECLARE_SERIAL(COverlayItemViewMenuButton)

public:
	COverlayItemViewMenuButton(HMENU hMenu = NULL) : CMFCToolBarMenuButton((UINT)-1, hMenu, -1)
	{
	}

	virtual void OnDraw(CDC* pDC, const CRect& rect, CMFCToolBarImages* pImages, BOOL bHorz = TRUE,
		BOOL bCustomizeMode = FALSE, BOOL bHighlight = FALSE, BOOL bDrawBorder = TRUE, BOOL bGrayDisabledButtons = TRUE)
	{
		pImages = CMFCToolBar::GetImages();

		CAfxDrawState ds;
		pImages->PrepareDrawImage(ds);

		CMFCToolBarMenuButton::OnDraw(pDC, rect, pImages, bHorz, bCustomizeMode, bHighlight, bDrawBorder, bGrayDisabledButtons);

		pImages->EndDrawImage(ds);
	}
};

IMPLEMENT_SERIAL(COverlayItemViewMenuButton, CMFCToolBarMenuButton, 1)

//////////////////////////////////////////////////////////////////////
// 생성/소멸
//////////////////////////////////////////////////////////////////////

COverlayItemView::COverlayItemView()
{
	//m_pMsgBaseOverList = NULL;
	m_nlistCtrl_DrawItem_Rs_cursor = -1;
}

COverlayItemView::~COverlayItemView()
{
}

BEGIN_MESSAGE_MAP(COverlayItemView, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_COP_ADD_MEMBER_FUNCTION, OnCOPAddMemberFunction)
	ON_COMMAND(ID_COP_ADD_MEMBER_VARIABLE, OnCOPAddMemberVariable)
	ON_COMMAND(ID_COP_DEFINITION, OnCOPDefinition)
	ON_COMMAND(ID_COP_PROPERTIES, OnCOPProperties)
	ON_COMMAND(ID_NEW_FOLDER_COP, OnNewFolder_COP)
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	ON_COMMAND_RANGE(ID_SORTING_GROUPBYTYPE_COP, ID_SORTING_SORTBYACCESS_COP, OnSort)
	ON_UPDATE_COMMAND_UI_RANGE(ID_SORTING_GROUPBYTYPE_COP, ID_SORTING_SORTBYACCESS_COP, OnUpdateSort)

	ON_NOTIFY(NM_CLICK, IDC_OVERLAY_LISTCTRL_ITEM, &COverlayItemView::OnNMClickListDrawitem)
	ON_NOTIFY(NM_DBLCLK, IDC_OVERLAY_LISTCTRL_ITEM, &COverlayItemView::OnNMDblclkListDrawitem)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_OVERLAY_LISTCTRL_ITEM, &COverlayItemView::OnLvnItemchangedListDrawitem)
	ON_NOTIFY(NM_RCLICK, IDC_OVERLAY_LISTCTRL_ITEM, &COverlayItemView::OnNMRClickListDrawitem)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_OVERLAY_LISTCTRL_ITEM, &COverlayItemView::OnNMCustomdrawListDrawitem)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_OVERLAY_LISTCTRL_ITEM, &COverlayItemView::OnLvnGetdispinfoOwnerDrawitem)

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CClassView 메시지 처리기

int COverlayItemView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();
	
	// 뷰를 만듭니다.
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | TVS_CHECKBOXES;

	if(!m_wndView.Create(
		dwViewStyle,
		rectDummy,
		this,
		IDC_OVERLAY_LISTCTRL_ITEM))
	{
		TRACE0("항목 뷰를 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}


	DWORD dwExStyle = m_wndView.GetExtendedStyle();
	m_wndView.SetExtendedStyle(dwExStyle | LVS_EX_CHECKBOXES | LVS_EX_BORDERSELECT | LVS_EX_HEADERDRAGDROP | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER | TVS_EX_MULTISELECT/*| LVS_OWNERDATA*/);
	//m_wndView.InsertColumn(0, _T(""), LVCFMT_LEFT, 25);
	m_wndView.InsertColumn(1, _T("명칭"), LVCFMT_LEFT, 150);
	m_wndView.InsertColumn(2, _T("활성"), LVCFMT_LEFT, 50);
	m_wndView.InsertColumn(3, _T("잠금"), LVCFMT_LEFT, 50);
	m_wndView.InsertColumn(4, _T("생성일시"), LVCFMT_LEFT, 150);
	m_wndView.EnableGroupView(TRUE);
	m_wndView.ModifyStyle(LVS_TYPEMASK, LVS_REPORT);

	// Owner 
	ListView_SetItemCount(m_wndView,100000); 

	// 이미지를 로드합니다.
	m_wndToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_SORT_COP);
	m_wndToolBar.LoadToolBar(IDR_SORT_COP, 0, 0, TRUE /* 잠금 */);

	OnChangeVisualStyle();

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);
	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));

	m_wndToolBar.SetOwner(this);

	// 모든 명령은 부모 프레임이 아닌 이 컨트롤을 통해 라우팅됩니다.
	m_wndToolBar.SetRouteCommandsViaFrame(FALSE);

	CMenu menuSort;
	menuSort.LoadMenu(IDR_POPUP_SORT_COP);

	m_wndToolBar.ReplaceButton(ID_SORT_MENU_COP, COverlayItemViewMenuButton(menuSort.GetSubMenu(0)->GetSafeHmenu()));

	COverlayItemViewMenuButton* pButton =  DYNAMIC_DOWNCAST(COverlayItemViewMenuButton, m_wndToolBar.GetButton(0));

	if (pButton != NULL)
	{
		pButton->m_bText = FALSE;
		pButton->m_bImage = TRUE;
		pButton->SetImage(GetCmdMgr()->GetCmdImage(m_nCurrSort));
		pButton->SetMessageWnd(this);
	}

	// 정적 트리 뷰 데이터를 더미 코드로 채웁니다.
	FillClassView();
#ifdef _DEBUG
	TRACE("[CREATE VIEW] COverlayItemView OnCreate\n");
#endif // _DEBUG
	return 0;
}

void COverlayItemView::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void COverlayItemView::FillClassView()
{
	
}

void COverlayItemView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CListCtrl* pWndList = (CListCtrl*)&m_wndView;
	ASSERT_VALID(pWndList);

	if (pWnd != pWndList)
	{
		CDockablePane::OnContextMenu(pWnd, point);
		return;
	}

	if (point != CPoint(-1, -1))
	{
		// 클릭한 항목을 선택합니다.
		CPoint ptList = point;
		pWndList->ScreenToClient(&ptList);

		UINT flags = 0;
		int nHit = pWndList->HitTest(ptList, &flags);
		if (nHit != -1)
		{
			pWndList->SetHotItem(nHit);
		}
	}

	pWndList->SetFocus();
	CMenu menu;
	if(menu.LoadMenu(IDR_POPUP_SORT_COP))
	{
		CMenu* pSumMenu = menu.GetSubMenu(0);

		if (AfxGetMainWnd()->IsKindOf(RUNTIME_CLASS(CMDIFrameWndEx)))
		{
			CMFCPopupMenu* pPopupMenu = new CMFCPopupMenu;

			if (!pPopupMenu->Create(this, point.x, point.y, (HMENU)pSumMenu->m_hMenu, FALSE, TRUE))
				return;

			((CMDIFrameWndEx*)AfxGetMainWnd())->OnShowPopupMenu(pPopupMenu);
			UpdateDialogControls(this, FALSE);
		}
	}
}

void COverlayItemView::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	int cyTlb = m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;

	m_wndToolBar.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndView.SetWindowPos(NULL, rectClient.left + 1, rectClient.top + cyTlb + 1, rectClient.Width() - 2, rectClient.Height() - cyTlb - 2, SWP_NOACTIVATE | SWP_NOZORDER);
	
}

BOOL COverlayItemView::PreTranslateMessage(MSG* pMsg)
{
	return CDockablePane::PreTranslateMessage(pMsg);
}

void COverlayItemView::OnSort(UINT id)
{
	if (m_nCurrSort == id)
	{
		return;
	}

	m_nCurrSort = id;

	COverlayItemViewMenuButton* pButton =  DYNAMIC_DOWNCAST(COverlayItemViewMenuButton, m_wndToolBar.GetButton(0));

	if (pButton != NULL)
	{
		pButton->SetImage(GetCmdMgr()->GetCmdImage(id));
		m_wndToolBar.Invalidate();
		m_wndToolBar.UpdateWindow();
	}
}

void COverlayItemView::OnUpdateSort(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(pCmdUI->m_nID == m_nCurrSort);
}

void COverlayItemView::OnCOPAddMemberFunction()
{
	AfxMessageBox(_T("멤버 함수 추가..."));
}

void COverlayItemView::OnCOPAddMemberVariable()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
}

void COverlayItemView::OnCOPDefinition()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
}

void COverlayItemView::OnCOPProperties()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
}

void COverlayItemView::OnNewFolder_COP()
{
	AfxMessageBox(_T("새 폴더..."));
}

void COverlayItemView::OnPaint()
{
	CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

	CRect rectList;
	m_wndView.GetWindowRect(rectList);
	ScreenToClient(rectList);

	rectList.InflateRect(1, 1);
	dc.Draw3dRect(rectList, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}

void COverlayItemView::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);

	m_wndView.SetFocus();
}

void COverlayItemView::OnChangeVisualStyle()
{
	m_ViewImages.DeleteImageList();

	UINT uiBmpId = theApp.m_bHiColorIcons ? IDB_COP_VIEW_24 : IDB_COP_VIEW;

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

	m_ViewImages.Create(16, bmpObj.bmHeight, nFlags, 0, 0);
	m_ViewImages.Add(&bmp, RGB(255, 0, 0));

	m_wndView.SetImageList(&m_ViewImages, TVSIL_NORMAL);

	m_wndToolBar.CleanUpLockedImages();
	m_wndToolBar.LoadBitmap(theApp.m_bHiColorIcons ? IDB_SORT_COP_24 : IDR_SORT_COP, 0, 0, TRUE /* 잠금 */);
}

//void COverlayItemView::SetMsgMgr(CMapXMsgProcBase_Overlay_List * pMsgBaseOverList) 
//{
//	m_pMsgBaseOverList = pMsgBaseOverList;
//}

void COverlayItemView::RefreshMsgList()
{

	//if(m_pMsgBaseOverList)
	{
		/*CComboBox *ComboDrawType = NULL;	// dummy
		m_pMsgBaseOverList->RefreshMsgList_DrawItem(&m_wndView, ComboDrawType);*/

		//m_pMsgBaseOverList->RefreshMsgList_DrawItem(&m_wndView);
	}
}

CListCtrl * COverlayItemView::GetView()
{
	return &m_wndView;
}

void COverlayItemView::OnNMClickListDrawitem(NMHDR *pNMHDR, LRESULT *pResult)
{    
	//리스트에서 하나만 선택했을 경우 기존 선택되어 있는것이 해제가 되지 않는 버그가 있어 주석처리(14-04-03)
	//	m_pMsgBaseOverList->SelectActiveShapes();

	//리스트에서 하나만 선택했을 경우 기존선택되어 있는 것은 해제하기 위해 추가(14-04-03)	
	//if(m_pMsgBaseOverList)
	{
		//m_pMsgBaseOverList->ShapeSelectFromList(); 
		m_nlistCtrl_DrawItem_Rs_cursor = -1;
	}
	*pResult = 0; 
}     

void COverlayItemView::OnNMDblclkListDrawitem(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	//if(m_pMsgBaseOverList)
	{
		//LONG nLayer = m_pMsgBaseOverList->moveShapePos(pNMLV->iItem);
		if(nLayer>=1)
		{
			//	m_pMsgBaseOverList->SetActiveMsg(nLayer);  //더블클릭 레이어 최상위로,, 불편해서 막음
			//	RefreshMsgList_All();                      //더블클릭 레이어 최상위로,, 불편해서 막음

			CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
			if(pFrame)
			{
				CC4IMapView* pView = (CC4IMapView*)pFrame->GetActiveView();
				if(pView)
				{
					pView->Invalidate(FALSE);
				}
			}
		}
		//m_pMsgBaseOverList->ShapeSelectFromList();
	}
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	if(pFrame)
	{
		CC4IMapView* pView = (CC4IMapView*)pFrame->GetActiveView();
		if(pView)
		{
			//pView->SendMessage(WM_USERLAYERLIST_UPDATEINDEX, (WPARAM)"");
		}
	}

	*pResult = 0;
}

// 리스트 컨트롤 Insert 속도 빠르게.. 리스트컨트롤 속성 : owner True  로 변경
void COverlayItemView::OnLvnGetdispinfoOwnerDrawitem(NMHDR *pNMHDR, LRESULT *pResult)
{
//	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
//	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.   
//
//	// 현재는 그냥 리턴
//	*pResult = 0;
//	return;
//
//	if(pDispInfo->item.mask & LVIF_TEXT) 
//	{
//		
//		if(m_pMsgBaseList)
//			m_pMsgBaseList->CopDrawItemOwnerSync(pDispInfo);
//
//#ifdef _DEBUG
//		TRACE("Owner insert [%d][%d][%d][%s]\n", pDispInfo->item.iItem, pDispInfo->item.iSubItem, (int)pDispInfo->item.lParam, pDispInfo->item.pszText);
//#endif // _DEBUG
//	}
//	else if(pDispInfo->item.mask & LVIF_IMAGE)
//	{
//		
//		if(m_pMsgBaseList)
//			m_pMsgBaseList->CopDrawItemOwnerSync(pDispInfo);
//	}
//	/*else if(pDispInfo->item.mask & LVIF_GROUPID) 
//	{
//		pDispInfo->item.mask |= LVIF_GROUPID;
//		pDispInfo->item.stateMask = LVIF_GROUPID;
//		if(m_pMsgBaseList)
//			m_pMsgBaseList->CopDrawItemOwnerSync(pDispInfo);
//	}*/
//
//
//	*pResult = 0;
}

void COverlayItemView::OnLvnItemchangedListDrawitem(NMHDR *pNMHDR, LRESULT *pResult)
{     
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	if(pNMLV->uChanged==LVIF_STATE)
	{
		UINT nOldCheck = pNMLV->uOldState & LVIS_STATEIMAGEMASK;
		UINT nNewCheck = pNMLV->uNewState & LVIS_STATEIMAGEMASK;
		if(nOldCheck && nNewCheck && nOldCheck != nNewCheck )
		{
			BOOL bShow=FALSE;
			if(((pNMLV->uNewState&LVIS_STATEIMAGEMASK)>>12)==2)	bShow = TRUE; //check

			//if(m_pMsgBaseOverList)
			{
				//m_pMsgBaseOverList->SetShapeView(pNMLV->iItem, bShow);
				CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
				if(pFrame)
				{
					CC4IMapView* pView = (CC4IMapView*)pFrame->GetActiveView();
					if(pView)
					{
						pView->Invalidate(FALSE);
					}
				}
			}
		}

	}
	*pResult = 0;               
}


void COverlayItemView::OnNMRClickListDrawitem(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	//if(m_pMsgBaseOverList)
	{
		//m_pMsgBaseOverList->SelectActiveShapes();
		//m_pMsgBaseOverList->RClickMenu();
		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		if(pFrame)
		{
			CC4IMapView* pView = (CC4IMapView*)pFrame->GetActiveView();
			if(pView)
			{
				pView->Invalidate(FALSE);
			}
		}
	}
	*pResult = 0;
}
void COverlayItemView::OnNMCustomdrawListDrawitem(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	//if(m_pMsgBaseOverList)
	//	m_pMsgBaseOverList->listEvent_CustomDraw(pNMHDR, pResult);
} 
