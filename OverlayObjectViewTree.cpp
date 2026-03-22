

#include "pch.h"
#include "C4IMap.h"
#include "MainFrm.h"
#include "C4IMapView.h"
#include "OverlayObjectViewTree.h"

#include "../inc/ComDefValue.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/**
* @struct TREEITEM_INFO
* @brief  트리 아이템 정보
*         only use in COverlayObjectViewTree class
*/
struct OVERLAYVIEWTREE_INFO
{
	int nItemType;			// 트리 아이템 타입
	CString strMyName;		// 표기 명칭
	CString strMyGuid;		// 내 식별자
	CString strParentGuid;	// 부모 식별자
	BOOL bCoordMap;			// 레이어의 경우에 사용 좌표계 전환 사용 여부
};

/**
* @brief  트리 아이템 타입
*/
typedef enum
{
	OVERLAYVIEWTREE_TITLE		= 0, /**< 트리 루트 */
	OVERLAYVIEWTREE_LAYER		= 1, /**< 레이어 */
	OVERLAYVIEWTREE_SHAPE		= 2, /**< 도형 */
} OVERLAYVIEWTREE_TYPE;

/////////////////////////////////////////////////////////////////////////////
// COverlayObjectViewTree

COverlayObjectViewTree::COverlayObjectViewTree()
{

}

COverlayObjectViewTree::~COverlayObjectViewTree()
{
	m_mapOverlayGuid.RemoveAll();
	m_mapShapeGuid.RemoveAll();
}

BEGIN_MESSAGE_MAP(COverlayObjectViewTree, CTreeCtrl)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, &COverlayObjectViewTree::OnNMCustomdraw)
	ON_WM_DESTROY()
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEWHEEL()

	ON_COMMAND(ID_POPOVER_SAVE, OnPopupOverlaySave)
	ON_COMMAND(ID_POPOVER_DELETE_OVERLAY, OnToolBarDelete)
	ON_COMMAND(ID_POPOVER_DELETE_SHAPE, OnToolBarDelete)
	ON_COMMAND(ID_POPOVER_COORD, OnPopupOverlayCoord)
	ON_COMMAND(ID_POPOVER_SCREEN, OnPopupOverlayScreen) 
	ON_COMMAND(ID_POPOVER_TOP, OnToolBarTop)
	ON_COMMAND(ID_POPOVER_UP, OnToolBarUp)
	ON_COMMAND(ID_POPOVER_DOWN, OnToolBarDown)
	ON_COMMAND(ID_POPOVER_BOTTOM, OnToolBarBottom)
	ON_COMMAND(ID_POPOVER_PROPERTY, OnPopupShapeProperty)
	ON_COMMAND(ID_POPOVER_MOVE, OnPopupShapeMove)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COverlayObjectViewTree 메시지 처리기

BOOL COverlayObjectViewTree::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	BOOL bRes = CTreeCtrl::OnNotify(wParam, lParam, pResult);

	NMHDR* pNMHDR = (NMHDR*)lParam;
	ASSERT(pNMHDR != NULL);

	if (pNMHDR && pNMHDR->code == TTN_SHOW && GetToolTips() != NULL)
	{
		GetToolTips()->SetWindowPos(&wndTop, -1, -1, -1, -1, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOSIZE);
	}

	return bRes;
}

void COverlayObjectViewTree::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMTVCUSTOMDRAW * pCustomDraw = (NMTVCUSTOMDRAW*)pNMHDR;
	*pResult = 0;

	HTREEITEM hItem = NULL;

	switch(pCustomDraw->nmcd.dwDrawStage)
	{
	case CDDS_PREPAINT:// 아이템 그리기 전에 알려주세요.
		*pResult = CDRF_NOTIFYITEMDRAW;
		break;

	case CDDS_ITEMPREPAINT:
		{
			hItem = (HTREEITEM)pCustomDraw->nmcd.dwItemSpec;

			HTREEITEM hSelectItem = GetSelectedItem();

			// 포커스 없을 때도 짙은 선택 색상 유지하려고 둠 
			if(hItem == hSelectItem)
			{
				pCustomDraw->clrText = RGB(255,255,255); // white text
				pCustomDraw->clrTextBk = RGB(51, 153, 255); // select Color
			}
			else
			{
				pCustomDraw->clrText = RGB(0,0,0); // Black text
				pCustomDraw->clrTextBk = RGB(255, 255, 255); // white Color
			}

			*pResult = CDRF_DODEFAULT; // do not set *pResult = CDRF_SKIPDEFAULT
		}
		break;
	default:
		break;
	}
}

// 투명도 목록 전체 삭제
void COverlayObjectViewTree::RemoveAllOverlayObjItems()
{
	m_mapOverlayGuid.RemoveAll();
	m_mapShapeGuid.RemoveAll();

	HTREEITEM hItem = GetRootItem();

	if(hItem)
	{
		while(hItem)
		{
			OVERLAYVIEWTREE_INFO* pTreeItem = (OVERLAYVIEWTREE_INFO*)GetItemData(hItem);
			SAFE_DELETE(pTreeItem);

			HTREEITEM hSubItem = GetChildItem(hItem);

			if(hSubItem)
				RemoveTreeItem(hSubItem, TRUE);

			HTREEITEM DeItem = hItem;
			hItem = GetNextSiblingItem(hItem);
			DeleteItem(DeItem);
		}
	}
}

// 트리 아이템 삭제
void COverlayObjectViewTree::RemoveTreeItem(HTREEITEM hItem, BOOL bRemoveNextSiblingItem)
{
	if(hItem)
	{
		OVERLAYVIEWTREE_INFO* pTreeItem = (OVERLAYVIEWTREE_INFO*)GetItemData(hItem);

		if(pTreeItem)
		{
			DeleteMapKey(pTreeItem->nItemType, pTreeItem->strMyGuid, pTreeItem->strParentGuid);
			SAFE_DELETE(pTreeItem);
		}

		HTREEITEM hSubItem = GetChildItem(hItem);

		if(hSubItem)
			RemoveTreeItem(hSubItem, TRUE);

		HTREEITEM hDelItem = hItem;
		hItem = GetNextSiblingItem(hItem);
		DeleteItem(hDelItem);

		if(bRemoveNextSiblingItem)
			RemoveTreeItem(hItem, bRemoveNextSiblingItem);
	}
}

// 트리 아이템 삭제 시 해당 맵 키 삭제
void COverlayObjectViewTree::DeleteMapKey(int nItemType, CString strMyGuid, CString strParentGuid)
{
	switch(nItemType)
	{
	case OVERLAYVIEWTREE_LAYER: // 레이어
		m_mapOverlayGuid.RemoveKey(strMyGuid);
		break;
	case OVERLAYVIEWTREE_SHAPE: // 도형
		m_mapShapeGuid.RemoveKey(strParentGuid + strMyGuid);
		break;
	}
}

// 소멸 시 처리
void COverlayObjectViewTree::OnDestroy()
{
	RemoveAllOverlayObjItems();
	CTreeCtrl::OnDestroy();
}

