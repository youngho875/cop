
#pragma once

#include "OverlayObjectViewTree.h"

class COverlayObjectViewToolBar : public CMFCToolBar
{
	virtual void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler)
	{
		CMFCToolBar::OnUpdateCmdUI((CFrameWnd*) GetOwner(), bDisableIfNoHndler);
	}

	virtual BOOL AllowShowOnList() const { return FALSE; }
};

// 투명도 관리 창
class COverlayObjectView : public CDockablePane
{
// 생성입니다.
public:
	COverlayObjectView();
	virtual ~COverlayObjectView();

	void AdjustLayout();
	void OnChangeVisualStyle();

	// 구현입니다.
public:
	// 투명도 추가
	void AddOverlay(CString strName, CString strGuid, BOOL bCoordMap);
	// 도형 목록 정보 추가
	void AddShapeList(CString strOverlayGuid, CString strShapeList);
	// 도형 추가
	void AddShape(CString strOverlayGuid, CString strName, CString strGuid, BOOL bVisible);

	// 투명도 전체 삭제
	void RemoveOverlayAll();
	// 투명도 선택 삭제
	void RemoveOverlay(CString strGuid);
	// 도형 전체 삭제
	void RemoveShapeAll(CString strOverlayGuid);
	// 도형 선택 삭제
	void RemoveShape(CString strOverlayGuid, CString strShapeGuid);

	// 투명도 선택
	void SelectOverlay(CString strOverlayGuid);
	// 도형 선택
	void SelectShape(CString strOverlayGuid, CString strShapeGuid);

	// 투명도 체크 설정
	void SetCheckOverlay(CString strOverlayGuid, BOOL bCheck);
	// 도형 체크 설정
	void SetCheckShape(CString strOverlayGuid, CString strShapeGuid, BOOL bCheck);

	// 도형 앞으로
	void MoveForwardShape(CString strOverlayGuid, CString strShapeGuid);
	// 도형 뒤로
	void MoveBackwardShape(CString strOverlayGuid, CString strShapeGuid);
	// 도형 맨 앞으로
	void MoveTopShape(CString strOverlayGuid, CString strShapeGuid);
	// 도형 맨 뒤로
	void MoveBottomShape(CString strOverlayGuid, CString strShapeGuid);

	// 투명도 앞으로
	void MoveForwardOverlay(CString strOverlayGuid);
	// 투명도 뒤로
	void MoveBackwardOverlay(CString strOverlayGuid);
	// 투명도 맨 앞으로
	void MoveTopOverlay(CString strOverlayGuid);
	// 투명도 맨 뒤로
	void MoveBottomOverlay(CString strOverlayGuid);

	// 투명도 좌표계 설정
	void SetOverlayCoordMap(CString strOverlayGuid, BOOL bCoordMap);
	// 도형 명칭 변경
	void SetShapeName(CString strOverlayGuid, CString strShapeGuid, CString strShapeName);
	// 투명도 명칭 변경
	void SetLayerName(CString strOverlayGuid, CString strLayerName);

// 특성입니다.
protected:
	COverlayObjectViewTree m_wndOverlayObjectViewTree;			// 투명도 관리 트리
	CImageList m_OverlayObjectViewImages;						// 투명도 관리 트리 아이템 이미지
	CImageList m_TreeImagesCheckBox;							// 투명도 관리 트리 체크 이미지
	COverlayObjectViewToolBar m_wndOverlayObjectViewToolBar;	// 투명도 관리 툴바 이미지

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnPaint();

	afx_msg void OnOverlayViewTop();
	afx_msg void OnOverlayViewUp();
	afx_msg void OnOverlayViewDown();
	afx_msg void OnOverlayViewBottom();
	afx_msg void OnOverlayViewNew();
	afx_msg void OnOverlayViewOpen();
	afx_msg void OnOverlayViewSave();
	afx_msg void OnOverlayViewDelete();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDestroy();
};

