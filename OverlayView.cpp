#include "pch.h"
#include "OverlayView.h"
#include "math.h"

#include "../LIBSrc/GeoDC/Coord.h"
#include "../LIBSrc/GeoDC/CoordMgr.h"
#include "../inc/includeOverlay.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace Gdiplus;

COverlayView::COverlayView(CWnd* pMainView)
:m_OverlayMain(MAX_LAYER, OBJECT_PER_LAYER)
,m_pNewObject(NULL)
,m_bIsInitialization(FALSE)
{
	m_pMainView				= pMainView;
	m_nPointCount			= 0;
	m_bIsObjectSelect		= FALSE;
	m_bEditLinkedLine		= FALSE;
	m_bIsObjectEditing		= FALSE;
	m_bIsObjectMove			= FALSE;
	m_bAddObject			= FALSE;
	m_bIsLButtonDown		= FALSE;
	m_bIsCtrl				= FALSE;
	m_bIsCopy				= FALSE;
	m_bIsOutLine			= FALSE;
	m_bIsLinkedLine			= FALSE;

	m_eSelectedOutLinePoint	= INVALIDATE_DATA;
	m_nSelectedOutLinePointIndex = INVALID_VALUE;
	m_nLinkedLinePointsCount	= 0;
	m_pSearchedObjectforLinkedLine	= NULL;
	m_pObjectLinkedLine				= NULL;

	Gdiplus::Color	color(255,0,0,0);	
}

COverlayView::~COverlayView()
{
	DeleteObject();
}

//---------------------------------------
// 작성자			: 유영호
// 함수명			: DrawEditObject()
// 목적				: 도형의 편집과정을 화면상에 전시함.
// 생성일			: 2024-10-07
// 최종수정자		:
// 최종수정일		:
//---------------------------------------
void COverlayView::DrawEditObject()
{
	std::map<UINT, UINT>::iterator it;
	m_OverlayMain.ClearEdit();
	OVERLAY::OBJECT* pObject = NULL;
	UINT nObjectID=0;

	if(m_bAddObject)
	{
		m_OverlayMain.EditObject(m_pNewObject);

	}
	else
	{
		m_OverlayMain.ExceptObjectsOnEditLayerClear();		
		for(it=m_SelectedObjectMap.begin(); it!=m_SelectedObjectMap.end(); it++)
		{			
			nObjectID = it->second;
			pObject =m_OverlayMain.SelectedObject(nObjectID);
			if(pObject !=NULL)
			{
				m_OverlayMain.EditObject(pObject);
			//	m_OverlayMain.ExceptObjectsOnEditLayer(nObjectID);//선택된 도형은 편집레이어에서 그리지 않는다.
			}
		}
	}
}

//---------------------------------------
// 작성자			: 유영호
// 함수명			: DrawEditObject()
// 목적				: 도형의 편집과정을 화면상에 전시함.
// 생성일			: 2024-10-07
// 최종수정자		:
// 최종수정일		:
//---------------------------------------
void COverlayView::DrawObjectForLinkedLine()
{		
	m_OverlayMain.ClearEdit();	
	m_OverlayMain.EditObject(m_pSearchedObjectforLinkedLine);
}

//---------------------------------------
// 작성자			: 유영호
// 함수명			: AddObject()
// 목적				: 편집된 도형 추가.
// 생성일			: 2024-10-07
// 최종수정자		:
// 최종수정일		:
//---------------------------------------
UINT COverlayView::AddObject()
{
	UINT nID = 0;
	if(m_pNewObject==NULL)
		return nID;

	if(m_OverlayMain.AddObject(m_pNewObject)==FALSE)
	{
		DeleteObject();		
		return nID;
	}
	m_SelectedObjectMap.clear();
	m_SelectedObjectMap[m_pNewObject->nID] = m_pNewObject->nID;
	MakeGroup(m_pNewObject);

	m_pNewObject = NULL;
	m_bAddObject = FALSE;

	OVERLAY::OBJECT* pSelected_object = NULL;
	if(m_SelectedObjectMap.size()>0)
		nID = m_SelectedObjectMap.begin()->second;
	pSelected_object = m_OverlayMain.SelectedObject(nID);
	if(pSelected_object !=NULL)
		m_bIsObjectSelect	= TRUE;
	else
		m_bIsObjectSelect	= FALSE;

	DrawEditObject();
	return nID;
}

//---------------------------------------
// 작성자			: 유영호
// 함수명			: SetObjectPoints()
// 목적				: 추가된 도형의 점 설정.
// 생성일			: 2024-10-07
// 최종수정자		:
// 최종수정일		:
//---------------------------------------
BOOL COverlayView::SetObjectPoints(UINT nFlags, CPoint point)
{
	BOOL bRet = FALSE;
	if(m_pNewObject==NULL)
		return bRet;
	switch(m_pNewObject->GetType())
	{		
	case OVERLAY::OBJECT_TYPE::ELLIPSE:
	case OVERLAY::OBJECT_TYPE::ROUND_RECT:
	case OVERLAY::OBJECT_TYPE::TRIANGLE:
	case OVERLAY::OBJECT_TYPE::RECT_TYPE:
	case OVERLAY::OBJECT_TYPE::SIX_ANGLE:
	case OVERLAY::OBJECT_TYPE::EIGHT_ANGLE:
	case OVERLAY::OBJECT_TYPE::DIAMOND:
	case OVERLAY::OBJECT_TYPE::PENTAGON:
	case OVERLAY::OBJECT_TYPE::PIE:
	case OVERLAY::OBJECT_TYPE::ARC:
	case OVERLAY::OBJECT_TYPE::LINE:
	case OVERLAY::OBJECT_TYPE::TEXT:
	case OVERLAY::OBJECT_TYPE::IMAGE:
		{			
			if(nFlags == MOUSE_MOVE)
				m_pNewObject->pt[1].SetXY(point.x, point.y);
			else if( nFlags ==LEFT_BUTTON_DOWN)
			{
				m_pNewObject->pt[0].SetXY(point.x, point.y);
				m_pNewObject->pt[1].SetXY(point.x, point.y);
				
				if(m_pNewObject->GetType() == OVERLAY::OBJECT_TYPE::ROUND_RECT)
				{
					m_pNewObject->pt[ROUND_RECT_VERTEX].SetXY(m_pNewObject->pt[0].X(),m_pNewObject->pt[0].Y());					
				}

				if(m_pNewObject->GetType() == OVERLAY::OBJECT_TYPE::TRIANGLE)

				{
					m_pNewObject->pt[ROUND_RECT_VERTEX].SetXY(m_pNewObject->pt[0].X(),m_pNewObject->pt[0].Y());
				}


			}
			else if( nFlags ==LEFT_BUTTON_UP)
				m_pNewObject->pt[1].SetXY(point.x, point.y);			

		}
		bRet = TRUE;
		
		break;
	case OVERLAY::OBJECT_TYPE::OUTLINE:
		bRet = TRUE;
		
		break;
	case OVERLAY::OBJECT_TYPE::POINT_TYPE:
		bRet = TRUE;
		
		break;
	case OVERLAY::OBJECT_TYPE::POLYGON:
	case OVERLAY::OBJECT_TYPE::POLYLINE:
	case OVERLAY::OBJECT_TYPE::CURVE:
		if( nFlags ==LEFT_BUTTON_DOWN)
		{			
			if(m_pNewObject->nPointIndex>=MAX_POINTS)
				break;
			m_pNewObject->pt[m_pNewObject->nPointIndex].SetXY(point.x, point.y);
			m_pNewObject->nPointIndex++;			
			m_nPointCount++;
		}
		else if( nFlags == LEFT_BUTTON_DCLICK )
		{
			if(m_pNewObject->nPointIndex>=MAX_POINTS)
				break;
			// 더블클릭 이벤트는 버튼클릭이벤트와 동시에 발생하기때문에 
			// 버튼클릭 이벤트로 생성된 Point는 무시한다.
			
		}
		else
			return FALSE;

		bRet = TRUE;

		break;
	default:
		bRet = FALSE;
		
		break;
	}

	if(bRet==TRUE)
	{
		DrawEditObject();
	}

	return bRet;
}

