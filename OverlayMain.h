#pragma once

#include "../inc/includeOverlay.h"
#include <map>
#include "LayerStructure.h"
#include "LayerManager.h"



//---------------------------------------
// 작성자			: 유영호
// 클래스명			: COverlayMain
// 기능				: 도형을 그릴 메모리 영역 할당 및 인터페이스 제공
// 생성일			: 2024-10-07
// 최종수정자		: 유영호
// 최종수정일		: 2009-10-16
//---------------------------------------

using namespace OVERLAY;

class COverlayFileMgr;
class CLayerStructure;

class COverlayMain : public CLayerManager
{
public:
	COverlayMain(UINT nMaxLayer, UINT nObjectPerLayer);
	virtual ~COverlayMain();

private:
	// 도형이 그려질 메모리 버퍼
	DrawField* m_pOverlayMem;			// 최상위 레이어를 제외한 모든 레이어의 도형이 도시되는 버퍼
	DrawField* m_pScreenMem;			// 화면 도시를 위한 버퍼
	DrawGridField* m_pGridMem;				// 도형 편집용 격자를 위한 버퍼
	DrawUpperLayer* m_pEditLayerMem;		// 최상위 레이어의 도형이 도시되는 버퍼
	DrawEditField* m_pEditMem;				// 편집중인 도형이 도시되는 버퍼
	DrawSelectEditPointField* m_pEditMemForSelect;	// 편집중인 도형의 선택영역
	DrawObjectIdMap* m_pObjectIDMap;			// 도형의 ID를 저장하는 버퍼

	// 레이어 파일 저장 및 불러오기
	COverlayFileMgr* m_pSave_n_Load_4_Layer;

	void Draw(OVERLAY::OBJECT* pObject, BOOL bIsEdit = FALSE);	// 도형을 그린다.
	void DrawEdit(OVERLAY::OBJECT* pObject);					// 편집영역에 도형을 그린다.
	void ClearEdit(OVERLAY::OBJECT* pObject);					// 편집영역에 그려진 도형을 지운다.	
	void ClearEditLayer();												// 최상위 레이어에 그려진 도형을 지운다.

	BOOL	m_bIsEdit; // 도형의 편집 유무.
	BOOL	m_bDrawOnScreen;//

	BOOL	m_bIsGrid; // 도형 격자 전시 유무.

	std::map<UINT, UINT>				m_ExceptOnEditLayerMap;				// 도형 편집시 편집레이어에서 그려지지 않을 도형.

public:
	void DrawObjectInEditLayer(OVERLAY::OBJECT* pObject);		// 최상위 레이어에 도형을 그린다.

public:
	void Initialization(Gdiplus::Graphics& g, INT nWidth, INT nHeight);		// 도형이 그려질 영역을 초기화 한다.	

	virtual BOOL AddLayer(CLayerStructure* pLayer);							// 레이어 추가
	virtual BOOL AddLayer(const char* strLayerName, int nStrLen);

	virtual BOOL DeleteLayer(INT nIndex);									// 레이어 삭제
	virtual void ClearLayer();												// 모든 레이어 삭제

	// 도형 추가/삭제
	virtual BOOL AddObject(OVERLAY::OBJECT* pObject);				// 도형 추가
	virtual BOOL DeleteObject(INT nID);										// 도형 삭제
	virtual BOOL DeleteAllObjects();										// 모든 도형 삭제

	// 레이어 위치변경
	virtual BOOL MoveLayerTop(INT nIndex);									//레이어를 맨위로 이동한다.
	virtual BOOL MoveLayerBottom(INT nIndex);								//레이어를 맨아래로 이동한다.
	virtual BOOL MoveLayerUp(INT nIndex);									//레이어를 위로 이동한다.
	virtual BOOL MoveLayerDown(INT nIndex);									//레이어를 아래로 이동한다.

	// 도형 위치변경
	virtual BOOL MoveObjectTop(INT nIndex);									//편집중인 레이어의 도형을 맨위로 이동한다.
	virtual BOOL MoveObjectBottom(INT nIndex);								//편집중인 레이어의 도형을 맨아래로 이동한다.
	virtual BOOL MoveObjectUp(INT nIndex);									//편집중인 레이어의 도형을 위로 이동한다.
	virtual BOOL MoveObjectDown(INT nIndex);								//편집중인 레이어의 도형을 아래로 이동한다.

	OVERLAY::OBJECT* SelectedObject(SHORT nObjectId);				// 선택된 도형을 얻어온다.

	void EditObject(OVERLAY::OBJECT* pObject);						// 편집중인 도형을 그린다.
	void DrawObjects();														// 최상위 레이어를 제외한 모든 레이어의 도형을 그린다.
	void DrawEditlayer();													// 최상위 레이어에 도형을 그린다.

	void ExceptObjectsOnEditLayerClear();									// 		
	void ExceptObjectsOnEditLayer(UINT nObjectID);							//선택된 도형은 편집레이어에서 그리지 않는다.

	// 편집용 도형 지우기
	void ClearEdit();														// 편집중인 도형을 지운다.

	void DrawOnScreen();
	void Redraw(HDC hDC, int nX, int nY);
	DrawField* GetOverlayImage();											// 화면에 도시되기 위한 이미지
	DrawField* GetRedarw();													// 화면 다시 그리기

	SHORT PickObjectID(int x, int y);										// 화면에서 도형을 선택한다.
	SHORT PickSelectEditPoint(int x, int y);								// 선택된 도형의 편집지점

	BOOL Save(char* strFileName, int nStrLen, UINT nLayerIndex);
	BOOL Load(char* strFileName, int nStrLen);

	// 격자 전시.
	void SetGrid(BOOL bGrid);
	void DrawGrid(HWND hWnd, UINT nDetail);
};
