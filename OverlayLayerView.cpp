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
#include "OverlayLayerView.h"
#include "Resource.h"
#include "C4IMap.h"
#include "C4IMapDoc.h"
#include "C4IMapView.h"

class COverlayLayerViewMenuButton : public CMFCToolBarMenuButton
{
	friend class COverlayLayerView;

	DECLARE_SERIAL(COverlayLayerViewMenuButton)

public:
	COverlayLayerViewMenuButton(HMENU hMenu = NULL) : CMFCToolBarMenuButton((UINT)-1, hMenu, -1)
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

IMPLEMENT_SERIAL(COverlayLayerViewMenuButton, CMFCToolBarMenuButton, 1)

//////////////////////////////////////////////////////////////////////
// 생성/소멸
//////////////////////////////////////////////////////////////////////

COverlayLayerView::COverlayLayerView()
{
	m_nCurrSort = ID_SORTING_GROUPBYTYPE_COP;
	m_pMsgBaseOvlList = NULL;
}

COverlayLayerView::~COverlayLayerView()
{
}

BEGIN_MESSAGE_MAP(COverlayLayerView, CDockablePane)
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

	ON_NOTIFY(NM_CLICK, IDC_OVERLAY_LISTCTRL_LAYER, &COverlayLayerView::OnNMClickListOverDlg)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_OVERLAY_LISTCTRL_LAYER, &COverlayLayerView::OnLvnItemchangedListOverDlg)
	ON_NOTIFY(NM_DBLCLK, IDC_OVERLAY_LISTCTRL_LAYER, &COverlayLayerView::OnNMDblclkListOverDlg)
	ON_NOTIFY(NM_RCLICK, IDC_OVERLAY_LISTCTRL_LAYER, &COverlayLayerView::OnNMRClickListOverDlg)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CClassView 메시지 처리기

int COverlayLayerView::OnCreate(LPCREATESTRUCT lpCreateStruct)
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
		IDC_OVERLAY_LISTCTRL_LAYER))
	{
		TRACE0("항목 뷰를 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}

	DWORD dwExStyle = m_wndView.GetExtendedStyle();
	m_wndView.SetExtendedStyle(dwExStyle | LVS_EX_CHECKBOXES | LVS_EX_BORDERSELECT | LVS_EX_HEADERDRAGDROP | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER | LVS_OWNERDATA);
	//m_wndView.InsertColumn(0, _T(""), LVCFMT_LEFT, 25);
	m_wndView.InsertColumn(1, _T("명칭"), LVCFMT_LEFT, 190);
	m_wndView.InsertColumn(2, _T("활성"), LVCFMT_CENTER, 55);
	//m_wndView.InsertColumn(3, _T("매핑"), LVCFMT_CENTER, 45);
	m_wndView.InsertColumn(3, _T("생성일시"), LVCFMT_CENTER, 45);
	m_wndView.EnableGroupView(TRUE);
	m_wndView.ModifyStyle(LVS_TYPEMASK, LVS_REPORT);

	SetDlgItemText(IDC_OVERLAY_LISTCTRL_LAYER, _T("10"));
	ListView_SetExtendedListViewStyle( m_wndView.GetSafeHwnd() , LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_ONECLICKACTIVATE | LVS_EX_CHECKBOXES | LVS_EX_DOUBLEBUFFER);


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

	m_wndToolBar.ReplaceButton(ID_SORT_MENU_COP, COverlayLayerViewMenuButton(menuSort.GetSubMenu(0)->GetSafeHmenu()));

	COverlayLayerViewMenuButton* pButton =  DYNAMIC_DOWNCAST(COverlayLayerViewMenuButton, m_wndToolBar.GetButton(0));

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
	TRACE("[CREATE VIEW] COverlayLayerView OnCreate\n");
#endif // _DEBUG
	return 0;
}

void COverlayLayerView::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void COverlayLayerView::FillClassView()
{
	
}

void COverlayLayerView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	HINSTANCE hOrgInstance = AfxGetResourceHandle();
	AfxSetResourceHandle( AfxGetInstanceHandle());

	CListCtrl* pWndList = (CListCtrl*)&m_wndView;
	ASSERT_VALID(pWndList);

	if (pWnd != pWndList)
	{
		CDockablePane::OnContextMenu(pWnd, point);
		AfxSetResourceHandle(hOrgInstance);
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
	AfxSetResourceHandle(hOrgInstance);
}

void COverlayLayerView::AdjustLayout()
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

BOOL COverlayLayerView::PreTranslateMessage(MSG* pMsg)
{
	return CDockablePane::PreTranslateMessage(pMsg);
}

void COverlayLayerView::OnSort(UINT id)
{
	if (m_nCurrSort == id)
	{
		return;
	}

	m_nCurrSort = id;

	COverlayLayerViewMenuButton* pButton =  DYNAMIC_DOWNCAST(COverlayLayerViewMenuButton, m_wndToolBar.GetButton(0));

	if (pButton != NULL)
	{
		pButton->SetImage(GetCmdMgr()->GetCmdImage(id));
		m_wndToolBar.Invalidate();
		m_wndToolBar.UpdateWindow();
	}
}

void COverlayLayerView::OnUpdateSort(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(pCmdUI->m_nID == m_nCurrSort);
}

void COverlayLayerView::OnCOPAddMemberFunction()
{
	AfxMessageBox(_T("멤버 함수 추가..."));
}

void COverlayLayerView::OnCOPAddMemberVariable()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
}