void COverlayView::DeleteObject()
{
	if(m_pNewObject==NULL)
		return;

	ObjectEllipse			*pEllipse			= (ObjectEllipse*)m_pNewObject;
	ObjectArc				*pArc				= (ObjectArc*)m_pNewObject;
	ObjectPie				*pPie				= (ObjectPie*)m_pNewObject;
	ObjectTriangle 			*pTriangle			= (ObjectTriangle*)m_pNewObject;
	ObjectDiamond			*pDiamond			= (ObjectDiamond*)m_pNewObject;
	ObjectRect				*pRect				= (ObjectRect*)m_pNewObject;
	ObjectPentagon			*pPentagon			= (ObjectPentagon*)m_pNewObject;
	ObjectRoundRect			*pRoundRect			= (ObjectRoundRect*)m_pNewObject;
	ObjectSixAngle			*pSixAngle			= (ObjectSixAngle*)m_pNewObject;
	ObjectEightAngle		*pEightAngle		= (ObjectEightAngle*)m_pNewObject;
	ObjectPolygon			*pPolygon			= (ObjectPolygon*)m_pNewObject;
	ObjectPolyLine			*pPolyLine			= (ObjectPolyLine*)m_pNewObject;
	ObjectLine				*pLine				= (ObjectLine*)m_pNewObject;
	ObjectText				*pText				= (ObjectText*)m_pNewObject;
	ObjectImage				*pImage				= (ObjectImage*)m_pNewObject;
	ObjectCurve				*pCurve				= (ObjectCurve*)m_pNewObject;
	ObjectLinkedLine		*pLinkedLine		= (ObjectLinkedLine*)m_pNewObject;
	ObjectLinkedMultiLine	*pLinkedMultiLine	= (ObjectLinkedMultiLine*)m_pNewObject;
	ObjectLinkedCurve		*pLinkedCurve		= (ObjectLinkedCurve*)m_pNewObject;
	ObjectLinkedMultiCurve	*pLinkedMultiCurve	= (ObjectLinkedMultiCurve*)m_pNewObject;

	switch(m_pNewObject->GetType())
	{
		case OVERLAY::OBJECT_TYPE::ELLIPSE:				SAFE_DELETE(pEllipse);			break;	
		case OVERLAY::OBJECT_TYPE::ROUND_RECT:			SAFE_DELETE(pRoundRect);		break;
		case OVERLAY::OBJECT_TYPE::TRIANGLE:			SAFE_DELETE(pTriangle);			break;
		case OVERLAY::OBJECT_TYPE::RECT_TYPE:			SAFE_DELETE(pRect);				break;
		case OVERLAY::OBJECT_TYPE::SIX_ANGLE:			SAFE_DELETE(pSixAngle);			break;
		case OVERLAY::OBJECT_TYPE::EIGHT_ANGLE:			SAFE_DELETE(pEightAngle);		break;
		case OVERLAY::OBJECT_TYPE::DIAMOND:				SAFE_DELETE(pDiamond);			break;
		case OVERLAY::OBJECT_TYPE::PENTAGON:			SAFE_DELETE(pPentagon);			break;							
		case OVERLAY::OBJECT_TYPE::PIE:					SAFE_DELETE(pPie);				break;							
		case OVERLAY::OBJECT_TYPE::LINE:				SAFE_DELETE(pLine);				break;
		case OVERLAY::OBJECT_TYPE::IMAGE:				SAFE_DELETE(pImage);			break;
		case OVERLAY::OBJECT_TYPE::ARC:					SAFE_DELETE(pArc);				break;
		case OVERLAY::OBJECT_TYPE::POLYGON:				SAFE_DELETE(pPolygon);			break;
		case OVERLAY::OBJECT_TYPE::POLYLINE:			SAFE_DELETE(pPolyLine);			break;
		case OVERLAY::OBJECT_TYPE::CURVE:				SAFE_DELETE(pCurve);			break;
		case OVERLAY::OBJECT_TYPE::LINKED_LINE:			SAFE_DELETE(pLinkedLine);		break;
		case OVERLAY::OBJECT_TYPE::LINKED_MULTI_LINE:	SAFE_DELETE(pLinkedMultiLine);	break;
		case OVERLAY::OBJECT_TYPE::LINKED_CURVE:		SAFE_DELETE(pLinkedCurve);		break;
		case OVERLAY::OBJECT_TYPE::LINKED_MULTI_CURVE:	SAFE_DELETE(pLinkedMultiCurve);	break;
		case OVERLAY::OBJECT_TYPE::TEXT:				SAFE_DELETE(pText);				break;
		default:										SAFE_DELETE(m_pNewObject);		break;
	}									
	
	m_pNewObject = NULL;
	m_bAddObject = FALSE;
}

void COverlayView::MakeGroup(OVERLAY::OBJECT* pObject)
{
	CObjectGroupInfo* pObjectGroupInfo = m_OverlayMain.GetLayer(0)->GetGroupInfo(pObject->GetGroupID());

	if(pObjectGroupInfo==NULL)
		return;

	for(UINT i=0; i<pObjectGroupInfo->Size(); i++)
	{
		m_SelectedObjectMap[pObjectGroupInfo->GetObjectID(i)] = pObjectGroupInfo->GetObjectID(i);
	}
}

void COverlayView::Initialize(int nlat_degree, int nlat_min, int nlat_sec, int nlng_degree, int nlng_min, int nlng_sec, char chlat/*='N'*/, char chlng/*='E'*/)
{
	//CCoord	coord;
	//coord.SetProjectionPoint(nlat_degree, nlat_min, nlat_sec, nlng_degree, nlng_min, nlng_sec, chlat, chlng);

	// 화면 좌표계 설정
	CRect pClientRect;
	m_pMainView->GetClientRect(pClientRect); 

	
	CCoordMgr* pCoord = OVERLAY::Point::g_pCoord;
	pCoord->InitDisplayParameter(pClientRect); 

	int nWidth, nHeight; 
	nWidth=pCoord->m_nDisplayViewWidth; 
	nHeight=pCoord->m_nDisplayViewHeight;

	int x = GetSystemMetrics(SM_CXSCREEN); 
	int y = GetSystemMetrics(SM_CYSCREEN); 

	m_sizeMem.cx = x;
	m_sizeMem.cy = y;

	CClientDC dc(m_pMainView);
	HDC hDC	= dc.GetSafeHdc();
	Graphics grp(hDC);
	m_OverlayMain.Initialization(grp,x, y);

	m_bIsInitialization = TRUE;	
}

void COverlayView::SetCoordMgr(CCoordMgr* pCoord)
{
	OVERLAY::Point::g_pCoord = pCoord;
}

void COverlayView::Redraw(HDC hDC, int nX, int nY)
{
	m_OverlayMain.Redraw(hDC,nX,nY);
}


COverlayMain* COverlayView::GetOverlay()
{
	return &m_OverlayMain;
}


INT COverlayView::GetLayerCount()
{
	return m_OverlayMain.NumberOfLayer();
}

CLayerStructure* COverlayView::GetLayer(UINT nIdex)
{
	return m_OverlayMain.GetLayer(nIdex);
}
//---------------------------------------
// 작성자			: 유영호
// 함수명			: DrawObjects()
// 목적				: 레이어 추가,삭제,변경이 발생할 경우 최상위레이어(편집가능레이어)를 
//					  제외한 모든 레이어의 도형을 그린다.
// 생성일			: 2024-10-07
// 최종수정자		:
// 최종수정일		:
//---------------------------------------
void COverlayView::DrawObjects()
{	
	m_OverlayMain.DrawObjects();
}
//---------------------------------------
// 작성자			: 유영호
// 함수명			: DrawEditlayer()
// 목적				: 최상위 레이어에 도형을 다시 그린다.
// 생성일			: 2024-10-07
// 최종수정자		:
// 최종수정일		:
//---------------------------------------
void COverlayView::DrawEditlayer()
{
	m_OverlayMain.DrawEditlayer();
}

void COverlayView::ReleaseSelectObject()
{
	m_SelectedObjectMap.clear();
	m_OverlayMain.ClearEdit();
}

OVERLAY::OBJECT* COverlayView::GetSelectedObject()
{
	OVERLAY::OBJECT* pObject = NULL;
	std::map<UINT, UINT>::iterator it;
	UINT nObjectID=0;

	if(m_SelectedObjectMap.size()>=1) //091130 if(m_SelectedObjectMap.size()==1) 에서 변경
	{
		it=m_SelectedObjectMap.begin();
		nObjectID = it->second;
		pObject =m_OverlayMain.SelectedObject(nObjectID);

		return pObject;
	}
	else
		return NULL;
}

