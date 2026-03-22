#if !defined(AFX_DLGOVERLAYEDITOBJECT_H__92CE9A2A_0BDD_48B4_995A_83AD61957565__INCLUDED_)
#define AFX_DLGOVERLAYEDITOBJECT_H__92CE9A2A_0BDD_48B4_995A_83AD61957565__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgOverlayEditObject.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// CDlgOverlayEditObject dialog
#include "../LibSrc/Overlay/Object.h"


class CDlgOverlayEditObject : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgOverlayEditObject)

// Construction
public:
	CDlgOverlayEditObject(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	enum { IDD = IDD_DLG_OVERLAY_EDIT_OBJECT };

private:
	BOOL m_bDegree;
	BOOL m_bDMS;

	CWnd*	m_pView;
	CString	m_strlat_left_top;
	CString	m_strlong_left_top;
	CString	m_latdd_left_top;
	CString	m_latmm_left_top;
	CString	m_latss_left_top;
	CString	m_longdd_left_top;
	CString	m_longmm_left_top;
	CString	m_longss_left_top;
	int CheckNum;
	////////////////////////////
	CString	m_strlat_right_bottom;
	CString	m_strlong_right_bottom;
	CString	m_latdd_right_bottom;
	CString	m_latmm_right_bottom;
	CString	m_latss_right_bottom;
	CString	m_longdd_right_bottom;
	CString	m_longmm_right_bottom;
	CString	m_longss_right_bottom;
	
	double  m_dRotation;

	// MGRS.
	CString m_strMGRS_left_top;
	CString m_strMGRS_right_bottom;

	// GEOREF.
	CString m_strGEOREF_left_top;
	CString m_strGEOREF_right_bottom;

	UINT m_nObjectID;
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
	afx_msg void OnUpdateLeftTopLatdd();
	afx_msg void OnUpdateLeftTopLatmm();
	afx_msg void OnUpdateLeftTopLatss();
	afx_msg void OnUpdateLeftTopLongdd();
	afx_msg void OnUpdateLeftTopLongmm();

	afx_msg void OnUpdateRightBottomLatdd();
	afx_msg void OnUpdateRightBottomLatmm();
	afx_msg void OnUpdateRightBottomLatss();
	afx_msg void OnUpdateRightBottomLongdd();
	afx_msg void OnUpdateRightBottomLongmm();

	afx_msg void OnUpdateObjectRoatation();

	// MGRS	
	afx_msg void OnBtnOverlayEditObjectMgrs();

	//GEOREF	
	afx_msg void OnBtnOverlayEditObjectGeoref();

	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	void ShowWindowEditDlg(UINT nObjectId);	// 속성을 변경할 도형의 포인터 설정.
	void UpdatePositionData(OVERLAY::OBJECT* pObject);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGOVERLAYEDITOBJECT_H__92CE9A2A_0BDD_48B4_995A_83AD61957565__INCLUDED_)