void COverlayLayerView::OnCOPDefinition()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
}

void COverlayLayerView::OnCOPProperties()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
}

void COverlayLayerView::OnNewFolder_COP()
{
	AfxMessageBox(_T("새 폴더..."));
}

void COverlayLayerView::OnPaint()
{
	CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

	CRect rectList;
	m_wndView.GetWindowRect(rectList);
	ScreenToClient(rectList);

	rectList.InflateRect(1, 1);
	dc.Draw3dRect(rectList, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}

void COverlayLayerView::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);

	m_wndView.SetFocus();
}

void COverlayLayerView::OnChangeVisualStyle()
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

CListCtrl * COverlayLayerView::GetView()
{
	return &m_wndView;
}
void COverlayLayerView::SetMsgMgr(CMapXMsgProcBase_Overlay_List * pMsgBaseOverList) 
{
	m_pMsgBaseOvlList = pMsgBaseOverList;
}

void COverlayLayerView::RefreshMsgList()
{
    if(m_pMsgBaseOvlList)
	{
		/*// 상황도 투명도 View
		CListCtrl * pListCtrl_Item = NULL;
		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		if(pFrame)
		{
			pListCtrl_Item = (CListCtrl*)pFrame->m_wndOverlayItemView.GetView();
			//CListCtrl * pListCtrl_Layer = (CListCtrl*)pFrame->m_wndOverlayLayerView.GetView();
		}
		//CListCtrl *listCtrl = NULL;
		CComboBox *ComboDrawType = NULL;	// dummy
		m_pMsgBaseOvlList->RefreshMsgList_All(&m_wndView, pListCtrl_Item, ComboDrawType);
		//m_pMsgBaseOvlList->RefreshMsgList_All(&m_wndView, pListCtrl_Item);*/

		m_pMsgBaseOvlList->RefreshMsgList_Layer(&m_wndView);
	}
}
void COverlayLayerView::OnNMClickListOverDlg(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	if(!m_pMsgBaseOvlList) return;

	//m_pMsgBaseOvlList->
	
	*pResult = 0;
}
void COverlayLayerView::OnLvnItemchangedListOverDlg(NMHDR *pNMHDR, LRESULT *pResult)
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

			if(m_pMsgBaseOvlList)
			{
				m_pMsgBaseOvlList->SetLayerView(pNMLV->iItem, bShow);
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
void COverlayLayerView::OnNMDblclkListOverDlg(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	if(!m_pMsgBaseOvlList) return;
	m_pMsgBaseOvlList->SetActiveMsg(pNMLV->iItem);	//더블클릭 레이어 최상위로,, 불편해서 막음
	//RefreshMsgList_All();

	*pResult = 0;
}
void COverlayLayerView::OnNMRClickListOverDlg(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	if(!m_pMsgBaseOvlList) return;

	//선택된 레이어가 없는 경우 팝업메뉴가 뜨는 버그 때문에 추가(14-04-21)
	if(pNMLV->iItem == -1)      
		return;

	POSITION pos = m_wndView.GetFirstSelectedItemPosition();


	int fixidx = m_pMsgBaseOvlList->GetCount() - 1;     
	int nSelect = 0;

	while(pos)
	{
		int nItem = m_wndView.GetNextSelectedItem(pos);	
		nSelect++;
	}

	if(nSelect == 1){
		m_pMsgBaseOvlList->RClickMenu_Layer(pNMLV->iItem);
	}
	else
		m_pMsgBaseOvlList->RClickMenu_Layer_Multi(pNMLV->iItem);



	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	if(pFrame)
	{
		CC4IMapView* pView = (CC4IMapView*)pFrame->GetActiveView();
		if(pView)
		{
			pView->Invalidate(FALSE);
		}
	}
	*pResult = 0;
}