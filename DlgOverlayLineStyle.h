#ifndef DLG_OVERLAY_LINE_STYLE_H
#define	DLG_OVERLAY_LINE_STYLE_H
#pragma once


#include "./Control/ShadeButtonST.h"

class CDlgOverlayProperty;

class CDlgOverlayLineStyle : public CDialog
{
	DECLARE_DYNAMIC(CDlgOverlayLineStyle)

public:
	CDlgOverlayLineStyle(CWnd* pParent = NULL);
	virtual ~CDlgOverlayLineStyle();

	enum { IDD = IDD_DLG_OVERLAY_LINE_DASH_STYLE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()

private:
	CDlgOverlayProperty*	m_pDlgOverlayProperty;
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedBtnOverlayLineDashStyleSolid();
	afx_msg void OnBnClickedBtnOverlayLineDashStyleDash();
	afx_msg void OnBnClickedBtnOverlayLineDashStyleDot();
	afx_msg void OnBnClickedBtnOverlayLineDashStyleDashdot();
	afx_msg void OnBnClickedBtnOverlayLineDashStyleDashdotdot();
	CMFCButton m_btnSolid;
	CMFCButton m_btnDash;
	CMFCButton m_btnDot;
	CMFCButton m_btnDashDot;
	CMFCButton m_btnDashDotDot;
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};

#endif