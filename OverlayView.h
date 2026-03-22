#pragma once

#include "OverlayMain.h"
#include <map>
#include "LayerStructure.h"


//---------------------------------------
// 작성자			: 유영호
// 클래스명			: COverlayView
// 목적				: 투명도 관련 윈도우 이벤트 처리
// 생성일			: 2024-10-07
// 최종수정자		:
// 최종수정일		:
//---------------------------------------
//enum { DEFAULT_SCREEN_WIDTH = 1024, DEFAULT_SCREEN_HEIGHT = 768 };

using namespace OVERLAY;

class CCoordMgr;


class COverlayView
{
public:
	COverlayView(CWnd* pMainView);
	~COverlayView();

private:
	CWnd*						m_pMainView;								// 메인 화면
	BOOL						m_bAddObject;								// 도형 추가
	BOOL						m_bEditLinkedLine;							// 도형간 연결선 편집
	BOOL						m_bIsLButtonDown;							// 마우스 왼쪽버튼 누름
	BOOL						m_bIsObjectSelect;							// 도형 선택
	BOOL						m_bIsObjectEditing;							// 도형 편집
	BOOL						m_bIsObjectMove;							// 선택된 도형 이동
	BOOL						m_bIsCtrl;									// 도형 다중 선택
	BOOL						m_bIsCopy;									// 도형 복사
	BOOL						m_bIsLinkedLine;							// 도형간 연결선	
	BOOL						m_bIsInitialization;

	COverlayMain				m_OverlayMain;								// 도형 그리기 관련
	OVERLAY::OUT_LINE	m_eSelectedOutLinePoint;					// 도형 편집 시 외곽선 정의
	INT							m_nSelectedOutLinePointIndex;				// 도형을 형성하고 있는 점
	BOOL						m_bIsOutLine;								// 도형의 모양 변경점 선택 유무

	std::map<UINT, UINT>			m_SelectedObjectMap;					// 선택된 도형 목록
	OVERLAY::OBJECT* m_pNewObject;							// 새로 추가될 도형의 포인터
	OVERLAY::OBJECT* m_pSearchedObjectforLinkedLine;			// 도형간 연결선을 그을 도형

	UINT							m_nPointCount;							// 도형을 이루고있는 점의 수 (Polygon,Curve..)
	int								m_nLinkedLinePointsCount;				// 도형간 연결선을 형성하고있는 점의 수

	CPoint							m_ptObjectMoveStart;					// 도형 이동시 시작 지점
	OVERLAY::Point			m_ptCurrentMouse;						// 현재 마우스 위치.
	CSize							m_sizeMem;								// 도형이 그려질 화면 크기

	OVERLAY::Point* m_ptLinkedLine[DEFAULT_LINKED_OBJECT];		// 도형간의 연결선.
	OVERLAY::ObjectLinkedLine* m_pObjectLinkedLine;						// 도형간의 연결선.

private:
	void DrawEditObject();													// 편집중인 도형 그리기
	void DrawObjectForLinkedLine();
	UINT AddObject();														// 도형 추가
	BOOL SetObjectPoints(UINT nFlags, CPoint point);						// 도형을 형성하고 있는 점 설정
	void DeleteObject();													// 도형 삭제
	void MakeGroup(OVERLAY::OBJECT* pObject);						// 도형 그룹.

public:
	void Initialize(int nlat_degree, int nlat_min, int nlat_sec, int nlng_degree, int nlng_min, int nlng_sec, char chlat = 'N', char chlng = 'E');
	void SetCoordMgr(CCoordMgr* pCoord);

	void Redraw(HDC hDC, int nX, int nY);// 다시 그리기
	COverlayMain* GetOverlay();												// 외부에서 도형그리기 기능 참조 
	INT	GetLayerCount();													// 레이어 수
	CLayerStructure* GetLayer(UINT nIdex);									// 원하는 레이어 참조
	void DrawObjects();														// 편집가능한 레이어를 제외한 모든 레이어의 있는 도형을 다시 그린다.
	void DrawEditlayer();													// 편집 가능한 레이어에 도형을 그린다.
	void ReleaseSelectObject();												// 선택된 도형의 선택해제

	OVERLAY::OBJECT* GetSelectedObject();							// 도형 속성 편집을 위해 한개의 도형이 선택될 시 도형정보를 얻어온다.
	void RePosition();														// 도형편집 시 도형 다시 그리기

	void OnLButtonDown(CPoint point);										// 윈도우 이벤트 -
	void OnLButtonUp(CPoint point);											// 윈도우 이벤트 -
	void OnLButtonDblClk(CPoint point);										// 윈도우 이벤트 -
	void OnRButtonDown(CPoint point);										// 윈도우 이벤트 -
	void OnRButtonUp(CPoint point);											// 윈도우 이벤트 -
	void OnRButtonDblClk(CPoint point);										// 윈도우 이벤트 -
	void OnMouseMove(CPoint point);											// 윈도우 이벤트 -
	void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);					// 윈도우 이벤트 -
	void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);					// 윈도우 이벤트 -
	void OnCreateObject(OVERLAY::OBJECT_TYPE eObjectType, char* strFileName = NULL, int nStrLen = 0); // 도형 생성.

	BOOL Save(char* strFileName, int nStrLen, UINT nLayerIndex);		// Save Layer 
	BOOL Load(char* strFileName, int nStrLen);							// Load Layer 

	void SetGroup();														// 도형 그룹 설정.
	void ReleaseGroup();													// 도형 그룹 해제.
	void SetGrid(BOOL bGrid);												// 도형격자 그리기 설정
	void DrawGrid(HWND hWnd, UINT nDetail);									// 도형격자 그리기
	UINT GetSelectedObjectSize();											// 현재 선택된 도형의 개수.

	void MoveToTop();	// 선택된 도형의 전시위치를 맨처음으로 이동.
	void MoveToBottom();// 선택된 도형의 전시위치를 맨아래로 이동.

private:
	BOOL IsMaxPoints();																// 도형을 형성하고 있는 점의 개수가 최대설정값보다 크다.
	void EditObject(CPoint point);													// 도형의 편집 유무 판단 및 편집 지점 선택
	void TemporaryDrawLinkedLine();													// 도형간 연결선을 화면에 도시
	void SearchPoint(CPoint point);													// 마우스로 클릭한 지점의 도형을 찾는다. 
	void SelectLinkedLinePoint(CPoint point);										// 찾은 도형에서 연결선이 연결될 점을 선택한다.
	void EditLinkedLinePoint(int nSelectedLinkedLinePointID, CPoint point);			// 연결선 편집시 찾은 도형에서 연결선이 연결될 점을 선택한다.
	void ResetLinkedLine();															// 도형간 연결선 편집정보 초기화
	void AddLinkedLineIdInObjects(OVERLAY::ObjectLinkedLine* pLinkedLine);			// 도형에 연결선 정보 추가
};