// 체크 이벤트 때문에 주석 처리
void COverlayObjectViewTree::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	//CTreeCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}

// 체크 이벤트 때문에 주석 처리
void COverlayObjectViewTree::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	//CTreeCtrl::OnLButtonDblClk(nFlags, point);
}

void COverlayObjectViewTree::OnLButtonDown(UINT nFlags, CPoint point)
{
	UINT uFlags;
	HTREEITEM hItem = HitTest(point, &uFlags);

	// 아이템 선택 상태
	if(hItem != NULL)
	{
		// 체크 클릭
		if(uFlags == TVHT_ONITEMSTATEICON)
		{
			SelectItem(hItem);
			SetCheckEventItem(hItem);
		}
		else
		{
			SelectItem(hItem);
			SendMsgSelectItem(hItem);
		}
	}

	// 체크 이벤트 때문에 예외 처리
	if(uFlags != TVHT_ONITEMSTATEICON)
		CTreeCtrl::OnLButtonDown(nFlags, point);
}

void COverlayObjectViewTree::OnRButtonDown(UINT nFlags, CPoint point)
{
	UINT uFlags;
	HTREEITEM hItem = HitTest(point, &uFlags);

	// 아이템 선택 상태
 	if(hItem != NULL)
	{
		if(uFlags != TVHT_ONITEMSTATEICON)
		{
			SelectItem(hItem);
			SendMsgSelectItem(hItem);
		}

		ShowPopupMenus(hItem, point);
	}

	// 체크 이벤트 때문에 주석 처리
	//CTreeCtrl::OnRButtonDown(nFlags, point);
}

// 아이템 체크 시 설정
void COverlayObjectViewTree::SetCheckEventItem(HTREEITEM hItem)
{
	if(hItem != NULL)
	{
		int nCheckOp = 0;	// 미사용

		// 클릭 이전 상태 
		UINT state  = GetItemState(hItem, TVIS_STATEIMAGEMASK);

		state = state >> 12;

		switch(state)
		{
		case TREE_MEMBER_HFCHECK:
		case TREE_MEMBER_UNCHECK:
			{
				// 반체크, 언체크 -> 체크로 변경
				SetItemState(hItem, TREE_STATE_ALLCHECK, TVIS_STATEIMAGEMASK);
				SendMsgSelectCheckItem(hItem, TRUE);
				nCheckOp = 1;	// 체크 사용
			}
			break;

		case TREE_MEMBER_ALLCHECK:
			{
				// 체크 -> 언체크로 변경
				SetItemState(hItem, TREE_STATE_UNCHECK, TVIS_STATEIMAGEMASK);
				SendMsgSelectCheckItem(hItem, FALSE);
				nCheckOp = 2; // 언체크 사용
			}
			break;

		case TREE_MEMBER_NOT:
		default:
			break;
		}

		if(nCheckOp)
		{
			// 하위 체크 반영
			if(ItemHasChildren(hItem))
			{
				HTREEITEM hSubItem = GetChildItem(hItem);

				if(nCheckOp == 1)
					UpdateCheckEventChildItem(hSubItem, TRUE);
				else
					UpdateCheckEventChildItem(hSubItem, FALSE);
			}

			// 상위 체크 업데이트
			HTREEITEM hParentItem = GetParentItem(hItem);

			if(hParentItem != NULL)
				UpdateCheckEventParentItem(hParentItem);
		}
	}
}

// 체크 변경 시 하위 아이템 업데이트
void COverlayObjectViewTree::UpdateCheckEventChildItem(HTREEITEM hItem, BOOL bCheck)
{
	if(hItem)
	{
		while(hItem)
		{
			UINT state  = GetItemState(hItem, TVIS_STATEIMAGEMASK);
			state = state >> 12;

			if(state != TREE_MEMBER_NOT)
				SetCheck(hItem, bCheck);

			if(ItemHasChildren(hItem))
			{
				HTREEITEM hSubItem = GetChildItem(hItem);			
				UpdateCheckEventChildItem(hSubItem, bCheck);
			}

			hItem = GetNextSiblingItem(hItem);
		}
	}
}

// 체크 변경 시 상위 아이템 업데이트
void COverlayObjectViewTree::UpdateCheckEventParentItem(HTREEITEM hItem)
{
	int nCheckCount = 0;
	int nUnCheckCount = 0;
	int nHalfCount = 0;

	if(hItem)
	{
		if(ItemHasChildren(hItem))
		{
			HTREEITEM hSubItem = GetChildItem(hItem);

			while(hSubItem)
			{
				UINT state  = GetItemState(hSubItem, TVIS_STATEIMAGEMASK);

				state = state >> 12;

				switch(state)
				{
				case TREE_MEMBER_NONE:
				case TREE_MEMBER_NOT:
					break;

				case TREE_MEMBER_UNCHECK:
					nUnCheckCount++;
					break;

				case TREE_MEMBER_ALLCHECK:
					nCheckCount++;
					break;

				case TREE_MEMBER_HFCHECK:
					nHalfCount++;
					break;

				default:
					break;
				}

				hSubItem = GetNextSiblingItem(hSubItem);
			}
		}

		if(0 < nUnCheckCount)
		{
			if(0 < nCheckCount || 0 < nHalfCount)
				SetItemState(hItem, TREE_STATE_HFCHECK, TVIS_STATEIMAGEMASK);
			else
				SetItemState(hItem, TREE_STATE_UNCHECK, TVIS_STATEIMAGEMASK);
		}
		else
		{
			if(0 < nHalfCount)
				SetItemState(hItem, TREE_STATE_HFCHECK, TVIS_STATEIMAGEMASK);
			else
				SetItemState(hItem, TREE_STATE_ALLCHECK, TVIS_STATEIMAGEMASK);
		}

		if(0 < nCheckCount || 0 < nUnCheckCount || 0 < nHalfCount)
		{
			HTREEITEM hParentItem = GetParentItem(hItem);

			if(hParentItem)
				UpdateCheckEventParentItem(hParentItem);
		}
	}
}

// 선택 메시지 전송
void COverlayObjectViewTree::SendMsgSelectItem(HTREEITEM hItem)
{
	OVERLAYVIEWTREE_INFO* pTreeItem = (OVERLAYVIEWTREE_INFO*)GetItemData(hItem);

	if(pTreeItem != NULL)
	{
		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();

		if(pFrame != NULL && pFrame->GetSafeHwnd() != NULL)
		{
			CC4IMapView* pView = (CC4IMapView*)pFrame->GetActiveView();
			if(pView)
			{
				switch(pTreeItem->nItemType)
				{
				case OVERLAYVIEWTREE_TITLE: // 트리 루트
					pView->SendMessage(OVERLAY_VIEW_SELECT_TITLE);
					break;

				case OVERLAYVIEWTREE_LAYER: // 레이어
					pView->SendMessage(OVERLAY_VIEW_SELECT_LAYER, (WPARAM)&pTreeItem->strMyGuid);
					break;

				case OVERLAYVIEWTREE_SHAPE: // 도형
					pView->SendMessage(OVERLAY_VIEW_SELECT_SHAPE, (WPARAM)&pTreeItem->strParentGuid, (LPARAM)&pTreeItem->strMyGuid);
					break;
				}
			}
			
		}		
	}
}

