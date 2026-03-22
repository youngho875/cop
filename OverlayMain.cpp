#include "pch.h"
#include "OverlayMain.h"
#include "./OverlayFileMgr/OverlayFileMgr.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif




COverlayMain::COverlayMain(UINT nMaxLayer, UINT nObjectPerLayer)
	:CLayerManager(nMaxLayer, nObjectPerLayer)
	, m_pOverlayMem(NULL)
	, m_pScreenMem(NULL)
	, m_pGridMem(NULL)
	, m_pEditLayerMem(NULL)
	, m_pEditMem(NULL)
	, m_pEditMemForSelect(NULL)
	, m_pObjectIDMap(NULL)
	, m_pSave_n_Load_4_Layer(NULL)
	, m_bIsGrid(FALSE)
{
	m_bIsEdit = FALSE;
	m_bDrawOnScreen = FALSE;
}

COverlayMain::~COverlayMain()
{
	SAFE_DELETE(m_pOverlayMem);
	SAFE_DELETE(m_pScreenMem);
	SAFE_DELETE(m_pGridMem);
	SAFE_DELETE(m_pEditLayerMem);
	SAFE_DELETE(m_pEditMem);
	SAFE_DELETE(m_pEditMemForSelect);
	SAFE_DELETE(m_pObjectIDMap);
	SAFE_DELETE(m_pSave_n_Load_4_Layer);
}
//---------------------------------------
// 작성자			: 유영호
// 함수명			: Draw()
// 기능				: 도형 그리기.
// 생성일			: 2024-10-07
// 최종수정자		:
// 최종수정일		:
//---------------------------------------
void COverlayMain::Draw(OVERLAY::OBJECT* pObject, BOOL bIsEdit)
{
	OVERLAY::OBJECT* pTemp_Object = NULL;
	m_bIsEdit = bIsEdit;
	if (pObject != NULL)
	{
		if (m_bIsEdit == TRUE)
		{
			if (pObject != NULL)
				DrawEdit(pObject);
		}
		else
		{
			if (pObject->nID == 0)
				return;

			pTemp_Object = SelectedObject(pObject->nID);
			if (pTemp_Object != NULL)
				DrawObjectInEditLayer(pObject);
		}
	}
	m_bDrawOnScreen = FALSE;

	//TRACE("Draw \n");
}

void COverlayMain::DrawEdit(OVERLAY::OBJECT* pObject)
{
	//if(pObject->nID<1)
	//	return;
//	m_pEditMemForSelect->Clear();
	m_pEditMem->Draw(pObject);
	m_pEditMemForSelect->Draw(pObject);
	//	TRACE("m_pEditMemForSelect->Draw(pObject): pObject->nID = %d \n",pObject->nID);
	m_bDrawOnScreen = FALSE;

}

void COverlayMain::ClearEdit(OVERLAY::OBJECT* pObject)
{
	m_pEditMem->Clear();
	m_pEditMemForSelect->Clear();
}

// 최상위 레이어에 도형을 도시한다.
void COverlayMain::DrawObjectInEditLayer(OVERLAY::OBJECT* pObject)
{
	m_pEditLayerMem->Draw(pObject);
	m_pObjectIDMap->Draw(pObject);
	m_bDrawOnScreen = FALSE;
	//	TRACE("DrawObjectInEditLayer\n");
}
// 최상위 레이어 클리어
void COverlayMain::ClearEditLayer()
{
	m_pEditLayerMem->Clear();
	m_pObjectIDMap->Clear();
}

