#pragma once

/////////////////////////////////////////////////////////////////////////////
// COverlayObjectViewTree 창입니다.

// 투명도 관리 창 트리
class COverlayObjectViewTree : public CTreeCtrl
{
// 생성입니다.
public:
	COverlayObjectViewTree();

public:
	// 투명도 추가
	void AddOverlayItem(CString strName, CString strGuid, BOOL bCoordMap);
	// 도형 목록 추가
	void AddShapeItems(CString strOverlayGuid, CString strShapeList);
	// 도형 추가
	void AddShapeItem(CString strOverlayGuid, CString strName, CString strGuid, BOOL bVisible);

	// 투명도 전체 삭제
	void RemoveAllOverlayObjItems();
	// 투명도 개별 삭제
	void RemoveOverlayItem(CString strGuid);
	// 도형 전체 삭제
	void RemoveShapeAllItems(CString strOverlayGuid);
	// 도형 개별 삭제
	void RemoveShapeItem(CString strOverlayGuid, CString strShapeGuid);

	// 투명도 선택 설정
	void SelectOverlayItem(CString strOverlayGuid);
	// 도형 선택 설정
	void SelectShapeItem(CString strOverlayGuid, CString strShapeGuid);

	// 투명도 체크 설정
	void SetCheckOverlayItem(CString strOverlayGuid, BOOL bCheck);
	// 도형 체크 설정
	void SetCheckShapeItem(CString strOverlayGuid, CString strShapeGuid, BOOL bCheck);

	// 도형 위로
	void MoveForwardShapeItem(CString strOverlayGuid, CString strShapeGuid);
	// 도형 아래로
	void MoveBackwardShapeItem(CString strOverlayGuid, CString strShapeGuid);
	// 도형 맨 위로
	void MoveTopShapeItem(CString strOverlayGuid, CString strShapeGuid);
	// 도형 맨 아래로
	void MoveBottomShapeItem(CString strOverlayGuid, CString strShapeGuid);
	
	// 투명도 위로
	void MoveForwardOverlayItem(CString strOverlayGuid);
	// 투명도 아래로
	void MoveBackwardOverlayItem(CString strOverlayGuid);
	// 투명도 맨 위로
	void MoveTopOverlayItem(CString strOverlayGuid);
	// 투명도 맨 아래로
	void MoveBottomOverlayItem(CString strOverlayGuid);

	// 툴바 - 맨 위로
	void OnToolBarTop();
	// 툴바 - 위로
	void OnToolBarUp();
	// 툴바 - 아래로
	void OnToolBarDown();
	// 툴바 - 맨 아래로
	void OnToolBarBottom();
	// 툴바 - 삭제
	void OnToolBarDelete();

	// 좌표계 설정
	void SetOverlayCoordMap(CString strOverlayGuid, BOOL bCoordMap);
	// 도형 명칭 설정
	void SetShapeName(CString strOverlayGuid, CString strShapeGuid, CString strShapeName);
	// 투명도 명칭 변경
	void SetLayerName(CString strOverlayGuid, CString strLayerName);
private:

	CMapStringToPtr m_mapOverlayGuid;  /**< Map for Overlay Guid */
	CMapStringToPtr m_mapShapeGuid;    /**< Map for Shape Guid */

	// 투명도 루트명 입력
	HTREEITEM InsertOverlayTitle();	
	// 투명도 아이템 반환
	HTREEITEM FindOverlayItem(CString strOverlayGuid);
	// 도형 아이템 반환
	HTREEITEM FindShapeItem(CString strOverlayGuid, CString strShapeGuid);

	// 트리 아이템 삭제 시 맵 키 삭제
	void DeleteMapKey(int nItemType, CString strMyGuid, CString strParentGuid);
	// 트리 아이템 삭제
	void RemoveTreeItem(HTREEITEM hItem, BOOL bRemoveNextSiblingItem);

	// 체크 이벤트 설정
	void SetCheckEventItem(HTREEITEM hItem);
	// 체크 변경 시 하위 아이템 설정
	void UpdateCheckEventChildItem(HTREEITEM hItem, BOOL bCheck);
	// 체크 변경 시 상위 아이템 설정
	void UpdateCheckEventParentItem(HTREEITEM hItem);

	// 선택 메시지 전달
	void SendMsgSelectItem(HTREEITEM hItem);
	// 체크 메시지 전달
	void SendMsgSelectCheckItem(HTREEITEM hItem, BOOL bChecked);
	// 팝업 메뉴
	void ShowPopupMenus(HTREEITEM hItem, CPoint point);
	// 투명도 순서 변경 시 도형 복사
	void CopyShapeItems(HTREEITEM hSourceItem, HTREEITEM hDestItem);

// 재정의입니다.
protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);

// 구현입니다.
public:
	virtual ~COverlayObjectViewTree();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDestroy();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnPopupOverlaySave();
	afx_msg void OnPopupOverlayCoord();
	afx_msg void OnPopupOverlayScreen();
	afx_msg void OnPopupShapeProperty();
	afx_msg void OnPopupShapeMove();
};