// 체크 메시지 전송
void COverlayObjectViewTree::SendMsgSelectCheckItem(HTREEITEM hItem, BOOL bChecked)
{
	OVERLAYVIEWTREE_INFO* pTreeItem = (OVERLAYVIEWTREE_INFO*)GetItemData(hItem);

	if(pTreeItem != NULL)
	{
		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();

		if(pFrame != NULL && pFrame->GetSafeHwnd() != NULL)
		{
			CC4IMapView* pView = (CC4IMapView*)pFrame->GetActiveView();
			if(pView)
			{
				switch(pTreeItem->nItemType)
				{
				case OVERLAYVIEWTREE_TITLE: // 트리 루트
					{
						if(bChecked)
							pView->SendMessage(OVERLAY_VIEW_CHECK_TITLE);
						else
							pView->SendMessage(OVERLAY_VIEW_UNCHECK_TITLE);
					}
					break;

				case OVERLAYVIEWTREE_LAYER: // 레이어
					{
						if(bChecked)
							pView->SendMessage(OVERLAY_VIEW_CHECK_LAYER, (WPARAM)&pTreeItem->strMyGuid);
						else
							pView->SendMessage(OVERLAY_VIEW_UNCHECK_LAYER, (WPARAM)&pTreeItem->strMyGuid);
					}
					break;

				case OVERLAYVIEWTREE_SHAPE: // 도형
					{
						if(bChecked)
							pView->SendMessage(OVERLAY_VIEW_CHECK_SHAPE, (WPARAM)&pTreeItem->strParentGuid, (LPARAM)&pTreeItem->strMyGuid);
						else
							pView->SendMessage(OVERLAY_VIEW_UNCHECK_SHAPE, (WPARAM)&pTreeItem->strParentGuid, (LPARAM)&pTreeItem->strMyGuid);
					}
					break;
				}
			}
		}
	}
}

// 팝업 메뉴
void COverlayObjectViewTree::ShowPopupMenus(HTREEITEM hItem, CPoint point)
{
	CRect rtTree;
	GetWindowRect(rtTree);

	point.x = point.x + rtTree.left;
	point.y = point.y + rtTree.top;

	OVERLAYVIEWTREE_INFO* pTreeItem = (OVERLAYVIEWTREE_INFO*)GetItemData(hItem);

	if(pTreeItem != NULL)
	{
		switch(pTreeItem->nItemType)
		{
			// 투명도 팝업 메뉴
		case OVERLAYVIEWTREE_LAYER:
			{
				CMenu menu;

				if (menu.LoadMenu(IDR_POPUP_OVERLAY))
				{
					CMenu* pPopup = menu.GetSubMenu(0);
					ASSERT(pPopup != NULL);
					if(pPopup != NULL)
					{
						if(pTreeItem->bCoordMap)
							pPopup->CheckMenuItem(ID_POPOVER_COORD, MF_CHECKED | MF_BYCOMMAND);
						else
							pPopup->CheckMenuItem(ID_POPOVER_SCREEN, MF_CHECKED | MF_BYCOMMAND);

						pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x , point.y, this);
					}
				}
			}
			break;

			// 도형 팝업 메뉴
		case OVERLAYVIEWTREE_SHAPE:
			{
				CMenu menu;

				if (menu.LoadMenu(IDR_POPUP_SHAPE))
				{
					CMenu* pPopup = menu.GetSubMenu(0);
					ASSERT(pPopup != NULL);
					if(pPopup != NULL)
						pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x , point.y, this);
				}
			}
			break;
		}
	}
}

// 휠 메인으로 리턴
BOOL COverlayObjectViewTree::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// 마우스 휠 이동 시 영역 외 일 경우 메인으로 포커스 이동
	CRect rt;
	GetClientRect(rt);
	ClientToScreen(rt);

	if(!rt.PtInRect(pt))
	{
		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		if(pFrame != NULL)
			pFrame->SetFocus();
		return FALSE;
	}

	return CTreeCtrl::OnMouseWheel(nFlags, zDelta, pt);
}

// 투명도 루트명 입력
HTREEITEM COverlayObjectViewTree::InsertOverlayTitle()
{
	HTREEITEM hRootItem = GetRootItem();

	if(hRootItem == NULL)
	{
		OVERLAYVIEWTREE_INFO* pTreeTitleInfo = new OVERLAYVIEWTREE_INFO;

		pTreeTitleInfo->nItemType = OVERLAYVIEWTREE_TITLE;	// 타이틀
		pTreeTitleInfo->strMyName = DEF_DOCKPANE_TITLE_OVERLAY_SUB1;
		pTreeTitleInfo->bCoordMap = FALSE;// 레이어의 경우에 사용 좌표계 전환 사용 여부	

		// insert tree item
		hRootItem = InsertItem(pTreeTitleInfo->strMyName, 0, 0);

		// set data to tree item
		SetItemData(hRootItem,  (DWORD)pTreeTitleInfo);
		SetItemState(hRootItem, TREE_STATE_NONE, TVIS_STATEIMAGEMASK);
	}

	return hRootItem;
}

// 투명도 추가
void COverlayObjectViewTree::AddOverlayItem(CString strName, CString strGuid, BOOL bCoordMap)
{
	HTREEITEM hOverlayItem = FindOverlayItem(strGuid);

	if(hOverlayItem == NULL)
	{
		HTREEITEM hRootItem = GetRootItem();

		if(hRootItem == NULL)
			hRootItem = InsertOverlayTitle();

		OVERLAYVIEWTREE_INFO* pTreeOverlayInfo = new OVERLAYVIEWTREE_INFO;

		pTreeOverlayInfo->nItemType = OVERLAYVIEWTREE_LAYER;
		pTreeOverlayInfo->strMyName = strName;
		pTreeOverlayInfo->strMyGuid = strGuid;		// 내 식별자
		pTreeOverlayInfo->bCoordMap = bCoordMap;	// 레이어의 경우에 사용 좌표계 전환 사용 여부			

		// insert tree item
		hOverlayItem = InsertItem(pTreeOverlayInfo->strMyName, 0, 0, hRootItem, TVI_FIRST);
		// set map
		m_mapOverlayGuid.SetAt(strGuid, (void*) hOverlayItem);
		// set data to tree item
		SetItemData(hOverlayItem,  (DWORD)pTreeOverlayInfo);
		SetItemState(hOverlayItem, TREE_STATE_UNCHECK, TVIS_STATEIMAGEMASK);
		SelectItem(hOverlayItem);
		Expand(hRootItem, TVE_EXPAND);
	}
}

