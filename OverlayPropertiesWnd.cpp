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

#include "pch.h"
#include "C4IMap.h"
#include "MainFrm.h"
#include "C4IMapView.h"
#include "OverlayPropertiesWnd.h"

#include "ComDefValue.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CResourceViewBar

COverlayPropertiesWnd::COverlayPropertiesWnd()
{
	InitOverlayPropertyGroup();
}

COverlayPropertiesWnd::~COverlayPropertiesWnd()
{
}

BEGIN_MESSAGE_MAP(COverlayPropertiesWnd, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_EXPAND_ALL, OnExpandAllProperties)
	ON_UPDATE_COMMAND_UI(ID_EXPAND_ALL, OnUpdateExpandAllProperties)
	ON_COMMAND(ID_SORTPROPERTIES, OnSortProperties)
	ON_UPDATE_COMMAND_UI(ID_SORTPROPERTIES, OnUpdateSortProperties)
	ON_WM_SETFOCUS()
	ON_WM_SETTINGCHANGE()
	ON_REGISTERED_MESSAGE(AFX_WM_PROPERTY_CHANGED, OnPropertyChanged)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CResourceViewBar 메시지 처리기

void COverlayPropertiesWnd::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	int cyTlb = m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;

	m_wndToolBar.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndPropList.SetWindowPos(NULL, rectClient.left, rectClient.top + cyTlb, rectClient.Width(), rectClient.Height() - cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
}

int COverlayPropertiesWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	if (!m_wndPropList.Create(WS_VISIBLE | WS_CHILD, rectDummy, this, 2))
	{
		TRACE0("속성 표를 만들지 못했습니다. \n");
		return -1;      // 만들지 못했습니다.
	}

	InitPropList();

	m_wndToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_PROPERTIES);
	m_wndToolBar.LoadToolBar(IDR_PROPERTIES, 0, 0, TRUE /* 잠금 */);
	m_wndToolBar.CleanUpLockedImages();
	m_wndToolBar.LoadBitmap(theApp.m_bHiColorIcons ? IDB_PROPERTIES_HC : IDR_PROPERTIES, 0, 0, TRUE /* 잠금 */);

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);
	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));
	m_wndToolBar.SetOwner(this);

	// 모든 명령은 부모 프레임이 아닌 이 컨트롤을 통해 라우팅됩니다.
	m_wndToolBar.SetRouteCommandsViaFrame(FALSE);

	AdjustLayout();
	return 0;
}

void COverlayPropertiesWnd::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void COverlayPropertiesWnd::OnExpandAllProperties()
{
	m_wndPropList.ExpandAll();
}

void COverlayPropertiesWnd::OnUpdateExpandAllProperties(CCmdUI* pCmdUI)
{
}

void COverlayPropertiesWnd::OnSortProperties()
{
	m_wndPropList.SetAlphabeticMode(!m_wndPropList.IsAlphabeticMode());
}

void COverlayPropertiesWnd::OnUpdateSortProperties(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_wndPropList.IsAlphabeticMode());
}

void COverlayPropertiesWnd::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);
	m_wndPropList.SetFocus();
}

void COverlayPropertiesWnd::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CDockablePane::OnSettingChange(uFlags, lpszSection);
	SetPropListFont();
}

void COverlayPropertiesWnd::SetPropListFont()
{
	::DeleteObject(m_fntPropList.Detach());

	LOGFONT lf;
	afxGlobalData.fontRegular.GetLogFont(&lf);

	NONCLIENTMETRICS info;
	info.cbSize = sizeof(info);

	afxGlobalData.GetNonClientMetrics(info);

	lf.lfHeight = info.lfMenuFont.lfHeight;
	lf.lfWeight = info.lfMenuFont.lfWeight;
	lf.lfItalic = info.lfMenuFont.lfItalic;

	m_fntPropList.CreateFontIndirect(&lf);

	m_wndPropList.SetFont(&m_fntPropList);
}


void COverlayPropertiesWnd::InitPropList()
{
	SetPropListFont();

	m_wndPropList.EnableHeaderCtrl(FALSE);
	m_wndPropList.EnableDescriptionArea();
	m_wndPropList.SetVSDotNetLook();
	m_wndPropList.MarkModifiedProperties();
}

void COverlayPropertiesWnd::ClearPropList()
{
	m_wndPropList.RemoveAll();
	m_wndPropList.Invalidate(FALSE);
	InitOverlayPropertyGroup();
}

// 투명도 레이어 속성 정보 설정
void COverlayPropertiesWnd::SetPropertiesAppLayer(CString strName, CString strGuid)
{
	ClearPropList();

	m_strOverLayerGuid = strGuid;
	m_strShapeName = strName;

	CMFCPropertyGridProperty* pShapePropertyGroup = new CMFCPropertyGridProperty(DEF_PROPERTY_OVERLAY_TITLE);
	pShapePropertyGroup->SetData(LAYERPROPERTY_TITLE);

	CMFCPropertyGridProperty * pString = new CMFCPropertyGridProperty(DEF_PROPERTY_OVERLAY_DEFAULT_CAPTION_NAME, (_variant_t)strName, DEF_PROPERTY_OVERLAY_DEFAULT_CAPTION_NAME_REMARK);
	pString->SetData(LAYERPROPERTY_NAME);
	pShapePropertyGroup->AddSubItem(pString);

	m_wndPropList.AddProperty(pShapePropertyGroup);
}

// 투명도 도형 속성 정보 설정
void COverlayPropertiesWnd::SetPropertyShape(CShape* pShape, CString strOverlayGuid)
{
	ClearPropList();

	if(pShape != NULL)
	{
		// 공통 속성 획득
		//pShape->AddPropertyPage();

		m_strShapeDescription = pShape->GetObjectDescription(); // 설명/비고			
		m_strShapeName = pShape->GetObjectName();				// 투명도 명칭
		m_strOverLayerGuid = strOverlayGuid;					// 투명도 식별 Guid
		m_strShapeGuid = pShape->GetObjectID();					// 객제 식별 Guid
		m_nShapeKind = pShape->GetObjectKind();					// 객체 종류
		m_bUseEditFillProperty = TRUE;							// 선속성만 사용 여부

		// 명칭 설명 설정
		AddShapeCommonProperty(m_strShapeName, m_strShapeDescription);

		switch(pShape->GetObjectKind())
		{
		case ID_OT_DRAW_POLYLINE:
		case ID_OT_DRAW_SPLINE:
		case ID_OT_DRAW_LINE:
		case ID_OT_DRAW_POLYGON:
		case ID_OT_DRAW_SPLINE_CLOSED:
		case ID_OT_DRAW_ARC:
		case ID_OT_DRAW_RECT:
		case ID_OT_DRAW_ROUNDRECT:
		case ID_OT_DRAW_TRIANGLE:
		case ID_OT_DRAW_NGON:
		case ID_OT_DRAW_CIRCLE:
		case ID_OT_DRAW_ELLIPSE:
		case ID_OT_DRAW_PIE:	
		/*case ID_OT_DRAW_ARROW:
		case ID_OT_DRAW_STAR:
		case ID_OT_DRAW_SECTOR:
		case ID_OT_DRAW_TEXT:*/
			{
				//AddShapeEditProperty(pShape->m_AttrShape); // 도형 편집 속성

				switch(pShape->GetObjectKind())
				{
				case ID_OT_DRAW_ARC: // 호
					m_bUseEditFillProperty = FALSE;
					break;

				case ID_OT_DRAW_POLYLINE:
				case ID_OT_DRAW_SPLINE:
				case ID_OT_DRAW_LINE:
				case ID_OT_DRAW_POLYGON:
				case ID_OT_DRAW_SPLINE_CLOSED:
					{/*
						((CShapeSpline*)pShape)->AddPropertyPage();

						m_nShapeKind = ((CShapeSpline*)pShape)->GetObjectKind();

						m_bUseEditFillProperty = ((CShapeSpline*)pShape)->m_AttrShapeSpline.m_bClosed;

						AddShapeSplineProperty(((CShapeSpline*)pShape)->m_AttrShapeSpline);*/
					}
					break;

				case ID_OT_DRAW_RECT:
				case ID_OT_DRAW_ROUNDRECT:
					{
						((CShapeRect*)pShape)->AddPropertyPage();
						m_nShapeKind = ((CShapeRect*)pShape)->GetObjectKind();
						AddShapeRectProperty(((CShapeRect*)pShape)->m_AttrShapeRect);
						//int nRound = ((CShapeRect*)pShape)->GetRound().x;
						//((CShapeRect*)pShape)->m_AttrShapeRect.m_nRound = 100;
						//((CShapeRect*)pShape)->UpdateData(TRUE);
					}
					break;

				case ID_OT_DRAW_NGON:
					{
						((CShapeNgon*)pShape)->AddPropertyPage();
						AddShapeNgonProperty(((CShapeNgon*)pShape)->m_AttrShapeNgon);
					}
					break;

					/*
				case ID_OT_DRAW_ARROW:
					{
						((CShapeArrow*)pShape)->AddPropertyPage();
						AddShapeArrowProperty(((CShapeArrow*)pShape)->m_AttrShapeArrow);
					}
					break;
					
				case ID_OT_DRAW_TEXT:
					{
						((CShapePointText*)pShape)->AddPropertyPage();
						AddShapeTextProperty(((CShapePointText*)pShape)->m_AttrShapePointText);
					}
					break;
					*/
				}
			}
			break;

#ifdef _MILITARY
		// 군대부호 점형
		case ID_OT_DRAW_POINT:
		case ID_OT_DRAW_OVEREXTPOINT:
			{
				((CShapePoint*)pShape)->AddPropertyPage();
				AddShapeMilPointProperty(((CShapePoint*)pShape)->GetAttrShapePoint(), pShape->GetObjectKind());
				m_strShapePointModifier = ((CShapePoint*)pShape)->GetTacSymModifierFlag();
			}
			break;

		// 비행회랑
		case ID_OT_DRAW_OVEREXTLINEAVIATION:
			{
				((COverlayBaseExtLineAviation*)pShape)->AddPropertyPage();
				AddShapeMilAirProperty(((COverlayBaseExtLineAviation*)pShape)->GetSelectInfo());
			}
			break;
#endif
		default:
			{
#ifdef _MILITARY
				// 군대부호 선,면형
				if(ID_OT_DRAW_OVEREXTPOINT < pShape->GetObjectKind() && pShape->GetObjectKind() < ID_OT_DRAW_OVEREXTLINEAVIATION)
				{
					((COverlayBaseExtLine*)pShape)->AddPropertyPage();
					AddShapeMilExtProperty(((COverlayBaseExtLine*)pShape)->m_AttrShapeExt);
					m_strShapeExtModifier = ((COverlayBaseExtLine*)pShape)->GetTacSymModifier();
				}
#endif
			}
			break;
		}

		UpdateShapeEditProperties();
	}
}

// 투명도 그룹 정보 초기화
void COverlayPropertiesWnd::InitOverlayPropertyGroup()
{
	m_bUseEditFillProperty = TRUE;
	m_nShapeKind = 0;

	m_pShapePropertyEditLineWidth = NULL;
	m_pShapePropertyEditLineColor = NULL;
	m_pShapePropertyEditLineStyle = NULL;
	m_pShapePropertyEditFillUse = NULL;
	m_pShapePropertyEditFillColor = NULL;
	m_pShapePropertyEditFillTrance = NULL;
	m_pShapePropertyEditPatternUse = NULL;
	m_pShapePropertyEditPatternColor = NULL;
	m_pShapePropertyEditPatternStyle = NULL;
	m_pShapePropertyEditRotate = NULL;

	m_pLineShapeStartArrow = NULL;
	m_pLineShapeEndArrow = NULL;

	m_pShapePropertyTextName = NULL;	// 글꼴
	m_pShapePropertyTextSize = NULL;	// 텍스트 크기
	m_pShapePropertyTextColor = NULL;	// 텍스트 색상
	m_pShapePropertyTextBold = NULL;	// 굵게 사용 여부
	m_pShapePropertyTextItalic = NULL;	// 기울이기
	m_pShapePropertyTextUnderline = NULL;
	m_pShapePropertyTextStrikeOut = NULL;
	m_pShapePropertyTextAlign = NULL;
}

// 명칭 및 설명
void COverlayPropertiesWnd::AddShapeCommonProperty(CString strShapeName, CString strShapeDescription)
{
	CMFCPropertyGridProperty* pShapePropertyGroupCommon = new CMFCPropertyGridProperty(DEF_PROPERTY_OVERLAY_DEFAULT_TITLE);
	pShapePropertyGroupCommon->SetData(SHAPEPROPERTY_COMMON);

	CMFCPropertyGridProperty* pString = new CMFCPropertyGridProperty(DEF_PROPERTY_OVERLAY_DEFAULT_CAPTION_NAME, (_variant_t)strShapeName, DEF_PROPERTY_OVERLAY_DEFAULT_CAPTION_NAME_REMARK);
	pString->SetData(SHAPEPROPERTY_NAME);
	pShapePropertyGroupCommon->AddSubItem(pString);

	pString = new CMFCPropertyGridProperty(DEF_PROPERTY_OVERLAY_DEFAULT_CAPTION_DES, (_variant_t)strShapeDescription, DEF_PROPERTY_OVERLAY_DEFAULT_CAPTION_DES_REMARK);
	pString->SetData(SHAPEPROPERTY_DESCRIPT);
	pShapePropertyGroupCommon->AddSubItem(pString);

	m_wndPropList.AddProperty(pShapePropertyGroupCommon);
}