void COverlayView::RePosition()
{
	CLayerStructure* pLayer = NULL;
	OVERLAY::OBJECT* pObject = NULL;
	for(int i=0; i<m_OverlayMain.NumberOfLayer(); i++)
	{
		pLayer = m_OverlayMain.GetLayer(i);

		if(pLayer!=NULL)
		{
			pObject = pLayer->get_first_object();
			for(int j=0; j<pLayer->NumberOfObjectInLayer(); j++)
			{
				if(pObject==NULL)
					break;
				for(int k=0; k<pObject->GetPoints(); k++)
				{
					pObject->pt[k].ReSetPoint();					
				}

				for(UINT l=0; l<POINTS_OF_OBJECT_OUTLINE; l++)
				{
					pObject->pt_out_line[l].ReSetPoint();
				}				
				pObject = pLayer->get_next_object();
			}
		}
	}

	m_OverlayMain.DrawObjects();	
	m_OverlayMain.DrawEditlayer();
}
//---------------------------------------
// 작성자			: 유영호
// 함수명			: OnLButtonDown()
// 목적				: 새로운 도형 생성, 생성중인 도형의 점 추가, 도형 선택, 도형 선택 해제
// 생성일			: 2024-10-07
// 최종수정자		:
// 최종수정일		:
//---------------------------------------
void COverlayView::OnLButtonDown(CPoint point)
{
	m_bIsLButtonDown = TRUE;

	if(m_bAddObject)
	{// 새로운 도형 생성.
		m_SelectedObjectMap.clear();

		if(m_pNewObject !=NULL)
		{
			if(	m_pNewObject->GetType() == OVERLAY::LINKED_LINE ||
				m_pNewObject->GetType() == OVERLAY::LINKED_MULTI_LINE ||
				m_pNewObject->GetType() == OVERLAY::LINKED_CURVE ||
				m_pNewObject->GetType() == OVERLAY::LINKED_MULTI_CURVE )
			{// 도형간 연결선 그리기
				SearchPoint(point);
			}
			else
			{
				if(IsMaxPoints()==TRUE)
					return;				

				// 도형을 형성하고 있는 점 추가.
				if(SetObjectPoints(LEFT_BUTTON_DOWN, point))
					return;
				else
					AddObject();// 도형 추가
			}
		}
	}

	// 화면에서 도형 선택.

	if(m_bIsCtrl==FALSE)
	{// Ctrl키가 눌리지 않은 상태		
		if(m_bIsObjectSelect==TRUE)
		{
			if(m_bEditLinkedLine==TRUE)
				SearchPoint(point);// 연결선으로 연결될 도형을 탐색
			else
				EditObject(point);	
		}
	}
	else
	{// 도형 선택 유무 확인
		
		UINT nObjectId = 0;
		OVERLAY::OBJECT* pSelected_object = NULL;
		nObjectId = m_OverlayMain.PickObjectID(point.x, point.y);
		pSelected_object = m_OverlayMain.SelectedObject(nObjectId);
		//TRACE("Selectd Object ID : %d\n",nObjectId);
		if(pSelected_object!=NULL)
		{
			if(	pSelected_object->GetType()==OVERLAY::LINKED_LINE ||
				pSelected_object->GetType()==OVERLAY::LINKED_MULTI_LINE ||
				pSelected_object->GetType()==OVERLAY::LINKED_CURVE ||
				pSelected_object->GetType()==OVERLAY::LINKED_MULTI_CURVE )
				m_pObjectLinkedLine = (ObjectLinkedLine*)pSelected_object;
			else
				m_pObjectLinkedLine = NULL;

			m_bIsObjectSelect = TRUE;
			m_SelectedObjectMap[pSelected_object->nID] = pSelected_object->nID;	
			MakeGroup(pSelected_object);

			DrawEditObject();
		}
		return;
	}

	if(!m_bIsObjectMove&!m_bIsObjectEditing&!m_bIsCtrl&!m_bEditLinkedLine&!m_bAddObject)
	{// 도형 선택 유무 확인
		
		UINT nObjectId = 0;
		OVERLAY::OBJECT* pSelected_object = NULL;
		m_SelectedObjectMap.clear();
		nObjectId = m_OverlayMain.PickObjectID(point.x,point.y);
		pSelected_object = m_OverlayMain.SelectedObject(nObjectId);

		if(pSelected_object!=NULL)
		{
			m_bIsObjectSelect = TRUE;
			m_SelectedObjectMap[pSelected_object->nID] = pSelected_object->nID;
			MakeGroup(pSelected_object);

			if(	pSelected_object->GetType() == OVERLAY::LINKED_LINE ||
				pSelected_object->GetType() == OVERLAY::LINKED_MULTI_LINE ||
				pSelected_object->GetType() == OVERLAY::LINKED_CURVE ||
				pSelected_object->GetType() == OVERLAY::LINKED_MULTI_CURVE )
			{
				m_pObjectLinkedLine = (OVERLAY::ObjectLinkedLine*)pSelected_object;				
			}
			else
				ResetLinkedLine();

			DrawEditObject();
		}
		else
		{
			m_SelectedObjectMap.clear();
			m_OverlayMain.ClearEdit();
			ResetLinkedLine();	
			m_OverlayMain.ExceptObjectsOnEditLayerClear();
		}		
	}
}
//---------------------------------------
// 작성자			: 유영호
// 함수명			: OnLButtonUp()
// 목적				: 도형 추가, 도형 편집
// 생성일			: 2024-10-07
// 최종수정자		:
// 최종수정일		:
//---------------------------------------
void COverlayView::OnLButtonUp(CPoint point)
{
	//1. 도형 편집중?
	//1.1 도형 좌표수정 적용
	m_bIsLButtonDown = FALSE;
	
	if(m_bAddObject)
	{
		if(SetObjectPoints(LEFT_BUTTON_UP, point)==TRUE)
			AddObject();
	}
	else if(m_bIsObjectMove)
	{
		m_bIsObjectMove = FALSE;
		DrawEditlayer();
		DrawEditObject();
	}

	else if(m_bIsObjectEditing)
	{		
		DrawEditlayer();
		DrawEditObject();

		std::map<UINT, UINT>::iterator it;
		OVERLAY::OBJECT* pObject = NULL;
		UINT nObjectID=0;
		for(it=m_SelectedObjectMap.begin(); it!=m_SelectedObjectMap.end(); it++)
		{
			nObjectID = it->second;
			pObject = m_OverlayMain.SelectedObject(nObjectID);
			if(pObject!=NULL)
				pObject->RearrangePoints();
		}
	}	
}
//---------------------------------------
// 작성자			: 유영호
// 함수명			: OnLButtonDblClk()
// 목적				: 도형 추가
// 생성일			: 2024-10-07
// 최종수정자		:
// 최종수정일		:
//---------------------------------------
void COverlayView::OnLButtonDblClk(CPoint point)
{
	//1. 도형 선택,
	//1.1 도형 편집영역 그리기.
	if(m_bAddObject)
	{
		if(SetObjectPoints(LEFT_BUTTON_DCLICK, point) == TRUE)
			AddObject();		

		if(m_bIsLinkedLine==TRUE)
		{
			SelectLinkedLinePoint(point);
		}

	}	
	else if(m_bEditLinkedLine)
	{
		EditLinkedLinePoint(m_nSelectedOutLinePointIndex, point);	
	}
}
void COverlayView::OnRButtonDown(CPoint point)
{
	if(m_bIsObjectSelect)
	{//도형이 선택된상태.
	// 선택된 도형의 ID값 기억
	// 선택된 ID값관 선택할 지점의 도형의 ID값이 동일한지 비교 확인
		// 도일하면 도형 편집 창 전시.
		CString strTmp = _T("");
		CColorDialog Dlg(RGB(0,0,0),CC_FULLOPEN);
		if(Dlg.DoModal() == IDOK)
		{
			COLORREF color = Dlg.GetColor();
			strTmp.Format(_T("RGB(%u, %u, %u)"),
				GetRValue(color),
				GetGValue(color),
				GetBValue(color));

			AfxMessageBox(strTmp);
		}

		///Font Dialog.
		CClientDC dc(m_pMainView);
		LOGFONT lfont;
		ZeroMemory(&lfont,sizeof(LOGFONT));
		lfont.lfHeight = -MulDiv(9, dc.GetDeviceCaps(LOGPIXELSY),72);
		wsprintf(lfont.lfFaceName,_T("%s"),_T("굴림"));

		CFontDialog DlgFont(&lfont);
		if(DlgFont.DoModal() == IDOK)
		{
			strTmp.Format(_T("Font: %s, Size: %d, Style : %s"),
				DlgFont.GetFaceName(),
				DlgFont.GetSize(),
				DlgFont.GetStyleName()
				);
			
			AfxMessageBox(strTmp);
		}
	}
}