// 도형 목록 추가
void COverlayObjectViewTree::AddShapeItems(CString strOverlayGuid, CString strShapeList)
{
	LONG nPos	= 0;
	BOOL bLast	= FALSE;
	BOOL bEnd	= FALSE;
	CString strBuf = _T("");

	nPos = strShapeList.Find("§", 0);

	if(nPos != -1)	
		strBuf = strShapeList.Left(nPos);
	else 
	{
		if(strShapeList.IsEmpty())
			bEnd = TRUE;
		else
		{
			bLast = TRUE;
			strBuf = strShapeList;
		}
	}

	while(!bEnd)
	{
		CString strIDs = strBuf;
		CString stObjID = "", stObjName = "";

		stObjID = strIDs.Left(strIDs.Find("¶"));
		stObjName = strIDs.Right(strIDs.GetLength()-strIDs.Find("¶")-2);

		if(!(stObjID.IsEmpty() || stObjName.IsEmpty()))
			AddShapeItem(strOverlayGuid, stObjID, stObjName, TRUE);

		if(nPos != -1)
			strShapeList = strShapeList.Right( strShapeList.GetLength() - (nPos+2));

		nPos = strShapeList.Find("§", 0);

		if(nPos != -1)
			strBuf = strShapeList.Left(nPos);
		else
		{
			if(strShapeList.IsEmpty() || bLast)
				bEnd = TRUE;
			else
			{
				bLast = TRUE;
				strBuf = strShapeList;
			}
		}
	}

	HTREEITEM hOverlayItem = FindOverlayItem(strOverlayGuid);
	SelectItem(hOverlayItem);
}

// 도형 추가
void COverlayObjectViewTree::AddShapeItem(CString strOverlayGuid, CString strName, CString strGuid, BOOL bVisible)
{
	HTREEITEM hOverlayItem = FindOverlayItem(strOverlayGuid);
	HTREEITEM hShapeItem = FindShapeItem(strOverlayGuid, strGuid);

	if(hOverlayItem != NULL && hShapeItem == NULL)
	{
		OVERLAYVIEWTREE_INFO* pTreeOverlayInfo = new OVERLAYVIEWTREE_INFO;

		pTreeOverlayInfo->nItemType = OVERLAYVIEWTREE_SHAPE;
		pTreeOverlayInfo->strMyName = strName;
		pTreeOverlayInfo->strMyGuid = strGuid;		// 내 식별자
		pTreeOverlayInfo->strParentGuid = strOverlayGuid;

		// insert tree item
		hShapeItem = InsertItem(pTreeOverlayInfo->strMyName, 1, 1, hOverlayItem, TVI_FIRST);
		// set map
		m_mapShapeGuid.SetAt(strOverlayGuid + strGuid, (void*) hShapeItem);
		// set data to tree item
		SetItemData(hShapeItem,  (DWORD)pTreeOverlayInfo);

		if(bVisible)
			SetItemState(hShapeItem, TREE_STATE_ALLCHECK, TVIS_STATEIMAGEMASK);
		else
			SetItemState(hShapeItem, TREE_STATE_UNCHECK, TVIS_STATEIMAGEMASK);

		UpdateCheckEventParentItem(hOverlayItem);
		Expand(hOverlayItem, TVE_EXPAND);
	}
}

// 투명도 찾기
HTREEITEM COverlayObjectViewTree::FindOverlayItem(CString strOverlayGuid)
{
	void * ptr = NULL;
	HTREEITEM hOverlayItem = NULL;

	if(m_mapOverlayGuid.Lookup(strOverlayGuid, (void*) ptr))
		hOverlayItem = (HTREEITEM) ptr;

	return hOverlayItem;
}

// 도형 찾기
HTREEITEM COverlayObjectViewTree::FindShapeItem(CString strOverlayGuid, CString strShapeGuid)
{
	void * ptr = NULL;
	HTREEITEM hShapeItem = NULL;

	if(m_mapShapeGuid.Lookup(strOverlayGuid + strShapeGuid, (void*) ptr))
		hShapeItem = (HTREEITEM) ptr;

	return hShapeItem;
}

// 투명도 삭제
void COverlayObjectViewTree::RemoveOverlayItem(CString strGuid)
{
	HTREEITEM hOverlayItem = FindOverlayItem(strGuid);

	if(hOverlayItem)
	{
		OVERLAYVIEWTREE_INFO* pTreeItem = (OVERLAYVIEWTREE_INFO*)GetItemData(hOverlayItem);

		if(pTreeItem != NULL)
			DeleteMapKey(pTreeItem->nItemType, pTreeItem->strMyGuid, pTreeItem->strParentGuid);

		SAFE_DELETE(pTreeItem);

		DeleteItem(hOverlayItem);
	}

	// 체크 업데이트
	UpdateCheckEventParentItem(GetRootItem());
}

// 도형 전체 삭제
void COverlayObjectViewTree::RemoveShapeAllItems(CString strOverlayGuid)
{
	HTREEITEM hOverlayItem = FindOverlayItem(strOverlayGuid);

	if(hOverlayItem)
	{
		HTREEITEM hShapeItem = GetChildItem(hOverlayItem);
		RemoveTreeItem(hShapeItem, TRUE);
		UpdateCheckEventParentItem(hOverlayItem);
	}
}

// 도형 삭제
void COverlayObjectViewTree::RemoveShapeItem(CString strOverlayGuid, CString strShapeGuid)
{
	HTREEITEM hShapeItem = FindShapeItem(strOverlayGuid, strShapeGuid);

	HTREEITEM hOverlayItem = GetParentItem(hShapeItem);

	if(hShapeItem != NULL)
	{
		OVERLAYVIEWTREE_INFO* pTreeItem = (OVERLAYVIEWTREE_INFO*)GetItemData(hShapeItem);

		if(pTreeItem != NULL)
			DeleteMapKey(pTreeItem->nItemType, pTreeItem->strMyGuid, pTreeItem->strParentGuid);

		SAFE_DELETE(pTreeItem);

		DeleteItem(hShapeItem);
	}

	// 체크 업데이트
	if(hOverlayItem != NULL)
		UpdateCheckEventParentItem(hOverlayItem);
}

// 투명도 선택 - 외부로부터
void COverlayObjectViewTree::SelectOverlayItem(CString strOverlayGuid)
{
	HTREEITEM hOverlayItem = FindOverlayItem(strOverlayGuid);

	if(hOverlayItem != NULL)
		SelectItem(hOverlayItem);
}

// 도형 선택 - 외부로부터
void COverlayObjectViewTree::SelectShapeItem(CString strOverlayGuid, CString strShapeGuid)
{
	HTREEITEM hShapeItem = FindShapeItem(strOverlayGuid, strShapeGuid);

	if(hShapeItem != NULL)
		SelectItem(hShapeItem);
}

