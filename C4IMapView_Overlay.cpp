#include "pch.h"
#include "C4IMapViewHeader.h"



// 현재 로든된 투명도의 전시여부 결정
void CC4IMapView::OnOverlayClear() 
{
	if(m_bOverlayLoad)
		m_bOverlayLoad = FALSE;
	else 
		m_bOverlayLoad = TRUE;
	
	//	DrawSituation(TRUE,TRUE,TRUE, TRUE);
}

void CC4IMapView::OnOverlayDraw()
{
	if(m_pDlgOverlayControler!=NULL)
		m_pDlgOverlayControler->ShowWindow(SW_SHOW);
}





void CC4IMapView::OnOverlayProperty()
{
	OVERLAY::OBJECT* pObject = NULL;
	pObject = m_pOverlayThread->GetOverlayView()->GetSelectedObject();
	
	if(pObject!=NULL)
	{	
		if(pObject->GetType()==OVERLAY::TEXT)
		{
			m_pDlgOverlayTextEdit->ShowWindowProperty(pObject);
		}
		else
			m_pDlgOverlayProperty->ShowWindowProperty(pObject);
	}
}

void CC4IMapView::OnOverlayGroup()
{
	m_pOverlayThread->GetOverlayView()->SetGroup();
}

void CC4IMapView::OnOverlayGroupRelease()
{
	m_pOverlayThread->GetOverlayView()->ReleaseGroup();
}

void CC4IMapView::OnOverlayMoveTop()
{
	m_pOverlayThread->GetOverlayView()->MoveToTop();
}

void CC4IMapView::OnOverlayMoveBottom()
{
	m_pOverlayThread->GetOverlayView()->MoveToBottom();
}

void CC4IMapView::OnOverlayEditObject()
{
	if(m_pOverlayThread!=NULL)
	{
		OVERLAY::OBJECT* pObject = m_pOverlayThread->GetOverlayView()->GetSelectedObject();
		
		if(pObject==NULL)
			return;
		
		if( pObject->GetType()== OVERLAY::LINKED_MULTI_LINE || 
			pObject->GetType()== OVERLAY::LINKED_MULTI_CURVE|| 
			pObject->GetType()== OVERLAY::POLYGON	|| 
			pObject->GetType()== OVERLAY::POLYLINE	|| 
			pObject->GetType()== OVERLAY::CURVE)
		{
			if(m_pDlgOverlayEditObjectPoint!=NULL)
			{
				//				if(m_pDlgOverlayEditObject!=NULL)
				//					m_pDlgOverlayEditObject->HideWindow();
				
				int nPointID = m_pOverlayThread->GetOverlayMain()->PickSelectEditPoint(m_nOverlyaEditObjectPointX, m_nOverlyaEditObjectPointY);
				nPointID -= OVERLAY::GENERAL_POINTS;
				if(nPointID>=0)
					m_pDlgOverlayEditObjectPoint->ShowWindowEditDlg(pObject->nID, nPointID);
				else
					AfxMessageBox("해당 도형은 점 편집만 가능합니다.\n점을 클릭한 후 편집메뉴를 선택하세요");
				
			}
		}
		/*else if(pObject->GetType()== OVERLAY::IMAGE ||
		pObject->GetType()== OVERLAY::TEXT)
		{
		
	}*/
		else
		{
			
			if(m_pDlgOverlayEditObject!=NULL)
			{
				//if(m_pDlgOverlayEditObjectPoint!=NULL)
				//m_pDlgOverlayEditObjectPoint->HideWindow()
				m_pDlgOverlayEditObject->ShowWindowEditDlg(pObject->nID);
			}
		}
	}
}


BOOL CC4IMapView::OnOverlayPopUpMenu(CPoint point)
{
	if(m_pOverlayThread==NULL)
		return FALSE;
	if(m_pOverlayThread->GetOverlayView()->GetSelectedObjectSize()<1)
		return FALSE;
	CMenu menu;
	menu.LoadMenu(IDR_POPUP);
	
	CMenu* pMenu = menu.GetSubMenu(8);
	
	
	OVERLAY::OBJECT* pObject = m_pOverlayThread->GetOverlayView()->GetSelectedObject();
	
	if(pObject==NULL)
		return FALSE;
	
	
	if( pObject->GetType()== OVERLAY::LINKED_MULTI_LINE ||
		pObject->GetType()== OVERLAY::LINKED_MULTI_CURVE||
		pObject->GetType()== OVERLAY::LINKED_LINE)
	{
		pMenu->DeleteMenu(5,MF_BYPOSITION);
		// 		if(m_pDlgOverlayEditObjectPoint!=NULL)
		// 		{}
	}
	//else if(pObject->GetType()== OVERLAY::IMAGE){
	else if(pObject->GetGroupID() >=1){
		pMenu->DeleteMenu(2,MF_BYPOSITION);
		pMenu->DeleteMenu(4,MF_BYPOSITION);       
	}else if(pObject->GetType()== OVERLAY::IMAGE)
		pMenu->DeleteMenu(2,MF_BYPOSITION);

	else{
		
	}
	
	pMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,
		point.x,point.y,AfxGetMainWnd());
	
	return TRUE;
}