// 도형 편집 속성
/*
void COverlayPropertiesWnd::AddShapeEditProperty(CAttrShape attrShape)
{
	m_AttrShape = attrShape;

	CMFCPropertyGridProperty* pShapePropertyGroupEdit = new CMFCPropertyGridProperty(DEF_PROPERTY_OVERLAY_EDIT_TITLE);
	pShapePropertyGroupEdit->SetData(SHAPEPROPERTY_EDIT);

	// 선 굵기
	m_pShapePropertyEditLineWidth = new CBoundedNumberSubProp(DEF_PROPERTY_OVERLAY_EDIT_CAPTION_LINEWIDTH, (_variant_t)m_AttrShape.m_nPenSize, 0, 20, DEF_PROPERTY_OVERLAY_EDIT_CAPTION_LINEWIDTH_REMARK, _T("dd"), _T("__"));
	m_pShapePropertyEditLineWidth->SetData(SHAPEPROPERTY_EDIT_LINEWIDTH);
	m_pShapePropertyEditLineWidth->EnableSpinControl(TRUE, 0, 20);
	pShapePropertyGroupEdit->AddSubItem(m_pShapePropertyEditLineWidth);

	// 선 색상
	m_pShapePropertyEditLineColor = new CMFCPropertyGridColorProperty(DEF_PROPERTY_OVERLAY_EDIT_CAPTION_LINECOLOR, m_AttrShape.m_ColorPen, NULL, DEF_PROPERTY_OVERLAY_EDIT_CAPTION_LINECOLOR_REMARK, 0);
	m_pShapePropertyEditLineColor->SetData(SHAPEPROPERTY_EDIT_LINECOLOR);
	m_pShapePropertyEditLineColor->EnableOtherButton(DEF_PROPERTY_USER_VALUE);
	m_pShapePropertyEditLineColor->AllowEdit(FALSE);
	pShapePropertyGroupEdit->AddSubItem(m_pShapePropertyEditLineColor);

	// 선 모양
	m_pShapePropertyEditLineStyle = new CMFCPropertyGridProperty(DEF_PROPERTY_OVERLAY_EDIT_CAPTION_LINESTYLE, _T(""), DEF_PROPERTY_OVERLAY_EDIT_CAPTION_LINESTYLE_REMARK);
	m_pShapePropertyEditLineStyle->SetData(SHAPEPROPERTY_EDIT_LINEKIND);
	m_pShapePropertyEditLineStyle->AddOption(DEF_PROPERTY_LINESTYLE_SOLID);
	m_pShapePropertyEditLineStyle->AddOption(DEF_PROPERTY_LINESTYLE_DASH);
	m_pShapePropertyEditLineStyle->AddOption(DEF_PROPERTY_LINESTYLE_DOT);
	m_pShapePropertyEditLineStyle->AddOption(DEF_PROPERTY_LINESTYLE_DASHDOT);
	m_pShapePropertyEditLineStyle->AddOption(DEF_PROPERTY_LINESTYLE_DASHDOTDOT);
	m_pShapePropertyEditLineStyle->AllowEdit(FALSE);
	pShapePropertyGroupEdit->AddSubItem(m_pShapePropertyEditLineStyle);

	switch(m_AttrShape.m_nStyle)
	{
	case 1:
		m_pShapePropertyEditLineStyle->SetValue(DEF_PROPERTY_LINESTYLE_DASH);
		break;
	case 2:
		m_pShapePropertyEditLineStyle->SetValue(DEF_PROPERTY_LINESTYLE_DOT);
		break;
	case 3:
		m_pShapePropertyEditLineStyle->SetValue(DEF_PROPERTY_LINESTYLE_DASHDOT);
		break;
	case 4:
		m_pShapePropertyEditLineStyle->SetValue(DEF_PROPERTY_LINESTYLE_DASHDOTDOT);
		break;
	case 0:
	default:
		m_pShapePropertyEditLineStyle->SetValue(DEF_PROPERTY_LINESTYLE_SOLID);
		break;
	}

	// 채움 사용 여부
	m_pShapePropertyEditFillUse = new CMFCPropertyGridProperty(DEF_PROPERTY_OVERLAY_EDIT_CAPTION_FILLUSE, _T(""), DEF_PROPERTY_OVERLAY_EDIT_CAPTION_FILLUSE_REMARK);
	m_pShapePropertyEditFillUse->SetData(SHAPEPROPERTY_EDIT_FILLUSE);
	m_pShapePropertyEditFillUse->AddOption(DEF_PROPERTY_ENABLE);
	m_pShapePropertyEditFillUse->AddOption(DEF_PROPERTY_DISABLE);
	m_pShapePropertyEditFillUse->AllowEdit(FALSE);
	pShapePropertyGroupEdit->AddSubItem(m_pShapePropertyEditFillUse);

	if(m_AttrShape.m_bBrush)
		m_pShapePropertyEditFillUse->SetValue(DEF_PROPERTY_ENABLE);
	else
		m_pShapePropertyEditFillUse->SetValue(DEF_PROPERTY_DISABLE);

	// 채움 색상
	m_pShapePropertyEditFillColor = new CMFCPropertyGridColorProperty(DEF_PROPERTY_OVERLAY_EDIT_CAPTION_FILLCOLOR, m_AttrShape.m_ColorBrush, NULL, DEF_PROPERTY_OVERLAY_EDIT_CAPTION_FILLCOLOR_REMARK, 0);
	m_pShapePropertyEditFillColor->SetData(SHAPEPROPERTY_EDIT_FILLCOLOR);
	m_pShapePropertyEditFillColor->EnableOtherButton(DEF_PROPERTY_USER_VALUE);
	m_pShapePropertyEditFillColor->AllowEdit(FALSE);
	pShapePropertyGroupEdit->AddSubItem(m_pShapePropertyEditFillColor);

	// 반투명 여부
	m_pShapePropertyEditFillTrance = new CMFCPropertyGridProperty(DEF_PROPERTY_OVERLAY_EDIT_CAPTION_FILLTRANCE, _T(""), DEF_PROPERTY_OVERLAY_EDIT_CAPTION_FILLTRANCE_REMARK);
	m_pShapePropertyEditFillTrance->SetData(SHAPEPROPERTY_EDIT_TRANSPARENT);
	m_pShapePropertyEditFillTrance->AddOption(DEF_PROPERTY_ENABLE);
	m_pShapePropertyEditFillTrance->AddOption(DEF_PROPERTY_DISABLE);
	m_pShapePropertyEditFillTrance->AllowEdit(FALSE);
	pShapePropertyGroupEdit->AddSubItem(m_pShapePropertyEditFillTrance);

	if(m_AttrShape.m_bTransparent)
		m_pShapePropertyEditFillTrance->SetValue(DEF_PROPERTY_ENABLE);
	else
		m_pShapePropertyEditFillTrance->SetValue(DEF_PROPERTY_DISABLE);

	// 패턴 사용 여부
	m_pShapePropertyEditPatternUse = new CMFCPropertyGridProperty(DEF_PROPERTY_OVERLAY_EDIT_CAPTION_PATTERNUSE, _T(""), DEF_PROPERTY_OVERLAY_EDIT_CAPTION_PATTERNUSE_REMARK);
	m_pShapePropertyEditPatternUse->SetData(SHAPEPROPERTY_EDIT_PATTERNUSE);
	m_pShapePropertyEditPatternUse->AddOption(DEF_PROPERTY_ENABLE);
	m_pShapePropertyEditPatternUse->AddOption(DEF_PROPERTY_DISABLE);
	m_pShapePropertyEditPatternUse->AllowEdit(FALSE);
	pShapePropertyGroupEdit->AddSubItem(m_pShapePropertyEditPatternUse);

	if(m_AttrShape.m_bPattern)
		m_pShapePropertyEditPatternUse->SetValue(DEF_PROPERTY_ENABLE);
	else
		m_pShapePropertyEditPatternUse->SetValue(DEF_PROPERTY_DISABLE);

	// 패턴 색상
	m_pShapePropertyEditPatternColor = new CMFCPropertyGridColorProperty(DEF_PROPERTY_OVERLAY_EDIT_CAPTION_PATTERNCOLOR, m_AttrShape.m_ColorPattern, NULL, DEF_PROPERTY_OVERLAY_EDIT_CAPTION_PATTERNCOLOR_REMARK, 0);
	m_pShapePropertyEditPatternColor->SetData(SHAPEPROPERTY_EDIT_PATTERNCOLOR);
	m_pShapePropertyEditPatternColor->EnableOtherButton(DEF_PROPERTY_USER_VALUE);
	m_pShapePropertyEditPatternColor->AllowEdit(FALSE);
	pShapePropertyGroupEdit->AddSubItem(m_pShapePropertyEditPatternColor);
	
	// 패턴 모양
	m_pShapePropertyEditPatternStyle = new CMFCPropertyGridProperty(DEF_PROPERTY_OVERLAY_EDIT_CAPTION_PATTERNSTYLE, _T(""), DEF_PROPERTY_OVERLAY_EDIT_CAPTION_PATTERNSTYLE_REMARK);
	m_pShapePropertyEditPatternStyle->SetData(SHAPEPROPERTY_EDIT_PATTERNKIND);
	m_pShapePropertyEditPatternStyle->AddOption(DEF_PROPERTY_PATTERNSTYLE_0);
	m_pShapePropertyEditPatternStyle->AddOption(DEF_PROPERTY_PATTERNSTYLE_1);
	m_pShapePropertyEditPatternStyle->AddOption(DEF_PROPERTY_PATTERNSTYLE_2);
	m_pShapePropertyEditPatternStyle->AddOption(DEF_PROPERTY_PATTERNSTYLE_3);
	m_pShapePropertyEditPatternStyle->AddOption(DEF_PROPERTY_PATTERNSTYLE_4);
	m_pShapePropertyEditPatternStyle->AddOption(DEF_PROPERTY_PATTERNSTYLE_5);
	m_pShapePropertyEditPatternStyle->AllowEdit(FALSE);
	pShapePropertyGroupEdit->AddSubItem(m_pShapePropertyEditPatternStyle);

	switch(m_AttrShape.m_nPattern)
	{
	case 1:
		m_pShapePropertyEditPatternStyle->SetValue(DEF_PROPERTY_PATTERNSTYLE_1);
		break;
	case 2:
		m_pShapePropertyEditPatternStyle->SetValue(DEF_PROPERTY_PATTERNSTYLE_2);
		break;
	case 3:
		m_pShapePropertyEditPatternStyle->SetValue(DEF_PROPERTY_PATTERNSTYLE_3);
		break;
	case 4:
		m_pShapePropertyEditPatternStyle->SetValue(DEF_PROPERTY_PATTERNSTYLE_4);
		break;
	case 5:
		m_pShapePropertyEditPatternStyle->SetValue(DEF_PROPERTY_PATTERNSTYLE_5);
		break;
	case 0:
	default:
		m_pShapePropertyEditPatternStyle->SetValue(DEF_PROPERTY_PATTERNSTYLE_0);
		break;
	}

	// 회전 값
	m_pShapePropertyEditRotate = new CBoundedNumberSubProp(DEF_PROPERTY_OVERLAY_EDIT_CAPTION_ROTATE, (_variant_t)m_AttrShape.m_nAngle, 0, 360, DEF_PROPERTY_OVERLAY_EDIT_CAPTION_ROTATE_REMARK, _T("ddd"), _T("___"));
	m_pShapePropertyEditRotate->SetData(SHAPEPROPERTY_EDIT_ROTATE);
	m_pShapePropertyEditRotate->EnableSpinControl(TRUE, 0, 360);
	pShapePropertyGroupEdit->AddSubItem(m_pShapePropertyEditRotate);

	m_wndPropList.AddProperty(pShapePropertyGroupEdit);
}


// 선 속성
void COverlayPropertiesWnd::AddShapeSplineProperty(CAttrShapeSpline attrShapeSpline)
{
	m_AttrShapeSpline = attrShapeSpline;

	CMFCPropertyGridProperty* pShapePropertyGroupSpline = new CMFCPropertyGridProperty(DEF_PROPERTY_OVERLAY_LINE_TITLE);
	pShapePropertyGroupSpline->SetData(SHAPEPROPERTY_LINE);

	// 단순 직선 제외
	if(m_nShapeKind != ID_OT_DRAW_LINE)
	{
		// 직선 or 곡선
		CMFCPropertyGridProperty* pShapePropertySplineStraight = new CMFCPropertyGridProperty(DEF_PROPERTY_OVERLAY_LINE_CAPTION_SELECT, _T(""), DEF_PROPERTY_OVERLAY_LINE_CAPTION_SELECT_REMARK);
		pShapePropertySplineStraight->SetData(SHAPEPROPERTY_LINE_STRAIGHT);
		pShapePropertySplineStraight->AddOption(DEF_PROPERTY_POLYLINE);
		pShapePropertySplineStraight->AddOption(DEF_PROPERTY_SPLINE);
		pShapePropertySplineStraight->AllowEdit(FALSE);

		if(m_AttrShapeSpline.m_bLine)
			pShapePropertySplineStraight->SetValue(DEF_PROPERTY_POLYLINE);
		else
			pShapePropertySplineStraight->SetValue(DEF_PROPERTY_SPLINE);

		pShapePropertyGroupSpline->AddSubItem(pShapePropertySplineStraight);

		// 끝점 연결
		CMFCPropertyGridProperty* pShapePropertySplineClosed = new CMFCPropertyGridProperty(DEF_PROPERTY_OVERLAY_LINE_CAPTION_CLOSED,  _T(""), DEF_PROPERTY_OVERLAY_LINE_CAPTION_CLOSED_REMARK);
		pShapePropertySplineClosed->SetData(SHAPEPROPERTY_LINE_CLOSED);
		pShapePropertySplineClosed->AddOption(DEF_PROPERTY_CONNECT);
		pShapePropertySplineClosed->AddOption(DEF_PROPERTY_DISCONNECT);
		pShapePropertySplineClosed->AllowEdit(FALSE);

		if(m_AttrShapeSpline.m_bClosed)
			pShapePropertySplineClosed->SetValue(DEF_PROPERTY_CONNECT);
		else
			pShapePropertySplineClosed->SetValue(DEF_PROPERTY_DISCONNECT);

		pShapePropertyGroupSpline->AddSubItem(pShapePropertySplineClosed);
	}

	// 시작 화살표
	m_pLineShapeStartArrow = new CMFCPropertyGridProperty(DEF_PROPERTY_OVERLAY_LINE_CAPTION_STARROW, _T(""), DEF_PROPERTY_OVERLAY_LINE_CAPTION_STARROW_REMARK);
	m_pLineShapeStartArrow->SetData(SHAPEPROPERTY_LINE_STARTARROW);
	m_pLineShapeStartArrow->AddOption(DEF_PROPERTY_DISABLE);
	m_pLineShapeStartArrow->AddOption(DEF_PROPERTY_CLOSE);
	m_pLineShapeStartArrow->AddOption(DEF_PROPERTY_OPEN);
	m_pLineShapeStartArrow->AllowEdit(FALSE);

	if(m_AttrShapeSpline.m_bFrontHead == FALSE)
		m_pLineShapeStartArrow->SetValue(DEF_PROPERTY_DISABLE);
	else
	{
		if(m_AttrShapeSpline.m_bFrontArrowClose)
			m_pLineShapeStartArrow->SetValue(DEF_PROPERTY_CLOSE);
		else
			m_pLineShapeStartArrow->SetValue(DEF_PROPERTY_OPEN);
	}

	pShapePropertyGroupSpline->AddSubItem(m_pLineShapeStartArrow);

	// 끝 화살표
	m_pLineShapeEndArrow = new CMFCPropertyGridProperty(DEF_PROPERTY_OVERLAY_LINE_CAPTION_EDARROW, _T(""), DEF_PROPERTY_OVERLAY_LINE_CAPTION_EDARROW_REMARK);
	m_pLineShapeEndArrow->SetData(SHAPEPROPERTY_LINE_ENDARROW);
	m_pLineShapeEndArrow->AddOption(DEF_PROPERTY_DISABLE);
	m_pLineShapeEndArrow->AddOption(DEF_PROPERTY_CLOSE);
	m_pLineShapeEndArrow->AddOption(DEF_PROPERTY_OPEN);
	m_pLineShapeEndArrow->AllowEdit(FALSE);

	if(m_AttrShapeSpline.m_bBackHead == FALSE)
		m_pLineShapeEndArrow->SetValue(DEF_PROPERTY_DISABLE);
	else
	{
		if(m_AttrShapeSpline.m_bBackArrowClose)
			m_pLineShapeEndArrow->SetValue(DEF_PROPERTY_CLOSE);
		else
			m_pLineShapeEndArrow->SetValue(DEF_PROPERTY_OPEN);
	}

	pShapePropertyGroupSpline->AddSubItem(m_pLineShapeEndArrow);


	m_wndPropList.AddProperty(pShapePropertyGroupSpline);

	UpdateShapeSplineProperties();
}
*/
// 다각형 속성
void COverlayPropertiesWnd::AddShapeNgonProperty(CAttrShapeNgon attrShapeNgon)
{
	m_AttrShapeNgon = attrShapeNgon;

	CMFCPropertyGridProperty* pShapePropertyGroupNgon = new CMFCPropertyGridProperty(DEF_PROPERTY_OVERLAY_NGON_TITLE);
	pShapePropertyGroupNgon->SetData(SHAPEPROPERTY_NGON);

	// 다각형 꼭지점의 수
	CBoundedNumberSubProp* pShapePropertyNgonCount = new CBoundedNumberSubProp(DEF_PROPERTY_OVERLAY_NGON_CAPTION_COUNT, (_variant_t)m_AttrShapeNgon.m_nNgonVar, 3, 30, DEF_PROPERTY_OVERLAY_NGON_CAPTION_COUNT_REMARK, _T("dd"), _T("__"));
	pShapePropertyNgonCount->SetData(SHAPEPROPERTY_NGON_COUNT);
	pShapePropertyNgonCount->EnableSpinControl(TRUE, 0, 30);
	pShapePropertyGroupNgon->AddSubItem(pShapePropertyNgonCount);

	m_wndPropList.AddProperty(pShapePropertyGroupNgon);
}