void COverlayView::OnRButtonUp(CPoint point)
{
}

void COverlayView::OnRButtonDblClk(CPoint point)
{
}
//---------------------------------------
// 작성자			: 유영호
// 함수명			: OnMouseMove()
// 목적				: 선택된 도형크기,위치 편집
// 생성일			: 2024-10-07
// 최종수정자		:
// 최종수정일		:
//---------------------------------------
void COverlayView::OnMouseMove(CPoint point)
{
	//1. 도형 편집중?
	//1.1 도형 좌표수정
	if(m_bIsLButtonDown)
	{
		CPoint ptMovedGap;
		
		if(m_bAddObject)
		{// 새로운 도형을 그리기 시작함.
			SetObjectPoints(MOUSE_MOVE, point);		
		}
		else if(m_bIsObjectMove)
		{// 기존도형의 위치를 변경함.
			std::map<UINT, UINT>::iterator it;
			OVERLAY::OBJECT* pObject = NULL;
			UINT nObjectID=0;

			ptMovedGap.x = m_ptObjectMoveStart.x - point.x;
			ptMovedGap.y = m_ptObjectMoveStart.y - point.y;

			m_ptObjectMoveStart.x =  point.x;
			m_ptObjectMoveStart.y =  point.y;

			for(it=m_SelectedObjectMap.begin(); it!=m_SelectedObjectMap.end(); it++)
			{// 선택된 도형들의 위치를 변경함.
				nObjectID = it->second;
				pObject = m_OverlayMain.SelectedObject(nObjectID);
				if(pObject!=NULL)
				{
					switch(pObject->GetType())
					{
					case OVERLAY::TRIANGLE:
					case OVERLAY::RECT_TYPE:
					case OVERLAY::DIAMOND:
					case OVERLAY::ELLIPSE:
					case OVERLAY::ROUND_RECT:
					case OVERLAY::PENTAGON:
					case OVERLAY::SIX_ANGLE:
					case OVERLAY::EIGHT_ANGLE:
					case OVERLAY::PIE:
					case OVERLAY::ARC:
					case OVERLAY::CURVE:
					case OVERLAY::POLYGON:
					case OVERLAY::POLYLINE:
					case OVERLAY::LINE:
					case OVERLAY::TEXT:
					case OVERLAY::IMAGE:
						{	
							pObject->Move(ptMovedGap);
						}
						break;

					case OVERLAY::OUTLINE:
						break;

					case OVERLAY::POINT_TYPE:
						break;
					}
				}
			}

			DrawEditObject();
		}
		else if(m_bIsObjectEditing)
		{// 도형 크기 변경

			if(m_SelectedObjectMap.size()>1)
				return;

			std::map<UINT, UINT>::iterator it;
			OVERLAY::OBJECT* pObject = NULL;
			UINT nObjectID=0;

			ptMovedGap.x = m_ptObjectMoveStart.x - point.x;
			ptMovedGap.y = m_ptObjectMoveStart.y - point.y;

			for(it=m_SelectedObjectMap.begin(); it!=m_SelectedObjectMap.end(); it++)
			{
				nObjectID = it->second;
				pObject = m_OverlayMain.SelectedObject(nObjectID);
				if(pObject!=NULL)
				{
					switch(pObject->GetType())					
					{
					case OVERLAY::TRIANGLE:
					case OVERLAY::RECT_TYPE:
					case OVERLAY::DIAMOND:
					case OVERLAY::ELLIPSE:
					case OVERLAY::ROUND_RECT:
					case OVERLAY::PENTAGON:
					case OVERLAY::SIX_ANGLE:
					case OVERLAY::EIGHT_ANGLE:
					case OVERLAY::PIE:
					case OVERLAY::ARC:
					case OVERLAY::TEXT:
					case OVERLAY::IMAGE:
						{	
							if(m_bIsOutLine==TRUE)
							{
								if(m_eSelectedOutLinePoint==ROTATE_CENTER || m_eSelectedOutLinePoint==SWEEP_ANGLE_1 || m_eSelectedOutLinePoint==SWEEP_ANGLE_2)
								{
									//도형 회전
									pObject->Move(point, m_eSelectedOutLinePoint);
								}								
								else
									pObject->Move(point, m_eSelectedOutLinePoint);
									//pObject->Move(point, m_eSelectedOutLinePoint);
							}
							else
							{
								pObject->Move(point, m_eSelectedOutLinePoint);
							}
							DrawEditObject();
						}

						break;					
					case OVERLAY::CURVE:
					case OVERLAY::POLYGON:
					case OVERLAY::POLYLINE:
					case OVERLAY::LINE:
						{	
							if(m_nSelectedOutLinePointIndex>=0)
								pObject->Move(point,m_eSelectedOutLinePoint,m_nSelectedOutLinePointIndex);
						}

						break;
					case OVERLAY::LINKED_LINE:
					case OVERLAY::LINKED_MULTI_LINE:
					case OVERLAY::LINKED_CURVE:
					case OVERLAY::LINKED_MULTI_CURVE:
						{	
							m_ptCurrentMouse.SetXY(point.x,point.y);
							m_pObjectLinkedLine	= (ObjectLinkedLine*)pObject;
							if(m_nSelectedOutLinePointIndex==0 || m_nSelectedOutLinePointIndex==pObject->GetPoints()-1)
							{
								m_bEditLinkedLine = TRUE;
								m_bIsObjectEditing= FALSE;

								m_pObjectLinkedLine->SetLinkPoint(m_nSelectedOutLinePointIndex, &m_ptCurrentMouse);
								TemporaryDrawLinkedLine();
							}
							else
							{
								pObject->Move(point,GENERAL_POINTS,m_nSelectedOutLinePointIndex);
								TemporaryDrawLinkedLine();
							}
								
						}

						break;					
					case OVERLAY::OUTLINE:

						break;
					case OVERLAY::POINT_TYPE:
						break;
					}
				}
			}

			m_ptObjectMoveStart.x =  point.x;
			m_ptObjectMoveStart.y =  point.y;

			DrawEditObject();
		}
		else if(m_bIsObjectSelect)
		{		
			if(m_bEditLinkedLine==TRUE)
			{
				m_ptCurrentMouse.SetXY(point.x,point.y);
				TemporaryDrawLinkedLine();
			}
		}
	}
	else	
	{
		if(m_pObjectLinkedLine!=NULL)
		{//도형 연결선.			
			if(m_bAddObject==TRUE)
			{			
				if(m_nLinkedLinePointsCount>0)
				{
					if(m_nLinkedLinePointsCount<(m_pObjectLinkedLine->GetPoints()-1))
					{
						m_pObjectLinkedLine->SetPoint(m_nLinkedLinePointsCount,point.x,point.y);
					}
					else
					{
						m_pObjectLinkedLine->SetLinkPoint(m_nLinkedLinePointsCount,&m_ptCurrentMouse);
					}
				}
			}
			else if(m_bEditLinkedLine==TRUE) 
			{
				m_pObjectLinkedLine->SetLinkPoint(m_nSelectedOutLinePointIndex,&m_ptCurrentMouse);
			}			

			m_ptCurrentMouse.SetXY(point.x,point.y);
			TemporaryDrawLinkedLine();
		}
	}	
}

//---------------------------------------
// 작성자			: 유영호
// 함수명			: OnKeyDown()
// 목적				: 새로 추가될 도형 선택
// 생성일			: 2024-10-07
// 최종수정자		:
// 최종수정일		:
//---------------------------------------
void COverlayView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch(nChar)
	{
	
	case 17:
		m_bIsCtrl		= TRUE;

		break;
	case 'c':
	case 'C':
		if(m_bIsCtrl && m_bIsObjectSelect)
		{
			m_bIsCopy = TRUE;
			m_bIsCtrl = FALSE;
		}

		break;
	case 'v':
	case 'V':
		if(m_bIsCopy)
		{
			AddObject();
			m_bIsCopy = FALSE;
		}

		break;
	case 46:// DELL
		if(m_bAddObject==TRUE)
		{
			if(m_pNewObject!=NULL)
			{
				delete m_pNewObject;
				m_pNewObject= NULL;
			}
			m_bAddObject = FALSE;
			m_SelectedObjectMap.clear();
			m_OverlayMain.ClearEdit();			
			ResetLinkedLine();
			break;
		}
		else if(m_SelectedObjectMap.size()>0)
		{
			UINT nObjectID=0;
			std::map<UINT,UINT>::iterator it;
			for(it=m_SelectedObjectMap.begin(); it!=m_SelectedObjectMap.end(); it++)
			{
				nObjectID = it->second;
				m_OverlayMain.DeleteObject(nObjectID);
			}
			m_SelectedObjectMap.clear();
			m_OverlayMain.ClearEdit();
			DrawObjects();
			ResetLinkedLine();
		}

		break;
	case 27:
		m_OverlayMain.ClearEdit();
		DrawObjects();
		ResetLinkedLine();

		break;
	default:
		break;
	}
}

