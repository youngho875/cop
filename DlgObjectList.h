#pragma once

#include "../LIBSrc/Overlay/Object.h"

// CDlgObjectList 대화 상자

class COverlayMain;

class CDlgObjectList : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgObjectList)

public:
	CDlgObjectList(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgObjectList();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_OBJECT_DRAW_ORDER_CONTROL };


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

private:
	CListCtrl		m_ObjectList;
	COverlayMain*	m_pOverlayMain;
	int				m_nSelectedLayer;
	int				m_nSelectedItem;
	CString			m_strSelectedObjectType;
	CString			m_strSelectedObjectID;
public:

	void BindOverlay(COverlayMain* pOverlay);
	void InitColumn();
	void SetLayer(UINT nLayerIndex);
	void AddRaw(OVERLAY::OBJECT* pObject);

	afx_msg void OnBnClickedBtnObjectMoveTop();
	afx_msg void OnBnClickedBtnObjectMoveUp();
	afx_msg void OnBnClickedBtnObjectMoveDown();
	afx_msg void OnBnClickedBtnObjectMoveBottom();
	afx_msg void OnBnClickedBtnObjectDeleteObject();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnHdnItemclickListObjectList(NMHDR* pNMHDR, LRESULT* pResult);

private:
	void ClearList();
	void UpdateListItem();
};