// 사각형 속성
void COverlayPropertiesWnd::AddShapeRectProperty(CAttrShapeRect attrShapeRect)
{
	m_AttrShapeRect = attrShapeRect;

	CMFCPropertyGridProperty* pShapePropertyGroupRect = new CMFCPropertyGridProperty(DEF_PROPERTY_OVERLAY_RECT_TITLE);
	pShapePropertyGroupRect->SetData(SHAPEPROPERTY_RECT);

	// 사각형 곡률
	CBoundedNumberSubProp* pShapePropertyRectRound = new CBoundedNumberSubProp(DEF_PROPERTY_OVERLAY_RECT_CAPTION_ROUND, (_variant_t)m_AttrShapeRect.m_nRound, 0, 360, DEF_PROPERTY_OVERLAY_RECT_CAPTION_ROUND_REMARK, _T("ddd"), _T("___"));
	pShapePropertyRectRound->SetData(SHAPEPROPERTY_RECT_ROUND);
	pShapePropertyRectRound->EnableSpinControl(TRUE, 0, 360);
	pShapePropertyGroupRect->AddSubItem(pShapePropertyRectRound);

	m_wndPropList.AddProperty(pShapePropertyGroupRect);
}
/*
// 화살표 속성
void COverlayPropertiesWnd::AddShapeArrowProperty(CAttrShapeArrow attrShapeArrow)
{
	m_AttrShapeArrow = attrShapeArrow;

	// 꼬리 모양
	CMFCPropertyGridProperty* pShapePropertyGroupArrow = new CMFCPropertyGridProperty(DEF_PROPERTY_OVERLAY_ARROW_TITLE);
	pShapePropertyGroupArrow->SetData(SHAPEPROPERTY_ARROW);

	CMFCPropertyGridProperty* pShapePropertyArrowTail = new CMFCPropertyGridProperty(DEF_PROPERTY_OVERLAY_ARROW_CAPTION_TAIL, _T(""), DEF_PROPERTY_OVERLAY_ARROW_CAPTION_TAIL_REMARK);
	pShapePropertyArrowTail->SetData(SHAPEPROPERTY_ARROW_TAIL);
	pShapePropertyArrowTail->AddOption(DEF_PROPERTY_OPEN);
	pShapePropertyArrowTail->AddOption(DEF_PROPERTY_CLOSE);
	pShapePropertyArrowTail->AddOption(DEF_PROPERTY_RIBBON);
	pShapePropertyArrowTail->AllowEdit(FALSE);

	switch(m_AttrShapeArrow.m_nTail)
	{
	case 1:
		pShapePropertyArrowTail->SetValue(DEF_PROPERTY_CLOSE);
		break;
	case 2:
		pShapePropertyArrowTail->SetValue(DEF_PROPERTY_RIBBON);
		break;
	case 0:
	default:
		pShapePropertyArrowTail->SetValue(DEF_PROPERTY_OPEN);
		break;
	}

	pShapePropertyGroupArrow->AddSubItem(pShapePropertyArrowTail);

	// 너비값
	CBoundedNumberSubProp* pShapePropertyGroupWidth = new CBoundedNumberSubProp(DEF_PROPERTY_OVERLAY_ARROW_CAPTION_WIDTH, (_variant_t)m_AttrShapeArrow.m_nWidth, 0, 3600, DEF_PROPERTY_OVERLAY_ARROW_CAPTION_WIDTH_REMARK, _T("dddd"), _T("____"));
	pShapePropertyGroupWidth->SetData(SHAPEPROPERTY_ARROW_WIDTH);
	pShapePropertyGroupWidth->EnableSpinControl(TRUE, 0, 3600);

	pShapePropertyGroupArrow->AddSubItem(pShapePropertyGroupWidth);

	m_wndPropList.AddProperty(pShapePropertyGroupArrow);
}

// 텍스트 속성
void COverlayPropertiesWnd::AddShapeTextProperty(CAttrShapePointText attrShapePointText)
{
	m_AttrShapePointText = attrShapePointText;

	CMFCPropertyGridProperty* pShapePropertyGroupText = new CMFCPropertyGridProperty(DEF_PROPERTY_OVERLAY_TEXT_TITLE);
	pShapePropertyGroupText->SetData(SHAPEPROPERTY_TEXT);

	// 글꼴
	m_pShapePropertyTextName = new CMFCPropertyGridProperty(DEF_PROPERTY_OVERLAY_TEXT_CAPTION_NAME, _T(""), DEF_PROPERTY_OVERLAY_TEXT_CAPTION_NAME_REMARK);
	m_pShapePropertyTextName->SetData(SHAPEPROPERTY_TEXT_KIND);

	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();

	if(pFrame != NULL && pFrame->GetSafeHwnd() != NULL)
	{
		for(int i=0; i<(int)pFrame->m_vSystemFontList.size(); i++)
			m_pShapePropertyTextName->AddOption(pFrame->m_vSystemFontList.at(i));
	}
	else
	{
		// 대표 글꼴 몇개 설정...
		m_pShapePropertyTextName->AddOption(_T("굴림"));
		m_pShapePropertyTextName->AddOption(_T("굴림체"));
		m_pShapePropertyTextName->AddOption(_T("궁서"));
		m_pShapePropertyTextName->AddOption(_T("궁서체"));
		m_pShapePropertyTextName->AddOption(_T("돋움"));
		m_pShapePropertyTextName->AddOption(_T("돋움체"));
		m_pShapePropertyTextName->AddOption(_T("바탕"));
		m_pShapePropertyTextName->AddOption(_T("바탕체"));
	}

	m_pShapePropertyTextName->SetValue(m_AttrShapePointText.m_logfontName);
	m_pShapePropertyTextName->AllowEdit(FALSE);
	pShapePropertyGroupText->AddSubItem(m_pShapePropertyTextName);

	// 텍스트 크기
	m_pShapePropertyTextSize = new CBoundedNumberSubProp(DEF_PROPERTY_OVERLAY_TEXT_CAPTION_SIZE, (_variant_t)abs(m_AttrShapePointText.m_logfontSize), 1, 50, DEF_PROPERTY_OVERLAY_TEXT_CAPTION_SIZE_REMARK, _T("dd"), _T("__"));
	m_pShapePropertyTextSize->SetData(SHAPEPROPERTY_TEXT_SIZE);
	m_pShapePropertyTextSize->EnableSpinControl(TRUE, 1, 50);
	pShapePropertyGroupText->AddSubItem(m_pShapePropertyTextSize);

	// 텍스트 색상
	m_pShapePropertyTextColor = new CMFCPropertyGridColorProperty(DEF_PROPERTY_OVERLAY_TEXT_CAPTION_COLOR, m_AttrShapePointText.m_dwFontColor, NULL, DEF_PROPERTY_OVERLAY_TEXT_CAPTION_COLOR_REMARK, 0);
	m_pShapePropertyTextColor->SetData(SHAPEPROPERTY_TEXT_COLOR);
	m_pShapePropertyTextColor->EnableOtherButton(DEF_PROPERTY_USER_VALUE);
	m_pShapePropertyTextColor->AllowEdit(FALSE);
	pShapePropertyGroupText->AddSubItem(m_pShapePropertyTextColor);

	// 굵게 사용 여부
	m_pShapePropertyTextBold = new CMFCPropertyGridProperty(DEF_PROPERTY_OVERLAY_TEXT_CAPTION_BOLD, _T(""), DEF_PROPERTY_OVERLAY_TEXT_CAPTION_BOLD_REMARK);
	m_pShapePropertyTextBold->SetData(SHAPEPROPERTY_TEXT_WEIGHT);
	m_pShapePropertyTextBold->AddOption(DEF_PROPERTY_ENABLE);
	m_pShapePropertyTextBold->AddOption(DEF_PROPERTY_DISABLE);
	m_pShapePropertyTextBold->AllowEdit(FALSE);

	if(m_AttrShapePointText.m_lWeight == FW_BOLD)
		m_pShapePropertyTextBold->SetValue(DEF_PROPERTY_ENABLE);
	else
		m_pShapePropertyTextBold->SetValue(DEF_PROPERTY_DISABLE);

	pShapePropertyGroupText->AddSubItem(m_pShapePropertyTextBold);

	// 기울이기
	m_pShapePropertyTextItalic = new CMFCPropertyGridProperty(DEF_PROPERTY_OVERLAY_TEXT_CAPTION_ITALIC, _T(""), DEF_PROPERTY_OVERLAY_TEXT_CAPTION_ITALIC_REMARK);
	m_pShapePropertyTextItalic->SetData(SHAPEPROPERTY_TEXT_ITALIC);
	m_pShapePropertyTextItalic->AddOption(DEF_PROPERTY_ENABLE);
	m_pShapePropertyTextItalic->AddOption(DEF_PROPERTY_DISABLE);
	m_pShapePropertyTextItalic->AllowEdit(FALSE);

	if(m_AttrShapePointText.m_bItalic)
		m_pShapePropertyTextItalic->SetValue(DEF_PROPERTY_ENABLE);
	else
		m_pShapePropertyTextItalic->SetValue(DEF_PROPERTY_DISABLE);

	pShapePropertyGroupText->AddSubItem(m_pShapePropertyTextItalic);

	// 밑줄
	m_pShapePropertyTextUnderline = new CMFCPropertyGridProperty(DEF_PROPERTY_OVERLAY_TEXT_CAPTION_UNDERLINE, _T(""), DEF_PROPERTY_OVERLAY_TEXT_CAPTION_UNDERLINE_REMARK);
	m_pShapePropertyTextUnderline->SetData(SHAPEPROPERTY_TEXT_UNDERLINE);
	m_pShapePropertyTextUnderline->AddOption(DEF_PROPERTY_ENABLE);
	m_pShapePropertyTextUnderline->AddOption(DEF_PROPERTY_DISABLE);
	m_pShapePropertyTextUnderline->AllowEdit(FALSE);

	if(m_AttrShapePointText.m_bUnderline)
		m_pShapePropertyTextUnderline->SetValue(DEF_PROPERTY_ENABLE);
	else
		m_pShapePropertyTextUnderline->SetValue(DEF_PROPERTY_DISABLE);

	pShapePropertyGroupText->AddSubItem(m_pShapePropertyTextUnderline);

	// 취소선
	m_pShapePropertyTextStrikeOut = new CMFCPropertyGridProperty(DEF_PROPERTY_OVERLAY_TEXT_CAPTION_STRIKEOUT, _T(""), DEF_PROPERTY_OVERLAY_TEXT_CAPTION_STRIKEOUT_REMARK);
	m_pShapePropertyTextStrikeOut->SetData(SHAPEPROPERTY_TEXT_STRIKEOUT);
	m_pShapePropertyTextStrikeOut->AddOption(DEF_PROPERTY_ENABLE);
	m_pShapePropertyTextStrikeOut->AddOption(DEF_PROPERTY_DISABLE);
	m_pShapePropertyTextStrikeOut->AllowEdit(FALSE);

	if(m_AttrShapePointText.m_bStrikeOut)
		m_pShapePropertyTextStrikeOut->SetValue(DEF_PROPERTY_ENABLE);
	else
		m_pShapePropertyTextStrikeOut->SetValue(DEF_PROPERTY_DISABLE);

	pShapePropertyGroupText->AddSubItem(m_pShapePropertyTextStrikeOut);

	// 정렬
	m_pShapePropertyTextAlign = new CMFCPropertyGridProperty(DEF_PROPERTY_OVERLAY_TEXT_CAPTION_ALIGN, _T(""), DEF_PROPERTY_OVERLAY_TEXT_CAPTION_ALIGN_REMARK);
	m_pShapePropertyTextAlign->SetData(SHAPEPROPERTY_TEXT_ALIGN);
	m_pShapePropertyTextAlign->AddOption(DEF_PROPERTY_LEFT);
	m_pShapePropertyTextAlign->AddOption(DEF_PROPERTY_CENTER);
	m_pShapePropertyTextAlign->AddOption(DEF_PROPERTY_RIGHT);
	m_pShapePropertyTextAlign->AllowEdit(FALSE);

	switch(m_AttrShapePointText.m_nAlign)
	{
	case 1:
		m_pShapePropertyTextAlign->SetValue(DEF_PROPERTY_CENTER);
		break;
	case 2:
		m_pShapePropertyTextAlign->SetValue(DEF_PROPERTY_RIGHT);
		break;
	case 0:
	default:
		m_pShapePropertyTextAlign->SetValue(DEF_PROPERTY_LEFT);
		break;
	}

	pShapePropertyGroupText->AddSubItem(m_pShapePropertyTextAlign);

	// 텍스트 내용
	CMultiLineEditProp* pShapePropertyTextString = new CMultiLineEditProp(DEF_PROPERTY_OVERLAY_TEXT_CAPTION_STRING, m_AttrShapePointText.m_lpszText, DEF_PROPERTY_OVERLAY_TEXT_CAPTION_STRING_REMARK);
	pShapePropertyTextString->SetData(SHAPEPROPERTY_TEXT_STRING);
	pShapePropertyGroupText->AddSubItem(pShapePropertyTextString);

	m_wndPropList.AddProperty(pShapePropertyGroupText);
}

#ifdef _MILITARY
// 군대부호 점형 속성
void COverlayPropertiesWnd::AddShapeMilPointProperty(CAttrShapePoint attrShapeMilitaryPt, int nShapeKind)
{
	m_AttrShapeMilitaryPt = attrShapeMilitaryPt;

	CMFCPropertyGridProperty* pShapePropertyGroupMilpt = new CMFCPropertyGridProperty("부호 속성");
	pShapePropertyGroupMilpt->SetData(MILITARYPROP_POINT);

	// 부호 크기
	CBoundedNumberSubProp* pShapeMilPtSize = new CBoundedNumberSubProp(_T("부호크기"), (_variant_t)abs(m_AttrShapeMilitaryPt.m_nMilSymSize), 50, 300, _T("50~300 범위의 부호의 크기를 설정합니다."), _T("ddd"), _T("__"));
	pShapeMilPtSize->SetData(MILITARY_SIZE);
	pShapeMilPtSize->EnableSpinControl(TRUE, 50, 300);
	pShapePropertyGroupMilpt->AddSubItem(pShapeMilPtSize);

	// 부호 색상
	CMFCPropertyGridColorProperty* pShapeMilPtColor = new CMFCPropertyGridColorProperty(_T("부호색상"), m_AttrShapeMilitaryPt.m_clrMilSym, NULL, _T("부호의 색상을 설정합니다."), 0);
	pShapeMilPtColor->SetData(MILITARY_COLOR);
	pShapeMilPtColor->EnableOtherButton(DEF_PROPERTY_USER_VALUE);
	pShapeMilPtColor->AllowEdit(FALSE);
	pShapePropertyGroupMilpt->AddSubItem(pShapeMilPtColor);

	m_wndPropList.AddProperty(pShapePropertyGroupMilpt);

	// 추가정보
		if(nShapeKind == ID_OT_DRAW_OVEREXTPOINT && m_strShapePointModifier.IsEmpty() == FALSE)
		{
			CMFCPropertyGridProperty* pShapePropertyGroupMilptAddInfo = new CMFCPropertyGridProperty("추가 속성");
			pShapePropertyGroupMilptAddInfo->SetData(MILITARYPROP_POINT_ADDINFO);

			// 부호 색상
			CMFCPropertyGridColorProperty* pShapeMilPtAddInfoColor = new CMFCPropertyGridColorProperty(_T("정보색상"), m_AttrShapeMilitaryPt.m_clrMilSymTextColor, NULL, _T("정보의 색상을 설정합니다."), 0);
			pShapeMilPtAddInfoColor->SetData(MILITARY_ADDINFO_COLOR);
			pShapeMilPtAddInfoColor->EnableOtherButton(DEF_PROPERTY_USER_VALUE);
			pShapeMilPtAddInfoColor->AllowEdit(FALSE);
			pShapePropertyGroupMilptAddInfo->AddSubItem(pShapeMilPtAddInfoColor);

			CString strSetModifierInfo = m_strShapePointModifier;
			while(!strSetModifierInfo.IsEmpty())
			{
				int nIndex = strSetModifierInfo.Find("#");

				CString strModifierKind = "";

				if(nIndex == -1)
					strModifierKind = strSetModifierInfo;
				else
					strModifierKind = strSetModifierInfo.Left(nIndex);

				strModifierKind.MakeUpper();

				if(strModifierKind == "T")
					AddShapeMilExtInfoT(pShapePropertyGroupMilptAddInfo);	// 고유명칭(T)
				else if(strModifierKind == "T1")
					AddShapeMilExtInfoT1(pShapePropertyGroupMilptAddInfo);	// 고유명칭(T1)
				else if(strModifierKind == "H")
					AddShapeMilExtInfoH(pShapePropertyGroupMilptAddInfo);	// 추가정보(H)
				else if(strModifierKind == "H1")
					AddShapeMilExtInfoH1(pShapePropertyGroupMilptAddInfo);	// 추가정보(H1)
				else if(strModifierKind == "W")
					AddShapeMilExtInfoW(pShapePropertyGroupMilptAddInfo);	// 활동시각(W)
				else if(strModifierKind == "W1")
					AddShapeMilExtInfoW1(pShapePropertyGroupMilptAddInfo);	// 활동시각(W1)
				else if(strModifierKind == "C")
					AddShapeMilExtInfoC(pShapePropertyGroupMilptAddInfo);	// 핵용량(C)
				else if(strModifierKind == "V")
					AddShapeMilExtInfoV(pShapePropertyGroupMilptAddInfo);	// 장비종류(V)
				else if(strModifierKind == "Y")
					AddShapeMilExtInfoY(pShapePropertyGroupMilptAddInfo);	// 위치(Y)
				else if(strModifierKind == "N")
					AddShapeMilExtInfoN(pShapePropertyGroupMilptAddInfo);	// 적군표시(N) True, False			
				else if(strModifierKind == "B")
					AddShapeMilExtInfoB(pShapePropertyGroupMilptAddInfo);	// 부대단위(B)
				else if(strModifierKind == "R")
					AddShapeMilExtInfoR(pShapePropertyGroupMilptAddInfo);	// 회전각(R)

				// next 
				strSetModifierInfo = strSetModifierInfo.Right(strSetModifierInfo.GetLength() - 2);
			}

			m_wndPropList.AddProperty(pShapePropertyGroupMilptAddInfo);
		}
		else if(nShapeKind == ID_OT_DRAW_POINT)
		{
			CMFCPropertyGridProperty* pShapePropertyGroupMilptAddInfo = new CMFCPropertyGridProperty("추가 속성");
			pShapePropertyGroupMilptAddInfo->SetData(MILITARYPROP_POINT_ADDINFO);

			// 부호 색상
			CMFCPropertyGridColorProperty* pShapeMilPtAddInfoColor = new CMFCPropertyGridColorProperty(_T("정보색상"), m_AttrShapeMilitaryPt.m_clrMilSymTextColor, NULL, _T("정보의 색상을 설정합니다."), 0);
			pShapeMilPtAddInfoColor->SetData(MILITARY_ADDINFO_COLOR);
			pShapeMilPtAddInfoColor->EnableOtherButton(DEF_PROPERTY_USER_VALUE);
			pShapeMilPtAddInfoColor->AllowEdit(FALSE);
			pShapePropertyGroupMilptAddInfo->AddSubItem(pShapeMilPtAddInfoColor);

			AddShapeMilPtAddInfoK(pShapePropertyGroupMilptAddInfo);	// T 전투력
			AddShapeMilPtAddInfoT(pShapePropertyGroupMilptAddInfo);	// T 고유명칭
			AddShapeMilPtAddInfoM(pShapePropertyGroupMilptAddInfo);	// T 상급제대 

			m_wndPropList.AddProperty(pShapePropertyGroupMilptAddInfo);
		}

		
}

// 군대부호 선,면형 속성
void COverlayPropertiesWnd::AddShapeMilExtProperty(CAttrShapeExt attrShapeMilitaryExt)
{
	m_AttrShapeMilitaryExt = attrShapeMilitaryExt;

}


// 비행회랑 속성
void COverlayPropertiesWnd::AddShapeMilAirProperty(OVEREXTAIRINFO attrShapeExtAir)
{
	m_pAttrShapeExtAir = attrShapeExtAir;
}
#endif
*/
void COverlayPropertiesWnd::AddShapeMilPtAddInfoC(CMFCPropertyGridProperty* pGroupProp)
{
	//MILITARY_ADDINFO_C,		// T 장비수량, 장비에 대한 심볼인 경우 장비의 수량을 숫자로 표현한다. 9의 3승?							[부x,장o,시x,신x,기x]
}
void COverlayPropertiesWnd::AddShapeMilPtAddInfoF(CMFCPropertyGridProperty* pGroupProp)
{
	//MILITARY_ADDINFO_F,		// T 부대증감, 부대의 증감을 표현한다. (+)증가, (-)감소, (+-)증감	20크기제한								[부o,장x,시x,신x,기o]
}
void COverlayPropertiesWnd::AddShapeMilPtAddInfoG(CMFCPropertyGridProperty* pGroupProp)
{
	//MILITARY_ADDINFO_G,		// T 군 및 국가구분, 부호가 나타내는 객체의 소속 군 및 국가를 표기한다. 예) 육군-미국	20 크기제한	[부o,장o,시o,신o,기o]
}
void COverlayPropertiesWnd::AddShapeMilPtAddInfoH(CMFCPropertyGridProperty* pGroupProp)
{
	//MILITARY_ADDINFO_H,		// T 추가사항, 활동상황 등과 같이 객체의 상태 또는 기타 부가적인 설명을 기재한다.						[부o,장o,시o,신o,기o]
}
void COverlayPropertiesWnd::AddShapeMilPtAddInfoJ1(CMFCPropertyGridProperty* pGroupProp)
{
	//MILITARY_ADDINFO_J1,	// T 신뢰도, 부호가 표현하는 정보에 대한 신뢰도 등급을 선택한다.										[부o,장o,시o,신o,기o]
	// A 완전 신뢰, B 신뢰도 높음, C 보통, D 신뢰도 낮음, E 신뢰할 수 없음, F 판단 불가
}
void COverlayPropertiesWnd::AddShapeMilPtAddInfoJ2(CMFCPropertyGridProperty* pGroupProp)
{
	//MILITARY_ADDINFO_J2,	// T 정확성, 부호가 표현하는 정보에 대한 정확성 등급을 선택한다.										[부o,장o,시o,신o,기o]
	// 1 다른 출처를 통해 확인됨, 2 정확성 높음, 3 정확성 있음, 4 정확성 낮음, 5 정확성 희박, 6 확인 불가
}
void COverlayPropertiesWnd::AddShapeMilPtAddInfoK(CMFCPropertyGridProperty* pGroupProp)
{
#ifdef _MILITARY
	// [부o,장x,시o,신x,기o]
	// SU, SI, O 경우만 가능
	CBoundedNumberSubProp* pShapePropertyMilPtAddInfoK = new CBoundedNumberSubProp("전투력", (_variant_t)atoi(m_AttrShapeMilitaryPt.strK), 0, 100, "부대/화기의 전투력 또는 시설의 능력을 백분율로 입력합니다.", _T("ddd"), _T("__"));
	pShapePropertyMilPtAddInfoK->SetData(MILITARY_ADDINFO_K);
	pShapePropertyMilPtAddInfoK->EnableSpinControl(TRUE, 1, 50);
	pGroupProp->AddSubItem(pShapePropertyMilPtAddInfoK);
#endif
}
void COverlayPropertiesWnd::AddShapeMilPtAddInfoL(CMFCPropertyGridProperty* pGroupProp)
{
	//MILITARY_ADDINFO_L,		// T 전자장비, 전자장비로 탐지 가능한 적 장비에 대하여 !로 표현한다.	탐지 가능						[부x,장o,시x,신o,기x]
}

