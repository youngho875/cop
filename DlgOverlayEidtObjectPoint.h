#if !defined(AFX_DLGOVERLAYEIDTOBJECTPOINT_H__0A9CBA45_6145_4399_A227_FAE369F96E7E__INCLUDED_)
#define AFX_DLGOVERLAYEIDTOBJECTPOINT_H__0A9CBA45_6145_4399_A227_FAE369F96E7E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgOverlayEidtObjectPoint.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgOverlayEditObjectPoint dialog
#include "../LibSrc/Overlay/Object.h"


class CDlgOverlayEditObjectPoint : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgOverlayEditObjectPoint)

// Construction
public:
	CDlgOverlayEditObjectPoint(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	enum { IDD = IDD_DLG_OVERLAY_EDIT_OBJECT_POINT };


	BOOL m_bDegree;
	BOOL m_bDMS;

	CString	m_strlat;
	CString	m_strlong;
	CString	m_latdd;
	CString	m_latmm;
	CString	m_latss;
	CString	m_longdd;
	CString	m_longmm;
	CString	m_longss;
	int CheckNum;
	// MGRS.
	CString m_strMGRS;

	// GEOREF.
	CString m_strGEOREF;

	UINT m_nObjectID;
	UINT m_nPointID;
	// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgOverlayEditObject)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CDlgOverlayEditObject)
	//위경도..
	afx_msg void OnBtnOverlayEditObjectLatLng();
	afx_msg void OnRadioLatLngDegree();
	afx_msg void OnRadioLatLngDMS();
	virtual BOOL OnInitDialog();
	afx_msg void OnUpdateLatdd();
	afx_msg void OnUpdateLatmm();
	afx_msg void OnUpdateLatss();
	afx_msg void OnUpdateLongdd();
	afx_msg void OnUpdateLongmm();

	// MGRS	
	afx_msg void OnBtnOverlayEditObjectMgrs();

	//GEOREF	
	afx_msg void OnBtnOverlayEditObjectGeoref();

	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	void ShowWindowEditDlg(UINT nObjectId, UINT nPointID);	// 속성을 변경할 도형의 포인터 설정.
	void UpdatePositionData(OVERLAY::OBJECT* pObject, UINT nPointID);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGOVERLAYEIDTOBJECTPOINT_H__0A9CBA45_6145_4399_A227_FAE369F96E7E__INCLUDED_)
