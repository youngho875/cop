#pragma once

#include "afxcmn.h"
#include "./control/ShadeButtonST.h"
#include "LayerStructure.h"

//#include "../inc/ICOPS2010V11520/ICOPS_Framework/IMapProjection.h"
//#include "../inc/ICOPS2010V11520/ICOPS_WarSym/ICOPS_WarSymCommon.h"
//#include "../inc/ICOPS2010V11520/ICOPS_WarSym/IWarSymbol.h"
//#include "../inc/ICOPS2010V11520/ICOPS_WarSym/IWarSymEditOwner.h"
//#include "../inc/ICOPS2010V11520/ICOPS_WarSym/IWarSymEditTool.h"
//#include "../inc/ICOPS2010V11520/ICOPS_WarSym/CwsSymEditOwner.h"
//#include "../inc/ICOPS2010V11520/ICOPS_Framework/CTypedBuffer.h"
//#include "../inc/ICOPS2010V11520/ICOPS_Framework/ICOPS_struct.h"
//#include "../inc/ICOPS2010V11520/ICOPS_Framework/CTypedBuffer.h"
//#include "../inc/ICOPS2010V11520/ICOPS_WarSym/CwsColorset.h"
//#include "../inc/ICOPS2010V11520/ICOPS_WarSym/CwsUtil.h"
//#include "../inc/ICOPS2010V11520/ICOPS_WarSym/IWarSymbol.h"
//#include "../inc/ICOPS2010V11520/ICOPS_WarSym/IWarSymView.h"
//#include "../inc/ICOPS2010V11520/ICOPS_WarSym/IWarSymEditable.h"
//#include "../inc/ICOPS2010V11520/ICOPS_WarSym/IWarSymEditOwner.h"
//#include "../inc/ICOPS2010V11520/ICOPS_WarSym/IWarSymFactory.h"
//#include "../inc/ICOPS2010V11520/ICOPS_WarSym/IWarSymPropertyEditor.h"
//#include "../inc/ICOPS2010V11520/ICOPS_WarSym/IWarSymPrpTacSymbol.h"
//#include "../inc/ICOPS2010V11520/ICOPS_LogManager/CLog.h"
//#include "CTest_OverItem.h"



#define MAX_GRID_DETAIL				120
//


using namespace OVERLAY;

class COverlayMain;
class COverlayThread;
//class CDlgLayerControler;
class CDlgObjectList;
//class CLayerStructure;


//#define D_ObjectEllipse				0
//#define D_ObjectArc					1
//#define D_ObjectPie					3
//#define D_ObjectLine				4
//#define D_ObjectPolyLine			5
//#define D_ObjectCurve				6
//#define D_ObjectPolygon				7
//#define D_ObjectLinkedLine			8
//#define D_ObjectMultiLine			9
//#define D_ObjectLinkedCurve			10
//#define D_ObjectMultiCurve			11
//#define D_ObjectText				12
//#define D_ObjectDiamond				13
//#define D_ObjectPicture				14
//#define D_ObjectRoundRect			15
//#define D_ObjectTriangle			16
//#define D_ObjectRect				17
//#define D_ObjectPentagon			18
//#define D_ObjectSixangle			19
//#define D_ObjectEightangle			20
//#define D_ObjectGrid				21
//#define D_OverlayLayer				22


// CDlgOverlayControler dialog
class CC4IMapDoc;


//class CDlgOverlayControler;
//typedef ICOPS::CwsSymEditOwner<CDlgOverlayControler*>	CMapViewViewEditOwner;

// CDlgOverlayControler 대화 상자

class CDlgOverlayControler : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgOverlayControler)