void COverlayMain::Initialization(Gdiplus::Graphics& g, INT nWidth, INT nHeight)
{
	if (m_pOverlayMem != NULL)				SAFE_DELETE(m_pOverlayMem);
	if (m_pScreenMem != NULL)				SAFE_DELETE(m_pScreenMem);
	if (m_pGridMem != NULL)				SAFE_DELETE(m_pGridMem);
	if (m_pEditLayerMem != NULL)			SAFE_DELETE(m_pEditLayerMem);
	if (m_pEditMem != NULL)				SAFE_DELETE(m_pEditMem);
	if (m_pObjectIDMap != NULL)			SAFE_DELETE(m_pObjectIDMap);
	if (m_pEditMemForSelect != NULL)		SAFE_DELETE(m_pEditMemForSelect);
	if (m_pSave_n_Load_4_Layer != NULL)	SAFE_DELETE(m_pSave_n_Load_4_Layer);

	m_pOverlayMem = new DrawField(g, nWidth, nHeight);
	m_pScreenMem = new DrawField(g, nWidth, nHeight);
	m_pGridMem = new DrawGridField(g, nWidth, nHeight);
	m_pEditLayerMem = new DrawUpperLayer(g, nWidth, nHeight);
	m_pEditMem = new DrawEditField(g, nWidth, nHeight);
	m_pObjectIDMap = new DrawObjectIdMap(g, nWidth, nHeight);
	m_pEditMemForSelect = new DrawSelectEditPointField(g, nWidth, nHeight);
	m_pSave_n_Load_4_Layer = new COverlayFileMgr(this);
}

BOOL COverlayMain::AddLayer(CLayerStructure* pLayer)
{
	BOOL bRet = CLayerManager::AddLayer(pLayer);
	ClearEditLayer();
	DrawObjects();
	return bRet;
}

BOOL COverlayMain::AddLayer(const char* strLayerName, int nStrLen)
{
	BOOL bRet = CLayerManager::AddLayer(strLayerName, nStrLen);
	ClearEditLayer();
	DrawObjects();
	return bRet;
}

BOOL COverlayMain::DeleteLayer(INT nIndex)
{
	BOOL bRet = CLayerManager::DeleteLayer(nIndex);
	DrawObjects();
	DrawEditlayer();
	return bRet;
}

void COverlayMain::ClearLayer()
{
	m_pOverlayMem->Clear();
	m_pScreenMem->Clear();
	//m_pGridMem->Clear();
	m_pEditMem->Clear();
	m_pEditLayerMem->Clear();
	m_pEditMemForSelect->Clear();
	m_pObjectIDMap->Clear();

	CLayerManager::ClearLayer();
}

BOOL COverlayMain::AddObject(OVERLAY::OBJECT* pObject)
{
	if (NumberOfLayer() <= 0)
		return FALSE;

	if (pObject == NULL)
		return FALSE;

	// 일정크기 이하의 도형은 새로 생성하지 않는다.
	if (pObject->GetType() != OVERLAY::OBJECT_TYPE::LINKED_LINE &&
		pObject->GetType() != OVERLAY::OBJECT_TYPE::LINKED_MULTI_LINE &&
		pObject->GetType() != OVERLAY::OBJECT_TYPE::LINKED_CURVE &&
		pObject->GetType() != OVERLAY::OBJECT_TYPE::LINKED_MULTI_CURVE)
	{
		if (abs(pObject->pt[0].X() - pObject->pt[1].X()) < 1 || abs(pObject->pt[0].Y() - pObject->pt[1].Y()) < 1)
			return FALSE;

		double temp = 0;
		temp = sqrt((double)((pObject->pt[0].X() - pObject->pt[1].X()) * (pObject->pt[0].X() - pObject->pt[1].X())) + (double)((pObject->pt[0].Y() - pObject->pt[1].Y()) * (pObject->pt[0].Y() - pObject->pt[1].Y())));
		if (temp < 2)
			return FALSE;
	}

	BOOL bRet = CLayerManager::AddObject(pObject);

	if (bRet)
		Draw(pObject);
	else
		return FALSE;
	//DeleteObject(pObject->nID);


	return TRUE;
}

BOOL COverlayMain::DeleteObject(INT nID)
{
	BOOL bRet = CLayerManager::DeleteObject(nID);
	DrawEditlayer();
	return bRet;
}

BOOL COverlayMain::DeleteAllObjects()
{
	CLayerStructure* pLayer = GetLayer(0);

	while (pLayer->NumberOfObjectInLayer() > 0)
	{
		if (pLayer->get_first_object() != NULL)
			DeleteObject(pLayer->get_first_object()->nID);
	}

	return TRUE;
}

BOOL COverlayMain::MoveLayerTop(INT nIndex)
{
	BOOL bRet = CLayerManager::MoveLayerTop(nIndex);
	DrawObjects();
	DrawEditlayer();
	return bRet;
}