// 상급제대
void COverlayPropertiesWnd::AddShapeMilPtAddInfoM(CMFCPropertyGridProperty* pGroupProp)
{
#ifdef _MILITARY
	// [부o,장x,시x,신x,기o]
	// SU, O 경우만 가능
	CSingleLineEditProp* pShapePropertyMilPtAddInfoM = new CSingleLineEditProp("상급제대", (_variant_t)m_AttrShapeMilitaryPt.strM, "상급제대 명을 숫자 또는 문자로  입력합니다.");
	pShapePropertyMilPtAddInfoM->SetData(MILITARY_ADDINFO_M);
	pGroupProp->AddSubItem(pShapePropertyMilPtAddInfoM);
#endif
}

void COverlayPropertiesWnd::AddShapeMilPtAddInfoN(CMFCPropertyGridProperty* pGroupProp)
{
	//MILITARY_ADDINFO_N,		// T 적군표시, 적 장비임을 표현하지 위하여 적 또는 ENY를 표시한다.		표시, 미표시					[부x,장o,시x,신x,기x]
	//   장비 부호에 외형 사용을 하지 않은 경우를 위해서 인데 우리는 외형 Off를 사용하지 않으니 사실 필요 없음
}
void COverlayPropertiesWnd::AddShapeMilPtAddInfoP(CMFCPropertyGridProperty* pGroupProp)
{
	//MILITARY_ADDINFO_P,		// T 피아식별 모드 및 코드, 피아식별(IFF/SIF) 모드와 식별된 코드를 표기한다.							[부o,장o,시o,신x,기o]
}
void COverlayPropertiesWnd::AddShapeMilPtAddInfoQ(CMFCPropertyGridProperty* pGroupProp)
{
	//MILITARY_ADDINFO_Q,		// T 이동방향, 부대, 장비 및 시설에 대한 이동 또는 예상 이동 방향을 표현한다.							[부o,장o,시o,신x,기o]
}
void COverlayPropertiesWnd::AddShapeMilPtAddInfoR2(CMFCPropertyGridProperty* pGroupProp)
{
	//MILITARY_ADDINFO_R2,	// T 전자장비 이동성, 전자장비의 기동성을 표시한다.	M 기동가능, S 고정, U 불확실						[부x,장x,시x,신o,기x]		
}