void COverlayView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch(nChar)
	{
	case 17:
		m_bIsCtrl = FALSE;
		break;
	default:
		break;
	}

}

void COverlayView::OnCreateObject(OVERLAY::OBJECT_TYPE eObjectType, char* strFileName, int nStrLen)
{
	UINT temp = m_nPointCount;
	m_nPointCount=0;	
	m_nLinkedLinePointsCount = 0;
	m_bIsLinkedLine = FALSE;
	
	ObjectEllipse			*pEllipse			= NULL;
	ObjectArc				*pArc				= NULL;
	ObjectPie				*pPie				= NULL;
	ObjectTriangle 			*pTriangle			= NULL;
	ObjectDiamond			*pDiamond			= NULL;
	ObjectRect				*pRect				= NULL;
	ObjectPentagon			*pPentagon			= NULL;
	ObjectRoundRect			*pRoundRect			= NULL;
	ObjectSixAngle			*pSixAngle			= NULL;
	ObjectEightAngle		*pEightAngle		= NULL;
	ObjectPolygon			*pPolygon			= NULL;
	ObjectPolyLine			*pPolyLine			= NULL;
	ObjectLine				*pLine				= NULL;
	ObjectText				*pText				= NULL;
	ObjectImage				*pImage				= NULL;
	ObjectCurve				*pCurve				= NULL;
	ObjectLinkedLine		*pLinkedLine		= NULL;
	ObjectLinkedMultiLine	*pLinkedMultiLine	= NULL;
	ObjectLinkedCurve		*pLinkedCurve		= NULL;
	ObjectLinkedMultiCurve	*pLinkedMultiCurve	= NULL;
		
	if(m_pNewObject!=NULL)
	{ 
		DeleteObject();
	}
 
	CLayerStructure* pLayer = m_OverlayMain.GetLayer(0);

	if(pLayer==NULL)
	{			
		m_OverlayMain.AddLayer("DefaultLayer",strlen("DefaultLayer"));	
		pLayer = m_OverlayMain.GetLayer(0);
		if(pLayer==NULL)
			return;
	}

	if(pLayer->NumberOfObjectInLayer()>=OBJECT_PER_LAYER)
	{
		AfxMessageBox(_T("현재 레이어에 더이상 도형을 추가할 수 없습니다."));
		return;
	}

	switch(eObjectType)
	{
	case OVERLAY::ELLIPSE:
		pEllipse		= new ObjectEllipse();
		if(pEllipse==NULL) break;
		m_pNewObject	= (OBJECT*)pEllipse;
		m_bAddObject	= TRUE;

		break;
	case OVERLAY::RECT_TYPE:
		pRect			= new ObjectRect();
		if(pRect==NULL) break;
		m_pNewObject	= (OBJECT*)pRect;
		m_bAddObject	= TRUE;

		break;
	case OVERLAY::ROUND_RECT:
		pRoundRect		= new ObjectRoundRect();
		if(pRoundRect==NULL) break;
		m_pNewObject	= (OBJECT*)pRoundRect;
		m_bAddObject	= TRUE;

		break;
	case OVERLAY::TRIANGLE:
		pTriangle		= new ObjectTriangle();
		if(pTriangle==NULL) break;
		m_pNewObject	= (OBJECT*)pTriangle;
		m_bAddObject = TRUE;

		break;
	case OVERLAY::PENTAGON:
		pPentagon		= new ObjectPentagon();
		if(pPentagon==NULL) break;
		m_pNewObject	= (OBJECT*)pPentagon;
		m_bAddObject	= TRUE;

		break;
	case OVERLAY::SIX_ANGLE:
		pSixAngle		= new ObjectSixAngle();
		if(pSixAngle==NULL) break;
		m_pNewObject	= (OBJECT*)pSixAngle;
		m_bAddObject	= TRUE;

		break;
	case OVERLAY::EIGHT_ANGLE:
		pEightAngle		= new ObjectEightAngle();
		if(pEightAngle==NULL) break;
		m_pNewObject	= (OBJECT*)pEightAngle;
		m_bAddObject	= TRUE;

		break;
	case OVERLAY::CURVE:
		pCurve			= new ObjectCurve();
		if(pCurve==NULL) break;
		m_pNewObject	= (OBJECT*)pCurve;
		m_bAddObject	= TRUE;

		break;
	case OVERLAY::DIAMOND:
		pDiamond		= new ObjectDiamond();
		if(pDiamond==NULL) break;
		m_pNewObject	= (OBJECT*)pDiamond;
		m_bAddObject	= TRUE;

		break;
	case OVERLAY::LINE:
		pLine			= new ObjectLine();
		if(pLine==NULL) break;
		m_pNewObject	= (OBJECT*)pLine;
		m_bAddObject	= TRUE;

		break;
	case OVERLAY::POLYGON:
		pPolygon		= new ObjectPolygon();
		if(pPolygon==NULL) break;
		m_pNewObject	= (OBJECT*)pPolygon;
		m_bAddObject	= TRUE;

		break;
	case OVERLAY::POLYLINE:
		pPolyLine		= new ObjectPolyLine();
		if(pPolyLine==NULL) break;
		m_pNewObject	= (OBJECT*)pPolyLine;
		m_bAddObject	= TRUE;

		break;
	case OVERLAY::ARC:
		pArc			= new ObjectArc();
		if(pArc==NULL) break;
		m_pNewObject	= (OBJECT*)pArc;
		m_bAddObject	= TRUE;		
		
		break;
	case OVERLAY::TEXT:
		pText			= new ObjectText();
		if(pText==NULL) break;
		m_pNewObject	= (OBJECT*)pText;
		m_bAddObject	= TRUE;
	
		break;
	case OVERLAY::PIE:
		pPie			= new ObjectPie();
		if(pPie==NULL) break;
		m_pNewObject	= (OBJECT*)pPie;
		m_bAddObject	= TRUE;

		break;
	case OVERLAY::IMAGE:
		pImage			= new ObjectImage(strFileName, nStrLen);
		if(pImage==NULL) break;
		m_pNewObject	= (OBJECT*)pImage;
		m_bAddObject	= TRUE;

		break;
	case OVERLAY::LINKED_LINE:
		pLinkedLine			= new ObjectLinkedLine(2);
		if(pLinkedLine==NULL) break;
		m_pNewObject		= (OBJECT*)pLinkedLine;
		m_pObjectLinkedLine = (ObjectLinkedLine*)pLinkedLine;
		m_bAddObject		= TRUE;
		m_bIsLinkedLine		= TRUE;

		break;
	case OVERLAY::LINKED_MULTI_LINE:
		pLinkedMultiLine	= new ObjectLinkedMultiLine(OBJECT_PER_LAYER);
		if(pLinkedMultiLine==NULL) break;
		m_pNewObject		= (OBJECT*)pLinkedMultiLine;
		m_pObjectLinkedLine = (ObjectLinkedLine*)pLinkedMultiLine;
		m_bAddObject		= TRUE;
		m_bIsLinkedLine		= TRUE;

		break;
	case OVERLAY::LINKED_CURVE:
		pLinkedCurve		= new ObjectLinkedCurve(2);
		if(pLinkedCurve==NULL) break;
		m_pNewObject		= (OBJECT*)pLinkedCurve;
		m_pObjectLinkedLine = (ObjectLinkedLine*)pLinkedCurve;
		m_bAddObject		= TRUE;
		m_bIsLinkedLine		= TRUE;

		break;
	case OVERLAY::LINKED_MULTI_CURVE:
		pLinkedMultiCurve	= new ObjectLinkedMultiCurve(3);
		if(pLinkedMultiCurve==NULL) break;
		m_pNewObject		= (OBJECT*)pLinkedMultiCurve;
		m_pObjectLinkedLine = (ObjectLinkedLine*)pLinkedMultiCurve;
		m_bAddObject		= TRUE;
		m_bIsLinkedLine		= TRUE;

		break;	
	default:
		break;
	}

}

