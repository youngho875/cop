#include "pch.h"

#include "ObjectThread.h"
#include "C4IMapView.h"
#include <process.h>


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif



COverlayThread::COverlayThread(CWnd* pMainView)
:m_bIsRun(FALSE)
,m_dwID(0)
,m_hThread(NULL)
,m_OverlayView(pMainView)
,m_EventMgr()
{  
	m_pViewPointer = NULL;

	// Thread »ý¼º. 
	m_hKillEvent				= ::CreateEvent(NULL, TRUE, FALSE, NULL);
	m_hSignalEvent				= ::CreateEvent(NULL, FALSE, FALSE, NULL);
	m_hSignalEventForMakeImage	= ::CreateEvent(NULL, FALSE, FALSE, NULL);

	m_hThread = (HANDLE) _beginthreadex(
		NULL,						// Security
		0,							// Stack size - use default
		ThreadProc,     			// Thread function entry point 
		(void*) this,
		0,							// Init flag
		&m_dwID);					// Thread address

	//if (m_hThread)
	//{
	//	ResumeThread(m_hThread);
	//}
}

COverlayThread::~COverlayThread()
{
	SetEvent(m_hKillEvent);
	WaitForSingleObject(m_hThread,INFINITE);	

}

void COverlayThread::PushEvent(OVERLAY::E_OVERLAY_EVENT eEvent, CPoint point)
{
	COverlayEvent OverlayEvent;
	OverlayEvent.e_evnet = eEvent;
	OverlayEvent.point = point;
	m_EventMgr.push(OverlayEvent);
	
	if(m_bIsRun==FALSE)
	{
		SetEvent(m_hSignalEvent);
	}
}

void COverlayThread::PushEvent(OVERLAY::E_OVERLAY_EVENT eEvent, int x, int y)
{
	COverlayEvent OverlayEvent;
	OverlayEvent.e_evnet = eEvent;
	OverlayEvent.point.x = x;
	OverlayEvent.point.y = y;
	m_EventMgr.push(OverlayEvent);
	if(m_bIsRun==FALSE)
	{
		SetEvent(m_hSignalEvent);
	}
}

void COverlayThread::End()
{
	//SetEvent(m_hKillEvent);
}

void COverlayThread::NewObject(OVERLAY::E_OVERLAY_EVENT eEvent, OVERLAY::OBJECT_TYPE eObjectType)
{
	COverlayEvent OverlayEvent;
	OverlayEvent.e_evnet = eEvent;
	OverlayEvent.eObjectType = eObjectType;
	m_EventMgr.push(OverlayEvent);
	if(m_bIsRun==FALSE)
	{
		SetEvent(m_hSignalEvent);
	}
}

void COverlayThread::NewObject(OVERLAY::E_OVERLAY_EVENT eEvent, OVERLAY::OBJECT_TYPE eObjectType, char* strFileName, int nStrLen)
{	
	COverlayEvent OverlayEvent;
	
	if(nStrLen > sizeof(OverlayEvent.lpszFileName) )
		return;
	OverlayEvent.e_evnet = eEvent;
	OverlayEvent.eObjectType = eObjectType;
	memset(OverlayEvent.lpszFileName,0,sizeof(OverlayEvent.lpszFileName));
	memcpy(OverlayEvent.lpszFileName,strFileName,nStrLen);
	m_EventMgr.push(OverlayEvent);
	if(m_bIsRun==FALSE)
	{
		SetEvent(m_hSignalEvent);
	}
}

void COverlayThread::Initialize(CCoordMgr* pCoord , int nlat_degree, int nlat_min, int nlat_sec, int nlng_degree, int nlng_min, int nlng_sec, char chlat/*='N'*/, char chlng/*='E'*/)
{
	m_bIsRun		= FALSE;
	m_OverlayView.SetCoordMgr(pCoord);
	m_OverlayView.Initialize(nlat_degree, nlat_min, nlat_sec, nlng_degree, nlng_min, nlng_sec, chlat, chlng);	
}

void COverlayThread::RePosition()
{
#if 1
	m_OverlayView.RePosition();
#else
	COverlayEvent OverlayEvent;
	OverlayEvent.e_evnet = OVERLAY_REPOSITION;
	m_EventMgr.push(OverlayEvent);
	if(m_bIsRun==FALSE)
	{
		SetEvent(m_hSignalEvent);
	}
#endif
}
void COverlayThread::Redraw(HDC hDC, int nX, int nY)
{
	//SetEvent(m_hSignalEventForMakeImage);
	m_OverlayView.Redraw(hDC, nX, nY);
/*	COverlayEvent OverlayEvent;
	OverlayEvent.e_evnet = OVERLYA_REDRAW;
	OverlayEvent.hDC = hDC;
	OverlayEvent.nX = nX;
	OverlayEvent.nY = nY;	
	m_EventMgr.push(OverlayEvent);*/
}

void COverlayThread::SetGrid(BOOL bGrid)
{
	m_OverlayView.SetGrid(bGrid);
}

void COverlayThread::DrawGrid(HWND hWnd, UINT nDetail)
{
	m_OverlayView.DrawGrid(hWnd, nDetail);
}

COverlayMain* COverlayThread::GetOverlayMain()
 {
	return m_OverlayView.GetOverlay();
}

COverlayView* COverlayThread::GetOverlayView()
{
	return &m_OverlayView;
}

void COverlayThread::LButtonDonw(CPoint pt)
{
	m_OverlayView.OnLButtonUp(pt);
}

void COverlayThread::MouseMove(CPoint pt)
{
	m_OverlayView.OnMouseMove(pt);
}