// T 고유명칭
void COverlayPropertiesWnd::AddShapeMilPtAddInfoT(CMFCPropertyGridProperty* pGroupProp)
{
#ifdef _MILITARY
	// [부o,장o,시o,신o,기o]
	CSingleLineEditProp* pShapePropertyMilPtAddInfoT = new CSingleLineEditProp("고유명칭", (_variant_t)m_AttrShapeMilitaryPt.strT, "고유식별자로 부대인 경우 부대명, 시설인 경우 시설명, 장비인 경우 장비의 고유번호를 입력합니다.");
	pShapePropertyMilPtAddInfoT->SetData(MILITARY_ADDINFO_T);
	pGroupProp->AddSubItem(pShapePropertyMilPtAddInfoT);
#endif
}
void COverlayPropertiesWnd::AddShapeMilPtAddInfoV(CMFCPropertyGridProperty* pGroupProp)
{
	//MILITARY_ADDINFO_V T 장비종류, 장비명 또는 장비종류명을 기입한다. 예) 105 곡사포 -> 105 표기			24자리				[부x,장o,시x,신o,기x]
}
void COverlayPropertiesWnd::AddShapeMilPtAddInfoW(CMFCPropertyGridProperty* pGroupProp)
{
	//MILITARY_ADDINFO_W T 활동시각, 날짜와 시각을 나타낸다. 형식) YYMMDDHHmmSS									20자리			[부o,장o,시o,신o,기o]
}
void COverlayPropertiesWnd::AddShapeMilPtAddInfoX(CMFCPropertyGridProperty* pGroupProp)
{
	//MILITARY_ADDINFO_X T 고도/심도 고도 단위는 미터(m)기본으로 하며 다른 단위의 경우 단위 표기를 한다.	6자리			[부o,장o,시o,신x,기o]
}
void COverlayPropertiesWnd::AddShapeMilPtAddInfoY(CMFCPropertyGridProperty* pGroupProp)
{
	//MILITARY_ADDINFO_Y T 위치, 경위도를 기본으로 하며 다른 좌표일 경우 첫 문자로 좌표 형식 식별자를 표기한다.	19자리	[부o,장o,시o,신x,기o]
	//	 경위도G, UTM U, MGRS M, NE N, GEOREF F
}
void COverlayPropertiesWnd::AddShapeMilPtAddInfoZ(CMFCPropertyGridProperty* pGroupProp)
{
	//MILITARY_ADDINFO_Z T 속도,																					8자리				[부o,장o,시o,신x,기o]
	//   이동속도 표시가 필요한 부대, 장비 및 시설의 속도를 표기한다. 기본단위는 Km/h 이며 다를 경우 단위를 표기한다.
}
void COverlayPropertiesWnd::AddShapeMilPtAddInfoAA(CMFCPropertyGridProperty* pGroupProp)
{
	//MILITARY_ADDINFO_AA T 지휘소명, 지휘소에 해당하는 부대는 지휘소명을 표기한다.							9자리				[부o,장x,시x,신x,기o]
}
void COverlayPropertiesWnd::AddShapeMilPtAddInfoAD(CMFCPropertyGridProperty* pGroupProp)
{
	//MILITARY_ADDINFO_AD T 기반형태, 전자전 장비의 플랫폼 형태를 ELNOT 또는 CENOT로 표기한다. (기타영역에 표시)		6자리	[부x,장x,시x,신o,기x]
}
void COverlayPropertiesWnd::AddShapeMilPtAddInfoAE(CMFCPropertyGridProperty* pGroupProp)
{
	//MILITARY_ADDINFO_AE T 장비분해시간, 전자전 장비 분해 시간을 분 단위로 표기한다. (기타영역에 표시)				3자리	[부x,장x,시x,신o,기x]
}
void COverlayPropertiesWnd::AddShapeMilPtAddInfoAF(CMFCPropertyGridProperty* pGroupProp)
{
	//MILITARY_ADDINFO_AF T 공통명칭, 장비에 대해 일반적으로 사용되는 이름을 표기한다.									12자리	[부x,장x,시x,신o,기x]
}
void COverlayPropertiesWnd::AddShapeMilPtAddInfoAG(CMFCPropertyGridProperty* pGroupProp)
{
	//MILITARY_ADDINFO_AG T 보조장비식별부호, 예인소나와 같은 보조장비가 있는 장비의 경우 이를 표시한다.						[부x,장o,시x,신x,기x]
}
void COverlayPropertiesWnd::AddShapeMilExtInfoT(CMFCPropertyGridProperty* pGroupProp)
{
	//MILITARY_EXTINFO_T 고유명칭(T)
}
void COverlayPropertiesWnd::AddShapeMilExtInfoT1(CMFCPropertyGridProperty* pGroupProp)
{
	//MILITARY_EXTINFO_T1 고유명칭(T1)
}
void COverlayPropertiesWnd::AddShapeMilExtInfoH(CMFCPropertyGridProperty* pGroupProp)
{
	//MILITARY_EXTINFO_H 추가정보(H)
}
void COverlayPropertiesWnd::AddShapeMilExtInfoH1(CMFCPropertyGridProperty* pGroupProp)
{
	//MILITARY_EXTINFO_H1 추가정보(H1)
}
//void COverlayPropertiesWnd::AddShapeMilExtInfoH2(CMFCPropertyGridProperty* pGroupProp)
//{
	//MILITARY_EXTINFO_H2 추가정보(H2)
//}
void COverlayPropertiesWnd::AddShapeMilExtInfoW(CMFCPropertyGridProperty* pGroupProp)
{
	//MILITARY_EXTINFO_W 활동시각(W)
}
void COverlayPropertiesWnd::AddShapeMilExtInfoW1(CMFCPropertyGridProperty* pGroupProp)
{
	//MILITARY_EXTINFO_W1 활동시각(W1)
}
void COverlayPropertiesWnd::AddShapeMilExtInfoC(CMFCPropertyGridProperty* pGroupProp)
{
	//MILITARY_EXTINFO_C 핵용량(C)
}
void COverlayPropertiesWnd::AddShapeMilExtInfoV(CMFCPropertyGridProperty* pGroupProp)
{
	//MILITARY_EXTINFO_V 장비종류(V)
}
void COverlayPropertiesWnd::AddShapeMilExtInfoN(CMFCPropertyGridProperty* pGroupProp)
{
	//MILITARY_EXTINFO_N 적군표시(N) True, False
}
void COverlayPropertiesWnd::AddShapeMilExtInfoB(CMFCPropertyGridProperty* pGroupProp)
{
	//MILITARY_EXTINFO_B 부대단위(B)
}
void COverlayPropertiesWnd::AddShapeMilExtInfoR(CMFCPropertyGridProperty* pGroupProp)
{
	//MILITARY_EXTINFO_R 회전각(R)
}
void COverlayPropertiesWnd::AddShapeMilExtInfoY(CMFCPropertyGridProperty* pGroupProp)
{
	//MILITARY_EXTINFO_Y 위치(Y)
}

// 선 색상
void COverlayPropertiesWnd::SetShapePropertyLineColor(COLORREF clrLine)
{
	/*if(m_AttrShape.m_ColorPen != clrLine)
	{
		m_AttrShape.m_ColorPen = clrLine;

		if(m_pShapePropertyEditLineColor != NULL)
			m_pShapePropertyEditLineColor->SetColor(m_AttrShape.m_ColorPen);
	}*/
}

// 선 굵기
void COverlayPropertiesWnd::SetShapePropertyLineWidth(int nLineWidth)
{
	/*if(m_AttrShape.m_nPenSize != nLineWidth)
	{
		m_AttrShape.m_nPenSize = nLineWidth;

		if(m_pShapePropertyEditLineWidth != NULL)
			m_pShapePropertyEditLineWidth->SetValue((_variant_t)m_AttrShape.m_nPenSize);
	}*/
}

// 도형 선 모양
void COverlayPropertiesWnd::SetShapePropertyLineStyle(int nLineStyle)
{
	/*if(m_AttrShape.m_nStyle != nLineStyle)
	{
		m_AttrShape.m_nStyle = nLineStyle;

		if(m_pShapePropertyEditLineStyle != NULL)
		{		
			switch(m_AttrShape.m_nStyle)
			{
			case 1:
				m_pShapePropertyEditLineStyle->SetValue(DEF_PROPERTY_LINESTYLE_DASH);
				break;
			case 2:
				m_pShapePropertyEditLineStyle->SetValue(DEF_PROPERTY_LINESTYLE_DOT);
				break;
			case 3:
				m_pShapePropertyEditLineStyle->SetValue(DEF_PROPERTY_LINESTYLE_DASHDOT);
				break;
			case 4:
				m_pShapePropertyEditLineStyle->SetValue(DEF_PROPERTY_LINESTYLE_DASHDOTDOT);
				break;
			default:
				m_pShapePropertyEditLineStyle->SetValue(DEF_PROPERTY_LINESTYLE_SOLID);
				break;
			}
		}
	}*/
}

// 도형 채움 사용 여부
void COverlayPropertiesWnd::SetShapePropertyFillUse(BOOL bFillUse)
{
	//if(m_AttrShape.m_bBrush != bFillUse)
	//{
	//	m_AttrShape.m_bBrush = bFillUse;

	//	if(m_pShapePropertyEditFillUse != NULL)
	//	{
	//		if(m_AttrShape.m_bBrush == TRUE)
	//			m_pShapePropertyEditFillUse->SetValue(DEF_PROPERTY_ENABLE);
	//		else
	//			m_pShapePropertyEditFillUse->SetValue(DEF_PROPERTY_DISABLE);

	//		UpdateShapeEditProperties();	// 공통 속성 UI 설정
	//	}
	//}
}

// 도형 채움 색상
void COverlayPropertiesWnd::SetShapePropertyFillColor(COLORREF clrFill)
{
	/*if(m_AttrShape.m_ColorBrush != clrFill)
	{
		m_AttrShape.m_ColorBrush = clrFill;

		if(m_pShapePropertyEditFillColor != NULL)
		{
			m_pShapePropertyEditFillColor->SetColor(m_AttrShape.m_ColorBrush);
		}
	}*/
}

// 도형 채움 반투명 여부
void COverlayPropertiesWnd::SetShapePropertyFillTrance(BOOL bTranceUse)
{
	/*if(m_AttrShape.m_bTransparent != bTranceUse)
	{
		m_AttrShape.m_bTransparent = bTranceUse;

		if(m_pShapePropertyEditFillTrance != NULL)
		{
			if(m_AttrShape.m_bTransparent == TRUE)
				m_pShapePropertyEditFillTrance->SetValue(DEF_PROPERTY_ENABLE);
			else
				m_pShapePropertyEditFillTrance->SetValue(DEF_PROPERTY_DISABLE);
		}
	}*/
}

// 도형 패턴 사용 여부
void COverlayPropertiesWnd::SetShapePropertyPatternUse(BOOL bPatternUse)
{
	//if(m_AttrShape.m_bPattern != bPatternUse)
	//{
	//	m_AttrShape.m_bPattern = bPatternUse;

	//	if(m_pShapePropertyEditPatternUse != NULL)
	//	{
	//		if(m_AttrShape.m_bPattern == TRUE)
	//			m_pShapePropertyEditPatternUse->SetValue(DEF_PROPERTY_ENABLE);
	//		else
	//			m_pShapePropertyEditPatternUse->SetValue(DEF_PROPERTY_DISABLE);

	//		UpdateShapeEditProperties();	// 공통 속성 UI 설정
	//	}
	//}
}

// 도형 패턴 색상
void COverlayPropertiesWnd::SetShapePropertyPatternColr(COLORREF clrPattern)
{
	/*if(m_AttrShape.m_ColorPattern != clrPattern)
	{
		m_AttrShape.m_ColorPattern = clrPattern;

		if(m_pShapePropertyEditPatternColor != NULL)
		{
			m_pShapePropertyEditPatternColor->SetColor(m_AttrShape.m_ColorPattern);
		}
	}*/
}

// 도형 패턴 모양
void COverlayPropertiesWnd::SetShapePropertyPatternStyle(int nPatternStyle)
{
	/*if(m_AttrShape.m_nPattern != nPatternStyle)
	{
		m_AttrShape.m_nPattern = nPatternStyle;

		if(m_pShapePropertyEditPatternStyle != NULL)
		{		
			switch(m_AttrShape.m_nPattern)
			{
			case 1:
				m_pShapePropertyEditPatternStyle->SetValue(DEF_PROPERTY_PATTERNSTYLE_1);
				break;
			case 2:
				m_pShapePropertyEditPatternStyle->SetValue(DEF_PROPERTY_PATTERNSTYLE_2);
				break;
			case 3:
				m_pShapePropertyEditPatternStyle->SetValue(DEF_PROPERTY_PATTERNSTYLE_3);
				break;
			case 4:
				m_pShapePropertyEditPatternStyle->SetValue(DEF_PROPERTY_PATTERNSTYLE_4);
				break;
			case 5:
				m_pShapePropertyEditPatternStyle->SetValue(DEF_PROPERTY_PATTERNSTYLE_5);
				break;
			default:
				m_pShapePropertyEditPatternStyle->SetValue(DEF_PROPERTY_PATTERNSTYLE_0);
				break;
			}
		}
	}*/
}

// 문자 글꼴
void COverlayPropertiesWnd::SetShapePropertyFontName(CString strFontName)
{
	/*m_AttrShapePointText.m_logfontName = strFontName;

	if(m_pShapePropertyTextName != NULL)
		m_pShapePropertyTextName->SetValue(m_AttrShapePointText.m_logfontName);*/
}

// 문자 크기
void COverlayPropertiesWnd::SetShapePropertyFontSize(int nFontSize)
{
	/*m_AttrShapePointText.m_logfontSize = nFontSize;

	if(m_pShapePropertyTextSize != NULL)
		m_pShapePropertyTextSize->SetValue((_variant_t)m_AttrShapePointText.m_logfontSize);*/
}

// 문자 색상
void COverlayPropertiesWnd::SetShapePropertyFontColor(COLORREF clrFont)
{
	/*m_AttrShapePointText.m_dwFontColor = clrFont;

	if(m_pShapePropertyTextColor != NULL)
		m_pShapePropertyTextColor->SetColor(clrFont);*/
}

// 문자 굵게
void COverlayPropertiesWnd::SetShapePropertyFontBold(BOOL bBold)
{
	/*if(bBold == TRUE)
		m_AttrShapePointText.m_lWeight = FW_BOLD;
	else
		m_AttrShapePointText.m_lWeight = FW_NORMAL;

	if(m_pShapePropertyTextBold != NULL)
	{
		if(m_AttrShapePointText.m_lWeight == FW_BOLD)
			m_pShapePropertyTextBold->SetValue(DEF_PROPERTY_ENABLE);
		else
			m_pShapePropertyTextBold->SetValue(DEF_PROPERTY_DISABLE);
	}*/
}

// 문자 취소선
void COverlayPropertiesWnd::SetShapePropertyFontStrikeOut(BOOL bStrikeOut)
{
	/*m_AttrShapePointText.m_bStrikeOut = bStrikeOut;

	if(m_pShapePropertyTextStrikeOut != NULL)
	{
		if(m_AttrShapePointText.m_bStrikeOut)
			m_pShapePropertyTextStrikeOut->SetValue(DEF_PROPERTY_ENABLE);
		else
			m_pShapePropertyTextStrikeOut->SetValue(DEF_PROPERTY_DISABLE);
	}*/
}

