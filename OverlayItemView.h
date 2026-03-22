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

#pragma once

#include "ViewList.h"
//#include "MapXMsgProcOVLBase_Overlay_List.h"
//#include "ShapeToXML.h"


class COverlayItemToolBar : public CMFCToolBar
{
	virtual void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler)
	{
		CMFCToolBar::OnUpdateCmdUI((CFrameWnd*) GetOwner(), bDisableIfNoHndler);
	}

	virtual BOOL AllowShowOnList() const { return FALSE; }
};

class COverlayItemView : public CDockablePane
{
public:
	COverlayItemView();
	virtual ~COverlayItemView();

	void AdjustLayout();
	void OnChangeVisualStyle();
	CListCtrl * GetView();
	//void SetMsgMgr(CMapXMsgProcBase_Overlay_List * pMsgBaseList);
	void RefreshMsgList();
protected:
	void msgBaseListPatrol(BOOL bPatrol);
	void ResetDrawItemFind();
private:

protected:
	COverlayItemToolBar m_wndToolBar;
	CViewList m_wndView;
	CImageList m_ViewImages;
	UINT m_nCurrSort;

private:
	int								m_nlistCtrl_DrawItem_Rs_cursor;
	/*
	@brief 도시요소 관리자
	*/
	//CMapXMsgProcBase_Overlay_List*	m_pMsgBaseOverList;

	void FillClassView();

// 재정의입니다.
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnCOPAddMemberFunction();
	afx_msg void OnCOPAddMemberVariable();
	afx_msg void OnCOPDefinition();
	afx_msg void OnCOPProperties();
	afx_msg void OnNewFolder_COP();
	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg LRESULT OnChangeActiveTab(WPARAM, LPARAM);
	afx_msg void OnSort(UINT id);
	afx_msg void OnUpdateSort(CCmdUI* pCmdUI);
	afx_msg void OnNMClickListDrawitem(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkListDrawitem(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemchangedListDrawitem(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRClickListDrawitem(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawListDrawitem(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnGetdispinfoOwnerDrawitem(NMHDR *pNMHDR, LRESULT *pResult);

	DECLARE_MESSAGE_MAP()
};