// 투명도 체크 설정 - 외부로부터
void COverlayObjectViewTree::SetCheckOverlayItem(CString strOverlayGuid, BOOL bCheck)
{
	HTREEITEM hOverlayItem = FindOverlayItem(strOverlayGuid);

	if(hOverlayItem != NULL)
	{
		if(bCheck == TRUE)
			SetItemState(hOverlayItem, TREE_STATE_ALLCHECK, TVIS_STATEIMAGEMASK);
		else
			SetItemState(hOverlayItem, TREE_STATE_UNCHECK, TVIS_STATEIMAGEMASK);

		UpdateCheckEventChildItem(GetChildItem(hOverlayItem), bCheck);
	}	
}

// 도형 체크 설정 - 외부로부터
void COverlayObjectViewTree::SetCheckShapeItem(CString strOverlayGuid, CString strShapeGuid, BOOL bCheck)
{
	HTREEITEM hShapeItem = FindShapeItem(strOverlayGuid, strShapeGuid);

	if(hShapeItem != NULL)
	{
		if(bCheck == TRUE)
			SetItemState(hShapeItem, TREE_STATE_ALLCHECK, TVIS_STATEIMAGEMASK);
		else
			SetItemState(hShapeItem, TREE_STATE_UNCHECK, TVIS_STATEIMAGEMASK);

		if(bCheck == FALSE)
			SelectItem(GetParentItem(hShapeItem));
		
		UpdateCheckEventParentItem(GetParentItem(hShapeItem));
	}	
}

// 투명도 위로 - 외부로부터
void COverlayObjectViewTree::MoveForwardOverlayItem(CString strOverlayGuid)
{
	HTREEITEM hRootItem = GetRootItem();
	HTREEITEM hMoveItem = FindOverlayItem(strOverlayGuid);

	// After라 두단 위의 아이템
	HTREEITEM hPreItem = GetPrevSiblingItem(hMoveItem);
	hPreItem = GetPrevSiblingItem(hPreItem);

	if(hMoveItem != NULL && hRootItem != NULL)
	{
		OVERLAYVIEWTREE_INFO* pMoveTreeInfo = (OVERLAYVIEWTREE_INFO*)GetItemData(hMoveItem);

		if(pMoveTreeInfo != NULL)
		{
			HTREEITEM hForwardItem = NULL;

			if(hPreItem != NULL)
				hForwardItem = InsertItem(pMoveTreeInfo->strMyName, 0, 0, hRootItem, hPreItem);
			else
				hForwardItem = InsertItem(pMoveTreeInfo->strMyName, 0, 0, hRootItem, TVI_FIRST);

			// set map
			m_mapOverlayGuid.SetAt(strOverlayGuid, (void*) hForwardItem);
			// set data to tree item
			SetItemData(hForwardItem,  (DWORD)pMoveTreeInfo);

			if(GetCheck(hMoveItem))
				SetItemState(hForwardItem, TREE_STATE_ALLCHECK, TVIS_STATEIMAGEMASK);
			else
				SetItemState(hForwardItem, TREE_STATE_UNCHECK, TVIS_STATEIMAGEMASK);

			CopyShapeItems(hMoveItem, hForwardItem);
			SelectItem(hForwardItem);
			Expand(hForwardItem, TVE_EXPAND);
			DeleteItem(hMoveItem);
		}
	}
}

// 투명도 아래로 - 외부로부터
void COverlayObjectViewTree::MoveBackwardOverlayItem(CString strOverlayGuid)
{
	HTREEITEM hRootItem = GetRootItem();
	HTREEITEM hMoveItem = FindOverlayItem(strOverlayGuid);
	HTREEITEM hNextItem = GetNextSiblingItem(hMoveItem);

	if(hMoveItem != NULL && hRootItem != NULL)
	{
		OVERLAYVIEWTREE_INFO* pMoveTreeInfo = (OVERLAYVIEWTREE_INFO*)GetItemData(hMoveItem);

		if(pMoveTreeInfo != NULL)
		{
			HTREEITEM hBackwardItem = NULL;

			if(hNextItem != NULL)
				hBackwardItem = InsertItem(pMoveTreeInfo->strMyName, 0, 0, hRootItem, hNextItem);
			else
				hBackwardItem = InsertItem(pMoveTreeInfo->strMyName, 0, 0, hRootItem, TVI_LAST);

			// set map
			m_mapOverlayGuid.SetAt(strOverlayGuid, (void*) hBackwardItem);
			// set data to tree item
			SetItemData(hBackwardItem,  (DWORD)pMoveTreeInfo);

			if(GetCheck(hMoveItem))
				SetItemState(hBackwardItem, TREE_STATE_ALLCHECK, TVIS_STATEIMAGEMASK);
			else
				SetItemState(hBackwardItem, TREE_STATE_UNCHECK, TVIS_STATEIMAGEMASK);

			CopyShapeItems(hMoveItem, hBackwardItem);
			SelectItem(hBackwardItem);
			Expand(hBackwardItem, TVE_EXPAND);
			DeleteItem(hMoveItem);
		}
	}
}

// 투명도 맨 위로 - 외부로부터
void COverlayObjectViewTree::MoveTopOverlayItem(CString strOverlayGuid)
{
	HTREEITEM hRootItem = GetRootItem();
	HTREEITEM hMoveItem = FindOverlayItem(strOverlayGuid);
	
	if(hMoveItem != NULL && hRootItem != NULL)
	{
		OVERLAYVIEWTREE_INFO* pMoveTreeInfo = (OVERLAYVIEWTREE_INFO*)GetItemData(hMoveItem);

		if(pMoveTreeInfo != NULL)
		{
			// 맨 앞에 추가
			HTREEITEM hTopItem = InsertItem(pMoveTreeInfo->strMyName, 0, 0, hRootItem, TVI_FIRST);

			// set map
			m_mapOverlayGuid.SetAt(strOverlayGuid, (void*) hTopItem);
			// set data to tree item
			SetItemData(hTopItem,  (DWORD)pMoveTreeInfo);

			if(GetCheck(hMoveItem))
				SetItemState(hTopItem, TREE_STATE_ALLCHECK, TVIS_STATEIMAGEMASK);
			else
				SetItemState(hTopItem, TREE_STATE_UNCHECK, TVIS_STATEIMAGEMASK);

			CopyShapeItems(hMoveItem, hTopItem);
			SelectItem(hTopItem);
			Expand(hTopItem, TVE_EXPAND);
			DeleteItem(hMoveItem);
		}
	}	
}

