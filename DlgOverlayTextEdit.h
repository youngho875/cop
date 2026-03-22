#ifndef _DLG_OVERLAY_TEXT_EDIT_H_
#define _DLG_OVERLAY_TEXT_EDIT_H_

#pragma once
#include "../LibSrc/Overlay/Object.h"
#include "../LibSrc/Overlay/ObjectText.h"
#include "./Control/ColorPicker.h"

#define TIMER_SHOW_PREVIEW_TEXT 10

class COverlayView;

class CDlgOverlayTextEdit : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgOverlayTextEdit)

public:
	CDlgOverlayTextEdit(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	enum { IDD = IDD_DLG_OVERLAY_TEXT_EDIT };


		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgOverlayTextEditor)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);       // DDX/DDV support
	//}}AFX_VIRTUAL

protected:

	COverlayView*				m_pOverlayView;
	OVERLAY::ObjectText*		m_pText;
	CString						m_strEdit;
	CString						m_strTextBlend;
	CStatic						m_staticPreview;
	CColorPicker				m_btnObjectTextColor;
	CComboBox					m_cmbObjectTextFontSize;
	CComboBox					m_cmbObjectTextFontStyle;
	CComboBox					m_cmbObjectTextFontStyle2;
	CComboBox					m_cmbObjectTextFont;
	CSliderCtrl					m_SliderTextBlend;
	COLORREF					m_TextFontBlend;
	int							m_NewFontSize;
	int							m_OldFontSize;
	BOOL						m_bCheckUnderLine;
	BOOL						m_bCheckStrikeOut;

	afx_msg void OnBtnOverlayTextOk();
	afx_msg void OnNMReleasedcaptureSliderOverlayTextBlend(NMHDR *pNMHDR, LRESULT *pResult);
	//afx_msg void OnSelChangeOverlayFontColorFontSize(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelChangeOverlayFontColorFontSize();
	afx_msg void OnSelChangeOverlayFontColorFontStyle();
	afx_msg void OnSelChangeOverlayFontColorFontStyle2();
	afx_msg void OnSelChangeOverlayFont();
	afx_msg void OnSelEditChangeOverlayFontSize();
	afx_msg LRESULT OnBnClickedBtnOverlayFontColor(WPARAM DlgCtrlID, LPARAM lParam);
	afx_msg void OnCheckTextUnderLine();
	afx_msg void OnCheckTextStrikeOut();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	DECLARE_MESSAGE_MAP()

private:
	void PreviewTextStyle(CStatic& staticWindow);

public:	
	virtual BOOL OnInitDialog();
	void ShowWindowProperty(OVERLAY::OBJECT* pObject);
	void BindOverlayView(COverlayView* pOverlayView);
};

#endif