BOOL COverlayMain::MoveLayerBottom(INT nIndex)
{
	BOOL bRet = CLayerManager::MoveLayerBottom(nIndex);
	DrawObjects();
	DrawEditlayer();
	return bRet;
}

BOOL COverlayMain::MoveLayerUp(INT nIndex)
{
	BOOL bRet = CLayerManager::MoveLayerUp(nIndex);
	DrawObjects();
	DrawEditlayer();
	return bRet;
}

BOOL COverlayMain::MoveLayerDown(INT nIndex)
{
	BOOL bRet = CLayerManager::MoveLayerDown(nIndex);
	DrawObjects();
	DrawEditlayer();
	return bRet;
}

BOOL COverlayMain::MoveObjectTop(INT nIndex)
{
	BOOL bRet = CLayerManager::MoveObjectTop(nIndex);
	DrawEditlayer();
	return bRet;
}

BOOL COverlayMain::MoveObjectBottom(INT nIndex)
{
	BOOL bRet = CLayerManager::MoveObjectBottom(nIndex);
	DrawEditlayer();
	return bRet;
}

BOOL COverlayMain::MoveObjectUp(INT nIndex)
{
	BOOL bRet = CLayerManager::MoveObjectUp(nIndex);
	DrawEditlayer();
	return bRet;
}

BOOL COverlayMain::MoveObjectDown(INT nIndex)
{
	BOOL bRet = CLayerManager::MoveObjectDown(nIndex);
	DrawEditlayer();
	return bRet;
}

//---------------------------------------
// 작성자			: 유영호
// 함수명			: SelectedObject()
// 기능				: 해당 ID에 해당하는 도형 선택.
// 생성일			: 2024-10-07
// 최종수정자		:
// 최종수정일		:
//---------------------------------------
OVERLAY::OBJECT* COverlayMain::SelectedObject(SHORT nObjectId)
{
	OVERLAY::OBJECT* pObject = NULL;
	pObject = GetOverlayObject(nObjectId);
	return pObject;
}
//---------------------------------------
// 작성자			: 유영호
// 함수명			: DrawEditObject()
// 기능				: 도형 편집.
// 생성일			: 2024-10-07
// 최종수정자		:
// 최종수정일		:
//---------------------------------------
void COverlayMain::EditObject(OVERLAY::OBJECT* pObject)
{
	if (m_pEditMem == NULL)
		return;

	Draw(pObject, TRUE);
}

//---------------------------------------
// 작성자			: 유영호
// 함수명			: DrawObjects()
// 기능				: 도형 그리기
// 생성일			: 2024-10-07
// 최종수정자		:
// 최종수정일		:
//---------------------------------------
void COverlayMain::DrawObjects()
{
	CLayerStructure* pLayer = NULL;
	OVERLAY::OBJECT* pObject = NULL;

	if (m_pOverlayMem == NULL)
		return;

	m_pOverlayMem->Clear();

	for (INT i = 1; i < NumberOfLayer(); i++)
	{
		pLayer = CLayerManager::GetLayer(i);

		if (pLayer != NULL)
		{
			pObject = pLayer->get_first_object();
			if (pLayer->IsDisplay())
			{
				for (INT j = 0; j < pLayer->NumberOfObjectInLayer(); j++)
				{
					if (pObject != NULL)
					{
						m_pOverlayMem->Draw(pObject);
						pObject = pLayer->get_next_object();
					}
				}
			}
		}
	}
	m_bDrawOnScreen = FALSE;
}