// 투명도 맨 아래로 - 외부로부터
void COverlayObjectViewTree::MoveBottomOverlayItem(CString strOverlayGuid)
{
	HTREEITEM hRootItem = GetRootItem();
	HTREEITEM hMoveItem = FindOverlayItem(strOverlayGuid);

	if(hMoveItem != NULL && hRootItem != NULL)
	{
		OVERLAYVIEWTREE_INFO* pMoveTreeInfo = (OVERLAYVIEWTREE_INFO*)GetItemData(hMoveItem);

		if(pMoveTreeInfo != NULL)
		{
			// 맨 앞에 추가
			HTREEITEM hBottomItem = InsertItem(pMoveTreeInfo->strMyName, 0, 0, hRootItem, TVI_LAST);

			// set map
			m_mapOverlayGuid.SetAt(strOverlayGuid, (void*) hBottomItem);
			// set data to tree item
			SetItemData(hBottomItem,  (DWORD)pMoveTreeInfo);

			if(GetCheck(hMoveItem))
				SetItemState(hBottomItem, TREE_STATE_ALLCHECK, TVIS_STATEIMAGEMASK);
			else
				SetItemState(hBottomItem, TREE_STATE_UNCHECK, TVIS_STATEIMAGEMASK);

			CopyShapeItems(hMoveItem, hBottomItem);

			SelectItem(hBottomItem);
			Expand(hBottomItem, TVE_EXPAND);
			DeleteItem(hMoveItem);
		}
	}	
}

// hSourceItem의 Shape들을 hDestItem으로 이동
void COverlayObjectViewTree::CopyShapeItems(HTREEITEM hSourceItem, HTREEITEM hDestItem)
{
	if(ItemHasChildren(hSourceItem))
	{
		HTREEITEM hChildItem = GetChildItem(hSourceItem);

		while(hChildItem)
		{
			OVERLAYVIEWTREE_INFO* pItemInfo = (OVERLAYVIEWTREE_INFO*)GetItemData(hChildItem);

			if(pItemInfo)
			{
				HTREEITEM hCopyItem = InsertItem(pItemInfo->strMyName, 1, 1, hDestItem, TVI_LAST);
				// set map
				m_mapShapeGuid.SetAt(pItemInfo->strParentGuid + pItemInfo->strMyGuid, (void*) hCopyItem);
				// set data to tree item
				SetItemData(hCopyItem,  (DWORD)pItemInfo);

				if(GetCheck(hChildItem))
					SetItemState(hCopyItem, TREE_STATE_ALLCHECK, TVIS_STATEIMAGEMASK);
				else
					SetItemState(hCopyItem, TREE_STATE_UNCHECK, TVIS_STATEIMAGEMASK);
			}
			hChildItem = GetNextItem(hChildItem, TVGN_NEXT);
		}
	}
}

// 도형 위로 - 외부로부터
void COverlayObjectViewTree::MoveForwardShapeItem(CString strOverlayGuid, CString strShapeGuid)
{
	HTREEITEM hMoveItem = FindShapeItem(strOverlayGuid, strShapeGuid);
	HTREEITEM hOvelayItem = FindOverlayItem(strOverlayGuid);

	// After라 두단 위의 아이템
	HTREEITEM hPreItem = GetPrevSiblingItem(hMoveItem);
	hPreItem = GetPrevSiblingItem(hPreItem);

	if(hMoveItem != NULL && hOvelayItem != NULL)
	{
		OVERLAYVIEWTREE_INFO* pMoveTreeInfo = (OVERLAYVIEWTREE_INFO*)GetItemData(hMoveItem);

		if(pMoveTreeInfo != NULL)
		{
			HTREEITEM hForwardItem = NULL;

			if(hPreItem != NULL)
				hForwardItem = InsertItem(pMoveTreeInfo->strMyName, 1, 1, hOvelayItem, hPreItem);
			else
				hForwardItem = InsertItem(pMoveTreeInfo->strMyName, 1, 1, hOvelayItem, TVI_FIRST);

			// set map
			m_mapShapeGuid.SetAt(strOverlayGuid + strShapeGuid, (void*) hForwardItem);
			// set data to tree item
			SetItemData(hForwardItem,  (DWORD)pMoveTreeInfo);

			if(GetCheck(hMoveItem))
				SetItemState(hForwardItem, TREE_STATE_ALLCHECK, TVIS_STATEIMAGEMASK);
			else
				SetItemState(hForwardItem, TREE_STATE_UNCHECK, TVIS_STATEIMAGEMASK);

			SelectItem(hForwardItem);
			DeleteItem(hMoveItem);
		}
	}
}

// 도형 아래로 - 외부로부터
void COverlayObjectViewTree::MoveBackwardShapeItem(CString strOverlayGuid, CString strShapeGuid)
{
	TRACE("\nTree - 선택 도형 아래로 이동 처리 \n");
	HTREEITEM hMoveItem = FindShapeItem(strOverlayGuid, strShapeGuid);
	HTREEITEM hOvelayItem = FindOverlayItem(strOverlayGuid);

	HTREEITEM hNextItem = GetNextSiblingItem(hMoveItem);

	if(hMoveItem != NULL && hOvelayItem != NULL)
	{
		OVERLAYVIEWTREE_INFO* pMoveTreeInfo = (OVERLAYVIEWTREE_INFO*)GetItemData(hMoveItem);

		if(pMoveTreeInfo != NULL)
		{
			HTREEITEM hBackwardItem = NULL;

			if(hNextItem != NULL)
				hBackwardItem = InsertItem(pMoveTreeInfo->strMyName, 1, 1, hOvelayItem, hNextItem);
			else
				hBackwardItem = InsertItem(pMoveTreeInfo->strMyName, 1, 1, hOvelayItem, TVI_LAST);

			// set map
			m_mapShapeGuid.SetAt(strOverlayGuid + strShapeGuid, (void*) hBackwardItem);
			// set data to tree item
			SetItemData(hBackwardItem,  (DWORD)pMoveTreeInfo);

			if(GetCheck(hMoveItem))
				SetItemState(hBackwardItem, TREE_STATE_ALLCHECK, TVIS_STATEIMAGEMASK);
			else
				SetItemState(hBackwardItem, TREE_STATE_UNCHECK, TVIS_STATEIMAGEMASK);

			SelectItem(hBackwardItem);
			DeleteItem(hMoveItem);
		}
	}
}

// 도형 맨 위로 - 외부로부터
void COverlayObjectViewTree::MoveTopShapeItem(CString strOverlayGuid, CString strShapeGuid)
{
	HTREEITEM hMoveItem = FindShapeItem(strOverlayGuid, strShapeGuid);
	HTREEITEM hOvelayItem = FindOverlayItem(strOverlayGuid);

	if(hMoveItem != NULL && hOvelayItem != NULL)
	{
		OVERLAYVIEWTREE_INFO* pMoveTreeInfo = (OVERLAYVIEWTREE_INFO*)GetItemData(hMoveItem);

		if(pMoveTreeInfo != NULL)
		{
			// 맨 앞에 추가
			HTREEITEM hTopItem = InsertItem(pMoveTreeInfo->strMyName, 1, 1, hOvelayItem, TVI_FIRST);

			// set map
			m_mapShapeGuid.SetAt(strOverlayGuid + strShapeGuid, (void*) hTopItem);
			// set data to tree item
			SetItemData(hTopItem,  (DWORD)pMoveTreeInfo);

			if(GetCheck(hMoveItem))
				SetItemState(hTopItem, TREE_STATE_ALLCHECK, TVIS_STATEIMAGEMASK);
			else
				SetItemState(hTopItem, TREE_STATE_UNCHECK, TVIS_STATEIMAGEMASK);

			SelectItem(hTopItem);
			DeleteItem(hMoveItem);
		}
	}	
}