BOOL COverlayView::Save(char* strFileName, int nStrLen, UINT nLayerIndex)
{
	return m_OverlayMain.Save(strFileName, nStrLen, nLayerIndex);
}

BOOL COverlayView::Load(char* strFileName, int nStrLen)
{
	return m_OverlayMain.Load(strFileName, nStrLen);
}

void COverlayView::SetGroup()
{
	std::map<UINT,UINT>::iterator it = m_SelectedObjectMap.begin();

	UINT nObjectId = 0;
	UINT nGroupId = 0;
	OVERLAY::OBJECT* pObject = NULL;
	CObjectGroupInfo *pObjectGroupInfo = NULL;

	pObjectGroupInfo = new CObjectGroupInfo;

	if(pObjectGroupInfo==NULL)
		return;

	if(m_SelectedObjectMap.size()<1)
		return;

	m_OverlayMain.GetLayer(0)->AddGroup(nGroupId, pObjectGroupInfo);

	for(;it!=m_SelectedObjectMap.end(); it++)
	{
		nObjectId = it->second;
		pObject = m_OverlayMain.SelectedObject(nObjectId);
		if(pObject!=NULL)
		{
			pObjectGroupInfo->AddObjectInGroup(nObjectId);		
			pObject->SetGroup(nGroupId);
		}

	}
}

void COverlayView::ReleaseGroup()
{
	std::map<UINT,UINT>::iterator it = m_SelectedObjectMap.begin();

	UINT nObjectId = 0;
	UINT nGroupId = 0;
	OVERLAY::OBJECT* pObject = NULL;

	if(m_SelectedObjectMap.size()<=1)
		return;

	for(;it!=m_SelectedObjectMap.end(); it++)
	{
		nObjectId = it->second;
		pObject = m_OverlayMain.SelectedObject(nObjectId);
		if(pObject!=NULL)
		{
			m_OverlayMain.GetLayer(0)->ReleaseGroup(pObject->GetGroupID());
			pObject->ReleaseGroup();
		}

	}
}

BOOL COverlayView::IsMaxPoints()
{	
	SHORT nID = 0;
	if(m_nPointCount>=MAX_POINTS)
	{// 도형을 형성하고 있는 점의 개수가 정의된 도형의 최대점의 개수보다 많을경우
		// 도형 편집을 중단하고 현재까지의 점을 이용해 새로운 도형을 생성함.
		m_nPointCount = 0;		
		// 도형추가
		AddObject();
		
		return TRUE;
	}
	return FALSE;
}

void COverlayView::EditObject(CPoint point)
{
	// 도형이 선택된 상태			
	//도형의 편집 지점 선택.
	SHORT			nSelected_point_id;
	nSelected_point_id = m_OverlayMain.PickSelectEditPoint(point.x, point.y);

	if(nSelected_point_id>=20)
		TRACE("----nSelected_point_id : %d\n",nSelected_point_id);
	if(nSelected_point_id>OUT_LINE_SELECT && nSelected_point_id!=INVALID_OBJECT_ID)
	{// 도형을 구성하고 있는 점이 선택됨.(PolyLine,Polygon..)
		m_bIsOutLine		= FALSE;
		
		if(nSelected_point_id>=GENERAL_POINTS)
		{
			m_eSelectedOutLinePoint = GENERAL_POINTS;
			m_nSelectedOutLinePointIndex = nSelected_point_id - GENERAL_POINTS;
		}
		else
		{
			m_eSelectedOutLinePoint = (OUT_LINE)nSelected_point_id;
			m_nSelectedOutLinePointIndex = nSelected_point_id;
		}

		if(m_nSelectedOutLinePointIndex<=MAX_POINTS)
		{
			m_bIsObjectMove		= FALSE;
			m_bIsObjectEditing	= TRUE;
		}
		else
		{
			m_bIsObjectEditing	= FALSE;
			m_bIsObjectSelect	= FALSE;
		}
	}
	else if( nSelected_point_id>=LEFT_TOP && nSelected_point_id<=OUT_LINE_SELECT)
	{// 도형의 편집 영역이 선택됨.				
		m_bIsOutLine		= TRUE;
		m_eSelectedOutLinePoint = (OUT_LINE)(nSelected_point_id);

		if(m_eSelectedOutLinePoint>=0 && m_eSelectedOutLinePoint<INVALIDATE_DATA)
		{// 도형의 편집영역 선택	
			m_ptObjectMoveStart	= point;

			if(m_eSelectedOutLinePoint==OUT_LINE_SELECT)
			{
				m_bIsObjectMove		= TRUE;
				m_bIsObjectEditing	= FALSE;
			}
			else
			{
				m_bIsObjectEditing	= TRUE;
				m_bIsObjectMove		= FALSE;
				//=========================================
				// 도형 편집 시작시 도형의 외곽선 위치 저장.
				// 단일 도형 선택시 편집 가능.
				if(m_SelectedObjectMap.size()==1)
				{
					std::map<UINT, UINT>::iterator it = m_SelectedObjectMap.begin();
					UINT nObjectID = it->second;
					OVERLAY::OBJECT* pObject = m_OverlayMain.SelectedObject(nObjectID);
					if(pObject!=NULL)
					{
						pObject->SetOldOutLinePosition();
					}
				}
				
			}
		}
		else
		{
			m_bIsObjectEditing	= FALSE;
			m_bIsObjectSelect	= FALSE;
		}
	}
	else
	{				
		m_bIsObjectEditing	= FALSE;
		m_bIsObjectSelect	= FALSE;
		m_bIsObjectMove		= FALSE;
	}	
}

// 도형간 연결선에대해 임시로 화면에 도시한다.
void COverlayView::TemporaryDrawLinkedLine()
{
	if(m_pNewObject!=NULL)
	{
		m_pObjectLinkedLine = (ObjectLinkedLine*)m_pNewObject;
	}
	//else
	//	m_pObjectLinkedLine = NULL;

	if(m_pObjectLinkedLine==NULL)
		return;

	//if(m_nLinkedLinePointsCount==0)
//		return;		

	std::map<UINT, UINT>::iterator it;
	m_OverlayMain.ClearEdit();
	OVERLAY::OBJECT* pObject2 = NULL;
	UINT nObjectID=0;

	for(it=m_SelectedObjectMap.begin(); it!=m_SelectedObjectMap.end(); it++)
	{
		nObjectID = it->second;
		pObject2 =m_OverlayMain.SelectedObject(nObjectID);
		if(pObject2 !=NULL)
			m_OverlayMain.EditObject(pObject2);
	}
	m_OverlayMain.EditObject(m_pObjectLinkedLine);
}

// 마우스로 클릭한 지점의 도형을 찾는다.
void COverlayView::SearchPoint(CPoint point)
{
	UINT nObjectId = 0;
	
	if(m_pObjectLinkedLine==NULL)
		return;

	if(m_nLinkedLinePointsCount>=m_pObjectLinkedLine->GetPoints())
		return;

	nObjectId = m_OverlayMain.PickObjectID(point.x, point.y);
	m_pSearchedObjectforLinkedLine = m_OverlayMain.SelectedObject(nObjectId);
	m_SelectedObjectMap.clear();
	
	if(m_pSearchedObjectforLinkedLine!=NULL)
	{
		m_SelectedObjectMap[m_pSearchedObjectforLinkedLine->nID] = m_pSearchedObjectforLinkedLine->nID;				
		MakeGroup(m_pSearchedObjectforLinkedLine);

		DrawObjectForLinkedLine();
	}

	MoveToTop();
	//m_bFindLinkedLineObject = TRUE;
}

void COverlayView::MoveToTop()
{
	CLayerStructure* pLayer = GetLayer(0);
	OVERLAY::OBJECT* pObject = NULL;
	OVERLAY::OBJECT* pObjectTemp = NULL;
	if(pLayer!=0 )
	{
		pObject = GetSelectedObject();

		int nIndex=0;
		if(pObject!=NULL)
		{
			pObjectTemp = pLayer->get_first_object();

			for(int i=0; i<pLayer->NumberOfObjectInLayer(); i++)
			{
				if(pObjectTemp==NULL)
					break;
				if(pObject->nID==pObjectTemp->nID)
				{
					pLayer->MoveToBottom(nIndex);//마지막에 그려지는 도형이 최상위에 위치하기 때문에...
					m_OverlayMain.DrawObjectInEditLayer(pObject);
					//DrawEditlayer();
					break;
				}
				pObjectTemp = pLayer->get_next_object();				
				nIndex++;
			}
		}
	}
}