public:
	CDlgOverlayControler(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgOverlayControler();

	enum { IDD = IDD_DLG_OVERLAYER_CONTROL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

private:
	COverlayThread* m_pOverlayThread;

	int					m_nGridDetail;
	CWnd*				m_pMainView;
	BOOL				m_bIsGrid;
	UINT				m_SelectObjectType;

	//CString m_strToolTip;
	//CEdit m_wndToolTip;
	//int m_iImage;

	CShadeButtonST m_btnObjectEllipse;//
	CShadeButtonST m_btnObjectArc;//
	CShadeButtonST m_btnObjectPie;//부채꼴
	CShadeButtonST m_btnObjectLine;
	CShadeButtonST m_btnObjectPolyLine;
	CShadeButtonST m_btnObjectCurve;
	CShadeButtonST m_btnObjectPolygon;
	CShadeButtonST m_btnObjectLinkedLine;
	CShadeButtonST m_btnObjectMultiLine;
	CShadeButtonST m_btnObjectLinkedCurve;
	CShadeButtonST m_btnObjectMultiCurve;
	CShadeButtonST m_btnObjectText;
	CShadeButtonST m_btnObjectDiamond;
	CShadeButtonST m_btnObjectPicture;
	CShadeButtonST m_btnObjectRoundRect;
	CShadeButtonST m_btnObjectTriangle;
	CShadeButtonST m_btnObjectRect;
	CShadeButtonST m_btnObjectPentagon;
	CShadeButtonST m_btnObjectSixangle;
	CShadeButtonST m_btnObjectEightangle;
	CShadeButtonST m_btnObjectGrid;

	BOOL m_bObjectEllipse;
	BOOL m_bObjectArc;
	BOOL m_bObjectPie;
	BOOL m_bObjectLine;
	BOOL m_bObjectPolyLine;
	BOOL m_bObjectCurve;
	BOOL m_bObjectPolygon;
	BOOL m_bObjectLinkedLine;
	BOOL m_bObjectMultiLine;
	BOOL m_bObjectLinkedCurve;
	BOOL m_bObjectMultiCurve;
	BOOL m_bObjectText;
	BOOL m_bObjectDiamond;
	BOOL m_bObjectPicture;
	BOOL m_bObjectRoundRect;
	BOOL m_bObjectTriangle;
	BOOL m_bObjectRect;
	BOOL m_bObjectPentagon;
	BOOL m_bObjectSixangle;
	BOOL m_bObjectEightangle;
	BOOL m_bObjectGrid;
	BOOL m_bOverlayLayer;


public:
	virtual BOOL OnInitDialog();
	BOOL BindOverlay(COverlayThread* pOverlay, CDlgObjectList* pDlgObjectList);
	void UpDateGridDraw();
	afx_msg void OnBnClickedBtnObjectLine();
	afx_msg void OnBnClickedBtnObjectCurve();
	afx_msg void OnBnClickedBtnObjectLinkedLine();
	afx_msg void OnBnClickedBtnObjectLinkedMultiLine();
	//afx_msg void OnBnClickedBtnObjectLinkedCurve();
	afx_msg void OnBnClickedBtnObjectLinkedMultiCurve();
	afx_msg void OnBnClickedBtnObjectPolyline();
	afx_msg void OnBnClickedBtnObjectPolygon();
	afx_msg void OnBnClickedBtnObjectArc();
	afx_msg void OnBnClickedBtnObjectPie();
	afx_msg void OnBnClickedBtnObjectEllipse();
	afx_msg void OnBnClickedBtnObjectTriangle();
	afx_msg void OnBnClickedBtnObjectDiamond();
	afx_msg void OnBnClickedBtnObjectRect();
	afx_msg void OnBnClickedBtnObjectRoundRect();
	afx_msg void OnBnClickedBtnObjectPentagon();
	afx_msg void OnBnClickedBtnObjectSixangle();
	afx_msg void OnBnClickedBtnObjectEightangle();
	afx_msg void OnBnClickedBtnObjectPicture();
	afx_msg void OnBnClickedBtnObjectText();
	afx_msg void OnBnClickedBtnShowGrid();
	//afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnClose();
	void ResetItemEvent(int KindObject);


	//afx_msg void OnBnClickedButton1();
	//afx_msg void OnBnClickedButton2();

public:
	afx_msg void OnBnClickedBtnLayerMoveTop();
	afx_msg void OnBnClickedBtnLayerMoveUp();
	afx_msg void OnBnClickedBtnLayerMoveDown();
	afx_msg void OnBnClickedBtnLayerMoveBottom();
	afx_msg void OnBnClickedBtnAddLayer();
	afx_msg void OnBnClickedBtnDeleteLayer();
	afx_msg void OnNMClickListOverlayLayer(NMHDR* pNMHDR, LRESULT* pResult);

private:
	CListCtrl		m_LayerList;
	COverlayMain*	m_pMainOverlay;
	CDlgObjectList* m_pDlgObjectList;
	UINT			m_nSelectedItemIndex;


	void InitList();
	void InitColumn();
	//void AddRawWithLayerList();
	void AddRaw(CLayerStructure* pLayer);
public:
	CString m_strLayerNameForAdd;
	CString m_strSelectedLayerName;

	void AddRawWithLayerList();
	void BindObjectListDlg(COverlayMain* pOverlay, CDlgObjectList* pDlgObjectList);
	void Clear();

	afx_msg void OnBnClickedBtnLayerDeleteAll();
	//afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnBnClickedBtnLayerSave();
	afx_msg void OnBnClickedBtnLayerLoad();
	//afx_msg void OnClose();
	
	void OnBnClickedRefresh();


	// 전술부호
public:
	//CMapViewViewEditOwner* m_pEditOwner;

	//ICOPS::IMapProjection* m_piMapProj;
	//ICOPS::IWarSymEditTool* m_piEditTool;
	BOOL	m_bCacheSymImg;
	double	m_rMPW;
	BOOL	m_bSymbolDraw;
	CString		m_strLastSymCode;	// 최근에 선택된 부호 코드
	Gdiplus::Bitmap* m_pDoubleBuffer;	// 더블버퍼링용 이미지

	//virtual void GetEditables(ICOPS::CPtrArrayIWarSymEditable& ret);
	//virtual void AddEditable(ICOPS::IWarSymEditable* piEditable, BOOL bCompleted);
	//virtual void DeleteEditable(ICOPS::IWarSymEditable* piEditable);
	//virtual void DoContextMenu(Gdiplus::Graphics* pGrpx, UINT nFlags, const CPoint& point, CPtrArrayIWarSymEditable* pSelected);
	virtual void Refresh();
	virtual CWnd* GetWnd();

public:
	void DrawOverItems(Gdiplus::Graphics* pGrpx);
	void ApplyDrawModeToAll();
	//void InitProjection();
	//void InitSymbolEditTool();
	void SelectSymbol();


	void OnEditProperty();
	void OnEditDelete();
	void SpecialSymbol();
	
};