// 도형 맨 아래로 - 외부로부터
void COverlayObjectViewTree::MoveBottomShapeItem(CString strOverlayGuid, CString strShapeGuid)
{
	TRACE("\nTree - 선택 도형 맨 아래로 이동 처리 \n");

	HTREEITEM hMoveItem = FindShapeItem(strOverlayGuid, strShapeGuid);
	HTREEITEM hOvelayItem = FindOverlayItem(strOverlayGuid);

	if(hMoveItem != NULL && hOvelayItem != NULL)
	{
		OVERLAYVIEWTREE_INFO* pMoveTreeInfo = (OVERLAYVIEWTREE_INFO*)GetItemData(hMoveItem);

		if(pMoveTreeInfo != NULL)
		{
			// 맨 아래에 추가
			HTREEITEM hBottomItem = InsertItem(pMoveTreeInfo->strMyName, 1, 1, hOvelayItem, TVI_LAST);

			// set map
			m_mapShapeGuid.SetAt(strOverlayGuid + strShapeGuid, (void*) hBottomItem);
			// set data to tree item
			SetItemData(hBottomItem,  (DWORD)pMoveTreeInfo);

			if(GetCheck(hMoveItem))
				SetItemState(hBottomItem, TREE_STATE_ALLCHECK, TVIS_STATEIMAGEMASK);
			else
				SetItemState(hBottomItem, TREE_STATE_UNCHECK, TVIS_STATEIMAGEMASK);

			SelectItem(hBottomItem);
			DeleteItem(hMoveItem);
		}
	}
}

// 툴바 메뉴 - 맨 위로
void COverlayObjectViewTree::OnToolBarTop()
{
	HTREEITEM hSelectItem = GetSelectedItem();
	HTREEITEM hPrevItem = GetPrevSiblingItem(hSelectItem);	// 이전 아이템이 존재 해야 처리 가능ㄴ

	if(hSelectItem != NULL && hPrevItem != NULL)
	{
		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		OVERLAYVIEWTREE_INFO* pTreeItem = (OVERLAYVIEWTREE_INFO*)GetItemData(hSelectItem);

		if(pTreeItem != NULL && pFrame != NULL && pFrame->GetSafeHwnd() != NULL)
		{
			CC4IMapView* pView = (CC4IMapView*)pFrame->GetActiveView();
			if(pView)
			{
				switch(pTreeItem->nItemType)
				{
				case OVERLAYVIEWTREE_LAYER: // 레이어
					pView->SendMessage(OVERLAY_VIEW_MOVETOP_LAYER, (WPARAM)&pTreeItem->strMyGuid);
					break;

				case OVERLAYVIEWTREE_SHAPE: // 도형
					pView->SendMessage(OVERLAY_VIEW_MOVETOP_SHAPE, (WPARAM)&pTreeItem->strParentGuid, (LPARAM)&pTreeItem->strMyGuid);
					break;
				}
			}
		}
	}
}

// 툴바 메뉴 - 위로
void COverlayObjectViewTree::OnToolBarUp()
{
	HTREEITEM hSelectItem = GetSelectedItem();
	HTREEITEM hPrevItem = GetPrevSiblingItem(hSelectItem);	// 이전 아이템이 존재 해야 처리 가능ㄴ

	if(hSelectItem != NULL && hPrevItem != NULL)
	{
		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		OVERLAYVIEWTREE_INFO* pTreeItem = (OVERLAYVIEWTREE_INFO*)GetItemData(hSelectItem);

		if(pTreeItem != NULL && pFrame != NULL && pFrame->GetSafeHwnd() != NULL)
		{
			CMapXCOP_View * pView = (CMapXCOP_View *)pFrame->GetActiveView();
			if(pView)
			{
				switch(pTreeItem->nItemType)
				{
				case OVERLAYVIEWTREE_LAYER: // 레이어
					pView->SendMessage(OVERLAY_VIEW_MOVEUP_LAYER, (WPARAM)&pTreeItem->strMyGuid);
					break;

				case OVERLAYVIEWTREE_SHAPE: // 도형
					pView->SendMessage(OVERLAY_VIEW_MOVEUP_SHAPE, (WPARAM)&pTreeItem->strParentGuid, (LPARAM)&pTreeItem->strMyGuid);
					break;
				}
			}
		}
	}
}

// 툴바 메뉴 - 아래로
void COverlayObjectViewTree::OnToolBarDown()
{
	HTREEITEM hSelectItem = GetSelectedItem();
	HTREEITEM hNextItem = GetNextSiblingItem(hSelectItem);	// 이후 아이템이 존재해야 아래로 가능

	if(hSelectItem != NULL && hNextItem != NULL)
	{
		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		OVERLAYVIEWTREE_INFO* pTreeItem = (OVERLAYVIEWTREE_INFO*)GetItemData(hSelectItem);

		if(pTreeItem != NULL && pFrame != NULL && pFrame->GetSafeHwnd() != NULL)
		{
			CC4IMapView* pView = (CC4IMapView*)pFrame->GetActiveView();
			if(pView)
			{
				switch(pTreeItem->nItemType)
				{
				case OVERLAYVIEWTREE_LAYER: // 레이어
					pView->SendMessage(OVERLAY_VIEW_MOVEDOWN_LAYER, (WPARAM)&pTreeItem->strMyGuid);
					break;

				case OVERLAYVIEWTREE_SHAPE: // 도형
					pView->SendMessage(OVERLAY_VIEW_MOVEDOWN_SHAPE, (WPARAM)&pTreeItem->strParentGuid, (LPARAM)&pTreeItem->strMyGuid);
					break;
				}
			}
		}
	}
}

// 툴바 메뉴 - 맨 아래로
void COverlayObjectViewTree::OnToolBarBottom()
{
	HTREEITEM hSelectItem = GetSelectedItem();
	HTREEITEM hNextItem = GetNextSiblingItem(hSelectItem);	// 이후 아이템이 존재해야 아래로 가능

	if(hSelectItem != NULL && hNextItem != NULL)
	{
		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		OVERLAYVIEWTREE_INFO* pTreeItem = (OVERLAYVIEWTREE_INFO*)GetItemData(hSelectItem);

		if(pTreeItem != NULL && pFrame != NULL && pFrame->GetSafeHwnd() != NULL)
		{
			CMapXCOP_View * pView = (CMapXCOP_View *)pFrame->GetActiveView();
			if(pView)
			{
				switch(pTreeItem->nItemType)
				{
				case OVERLAYVIEWTREE_LAYER: // 레이어
					pView->SendMessage(OVERLAY_VIEW_MOVEBOTTOM_LAYER, (WPARAM)&pTreeItem->strMyGuid);
					break;

				case OVERLAYVIEWTREE_SHAPE: // 도형
					pView->SendMessage(OVERLAY_VIEW_MOVEBOTTOM_SHAPE, (WPARAM)&pTreeItem->strParentGuid, (LPARAM)&pTreeItem->strMyGuid);
					break;
				}
			}
		}
	}
}