// 문자 기울이기
void COverlayPropertiesWnd::SetShapePropertyFontItalic(BOOL bItalic)
{
	/*m_AttrShapePointText.m_bItalic = bItalic;

	if(m_pShapePropertyTextItalic != NULL)
	{
		if(m_AttrShapePointText.m_bItalic)
			m_pShapePropertyTextItalic->SetValue(DEF_PROPERTY_ENABLE);
		else
			m_pShapePropertyTextItalic->SetValue(DEF_PROPERTY_DISABLE);
	}*/
}

// 문자 밑줄
void COverlayPropertiesWnd::SetShapePropertyFontUnderLine(BOOL bUnderLine)
{
	/*m_AttrShapePointText.m_bUnderline = bUnderLine;

	if(m_pShapePropertyTextUnderline != NULL)
	{
		if(m_AttrShapePointText.m_bUnderline)
			m_pShapePropertyTextUnderline->SetValue(DEF_PROPERTY_ENABLE);
		else
			m_pShapePropertyTextUnderline->SetValue(DEF_PROPERTY_DISABLE);
	}*/
}

// 문자 정렬
void COverlayPropertiesWnd::SetShapePropertyFontAlign(FONT_ALIGN_MODE emAlignMode)
{
	/*m_AttrShapePointText.m_nAlign = (int)emAlignMode;

	if(m_pShapePropertyTextAlign != NULL)
	{
		switch(m_AttrShapePointText.m_nAlign)
		{
		case 1:
			m_pShapePropertyTextAlign->SetValue(DEF_PROPERTY_CENTER);
			break;
		case 2:
			m_pShapePropertyTextAlign->SetValue(DEF_PROPERTY_RIGHT);
			break;
		case 0:
		default:
			m_pShapePropertyTextAlign->SetValue(DEF_PROPERTY_LEFT);
			break;
		}
	}*/
}

// wparam  - The control ID of the property list.
// lparam   - A pointer to the property (CMFCPropertyGridProperty) that changed.
LRESULT COverlayPropertiesWnd::OnPropertyChanged( WPARAM wparam, LPARAM lparam)
{
	CMFCPropertyGridProperty *pChangedProp = (CMFCPropertyGridProperty*)lparam;

	if(pChangedProp != NULL)
	{
		CMFCPropertyGridProperty *pParent = pChangedProp->GetParent();

		if(pParent != NULL)
		{
			switch(pParent->GetData())
			{
			case LAYERPROPERTY_TITLE:	// 레이어 속성
				ModifyLayerProperty(pChangedProp);
				break;

			case SHAPEPROPERTY_COMMON:	// 명칭, 설명 변경
				ModifyShapeCommonProperty(pChangedProp);
				break;

			case SHAPEPROPERTY_EDIT: // 도형 편집 속성 변경
				ModifyShapeEditProperty(pChangedProp);
				break;

			case SHAPEPROPERTY_LINE: // 선속성 변경
				ModifyShapeSplineProperty(pChangedProp);
				break;

			case SHAPEPROPERTY_RECT: // 사각형 변경
				ModifyShapeRectProperty(pChangedProp);
				break;

			case SHAPEPROPERTY_NGON: // 다각형 변경
				ModifyShapeNgonProperty(pChangedProp);
				break;

			case SHAPEPROPERTY_ARROW: // 화살표 변경
				ModifyShapeArrowProperty(pChangedProp);
				break;

			case SHAPEPROPERTY_TEXT: // 텍스트 변경
				ModifyShapeTextProperty(pChangedProp);
				break;

			case MILITARYPROP_POINT:	// 군대부호 점형 변경
			case MILITARYPROP_POINT_ADDINFO:// 군대부호 점형 속성 변경
				ModifyShapeMilPtProperty(pChangedProp);
				break;
			}
		}
	}

	return TRUE;
}

// 레이어 속성
void COverlayPropertiesWnd::ModifyLayerProperty(CMFCPropertyGridProperty* pLayerProperty)
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();

	if(pFrame != NULL && pFrame->GetSafeHwnd() != NULL && pLayerProperty != NULL && pLayerProperty->GetData() == LAYERPROPERTY_NAME)
	{
		CString strChangedName = (CString)pLayerProperty->GetValue().bstrVal;

		if(strChangedName.IsEmpty())
		{
			pLayerProperty->SetValue(m_strShapeName);
		}
		else
			pFrame->SendMessage(OVERLAY_PROP_LAYER_NAME, (WPARAM)&m_strOverLayerGuid, (LPARAM)&strChangedName);
	}
}

// 명칭 및 설명
void COverlayPropertiesWnd::ModifyShapeCommonProperty(CMFCPropertyGridProperty* pCommonProperty)
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();

	if(pCommonProperty != NULL && pFrame != NULL && pFrame->GetSafeHwnd() != NULL)
	{
		switch(pCommonProperty->GetData())
		{
		case SHAPEPROPERTY_NAME:	// 공통 - 명칭
			{
				CString strChangedValue = (CString)pCommonProperty->GetValue().bstrVal;

				if(strChangedValue.IsEmpty())
				{
					pCommonProperty->SetValue(m_strShapeName);
				}
				else if(m_strShapeName != strChangedValue)
				{
					m_strShapeName = strChangedValue;

					SHAPE_PROPERTY_CHANGED stChangedValue;
					stChangedValue.strOverlayGuid = m_strOverLayerGuid;
					stChangedValue.strShapeGuid = m_strShapeGuid;
					stChangedValue.strChangedValue = m_strShapeName;
					
					//pFrame->SendMessage(OVERLAY_PROP_COMMON_NAME, (WPARAM)&stChangedValue);
					CString strType;
					strType.Format("%d", OV_COMMON_OBJECTNAME);
					CMapXCOP_View * pView = (CMapXCOP_View *)pFrame->GetActiveView();
					pView->SendMessage(MAPX_OVER_SET_SHAPEPROPERTY, (WPARAM)&strType, (LPARAM)&m_strShapeName);
					pView->OvlMessageProc(MAPX_OVER_SET_SHAPEPROPERTY, strType, m_strShapeName, NULL);
				}
			}
			break;

		case SHAPEPROPERTY_DESCRIPT:// 공통 - 설명(비고)
			{
				CString strChangedValue = (CString)pCommonProperty->GetValue().bstrVal;

				if(m_strShapeDescription != strChangedValue)
				{
					m_strShapeDescription = strChangedValue;

					SHAPE_PROPERTY_CHANGED stChangedValue;
					stChangedValue.strOverlayGuid = m_strOverLayerGuid;
					stChangedValue.strShapeGuid = m_strShapeGuid;
					stChangedValue.strChangedValue = m_strShapeDescription;

					//pFrame->SendMessage(OVERLAY_PROP_COMMON_DESCRIPT, (WPARAM)&stChangedValue);
					CString strType;
					strType.Format("%d", OV_COMMON_OBJECTDESCRIPTION);
					CMapXCOP_View * pView = (CMapXCOP_View *)pFrame->GetActiveView();
					pView->SendMessage(MAPX_OVER_SET_SHAPEPROPERTY, (WPARAM)&strType, (LPARAM)&m_strShapeDescription);
					pView->OvlMessageProc(MAPX_OVER_SET_SHAPEPROPERTY, strType, m_strShapeDescription, NULL);
				}
			}
			break;
		}
	}
}

// 도형 편집 속성
void COverlayPropertiesWnd::ModifyShapeEditProperty(CMFCPropertyGridProperty* pEditProperty)
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();

	if(pEditProperty != NULL && pFrame != NULL && pFrame->GetSafeHwnd() != NULL)
	{
		switch(pEditProperty->GetData())
		{
		case SHAPEPROPERTY_EDIT_LINEWIDTH:	// 도형편집 - 선 굵기
			{
				int nChangedValue = pEditProperty->GetValue().lVal;

				/*if(m_AttrShape.m_nPenSize != nChangedValue)
				{
					m_AttrShape.m_nPenSize = nChangedValue;

					SHAPE_PROPERTY_CHANGED stChangedValue;
					stChangedValue.strOverlayGuid = m_strOverLayerGuid;
					stChangedValue.strShapeGuid = m_strShapeGuid;
					stChangedValue.nChangedValue = m_AttrShape.m_nPenSize;

					pFrame->SendMessage(OVERLAY_PROP_EDIT_LINEWIDTH, (WPARAM)&stChangedValue);
				}*/
			}
			break;

		case SHAPEPROPERTY_EDIT_LINECOLOR:	// 도형편집 - 선 색상
			{
				COLORREF clrChangedValue = pEditProperty->GetValue().lVal;

				/*if(m_AttrShape.m_ColorPen != clrChangedValue)
				{
					m_AttrShape.m_ColorPen = clrChangedValue;

					SHAPE_PROPERTY_CHANGED stChangedValue;
					stChangedValue.strOverlayGuid = m_strOverLayerGuid;
					stChangedValue.strShapeGuid = m_strShapeGuid;
					stChangedValue.clrChangedValue = m_AttrShape.m_ColorPen;

					pFrame->SendMessage(OVERLAY_PROP_EDIT_LINECOLOR, (WPARAM)&stChangedValue);
				}*/
			}
			break;

		case SHAPEPROPERTY_EDIT_LINEKIND:	// 도형편집 - 선 종류
			{
				CString strOption = (CString)pEditProperty->GetValue().bstrVal;

				int nChangedValue = 0;

				if(strOption == DEF_PROPERTY_LINESTYLE_DASH)
					nChangedValue = 1;
				else if(strOption == DEF_PROPERTY_LINESTYLE_DOT)
					nChangedValue = 2;
				else if(strOption == DEF_PROPERTY_LINESTYLE_DASHDOT)
					nChangedValue = 3;
				else if(strOption == DEF_PROPERTY_LINESTYLE_DASHDOTDOT)
					nChangedValue = 4;

				/*if(m_AttrShape.m_nStyle != nChangedValue)
				{
					m_AttrShape.m_nStyle = nChangedValue;

					SHAPE_PROPERTY_CHANGED stChangedValue;
					stChangedValue.strOverlayGuid = m_strOverLayerGuid;
					stChangedValue.strShapeGuid = m_strShapeGuid;
					stChangedValue.nChangedValue = m_AttrShape.m_nStyle;

					pFrame->SendMessage(OVERLAY_PROP_EDIT_LINESTYLE, (WPARAM)&stChangedValue);
				}*/
			}
			break;

		case SHAPEPROPERTY_EDIT_FILLUSE:	// 도형편집 - 채움여부
			{
				CString strOption = (CString)pEditProperty->GetValue().bstrVal;

				BOOL bChangedValue = TRUE;

				if(strOption == DEF_PROPERTY_DISABLE)
					bChangedValue = FALSE;

				//if(m_AttrShape.m_bBrush != bChangedValue)
				//{
				//	m_AttrShape.m_bBrush = bChangedValue;

				//	SHAPE_PROPERTY_CHANGED stChangedValue;
				//	stChangedValue.strOverlayGuid = m_strOverLayerGuid;
				//	stChangedValue.strShapeGuid = m_strShapeGuid;
				//	stChangedValue.bChangedValue = m_AttrShape.m_bBrush;

				//	pFrame->SendMessage(OVERLAY_PROP_EDIT_FILLUSE, (WPARAM)&stChangedValue);

				//	// 채움 사용 여부에 따른 Enable 설정
				//	UpdateShapeEditProperties();
				//}
			}
			break;

		case SHAPEPROPERTY_EDIT_FILLCOLOR:	// 도형편집 - 채움색상
			{
				COLORREF clrChangedValue = pEditProperty->GetValue().lVal;

				if(m_AttrShape.m_ColorBrush != clrChangedValue)
				{
					m_AttrShape.m_ColorBrush = clrChangedValue;

					SHAPE_PROPERTY_CHANGED stChangedValue;
					stChangedValue.strOverlayGuid = m_strOverLayerGuid;
					stChangedValue.strShapeGuid = m_strShapeGuid;
					stChangedValue.clrChangedValue = m_AttrShape.m_ColorBrush;

					pFrame->SendMessage(OVERLAY_PROP_EDIT_FILLCOLOR, (WPARAM)&stChangedValue);
				}
			}
			break;

		case SHAPEPROPERTY_EDIT_TRANSPARENT:// 도형편집 - 반투명
			{
				CString strOption = (CString)pEditProperty->GetValue().bstrVal;

				BOOL bChangedValue = TRUE;

				if(strOption == DEF_PROPERTY_DISABLE)
					bChangedValue = FALSE;

				/*if(m_AttrShape.m_bTransparent != bChangedValue)
				{
					m_AttrShape.m_bTransparent = bChangedValue;

					SHAPE_PROPERTY_CHANGED stChangedValue;
					stChangedValue.strOverlayGuid = m_strOverLayerGuid;
					stChangedValue.strShapeGuid = m_strShapeGuid;
					stChangedValue.bChangedValue = m_AttrShape.m_bTransparent;

					pFrame->SendMessage(OVERLAY_PROP_EDIT_FILLTRANCE, (WPARAM)&stChangedValue);
				}*/
			}
			break;

		case SHAPEPROPERTY_EDIT_PATTERNUSE:	// 도형편집 - 패턴사용여부
			{
				CString strOption = (CString)pEditProperty->GetValue().bstrVal;

				BOOL bChangedValue = TRUE;

				if(strOption == DEF_PROPERTY_DISABLE)
					bChangedValue = FALSE;

				//if(m_AttrShape.m_bPattern != bChangedValue)
				//{
				//	m_AttrShape.m_bPattern = bChangedValue;

				//	SHAPE_PROPERTY_CHANGED stChangedValue;
				//	stChangedValue.strOverlayGuid = m_strOverLayerGuid;
				//	stChangedValue.strShapeGuid = m_strShapeGuid;
				//	stChangedValue.bChangedValue = m_AttrShape.m_bPattern;

				//	pFrame->SendMessage(OVERLAY_PROP_EDIT_PATTERNUSE, (WPARAM)&stChangedValue);

				//	// 패턴 사용 여부에 따른 Enable 설정
				//	UpdateShapeEditProperties();
				//}
			}
			break;

		case SHAPEPROPERTY_EDIT_PATTERNCOLOR:	// 도형편집 - 패턴색상
			{
				COLORREF clrChangedValue = pEditProperty->GetValue().lVal;

				/*if(m_AttrShape.m_ColorPattern != clrChangedValue)
				{
					m_AttrShape.m_ColorPattern = clrChangedValue;

					SHAPE_PROPERTY_CHANGED stChangedValue;
					stChangedValue.strOverlayGuid = m_strOverLayerGuid;
					stChangedValue.strShapeGuid = m_strShapeGuid;
					stChangedValue.clrChangedValue = m_AttrShape.m_ColorPattern;

					pFrame->SendMessage(OVERLAY_PROP_EDIT_PATTERNCOLOR, (WPARAM)&stChangedValue);
				}*/
			}
			break;

		case SHAPEPROPERTY_EDIT_PATTERNKIND:	// 도형편집 - 패턴종류
			{
				CString strOption = (CString)pEditProperty->GetValue().bstrVal;

				int nChangedValue = 0;

				if(strOption == DEF_PROPERTY_PATTERNSTYLE_1)
					nChangedValue = 1;
				else if(strOption == DEF_PROPERTY_PATTERNSTYLE_2)
					nChangedValue = 2;
				else if(strOption == DEF_PROPERTY_PATTERNSTYLE_3)
					nChangedValue = 3;
				else if(strOption == DEF_PROPERTY_PATTERNSTYLE_4)
					nChangedValue = 4;
				else if(strOption == DEF_PROPERTY_PATTERNSTYLE_5)
					nChangedValue = 5;

				if(m_AttrShape.m_nPattern != nChangedValue)
				{
					m_AttrShape.m_nPattern = nChangedValue;

					SHAPE_PROPERTY_CHANGED stChangedValue;
					stChangedValue.strOverlayGuid = m_strOverLayerGuid;
					stChangedValue.strShapeGuid = m_strShapeGuid;
					stChangedValue.nChangedValue = m_AttrShape.m_nPattern;

					pFrame->SendMessage(OVERLAY_PROP_EDIT_PATTERNSTYLE, (WPARAM)&stChangedValue);
				}
			}
			break;

		case SHAPEPROPERTY_EDIT_ROTATE:	// 도형편집 - 회전
			{
				int nChangedValue = pEditProperty->GetValue().lVal;

				if(m_AttrShape.m_nAngle != nChangedValue)
				{
					m_AttrShape.m_nAngle = nChangedValue;

					SHAPE_PROPERTY_CHANGED stChangedValue;
					stChangedValue.strOverlayGuid = m_strOverLayerGuid;
					stChangedValue.strShapeGuid = m_strShapeGuid;
					stChangedValue.nChangedValue = m_AttrShape.m_nAngle;

					pFrame->SendMessage(OVERLAY_PROP_EDIT_ROTATE, (WPARAM)&stChangedValue);
				}
			}
			break;
		}
	}
}