void COverlayView::MoveToBottom()
{
	CLayerStructure* pLayer = GetLayer(0);
	OVERLAY::OBJECT* pObject = NULL;
	OVERLAY::OBJECT* pObjectTemp = NULL;
	if(pLayer!=0 )
	{
		pObject = GetSelectedObject();

		int nIndex=0;
		if(pObject!=NULL)
		{
			pObjectTemp = pLayer->get_first_object();

			for(int i=0; i<pLayer->NumberOfObjectInLayer(); i++)
			{
				if(pObjectTemp==NULL)
					break;
				if(pObject->nID==pObjectTemp->nID)
				{
					pLayer->MoveToTop(nIndex);//마지막에 그려지는 도형이 최상위에 위치하기 때문에...
				//	DrawEditlayer();
					break;
				}
				pObjectTemp = pLayer->get_next_object();
				nIndex++;
			}
		}
	}
}
// 찾은 도형에서 도형의 연결선이 연결될 점을 선택한다.
void COverlayView::SelectLinkedLinePoint(CPoint point)
{
	if(m_pObjectLinkedLine==NULL)
		return;
	if(m_nLinkedLinePointsCount>=m_pObjectLinkedLine->GetPoints())
		return;
	if(m_SelectedObjectMap.size()<1 && m_pObjectLinkedLine->GetType()==OVERLAY::LINKED_LINE)
		return;

	m_pSearchedObjectforLinkedLine = NULL;
	SHORT nObjectPointId = 0;
	
	// 선택된 도형의 ID
	UINT nObjectID = m_SelectedObjectMap.begin()->second;

	// 도형에서 연결선과 이어질 지점 ID.
	nObjectPointId = m_OverlayMain.PickSelectEditPoint(point.x, point.y);
	
	if(nObjectPointId==OVERLAY::OUT_LINE_SELECT)
		return;

	TRACE("--------Object ID : %d Point ID:%d --------- \n",nObjectID,nObjectPointId);
	OVERLAY::OBJECT* pObject = m_OverlayMain.SelectedObject(nObjectID);
	if(pObject!=NULL)
	{
		if(	pObject->GetType()==OVERLAY::LINKED_MULTI_LINE ||
			pObject->GetType()==OVERLAY::LINKED_CURVE ||
			pObject->GetType()==OVERLAY::LINKED_MULTI_CURVE )
		{
			return;
			OVERLAY::ObjectLinkedLine* pObjectLinkedLine = (OVERLAY::ObjectLinkedLine*)pObject;			
			if( nObjectPointId==GENERAL_POINTS)
			{
				nObjectID = pObjectLinkedLine->nLinkedLine_Start_ID;
				nObjectPointId = pObjectLinkedLine->nObject_start_point_id;
				m_pSearchedObjectforLinkedLine = m_OverlayMain.SelectedObject(nObjectID);
			}
			else if( nObjectPointId==GENERAL_POINTS+2)
			{
				nObjectID = pObjectLinkedLine->nLinkedLine_End_ID;
				nObjectPointId = pObjectLinkedLine->nObject_end_point_id;
				m_pObjectLinkedLine->AddPoints();			
				m_pSearchedObjectforLinkedLine = m_OverlayMain.SelectedObject(nObjectID);
			}
			else if( nObjectPointId==GENERAL_POINTS+1)
			{
				nObjectID = pObjectLinkedLine->nID;
				//nObjectPointId = pObjectLinkedLine->nObject_end_point_id;
				m_pObjectLinkedLine->AddPoints();
				m_pSearchedObjectforLinkedLine = m_OverlayMain.SelectedObject(nObjectID);
			}
		}
		else if( pObject->GetType()==OVERLAY::LINKED_LINE )
		{
			OVERLAY::ObjectLinkedLine* pObjectLinkedLine = (OVERLAY::ObjectLinkedLine*)pObject;			
			if( nObjectPointId==GENERAL_POINTS)
			{
				nObjectID = pObjectLinkedLine->nLinkedLine_Start_ID;
				nObjectPointId = pObjectLinkedLine->nObject_start_point_id;
				m_pObjectLinkedLine->AddPoints();
				m_pSearchedObjectforLinkedLine = m_OverlayMain.SelectedObject(nObjectID);
			}
			else if( nObjectPointId==GENERAL_POINTS+1)
			{
				nObjectID = pObjectLinkedLine->nLinkedLine_End_ID;
				nObjectPointId = pObjectLinkedLine->nObject_end_point_id;
				m_pObjectLinkedLine->AddPoints();
				m_pSearchedObjectforLinkedLine = m_OverlayMain.SelectedObject(nObjectID);
			}
		}
		else
		{
			m_pObjectLinkedLine->AddPoints();
			if(m_pObjectLinkedLine->GetType()==OVERLAY::LINKED_MULTI_LINE && m_pObjectLinkedLine->GetAddedPointsNum()>1)
			{				
				// 끝점의 Object ID 및 point ID 저장
				m_pObjectLinkedLine->SetPoints(m_pObjectLinkedLine->GetAddedPointsNum());
				m_pObjectLinkedLine->nLinkedLine_End_ID = nObjectID;
				m_pObjectLinkedLine->nObject_end_point_id = nObjectPointId;
			}
			
			m_pSearchedObjectforLinkedLine = m_OverlayMain.SelectedObject(nObjectID);	
		}
	}

	if(m_nLinkedLinePointsCount==0)
	{// 시작점의 Object ID 및 point ID 저장
		if( nObjectPointId == INVALID_OBJECT_ID )
			return;	
		m_pObjectLinkedLine->nLinkedLine_Start_ID = nObjectID;
		m_pObjectLinkedLine->nObject_start_point_id = nObjectPointId;	
	}
	else if(m_nLinkedLinePointsCount==m_pObjectLinkedLine->GetPoints()-1)
	{// 끝점의 Object ID 및 point ID 저장
		if( nObjectPointId == INVALID_OBJECT_ID )
			return;	
		m_pObjectLinkedLine->nLinkedLine_End_ID = nObjectID;
		m_pObjectLinkedLine->nObject_end_point_id = nObjectPointId;
	}
	else
	{
		if(m_nLinkedLinePointsCount>=m_pObjectLinkedLine->GetPoints())
		{
			m_nLinkedLinePointsCount--;
			
			return;
		}
		m_pObjectLinkedLine->SetPoint(m_nLinkedLinePointsCount ,point.x, point.y);
		m_nLinkedLinePointsCount++;
		m_pObjectLinkedLine->AddPoints();
		return;
	}

	if(nObjectPointId<MAX_OUT_LINE_POINT )
	{		
		if(m_pSearchedObjectforLinkedLine==NULL)
			return;
		m_pObjectLinkedLine->SetLinkPoint(m_nLinkedLinePointsCount, &m_pSearchedObjectforLinkedLine->pt_out_line[nObjectPointId]);
	}
	else
	{
		if(m_pSearchedObjectforLinkedLine==NULL)
			return;
		if(m_pSearchedObjectforLinkedLine->GetPoints()>(int)(nObjectPointId-GENERAL_POINTS))
		{
			m_pObjectLinkedLine->SetLinkPoint(m_nLinkedLinePointsCount, &m_pSearchedObjectforLinkedLine->pt[nObjectPointId-GENERAL_POINTS]);
		}
		else
			return;
	}

	m_nLinkedLinePointsCount++;

	if(m_nLinkedLinePointsCount>=m_pObjectLinkedLine->GetPoints())
	{
		if(m_bAddObject==TRUE)
		{
			AddObject();
			AddLinkedLineIdInObjects(m_pObjectLinkedLine);

			ResetLinkedLine();
			DrawEditlayer();
			ResetLinkedLine();
		}
	}
}