// 툴바 메뉴 - 삭제
void COverlayObjectViewTree::OnToolBarDelete()
{
	HTREEITEM hSelectItem = GetSelectedItem();
	
	if(hSelectItem != NULL)
	{
		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		OVERLAYVIEWTREE_INFO* pTreeItem = (OVERLAYVIEWTREE_INFO*)GetItemData(hSelectItem);

		if(pTreeItem != NULL && pFrame != NULL && pFrame->GetSafeHwnd() != NULL)
		{
			CC4IMapView* pView = (CC4IMapView*)pFrame->GetActiveView();
			if(pView)
			{
				switch(pTreeItem->nItemType)
				{
				case OVERLAYVIEWTREE_LAYER: // 레이어
					pView->SendMessage(OVERLAY_VIEW_DELETE_LAYER, (WPARAM)&pTreeItem->strMyGuid);
					break;

				case OVERLAYVIEWTREE_SHAPE: // 도형
					pView->SendMessage(OVERLAY_VIEW_DELETE_SHAPE, (WPARAM)&pTreeItem->strParentGuid, (LPARAM)&pTreeItem->strMyGuid);
					break;
				}
			}
		}
	}
}

// 좌표계 설정
void COverlayObjectViewTree::SetOverlayCoordMap(CString strOverlayGuid, BOOL bCoordMap)
{
	HTREEITEM hOverlayItem = FindOverlayItem(strOverlayGuid);

	if(hOverlayItem)
	{
		OVERLAYVIEWTREE_INFO* pTreeItem = (OVERLAYVIEWTREE_INFO*)GetItemData(hOverlayItem);

		if(pTreeItem != NULL)
			pTreeItem->bCoordMap = bCoordMap;
	}
}

// 투명도 저장
void COverlayObjectViewTree::OnPopupOverlaySave()
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();

	if(pFrame != NULL && pFrame->GetSafeHwnd() != NULL)
	{
		CMapXCOP_View * pView = (CMapXCOP_View *)pFrame->GetActiveView();
		if(pView)
		{
			pView->SendMessage(OVERLAY_VIEW_SAVE_LAYER);
		}
	}
}

// 투명도 지리 좌표계 설정
void COverlayObjectViewTree::OnPopupOverlayCoord()
{
	HTREEITEM hSelectItem = GetSelectedItem();

	if(hSelectItem != NULL)
	{
		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		OVERLAYVIEWTREE_INFO* pTreeItem = (OVERLAYVIEWTREE_INFO*)GetItemData(hSelectItem);

		if(pTreeItem != NULL && pTreeItem->nItemType == OVERLAYVIEWTREE_LAYER && pFrame != NULL && pFrame->GetSafeHwnd() != NULL)
		{
			CC4IMapView* pView = (CC4IMapView*)pFrame->GetActiveView();
			if(pView)
			{
				if(pTreeItem->bCoordMap == FALSE)
					pView->SendMessage(OVERLAY_VIEW_COORD_LAYER, (WPARAM)&pTreeItem->strMyGuid, (LPARAM)1);
			}
		}
	}
}

// 투명도 화면 좌표계 설정
void COverlayObjectViewTree::OnPopupOverlayScreen()
{
	HTREEITEM hSelectItem = GetSelectedItem();

	if(hSelectItem != NULL)
	{
		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		OVERLAYVIEWTREE_INFO* pTreeItem = (OVERLAYVIEWTREE_INFO*)GetItemData(hSelectItem);

		if(pTreeItem != NULL && pTreeItem->nItemType == OVERLAYVIEWTREE_LAYER && pFrame != NULL && pFrame->GetSafeHwnd() != NULL)
		{
			CMapXCOP_View * pView = (CMapXCOP_View *)pFrame->GetActiveView();
			if(pView)
			{
				if(pTreeItem->bCoordMap == TRUE)
					pView->SendMessage(OVERLAY_VIEW_COORD_LAYER, (WPARAM)&pTreeItem->strMyGuid, (LPARAM)0);
			}
		}
	}
}

// 도형 속성 창 설정
void COverlayObjectViewTree::OnPopupShapeProperty()
{
	HTREEITEM hSelectItem = GetSelectedItem();

	if(hSelectItem != NULL)
	{
		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		OVERLAYVIEWTREE_INFO* pTreeItem = (OVERLAYVIEWTREE_INFO*)GetItemData(hSelectItem);

		CC4IMapView* pView = (CC4IMapView*)pFrame->GetActiveView();
		if(pView)
		{
			if(pTreeItem != NULL && pTreeItem->nItemType == OVERLAYVIEWTREE_SHAPE && pFrame != NULL && pFrame->GetSafeHwnd() != NULL)
				pView->SendMessage(OVERLAY_VIEW_SHAPE_PROPERTY, (WPARAM)&pTreeItem->strParentGuid, (LPARAM)&pTreeItem->strMyGuid);
		}
	}
}

void COverlayObjectViewTree::OnPopupShapeMove()
{
	HTREEITEM hSelectItem = GetSelectedItem();

	if(hSelectItem != NULL)
	{
		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		OVERLAYVIEWTREE_INFO* pTreeItem = (OVERLAYVIEWTREE_INFO*)GetItemData(hSelectItem);

		CMapXCOP_View * pView = (CMapXCOP_View *)pFrame->GetActiveView();
		if(pView)
		{
			if(pTreeItem != NULL && pTreeItem->nItemType == OVERLAYVIEWTREE_SHAPE && pFrame != NULL && pFrame->GetSafeHwnd() != NULL)
				pView->SendMessage(OVERLAY_VIEW_SHAPE_MOVE, (WPARAM)&pTreeItem->strParentGuid, (LPARAM)&pTreeItem->strMyGuid);
		}
	}
}

// 도형 명칭 변경
void COverlayObjectViewTree::SetShapeName(CString strOverlayGuid, CString strShapeGuid, CString strShapeName)
{
	HTREEITEM hShapeItem = FindShapeItem(strOverlayGuid, strShapeGuid);

	if(hShapeItem != NULL)
	{
		OVERLAYVIEWTREE_INFO* pTreeItem = (OVERLAYVIEWTREE_INFO*)GetItemData(hShapeItem);
		pTreeItem->strMyName = strShapeName;
		SetItemText(hShapeItem, strShapeName);
	}
}

// 투명도 명칭 변경
void COverlayObjectViewTree::SetLayerName(CString strOverlayGuid, CString strLayerName)
{
	HTREEITEM hLayerItem = FindOverlayItem(strOverlayGuid);

	if(hLayerItem != NULL)
	{
		OVERLAYVIEWTREE_INFO* pTreeItem = (OVERLAYVIEWTREE_INFO*)GetItemData(hLayerItem);
		pTreeItem->strMyName = strLayerName;
		SetItemText(hLayerItem, strLayerName);
	}
}