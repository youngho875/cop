#pragma once

#include "OverlayView.h"
#include "../LIBSrc/Overlay/OverlayEventMgr.h"

using namespace OVERLAY;

class CCoordMgr;
//class CC4IMapView;

class COverlayThread
{ 
public:
	COverlayThread(CWnd* pMainView);
	virtual ~COverlayThread();

private:
	UINT	m_dwID;
	
	HANDLE	m_hThread;
	HANDLE	m_hKillEvent;
	HANDLE	m_hSignalEvent;
	HANDLE	m_hSignalEventForMakeImage;
	
	COverlayView		m_OverlayView;
	COverlayEventMgr	m_EventMgr;
	BOOL				m_bIsRun;

	double m_dCenterX;
	double m_dCenterY;
	double m_dScale;

	void ProcDraw();
	void* m_pViewPointer;
	
public:
	static UINT ThreadProc(void* pThis);
	void PushEvent(E_OVERLAY_EVENT eEvent, CPoint point);
	void PushEvent(E_OVERLAY_EVENT eEvent, int x, int y);
	void End();
	void NewObject(E_OVERLAY_EVENT eEvent, OBJECT_TYPE eObjectType);
	void NewObject(E_OVERLAY_EVENT eEvent, OBJECT_TYPE eObjectType, char* strFileName, int nStrLen);
	void Initialize(CCoordMgr* pCoord ,int nlat_degree, int nlat_min, int nlat_sec, int nlng_degree, int nlng_min, int nlng_sec, char chlat='N', char chlng='E');
	void RePosition();
	void Redraw(HDC hDC, int nX=0, int nY=0);
	void SetGrid(BOOL bGrid);
	void DrawGrid(HWND hWnd, UINT nDetail);
	void MakeImage();
	
	COverlayMain*	GetOverlayMain();
	COverlayView*	GetOverlayView();
	
	HANDLE	SignalEvent();
	BOOL	IsRemainEvent();

	void LButtonDonw(CPoint pt);
	void MouseMove(CPoint pt);
	void SetViewPointer(LPVOID pViewPointer);
};