void COverlayMain::DrawEditlayer()
{
	CLayerStructure* pLayer = NULL;
	OVERLAY::OBJECT* pObject = NULL;

	if (m_pEditLayerMem == NULL)
		return;

	ClearEditLayer();

	UINT nID = 0;
	if (CLayerManager::NumberOfLayer() > 0)
	{
		pLayer = CLayerManager::GetLayer(0);
		if (pLayer != NULL)
		{
			pObject = pLayer->get_first_object();
			if (pLayer->IsDisplay())
			{
				for (INT j = 0; j < pLayer->NumberOfObjectInLayer(); j++)
				{
					if (pObject != NULL)
					{
						//	nID = m_ExceptOnEditLayerMap[pObject->nID];
						//	if( nID != pObject->nID)
						DrawObjectInEditLayer(pObject);
						pObject = pLayer->get_next_object();
					}
				}
			}
		}
	}
	m_bDrawOnScreen = FALSE;
}
//---------------------------------------
// 작성자			: 유영호
// 함수명			: ExceptObjectsOnEditLayerClear()
// 기능				: 
// 생성일			: 2024-10-07
// 최종수정자		:
// 최종수정일		:
//---------------------------------------
void COverlayMain::ExceptObjectsOnEditLayerClear()
{
	//	m_ExceptOnEditLayerMap.clear();
}
//---------------------------------------
// 작성자			: 유영호
// 함수명			: ExceptObjectsOnEditLayer()
// 기능				: 
// 생성일			: 2024-10-07
// 최종수정자		:
// 최종수정일		:
//---------------------------------------
void COverlayMain::ExceptObjectsOnEditLayer(UINT nObjectID)
{
	//m_ExceptOnEditLayerMap[nObjectID] = nObjectID;
}

//---------------------------------------
// 작성자			: 유영호
// 함수명			: ClearEdit()
// 기능				: 도형 편집화면 클리어.
// 생성일			: 2024-10-07
// 최종수정자		:
// 최종수정일		:
//---------------------------------------
void COverlayMain::ClearEdit()
{
	m_pEditMem->Clear();
	m_pEditMemForSelect->Clear();
	m_bDrawOnScreen = FALSE;
}

void COverlayMain::DrawOnScreen()
{
	if (m_bDrawOnScreen == TRUE)
	{
		return;
	}

	m_pScreenMem->Clear();
	if (m_bIsGrid == TRUE)
		m_pScreenMem->DrawImages(*m_pGridMem->GetImage());
	m_pScreenMem->DrawImages(*m_pOverlayMem->GetImage());
	m_pScreenMem->DrawImages(*m_pEditLayerMem->GetImage());

	if (m_bIsEdit == TRUE)
	{
		m_pScreenMem->DrawImages(*m_pEditMem->GetImage());
	}

	m_bDrawOnScreen = TRUE;
}


void COverlayMain::Redraw(HDC hDC, int nX, int nY)
{
	Gdiplus::Graphics	g(hDC);
	DrawOnScreen();
	g.DrawImage(m_pScreenMem->GetImage(), nX, nY);
}

SHORT COverlayMain::PickObjectID(int x, int y)
{
	return m_pObjectIDMap->GetObjectID(x, y);
}

SHORT COverlayMain::PickSelectEditPoint(int x, int y)
{
	return m_pEditMemForSelect->GetObjectID(x, y);
}

BOOL COverlayMain::Save(char* strFileName, int nStrLen, UINT nLayerIndex)
{
	if (nStrLen < 1)
	{
		AfxMessageBox(_T("저장할 레이어를 선택해 주세요."));
		return FALSE;
	}
	if (m_pSave_n_Load_4_Layer == NULL)
		return FALSE;

	CLayerStructure* pLayer = GetLayer(nLayerIndex);
	if (pLayer == NULL)
		return FALSE;

	return m_pSave_n_Load_4_Layer->Save(strFileName, nStrLen, pLayer);
}

BOOL COverlayMain::Load(char* strFileName, int nStrLen)
{
	if (m_pSave_n_Load_4_Layer == NULL)
		return FALSE;

	return m_pSave_n_Load_4_Layer->LoadXML(strFileName, nStrLen);
}

void COverlayMain::SetGrid(BOOL bGrid)
{
	m_bIsGrid = bGrid;
}

void COverlayMain::DrawGrid(HWND hWnd, UINT nDetail)
{
	if (nDetail < 1 || m_bIsGrid == FALSE)
	{
		m_pGridMem->Clear();
		return;
	}
	m_pGridMem->Clear();
	m_pGridMem->DrawGrid(hWnd, nDetail);
	m_pScreenMem->DrawImages(*m_pGridMem->GetImage());
}