// 선 속성
void COverlayPropertiesWnd::ModifyShapeSplineProperty(CMFCPropertyGridProperty* pSplineProperty)
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();

	if(pSplineProperty != NULL && pFrame != NULL && pFrame->GetSafeHwnd() != NULL)
	{
		switch(pSplineProperty->GetData())
		{
		case SHAPEPROPERTY_LINE_STRAIGHT:	// 선속성 - 직선/곡선
			{
				CString strOption = (CString)pSplineProperty->GetValue().bstrVal;

				BOOL bLine = TRUE;

				if(strOption == DEF_PROPERTY_SPLINE)
					bLine = FALSE;

				/*if(m_AttrShapeSpline.m_bLine != bLine)
				{
					m_AttrShapeSpline.m_bLine = bLine;

					SHAPE_PROPERTY_CHANGED stChangedValue;
					stChangedValue.strOverlayGuid = m_strOverLayerGuid;
					stChangedValue.strShapeGuid = m_strShapeGuid;
					stChangedValue.bChangedValue = m_AttrShapeSpline.m_bLine;

					pFrame->SendMessage(OVERLAY_PROP_LINE_STRAIGHT, (WPARAM)&stChangedValue);
				}*/
			}
			break;

		case SHAPEPROPERTY_LINE_STARTARROW:	// 선속성 - 시작화살표
			{
				CString strOption = (CString)pSplineProperty->GetValue().bstrVal;

				BOOL bFrontHead = TRUE;
				BOOL m_bFrontArrowClose = TRUE;

				if(strOption == DEF_PROPERTY_DISABLE)
					bFrontHead = FALSE;
				else
				{
					if(strOption == DEF_PROPERTY_OPEN)
						m_bFrontArrowClose = FALSE;
				}

				if(m_AttrShapeSpline.m_bFrontArrowClose != m_bFrontArrowClose || m_AttrShapeSpline.m_bFrontHead != bFrontHead)
				{
					m_AttrShapeSpline.m_bFrontHead = bFrontHead;
					m_AttrShapeSpline.m_bFrontArrowClose = m_bFrontArrowClose;

					SHAPE_PROPERTY_CHANGED stChangedValue;
					stChangedValue.strOverlayGuid = m_strOverLayerGuid;
					stChangedValue.strShapeGuid = m_strShapeGuid;
					stChangedValue.bChangedValue = m_AttrShapeSpline.m_bFrontHead;
					stChangedValue.bChangedSecondValue = m_AttrShapeSpline.m_bFrontArrowClose;

					pFrame->SendMessage(OVERLAY_PROP_LINE_STARTARROW, (WPARAM)&stChangedValue);
				}
			}
			break;

		case SHAPEPROPERTY_LINE_ENDARROW:	// 선속성 - 종료화살표
			{
				CString strOption = (CString)pSplineProperty->GetValue().bstrVal;

				BOOL bBackHead = TRUE;
				BOOL m_bBackArrowClose = TRUE;

				if(strOption == DEF_PROPERTY_DISABLE)
					bBackHead = FALSE;
				else
				{
					if(strOption == DEF_PROPERTY_OPEN)
						m_bBackArrowClose = FALSE;
				}

				/*if(m_AttrShapeSpline.m_bBackArrowClose != m_bBackArrowClose || m_AttrShapeSpline.m_bBackHead != bBackHead)
				{
					m_AttrShapeSpline.m_bBackHead = bBackHead;
					m_AttrShapeSpline.m_bBackArrowClose = m_bBackArrowClose;

					SHAPE_PROPERTY_CHANGED stChangedValue;
					stChangedValue.strOverlayGuid = m_strOverLayerGuid;
					stChangedValue.strShapeGuid = m_strShapeGuid;
					stChangedValue.bChangedValue = m_AttrShapeSpline.m_bBackHead;
					stChangedValue.bChangedSecondValue = m_AttrShapeSpline.m_bBackArrowClose;

					pFrame->SendMessage(OVERLAY_PROP_LINE_ENDARROW, (WPARAM)&stChangedValue);
				}*/
			}
			break;

		case SHAPEPROPERTY_LINE_CLOSED:		// 선속성 - 시작점끝점연결여부
			{
				CString strOption = (CString)pSplineProperty->GetValue().bstrVal;

				BOOL bClosed = FALSE;

				if(strOption == DEF_PROPERTY_CONNECT)
					bClosed = TRUE;

				//if(m_AttrShapeSpline.m_bClosed != bClosed)
				//{
				//	m_AttrShapeSpline.m_bClosed = bClosed;
				//	m_bUseEditFillProperty = m_AttrShapeSpline.m_bClosed;

				//	SHAPE_PROPERTY_CHANGED stChangedValue;
				//	stChangedValue.strOverlayGuid = m_strOverLayerGuid;
				//	stChangedValue.strShapeGuid = m_strShapeGuid;
				//	stChangedValue.bChangedValue = m_AttrShapeSpline.m_bClosed;

				//	pFrame->SendMessage(OVERLAY_PROP_LINE_CLOSED, (WPARAM)&stChangedValue);

				//	// 선면 변경에 따른 Enable 설정
				//	UpdateShapeEditProperties();
				//	UpdateShapeSplineProperties();
				//}
			}
			break;
		}
	}
}

// 다각형 속성
void COverlayPropertiesWnd::ModifyShapeNgonProperty(CMFCPropertyGridProperty* pNgonProperty)
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();

	if(pNgonProperty != NULL && pFrame != NULL && pFrame->GetSafeHwnd() != NULL)
	{
		if(pNgonProperty->GetData() == SHAPEPROPERTY_NGON_COUNT)
		{
			int nChangedValue = pNgonProperty->GetValue().lVal;

			if(m_AttrShapeNgon.m_nNgonVar != nChangedValue)
			{
				m_AttrShapeNgon.m_nNgonVar = nChangedValue;

				SHAPE_PROPERTY_CHANGED stChangedValue;
				stChangedValue.strOverlayGuid = m_strOverLayerGuid;
				stChangedValue.strShapeGuid = m_strShapeGuid;
				stChangedValue.nChangedValue = m_AttrShapeNgon.m_nNgonVar;

				//pFrame->SendMessage(OVERLAY_PROP_NGON_COUNT, (WPARAM)&stChangedValue);
				CString strType, strValue;
				strType.Format("%d", OV_PROP_NGONANGLE);
				strValue.Format("%d", nChangedValue);
				//pFrame->SendMessage(MAPX_OVER_SET_SHAPEPROPERTY, (WPARAM)&strType, (LPARAM)&strValue);
				CMapXCOP_View * pView = (CMapXCOP_View *)pFrame->GetActiveView();
				//pView->SendMessage(MAPX_OVER_SET_SHAPEPROPERTY, (WPARAM)&strType, (LPARAM)&strValue);
				pView->OvlMessageProc(MAPX_OVER_SET_SHAPEPROPERTY, strType, strValue, NULL);
			}
		}
	}
}

// 사각형 속성
void COverlayPropertiesWnd::ModifyShapeRectProperty(CMFCPropertyGridProperty* pRectProperty)
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();

	if(pRectProperty != NULL && pFrame != NULL && pFrame->GetSafeHwnd() != NULL)
	{
		if(pRectProperty->GetData() == SHAPEPROPERTY_RECT_ROUND)
		{
			int nChangedValue = pRectProperty->GetValue().lVal;

			if(m_AttrShapeRect.m_nRound != nChangedValue)
			{
				m_AttrShapeRect.m_nRound = nChangedValue;

				SHAPE_PROPERTY_CHANGED stChangedValue;
				stChangedValue.strOverlayGuid = m_strOverLayerGuid;
				stChangedValue.strShapeGuid = m_strShapeGuid;
				stChangedValue.nChangedValue = m_AttrShapeRect.m_nRound;

				//pFrame->SendMessage(OVERLAY_PROP_RECT_ROUND, (WPARAM)&stChangedValue);
				CString strType, strValue;
				strType.Format("%d", OV_PROP_RECTANGLEROUND);
				strValue.Format("%d", nChangedValue);
				//pFrame->SendMessage(MAPX_OVER_SET_SHAPEPROPERTY, (WPARAM)&strType, (LPARAM)&strValue);
				CMapXCOP_View * pView = (CMapXCOP_View *)pFrame->GetActiveView();
				pView->SendMessage(MAPX_OVER_SET_SHAPEPROPERTY, (WPARAM)&strType, (LPARAM)&strValue);
				pView->OvlMessageProc(MAPX_OVER_SET_SHAPEPROPERTY, strType, strValue, NULL);
			}
		}
	}	
}

// 화살표 속성
void COverlayPropertiesWnd::ModifyShapeArrowProperty(CMFCPropertyGridProperty* pArrowProperty)
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();

	if(pArrowProperty != NULL && pFrame != NULL && pFrame->GetSafeHwnd() != NULL)
	{
		switch(pArrowProperty->GetData())
		{
		case SHAPEPROPERTY_ARROW_TAIL:
			{
				CString strOption = (CString)pArrowProperty->GetValue().bstrVal;

				int nCahngedValue = 0;

				if(strOption == DEF_PROPERTY_CLOSE)
					nCahngedValue = 1;
				else if(strOption == DEF_PROPERTY_RIBBON)
					nCahngedValue = 2;

				if(m_AttrShapeArrow.m_nTail != nCahngedValue)
				{
					m_AttrShapeArrow.m_nTail = nCahngedValue;

					SHAPE_PROPERTY_CHANGED stChangedValue;
					stChangedValue.strOverlayGuid = m_strOverLayerGuid;
					stChangedValue.strShapeGuid = m_strShapeGuid;
					stChangedValue.nChangedValue = m_AttrShapeArrow.m_nTail;

					pFrame->SendMessage(OVERLAY_PROP_ARROW_TAIL, (WPARAM)&stChangedValue);
				}
			}
			break;

		case SHAPEPROPERTY_ARROW_WIDTH:
			{
				int nChangedValue = pArrowProperty->GetValue().lVal;

				if(m_AttrShapeArrow.m_nWidth != nChangedValue)
				{
					m_AttrShapeArrow.m_nWidth = nChangedValue;

					SHAPE_PROPERTY_CHANGED stChangedValue;
					stChangedValue.strOverlayGuid = m_strOverLayerGuid;
					stChangedValue.strShapeGuid = m_strShapeGuid;
					stChangedValue.nChangedValue = m_AttrShapeArrow.m_nWidth;

					pFrame->SendMessage(OVERLAY_PROP_ARROW_WIDTH, (WPARAM)&stChangedValue);
				}
			}
			break;
		}
	}
}

