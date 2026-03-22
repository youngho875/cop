#ifndef DLG_OVERLAY_PROPERTY_H
#define DLG_OVERLAY_PROPERTY_H
#pragma once
#include "../LibSrc/Overlay/Object.h"
#include "./control/ColorPicker.h"
#include "afxcmn.h"
#include "afxwin.h"

//class CDlgOverlayLineStyle;
class CC4IMapView;
class COverlayView;

#define TIMER_SHOW_PREVIEW 1

class CDlgOverlayProperty : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgOverlayProperty)

public:
	CDlgOverlayProperty(CWnd* pParent, COverlayView* pOverlayView);
	virtual ~CDlgOverlayProperty();

	// Dialog Data
	enum { IDD = IDD_DLG_OVERLAY_PROPERTY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

private:
	//CDlgOverlayLineStyle*		m_pOverlayLineStyle;
	CC4IMapView*				m_pMainView;
	COverlayView*				m_pOverlayView;
	OVERLAY::OBJECT*	m_pObject; // 속성을 변경할 도형의 포인터
	OVERLAY::ObjectProperty	m_Property;

	CComboBox					m_cmbLineDashStyle;
	CComboBox					m_cmbLineStartKind;
	CComboBox					m_cmbLineEndKind;

	void SetPreviewColor(CStatic& staticWindow, COLORREF color);
	void SetPreviewLine(CStatic& staticWindow, OVERLAY::ObjectProperty& Property);

public:
	void InitCombo();
	void InitSlider();
	void InitSpin();
	void ShowWindowProperty(OVERLAY::OBJECT* pObject);	// 속성을 변경할 도형의 포인터 설정.
		
	UINT m_nEdtFillBlend;
	UINT m_nEdtLineBlend;
	//CEdit m_edtLineBlend;
	UINT m_nEdtLineThickness;
	CSliderCtrl m_SliderFillBlend;
	CSliderCtrl m_SliderLineBlend;
	CSpinButtonCtrl m_SpinLineBlend;
	CSpinButtonCtrl m_SpinFillBlend;
	CSpinButtonCtrl m_SpinLineThickness;
	CString m_strFillColor;
	CString m_strLineColor;
	CStatic m_staticFillColor;
	CStatic m_staticLineColor;
	CStatic m_staticLineThick;
	CColorPicker m_btnFillColor;
	CColorPicker m_btnLineColor;
	BOOL m_bFill;
	BOOL m_bLine;
	CButton m_RadioOverlayNoFill; //색 없음 
	CButton m_RadioOverlayNoLine; //선 없음

	afx_msg void OnBnClickedRadioOverlayFill();
	afx_msg void OnBnClickedRadioOverlayLine();
	afx_msg void OnBnClickedRadioOverlayNoFill();
	afx_msg void OnBnClickedRadioOverlayNoLine();
	//afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnNMReleasedcaptureSliderOverlayFillBlend(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMReleasedcaptureSliderOverlayLineBlend(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinOverlayLineBlend(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinOverlayFillBlend(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinOverlayLineThickness(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeEdtOverlayFillBlend();
	afx_msg void OnEnChangeEdtOverlayLineBlend();
	afx_msg void OnEnChangeEdtOverlayThickness();
	afx_msg void OnBnClickedBtnOverlayPropertyOk();
	afx_msg void OnBnClickedBtnOverlayPropertyCancel();
	afx_msg void OnSelChangeLineDashStyle();
	afx_msg void OnSelChangeLineStartStyle();
	afx_msg void OnSelChangeLineEndStyle();
	afx_msg LRESULT OnBnClickedBtnOverlayLineFillColor(WPARAM DlgCtrlID, LPARAM lParam);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnTimer(UINT_PTR nIDEvent);

};
#endif