// 찾은 도형에서 도형의 연결선이 연결될 점을 선택한다.
void COverlayView::EditLinkedLinePoint(int nSelectedLinkedLinePointID, CPoint point)
{	
	if(m_pObjectLinkedLine==NULL)
		return;

	if(m_SelectedObjectMap.size()!=1)
		return;

	SHORT nObjectPointId = 0;	
	// 선택된 도형의 ID
	UINT nObjectID = m_SelectedObjectMap.begin()->second;

	OVERLAY::OBJECT* pObject = m_OverlayMain.SelectedObject(nObjectID);
	OVERLAY::OBJECT* pReleaseLinkedLineIdInObjects = NULL;
	if(pObject==NULL)
		return;

	// 도형에서 연결선과 이어질 지점 ID.
	nObjectPointId = m_OverlayMain.PickSelectEditPoint(point.x, point.y);

	TRACE("nSelectedLinkedLinePointID: %d--------Object ID : %d Point ID:%d --------- \n",nSelectedLinkedLinePointID,nObjectID,nObjectPointId);
	if(nObjectPointId>=MAX_OUT_LINE_POINT && nObjectPointId<GENERAL_POINTS)
	{
		return;	
	}

	//--------------------------------------------------------
	// 연결선 과 연결된 도형에서 현제 연결선 정보를 제거한다.
	if(nSelectedLinkedLinePointID==0)
	{
		pReleaseLinkedLineIdInObjects = m_OverlayMain.SelectedObject(m_pObjectLinkedLine->nLinkedLine_Start_ID );
		m_pObjectLinkedLine->nLinkedLine_Start_ID = nObjectID;
		m_pObjectLinkedLine->nObject_start_point_id = nObjectPointId;	
	}
	else if( nSelectedLinkedLinePointID == m_pObjectLinkedLine->GetPoints()-1 )
	{
		pReleaseLinkedLineIdInObjects = m_OverlayMain.SelectedObject(m_pObjectLinkedLine->nLinkedLine_End_ID );
		m_pObjectLinkedLine->nLinkedLine_End_ID = nObjectID;
		m_pObjectLinkedLine->nObject_end_point_id = nObjectPointId;	
	}
	if(pReleaseLinkedLineIdInObjects!=NULL)
		pReleaseLinkedLineIdInObjects->ObjectLinkedLineProperty.RemoveObjectID(m_pObjectLinkedLine->nID);
	/////////////////////////////////////////////////////////


	// 연결될 도형의 종류가 연결선일 경우.
	// 연결선과 연결된 도형으로부터 연결선정보를 제거한다.
	// 
	m_pSearchedObjectforLinkedLine = NULL;
	if(	pObject->GetType()==OVERLAY::LINKED_MULTI_LINE ||
		pObject->GetType()==OVERLAY::LINKED_CURVE ||
		pObject->GetType()==OVERLAY::LINKED_MULTI_CURVE )
	{
		OVERLAY::ObjectLinkedLine* pObjectLinkedLine = (OVERLAY::ObjectLinkedLine*)pObject;			
		if( nObjectPointId==GENERAL_POINTS)
		{
			nObjectID = pObjectLinkedLine->nLinkedLine_Start_ID;
			nObjectPointId = pObjectLinkedLine->nObject_start_point_id;
			pObject = m_OverlayMain.SelectedObject(nObjectID);
		}
		else if( nObjectPointId==GENERAL_POINTS+2)
		{
			nObjectID = pObjectLinkedLine->nLinkedLine_End_ID;
			nObjectPointId = pObjectLinkedLine->nObject_end_point_id;
			pObject = m_OverlayMain.SelectedObject(nObjectID);
		}
		else
		{// 다중 연결선의 중간선일경우 도형과 직접 연결하고 연결선 정보를 저장한다.			
			m_pObjectLinkedLine->SetLinkPoint(nSelectedLinkedLinePointID, &pObject->pt[nObjectPointId-GENERAL_POINTS]);
			m_pObjectLinkedLine->ObjectLinkedLineProperty.AddObjectID(pObject->nID);
			// 선택된 도형에 연결선 정보 추가
			pObject->ObjectLinkedLineProperty.AddObjectID(m_pObjectLinkedLine->nID);

			DrawEditlayer();
			ResetLinkedLine();
			return;
		}
	}
	else if( pObject->GetType()==OVERLAY::LINKED_LINE )
	{
		OVERLAY::ObjectLinkedLine* pObjectLinkedLine = (OVERLAY::ObjectLinkedLine*)pObject;			
		if( nObjectPointId==GENERAL_POINTS)
		{
			nObjectID = pObjectLinkedLine->nLinkedLine_Start_ID;
			nObjectPointId = pObjectLinkedLine->nObject_start_point_id;
			pObject = m_OverlayMain.SelectedObject(nObjectID);
		}
		else if( nObjectPointId==GENERAL_POINTS+1)
		{
			nObjectID = pObjectLinkedLine->nLinkedLine_End_ID;
			nObjectPointId = pObjectLinkedLine->nObject_end_point_id;
			pObject = m_OverlayMain.SelectedObject(nObjectID);
		}

		m_pObjectLinkedLine->SetLinkPoint(nSelectedLinkedLinePointID, &pObject->pt[nObjectPointId-GENERAL_POINTS]);
		pObject->ObjectLinkedLineProperty.AddObjectID(m_pObjectLinkedLine->nID);
	}
	else if(pObject->GetType()==OVERLAY::POLYGON ||
		pObject->GetType()==OVERLAY::POLYLINE ||
		pObject->GetType()==OVERLAY::CURVE  ||
		pObject->GetType()==OVERLAY::LINE)
	{
		if(nObjectPointId<GENERAL_POINTS)
			return;
		m_pObjectLinkedLine->SetLinkPoint(nSelectedLinkedLinePointID, &pObject->pt[nObjectPointId-GENERAL_POINTS]);
		pObject->ObjectLinkedLineProperty.AddObjectID(m_pObjectLinkedLine->nID);
	}
	else
	{
		if(nObjectPointId>=GENERAL_POINTS)
			return;
		m_pObjectLinkedLine->SetLinkPoint(nSelectedLinkedLinePointID, &pObject->pt_out_line[nObjectPointId]);
		// 선택된 도형에 연결선 정보 추가
		pObject->ObjectLinkedLineProperty.AddObjectID(m_pObjectLinkedLine->nID);

	}

	/*
	if(nObjectPointId>=GENERAL_POINTS)
	{	
	}
	else if(nObjectPointId<OUT_LINE_SELECT)
	{	
	}
	*/	
	DrawEditlayer();
	ResetLinkedLine();
}

// 연결선을 완성하면 임시로 연결지점의 포인터로 활용하던걸 다시 초기화 한다.
void COverlayView::ResetLinkedLine()
{
	for(int i=0; i<DEFAULT_LINKED_OBJECT; i++)
		m_ptLinkedLine[i] = NULL;
	
	m_nLinkedLinePointsCount	= 0;
	m_pObjectLinkedLine			= NULL;
	m_bEditLinkedLine			= FALSE;
	m_bIsLinkedLine				= FALSE;
}

// 도형 삭제시 도형과 연결된 선을 삭제하기위해 연결선의 ID를 저장한다.
void COverlayView::AddLinkedLineIdInObjects(OVERLAY::ObjectLinkedLine* pLinkedLine)
{	
	if(pLinkedLine==NULL)
		return;

	OVERLAY::OBJECT* pObject = pLinkedLine;
	pObject = m_OverlayMain.GetOverlayObject(pLinkedLine->nLinkedLine_Start_ID);
	if(pObject!=NULL)
	{
		//m_OverlayMain.GetOverlayObject(m_pSearchedObjectforLinkedLine->nID)->ObjectLinkedLineProperty.AddObjectID(pLinkedLine->nID);
		pObject->ObjectLinkedLineProperty.AddObjectID(pLinkedLine->nID);
	}

	pObject = m_OverlayMain.GetOverlayObject(pLinkedLine->nLinkedLine_End_ID);			
	if(pObject!=NULL)
	{
		//m_OverlayMain.GetOverlayObject(m_pSearchedObjectforLinkedLine->nID)->ObjectLinkedLineProperty.AddObjectID(pLinkedLine->nID);
		pObject->ObjectLinkedLineProperty.AddObjectID(pLinkedLine->nID);
	}
}

void COverlayView::SetGrid(BOOL bGrid)
{
	m_OverlayMain.SetGrid(bGrid);
}

void COverlayView::DrawGrid(HWND hWnd, UINT nDetail)
{
	m_OverlayMain.DrawGrid(hWnd, nDetail);
}

UINT COverlayView::GetSelectedObjectSize()
{
	return m_SelectedObjectMap.size();
}