// 텍스트 속성
void COverlayPropertiesWnd::ModifyShapeTextProperty(CMFCPropertyGridProperty* pPointText)
{
	/*CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();

	if(pPointText != NULL && pFrame != NULL && pFrame->GetSafeHwnd() != NULL)
	{
		switch(pPointText->GetData())
		{
		case SHAPEPROPERTY_TEXT_SIZE:// 텍스트 - 크기
			{
				int nChangedValue = pPointText->GetValue().lVal;

				if(abs(m_AttrShapePointText.m_logfontSize) != nChangedValue)
				{
					m_AttrShapePointText.m_logfontSize = -nChangedValue;

					SHAPE_PROPERTY_CHANGED stChangedValue;
					stChangedValue.strOverlayGuid = m_strOverLayerGuid;
					stChangedValue.strShapeGuid = m_strShapeGuid;
					stChangedValue.nChangedValue = nChangedValue;

					pFrame->SendMessage(OVERLAY_PROP_TEXT_SIZE, (WPARAM)&stChangedValue);
				}
			}
			break;

		case SHAPEPROPERTY_TEXT_COLOR:// 텍스트 - 색상
			{
				COLORREF clrChangedValue = pPointText->GetValue().lVal;

				if(m_AttrShapePointText.m_dwFontColor != clrChangedValue)
				{
					m_AttrShapePointText.m_dwFontColor = clrChangedValue;

					SHAPE_PROPERTY_CHANGED stChangedValue;
					stChangedValue.strOverlayGuid = m_strOverLayerGuid;
					stChangedValue.strShapeGuid = m_strShapeGuid;
					stChangedValue.clrChangedValue = m_AttrShapePointText.m_dwFontColor;

					pFrame->SendMessage(OVERLAY_PROP_TEXT_COLOR, (WPARAM)&stChangedValue);
				}
			}
			break;

		case SHAPEPROPERTY_TEXT_KIND:// 텍스트 - 종류
			{
				CString strValue = (CString)pPointText->GetValue().bstrVal;

				if(m_AttrShapePointText.m_logfontName != strValue)
				{
					m_AttrShapePointText.m_logfontName = strValue;

					SHAPE_PROPERTY_CHANGED stChangedValue;
					stChangedValue.strOverlayGuid = m_strOverLayerGuid;
					stChangedValue.strShapeGuid = m_strShapeGuid;
					stChangedValue.strChangedValue = m_AttrShapePointText.m_logfontName;

					pFrame->SendMessage(OVERLAY_PROP_TEXT_KIND, (WPARAM)&stChangedValue);
				}
			}
			break;

		case SHAPEPROPERTY_TEXT_ALIGN:// 텍스트 - 정렬
			{
				CString strOption = (CString)pPointText->GetValue().bstrVal;
				
				int nAlign = 0;

				if(strOption == DEF_PROPERTY_CENTER)
					nAlign = 1;
				else if(strOption == DEF_PROPERTY_RIGHT)
					nAlign = 2;

				if(m_AttrShapePointText.m_nAlign != nAlign)
				{
					m_AttrShapePointText.m_nAlign = nAlign;

					SHAPE_PROPERTY_CHANGED stChangedValue;
					stChangedValue.strOverlayGuid = m_strOverLayerGuid;
					stChangedValue.strShapeGuid = m_strShapeGuid;
					stChangedValue.nChangedValue = m_AttrShapePointText.m_nAlign;

					pFrame->SendMessage(OVERLAY_PROP_TEXT_ALIGN, (WPARAM)&stChangedValue);
				}
			}
			break;

		case SHAPEPROPERTY_TEXT_STRING:// 텍스트 - 내용
			{
				CString strValue = (CString)pPointText->GetValue().bstrVal;

				if(m_AttrShapePointText.m_lpszText != strValue)
				{
					m_AttrShapePointText.m_lpszText = strValue;

					SHAPE_PROPERTY_CHANGED stChangedValue;
					stChangedValue.strOverlayGuid = m_strOverLayerGuid;
					stChangedValue.strShapeGuid = m_strShapeGuid;
					stChangedValue.strChangedValue = m_AttrShapePointText.m_lpszText;

					pFrame->SendMessage(OVERLAY_PROP_TEXT_STRING, (WPARAM)&stChangedValue);
				}
			}
			break;

		case SHAPEPROPERTY_TEXT_WEIGHT:// 텍스트 - 굵게
			{
				CString strOption = (CString)pPointText->GetValue().bstrVal;

				BOOL bPreBold = TRUE, bNextBold = TRUE;

				if(strOption == DEF_PROPERTY_DISABLE)
					bNextBold = FALSE;

				if(m_AttrShapePointText.m_lWeight != FW_BOLD)
					bPreBold = FALSE;

				if(bPreBold != bNextBold)
				{
					if(bNextBold == TRUE)
						m_AttrShapePointText.m_lWeight = FW_BOLD;
					else
						m_AttrShapePointText.m_lWeight = FW_NORMAL;

					SHAPE_PROPERTY_CHANGED stChangedValue;
					stChangedValue.strOverlayGuid = m_strOverLayerGuid;
					stChangedValue.strShapeGuid = m_strShapeGuid;
					stChangedValue.bChangedValue = bNextBold;

					pFrame->SendMessage(OVERLAY_PROP_TEXT_WEIGHT, (WPARAM)&stChangedValue);
				}
			}
			break;

		case SHAPEPROPERTY_TEXT_ITALIC:// 텍스트 - 기울임
			{
				CString strOption = (CString)pPointText->GetValue().bstrVal;

				BOOL bItalic = TRUE;

				if(strOption == DEF_PROPERTY_DISABLE)
					bItalic = FALSE;

				if(m_AttrShapePointText.m_bItalic != bItalic)
				{
					m_AttrShapePointText.m_bItalic = bItalic;

					SHAPE_PROPERTY_CHANGED stChangedValue;
					stChangedValue.strOverlayGuid = m_strOverLayerGuid;
					stChangedValue.strShapeGuid = m_strShapeGuid;
					stChangedValue.bChangedValue = m_AttrShapePointText.m_bItalic;

					pFrame->SendMessage(OVERLAY_PROP_TEXT_ITALIC, (WPARAM)&stChangedValue);
				}
			}
			break;

		case SHAPEPROPERTY_TEXT_UNDERLINE:// 텍스트 - 하단 라인
			{
				CString strOption = (CString)pPointText->GetValue().bstrVal;

				BOOL bUnderline = TRUE;

				if(strOption == DEF_PROPERTY_DISABLE)
					bUnderline = FALSE;

				if(m_AttrShapePointText.m_bUnderline != bUnderline)
				{
					m_AttrShapePointText.m_bUnderline = bUnderline;

					SHAPE_PROPERTY_CHANGED stChangedValue;
					stChangedValue.strOverlayGuid = m_strOverLayerGuid;
					stChangedValue.strShapeGuid = m_strShapeGuid;
					stChangedValue.bChangedValue = m_AttrShapePointText.m_bUnderline;

					pFrame->SendMessage(OVERLAY_PROP_TEXT_UNDERLINE, (WPARAM)&stChangedValue);
				}
			}
			break;

		case SHAPEPROPERTY_TEXT_STRIKEOUT:// 텍스트 - 취소선
			{
				CString strOption = (CString)pPointText->GetValue().bstrVal;

				BOOL bStrikeOut = TRUE;

				if(strOption == DEF_PROPERTY_DISABLE)
					bStrikeOut = FALSE;

				if(m_AttrShapePointText.m_bStrikeOut != bStrikeOut)
				{
					m_AttrShapePointText.m_bStrikeOut = bStrikeOut;

					SHAPE_PROPERTY_CHANGED stChangedValue;
					stChangedValue.strOverlayGuid = m_strOverLayerGuid;
					stChangedValue.strShapeGuid = m_strShapeGuid;
					stChangedValue.bChangedValue = m_AttrShapePointText.m_bStrikeOut;

					pFrame->SendMessage(OVERLAY_PROP_TEXT_STRIKEOUT, (WPARAM)&stChangedValue);
				}
			}
			break;
		}
	}*/
}

void COverlayPropertiesWnd::ModifyShapeMilPtProperty(CMFCPropertyGridProperty* pMilPtProperty)
{
#ifdef _MILITARY
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();

	if(pMilPtProperty != NULL && pFrame != NULL && pFrame->GetSafeHwnd() != NULL)
	{
		switch(pMilPtProperty->GetData())
		{
			// 부호 크기
		case MILITARY_SIZE:
			{
				int nChangedValue = pMilPtProperty->GetValue().lVal;

				// 범위 확인
				if(50 <= nChangedValue && nChangedValue <= 300)
				{
					if(abs(m_AttrShapeMilitaryPt.m_nMilSymSize) != nChangedValue)
					{
						m_AttrShapeMilitaryPt.m_nMilSymSize = nChangedValue;

						SHAPE_PROPERTY_CHANGED stChangedValue;
						stChangedValue.strOverlayGuid = m_strOverLayerGuid;
						stChangedValue.strShapeGuid = m_strShapeGuid;
						stChangedValue.nChangedValue = nChangedValue;

						pFrame->SendMessage(OVERLAY_PROP_MILPT_SIZE, (WPARAM)&stChangedValue);
					}
				}
			}
			break;

			// 부호 색상
		case MILITARY_COLOR:
			{
				COLORREF clrChangedValue = pMilPtProperty->GetValue().lVal;

				if(m_AttrShapeMilitaryPt.m_clrMilSym != clrChangedValue)
				{
					m_AttrShapeMilitaryPt.m_clrMilSym = clrChangedValue;

					SHAPE_PROPERTY_CHANGED stChangedValue;
					stChangedValue.strOverlayGuid = m_strOverLayerGuid;
					stChangedValue.strShapeGuid = m_strShapeGuid;
					stChangedValue.clrChangedValue = clrChangedValue;

					pFrame->SendMessage(OVERLAY_PROP_MILPT_COLOR, (WPARAM)&stChangedValue);
				}
			}
			break;

			// 추가정보 색상
		case MILITARY_ADDINFO_COLOR:
			{
				COLORREF clrChangedValue = pMilPtProperty->GetValue().lVal;

				if(m_AttrShapeMilitaryPt.m_clrMilSymTextColor != clrChangedValue)
				{
					m_AttrShapeMilitaryPt.m_clrMilSymTextColor = clrChangedValue;

					SHAPE_PROPERTY_CHANGED stChangedValue;
					stChangedValue.strOverlayGuid = m_strOverLayerGuid;
					stChangedValue.strShapeGuid = m_strShapeGuid;
					stChangedValue.clrChangedValue = clrChangedValue;

					pFrame->SendMessage(OVERLAY_PROP_MILPT_ADDINFO_COLOR, (WPARAM)&stChangedValue);
				}
			}
			break;

		case MILITARY_ADDINFO_C:// 장비수량 
		case MILITARY_ADDINFO_F:// 부대증감, 부대의 증감을 표현한다. (+)증가, (-)감소, (+-)증감	20크기제한								[부o,장x,시x,신x,기o]
		case MILITARY_ADDINFO_G:// 군 및 국가구분, 부호가 나타내는 객체의 소속 군 및 국가를 표기한다. 예) 육군-미국	20 크기제한	[부o,장o,시o,신o,기o]
			break;
			// T 추가사항
		case MILITARY_ADDINFO_H:
		case MILITARY_ADDINFO_J1:// 신뢰도
		case MILITARY_ADDINFO_J2:// 정확성
			break;
			// T 전투력
		case MILITARY_ADDINFO_K:
			{
				TRACE("\n MILITARY_ADDINFO_K \n ");
			}
			break;
		case MILITARY_ADDINFO_L:// 전자장비
			break;
		case MILITARY_ADDINFO_M:// 상급제대
			{
				TRACE("\n MILITARY_ADDINFO_M \n ");
			}
			break;
		case MILITARY_ADDINFO_N:// 적군표시
		case MILITARY_ADDINFO_P:// 피아식별 모드 및 코드
		case MILITARY_ADDINFO_Q:// 이동방향
		case MILITARY_ADDINFO_R2:// 전자장비 이동성
			break;
		case MILITARY_ADDINFO_T:// 고유명칭
			{
				TRACE("\n MILITARY_ADDINFO_T \n ");
			}
			break;
		case MILITARY_ADDINFO_V:// 장비종류
		case MILITARY_ADDINFO_W:// 활동시각
		case MILITARY_ADDINFO_X:// 고도/심도
		case MILITARY_ADDINFO_Y:// 위치
		case MILITARY_ADDINFO_Z:// 속도
		case MILITARY_ADDINFO_AA:// 지휘소명
		case MILITARY_ADDINFO_AD:// 기반형태
		case MILITARY_ADDINFO_AE:// 장비분해시간
		case MILITARY_ADDINFO_AF:// 공통명칭
		case MILITARY_ADDINFO_AG:// 보조장비식별부호
			break;
		}
	}
#endif
}

// 공통 속성 UI 설정
void COverlayPropertiesWnd::UpdateShapeEditProperties()
{
	if(m_bUseEditFillProperty)
	{
		// 면 속성
		if(m_pShapePropertyEditRotate != NULL)// 회전
		{
			//if(m_nShapeKind == ID_OT_DRAW_ARROW)
			if(m_nShapeKind == 8)
				m_pShapePropertyEditRotate->Enable(FALSE);
			else
				m_pShapePropertyEditRotate->Enable(TRUE);
		}

		if(m_pShapePropertyEditFillUse != NULL)// 채움사용
			m_pShapePropertyEditFillUse->Enable(TRUE);

		if(m_AttrShape.m_bBrush)
		{
			if(m_pShapePropertyEditFillColor != NULL)// 채움색상(RGB)
				m_pShapePropertyEditFillColor->Enable(TRUE);
			if(m_pShapePropertyEditFillTrance != NULL)// 반투명
				m_pShapePropertyEditFillTrance->Enable(TRUE);

			if(m_pShapePropertyEditPatternUse != NULL)// 패턴사용
				m_pShapePropertyEditPatternUse->Enable(TRUE); // 패턴사용
		}
		else
		{
			if(m_pShapePropertyEditFillColor != NULL)// 채움색상(RGB)
				m_pShapePropertyEditFillColor->Enable(FALSE);
			if(m_pShapePropertyEditFillTrance != NULL)// 반투명
				m_pShapePropertyEditFillTrance->Enable(FALSE);

			if(m_pShapePropertyEditPatternUse != NULL)// 패턴사용
				m_pShapePropertyEditPatternUse->Enable(FALSE); // 패턴사용
		}

		if(m_AttrShape.m_bPattern)
		{
			if(m_pShapePropertyEditPatternColor != NULL)// 패턴색상(RGB)
				m_pShapePropertyEditPatternColor->Enable(TRUE);
			if(m_pShapePropertyEditPatternStyle != NULL)// 패턴종류
				m_pShapePropertyEditPatternStyle->Enable(TRUE);
		}
		else
		{
			if(m_pShapePropertyEditPatternColor != NULL)// 패턴색상(RGB)
				m_pShapePropertyEditPatternColor->Enable(FALSE);
			if(m_pShapePropertyEditPatternStyle != NULL)// 패턴종류
				m_pShapePropertyEditPatternStyle->Enable(FALSE);
		}
	}
	else
	{
		// 라인 속성
		if(m_pShapePropertyEditRotate != NULL)// 회전
			m_pShapePropertyEditRotate->Enable(FALSE);

		if(m_pShapePropertyEditFillUse != NULL)// 채움사용
			m_pShapePropertyEditFillUse->Enable(FALSE);
		if(m_pShapePropertyEditFillColor != NULL)// 채움색상(RGB)
			m_pShapePropertyEditFillColor->Enable(FALSE);
		if(m_pShapePropertyEditFillTrance != NULL)// 반투명
			m_pShapePropertyEditFillTrance->Enable(FALSE);

		if(m_pShapePropertyEditPatternUse != NULL)// 패턴사용
			m_pShapePropertyEditPatternUse->Enable(FALSE);
		if(m_pShapePropertyEditPatternColor != NULL)// 패턴색상(RGB)
			m_pShapePropertyEditPatternColor->Enable(FALSE);
		if(m_pShapePropertyEditPatternStyle != NULL)// 패턴종류
			m_pShapePropertyEditPatternStyle->Enable(FALSE);
	}	
}

// 라인 속성 UI 설정
void COverlayPropertiesWnd::UpdateShapeSplineProperties()
{
	// 선 연결 유무에 따른 화살표 Enable 변경
	if(m_AttrShapeSpline.m_bClosed == FALSE && m_bUseEditFillProperty == FALSE)
	{
		m_pLineShapeStartArrow->Enable(TRUE);
		m_pLineShapeEndArrow->Enable(TRUE);
	}
	else
	{
		m_pLineShapeStartArrow->Enable(FALSE);
		m_pLineShapeEndArrow->Enable(FALSE);
	}
}