void COverlayThread::ProcDraw()
{	
	static BOOL m_bIsLbuttonDown = FALSE;
	COverlayEvent* pOverlayEvent	 = m_EventMgr.pop();
	COverlayEvent* pNextOverlayEvent = NULL;
	CC4IMapView* pView = reinterpret_cast<CC4IMapView*>(m_pViewPointer);

	while(m_EventMgr.size()>0 && pOverlayEvent->e_evnet==OVERLAY_MOUSE_MOVE)
	{
		pNextOverlayEvent	= m_EventMgr.get_next();
		if(pNextOverlayEvent!=NULL)
		{
			if(pNextOverlayEvent->e_evnet==OVERLAY_MOUSE_MOVE)
			{
				pOverlayEvent = m_EventMgr.pop();
			}
			else
				break;
		}
		else
			pOverlayEvent = m_EventMgr.pop();
	}

	if(pOverlayEvent==NULL)
		return;
	
	switch(pOverlayEvent->e_evnet)
	{
	case OVERLAY_MOUSE_LBUTTON_DOWN:
		m_OverlayView.OnLButtonDown(pOverlayEvent->point);
		m_bIsLbuttonDown = TRUE;

		break;
	case OVERLAY_MOUSE_LBUTTON_UP:
		m_OverlayView.OnLButtonUp(pOverlayEvent->point);
	/*	if(pView!=NULL && pView->m_bIsDrawSituationInProcess==FALSE)
		{
			m_bIsLbuttonDown = FALSE;
//			pView->DrawSituation(FALSE,FALSE,FALSE);
		}*/
		
		break;
	case OVERLAY_MOUSE_RBUTTON_DOWN:
		
		break;
	case OVERLAY_MOUSE_RBUTTON_UP:
		
		break;
	case OVERLAY_MOUSE_LBUTTON_DCLICK:
		m_OverlayView.OnLButtonDblClk(pOverlayEvent->point);
		
		break;
	case OVERLAY_MOUSE_RBUTTON_DCLICK:
		
		break;
	case OVERLAY_MOUSE_MBUTTON_DOWN:
		
		break;
	case OVERLAY_MOUSE_MBUTTON_UP:
		
		break;
	case OVERLAY_MOUSE_MOVE:
		m_OverlayView.OnMouseMove(pOverlayEvent->point);
		/*if(pView!=NULL && m_bIsLbuttonDown && pView->m_bIsDrawSituationInProcess==FALSE)
//			pView->DrawSituation(FALSE,FALSE,FALSE);*/
		
		break;
	case OVERLAY_MOUSE_WHEEL:
		
		break;
	case OVERLAY_CREATE_NEW:
		m_OverlayView.OnCreateObject(pOverlayEvent->eObjectType, pOverlayEvent->lpszFileName, strlen(pOverlayEvent->lpszFileName));
		
		break;
	case OVERLAY_DELETE_OBJECT:
		m_OverlayView.OnKeyDown(VK_DELETE,0,0);
		
		break;
	case OVERLAY_MULTI_SELECT:
		m_OverlayView.OnKeyDown(VK_CONTROL,0,0);
 
		break;
	case OVERLAY_CANCEL:
		m_OverlayView.OnKeyDown(VK_ESCAPE,0,0);

		break;
	case OVERLAY_RELEASE_SELECT:		
		m_OverlayView.OnKeyUp(VK_CONTROL,0,0);
		
		break;
	case OVERLYA_REDRAW:
		m_OverlayView.Redraw(pOverlayEvent->hDC,pOverlayEvent->nX,pOverlayEvent->nY);

		break;
	case OVERLAY_REPOSITION:
		m_OverlayView.RePosition();
		if(pView!=NULL)
		{
//			if(pView->m_bIsDrawSituationInProcess==FALSE)
//				pView->DrawSituation(TRUE,TRUE,TRUE, TRUE);
		}

		break;
	case OVERLYA_SET_GRID:
		m_OverlayView.SetGrid(pOverlayEvent->bGrid);

		break;
	case OVERLYA_DRAW_GRID:
		m_OverlayView.DrawGrid(pOverlayEvent->hWnd, pOverlayEvent->nDetail);

		break;
	default:
		break;
	}
}

HANDLE COverlayThread::SignalEvent()
{
	return m_hSignalEvent;
}

BOOL COverlayThread::IsRemainEvent()
{
	if(m_EventMgr.size() > 0)
		return TRUE;
	return FALSE;
}

void COverlayThread::MakeImage()
{
}

void COverlayThread::SetViewPointer(void* pViewPointer)
{
	m_pViewPointer = pViewPointer;
}

UINT COverlayThread::ThreadProc(void* lpParam)
{
	COverlayThread*	pThis = reinterpret_cast<COverlayThread*>(lpParam);
	//COverlayThread* pThis = static_cast<COverlayThread*>(lpParam);

	HANDLE hObjects[2];
	hObjects[0] = pThis->m_hKillEvent;
	hObjects[1] = pThis->m_hSignalEvent;	
	hObjects[2] = pThis->m_hSignalEventForMakeImage;

	while(1)	
	{			
		DWORD dwWait = WaitForMultipleObjects(2, hObjects, FALSE, INFINITE);
		if (dwWait == WAIT_OBJECT_0)		
			break;

		if (dwWait == WAIT_OBJECT_0 + 1)
		{		
			pThis->m_bIsRun = TRUE;
			pThis->ProcDraw();
			if(pThis->IsRemainEvent())
				SetEvent(pThis->SignalEvent());
			else
				pThis->m_bIsRun = FALSE;
		}

		
		//if (dwWait == WAIT_OBJECT_0 + 2)
		//{
		//	pThis->m_bIsRun = TRUE;
		//	pThis->MakeImage();
		//	pThis->m_bIsRun = FALSE;
		//}

	}

	return 0;
}
