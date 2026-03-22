// GMapDisplayMgr.cpp: implementation of the CGMapDisplayMgr class.
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "C4IMap.h"
#include "MainFrm.h"
#include "C4IMapView.h"

#include "GMapDisplayMgr.h"
#include "GLib.h"
#include "../LIBSrc/CIDataMgr/BAMgr.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGMapDisplayMgr::CGMapDisplayMgr()
{
	//load_mgr.set_path_file( "C:\\path_shp.afccs" ); 
	//load_mgr.load_path_shp();
}

CGMapDisplayMgr::CGMapDisplayMgr(CWnd *pParentWnd)
{
	m_pWnd = pParentWnd;
	m_pMapInfo = new CGMapInfo;
	m_pMapInfo->InitInstance();

	m_bBkMode = TRUE;	
	m_dPixelPerMM = 3;
	m_lEllipsoid = EWGE;
	m_pSelmemDC = NULL;
	

	m_pGeoObj = (CGGeoObject *) new CGGeoObjGP;
	m_pGeoObj->SetEllipsoid(EWGE);				// 타원체를 WGS84로 지정함
	m_pGeoObj->SetMapMode(GMM_UTM);
	m_pGeoObj->SetProjBase(129.0, 0.0);		
	m_pGeoObj->SetProjScaleFactor(0.9996);		// 52 zone UTM투영의 원점
	//m_pGeoObj->SetValidity(1);				// 1m 단위의 유효숫자로 지정함 : 우선은 1m로 함
	//m_pGeoObj->SetEllipsoid(nEllipsoid);		// 타원체를 WGS84로 설정(default)
	m_pGeoObj->SetValidity(0.00001);			// 유효숫자로 지정

	CBAMgr::ClearAll();
}

CGMapDisplayMgr::CGMapDisplayMgr(IGModuleView *pContainer)
{
	m_pWnd = NULL;
	m_pMapInfo = NULL;
	m_ifViewInfo = NULL;
	m_bBkMode = TRUE;	
	m_ifView = NULL;
	m_ifView = pContainer;
	//m_ifView.p->QueryInterface(&m_ifMapView);
	m_dPixelPerMM = 3;
	m_lEllipsoid = EWGE;
	m_pSelmemDC = NULL;
	//CBAMgr::ClearAll();
}

CGMapDisplayMgr::~CGMapDisplayMgr()
{
	if(m_pSelmemDC != NULL)
	{
		m_pSelmemDC->DeleteDC();
		delete m_pSelmemDC;
		m_pSelmemDC = NULL;
	}
}

static BOOL is_first_drawing = TRUE;
static int iDispMapType = 0;
void CGMapDisplayMgr::Draw(CGDCGeo* pDC)
{		
	//TCHAR   inBuf[80];
	//::GetPrivateProfileString (TEXT("AFCCS_COP"), 
	//	TEXT("TYPE_BUSINESS"), 
	//	TEXT("Error: GPPS failed"),
	//	inBuf , 
	//	80, 
	//	TEXT("afccs_cop_type.ini")); 

	//CString type_business = inBuf;
	//
	//iDispMapType = CBAMgr::GetDispMapType();		
	//if( _T("ATO") ==  type_business)
	{		
		if(!m_ifViewInfo)
		{			
			return; 		
		}

		long MapMode = GMM_GP;
		m_ifViewInfo->GetMappingMode(&MapMode);			// 현재 사용되고 있는 좌표체계
														// 경위도, utm, ups, tm, 등

		if( m_lEllipsoid != 18 || MapMode != GMM_GP)
		{
			m_lEllipsoid = EWGE;
			//CGMessenger::PostToMainframe(WMG_AX_MAP_RESET, GMM_GP, (int)m_lEllipsoid);
		}
	
		double left, top, right, bottom;
		double dZoomFactor;
		m_ifViewInfo->GetSelectedRange(&left, &top, &right, &bottom);	
		
		
		CGRectGP rect(left, top, right, bottom);
		m_ClientRectGp = rect;	
		m_dScreenScale = GetScreenScale(pDC, &rect);
		m_ifViewInfo->GetZoomFactor(&dZoomFactor);	

		m_dZoomFactor = dZoomFactor;
		dZoomFactor *= 100;
		
		m_clBackColor[0] = RGB(145, 177, 190);	//배경 색상 설정 
		m_clBackColor[1] = RGB(187, 196, 190);	//배경 색상 설정 
		
		if( m_bIsSmallMapOpen != TRUE )	CMapDataMgr::smallMapOpen();				// 소축척지도 load 		
		//배경 지도 도시 
		CRect dpRect = CGLib::GetClientRect();
		CGRectGP gpRect;
		pDC->DPtoGeoP(&gpRect, &dpRect);
		pDC->FillSolidRect(&gpRect, m_clBackColor[0]);	 		
		backgroundShpDraw(pDC, dZoomFactor);
		
		long  lMapScale;	
		m_ifViewInfo->GetMapScale(&lMapScale);	
		int iScale = 0;
		EGMapScale nScale = MS_NOTHING;
		

		switch(lMapScale)
		{
		case 50000 :	nScale = MS_1_50000;	iScale = 50000;		break;
		case 100000 :	nScale = MS_1_100000;	iScale = 100000;	break;
		case 250000 :	nScale = MS_1_250000;	iScale = 250000;	break;
		case 500000 :	nScale = MS_1_500000;	iScale = 500000;	break;
		case 1000000 :	nScale = MS_1_1000000;	iScale = 1000000;	break;
		case 2000000 :	nScale = MS_1_2000000;	iScale = 2000000;	break;
		default: break;
		}
		

			//현재 정보 메모리에 저장 
		{
			long lMapKind, lMapScale;				
			BOOL bIsAutoMode;
			BOOL  bIsWorldMapOpen;
			long lMapMode;
			int iMapRatio, iMapType;
			double dleft, dtop, dright, dbottom, dZoomFactor;
			CGRectGP gpSelRect; 	
			
			// 자동축척유/무 get
			m_ifViewInfo->GetAutoScaleMode(&bIsAutoMode);
			// 맵 종류 get
			//값을 가져오긴 하나 사용하지 않는다.
			m_ifViewInfo->GetMapKind(&lMapKind);
			// 맵 scale get
			m_ifViewInfo->GetMapScale(&lMapScale);	
			// 맵 scale, 배율, 확대축소비율 get
			m_ifViewInfo->GetMapScale(&lMapScale);
			m_ifViewInfo->GetZoomFactor(&dZoomFactor);
			iMapRatio = (int)((double)lMapScale / dZoomFactor);
			// 맵 mode get
			m_ifViewInfo->GetMappingMode(&lMapMode);
			// 맵 도시영역 get
			m_ifViewInfo->GetSelectedRange(&dleft, &dtop, &dright, &dbottom);
			// 세계지도 open 유무 get
			bIsWorldMapOpen = CMapDataMgr::IsWorldMapOpen();
			// 지도 종류 get
			iMapType = iDispMapType;		
			
			S_MAP_INFO map_info;
			map_info.bIsAutoMode = bIsAutoMode;
			map_info.bIsWorldMapOpen = bIsWorldMapOpen;
			map_info.lMapMode = lMapMode;
			map_info.lMapScale = lMapScale;
			map_info.iMapRatio = iMapRatio;
			map_info.iMapType = iMapType;			
			map_info.dleft = dleft;
			map_info.dtop = dtop;
			map_info.dright = dright;
			map_info.dbottom = dbottom;			
			map_info.dZoomFactor = dZoomFactor;
			map_info.gpSelRect = gpSelRect;
			map_info.lMapKind = (EGMapKind)iDispMapType;			
			
			CRect rect;
			CPoint point;
			rect = CGLib::GetClientRect();	
			point.x = (rect.right + rect.left) / 2;
			point.y = (rect.bottom + rect.top) / 2;
			CGPointGP geo_pt; 		
			pDC->DPtoGeoP(&geo_pt, &point);	
			
			CString coord;		
			coord = CBAMgr::DegreeToDmsLatEx(geo_pt.m_P.lat);
			coord += CBAMgr::DegreeToDmsLonEx(geo_pt.m_P.lon);		
			map_info.coord_center = coord;						
			CMapDataMgr::save_current_map_info_on_mem( map_info );
		}


	//	if( dZoomFactor < 500 )				    //1만 이상은 패스
		if( dZoomFactor < 2000 )				    //1만 이상은 패스
			return;
				
		int iCnt;		
		stSatelliteMapIndex *pIndex = NULL;
		int iZoomRatio = (int)((double)lMapScale / (dZoomFactor/100.0));
		CGRectGP rectGP(left, top, right, bottom);		
		CMapConfigMgr::SetCurrentMapInfo((int)lMapScale, iZoomRatio);
		

		UINT uiRasterMapKind = 0;
		BOOL bIsAutoMode;

		m_lEllipsoid = m_geoObject.GetEllipsoid();
		//m_ifViewInfo->GetEllipsoid(&m_lEllipsoid);		
		m_ifViewInfo->GetAutoScaleMode(&bIsAutoMode);
		uiRasterMapKind = CMapDataMgr::GetRasterMapKind(bIsAutoMode, iZoomRatio);
		 
		
		if(CMapDataMgr::ExtractSatelliteMap(bIsAutoMode, rectGP, iDispMapType, iZoomRatio))
		{
			iCnt = CMapDataMgr::GetSatelliteMapIndexCount();				
			int cnt_draw = 0;
			
			for(int i=0; i<iCnt; i++)
			{
				pIndex = CMapDataMgr::GetSatelliteMapIndex(i);
				if(pIndex->m_pData != NULL)
				{
					CMapDataMgr::DrawSatelliteMap(pDC, pIndex);
					++cnt_draw;
				}					
			}
		}

		return;
	}
////////////////////////////////////////////////////////////////////////////////////////////////

	
//	clock_t startTimeDraw = clock(); 	
	
	if(!m_ifViewInfo) return; 		
	HRESULT hRes;
	double left, top, right, bottom;
	double dZoomFactor;
	m_ifViewInfo->GetSelectedRange(&left, &top, &right, &bottom);	
	CGRectGP rect(left, top, right, bottom);
	m_ClientRectGp = rect;	
	m_dScreenScale = GetScreenScale(pDC, &rect);
	hRes = m_ifViewInfo->GetZoomFactor(&dZoomFactor);	
	m_dZoomFactor = dZoomFactor;
	dZoomFactor *= 100;
	
	m_clBackColor[0] = RGB(145, 177, 207);	//배경 색상 설정 
	m_clBackColor[1] = RGB(187, 196, 204);	//배경 색상 설정 

//	m_clBackColor[1] = RGB(255, 255, 255);
	
	if( m_bIsSmallMapOpen != TRUE )	
		CMapDataMgr::smallMapOpen();				// 소축척지도 load
	if( m_bIsSmallWorldMapOpen != TRUE ) 
		CMapDataMgr::smallworldMapOpen();		// 세계지도(소축척) load
	
	if(CMapDataMgr::IsWorldMapOpen())	//세계 지도 도시 
	{	
		worldmapShpDraw(pDC, dZoomFactor, SHPT_POLYGON); 
		worldmapShpDraw(pDC, dZoomFactor, SHPT_ARC);
		worldmapShpDraw(pDC, dZoomFactor, SHPT_POINT);
	}
	else 
	{
		//배경 지도 도시 
		CRect dpRect = CGLib::GetClientRect();
		CGRectGP gpRect;
		gpRect = m_ClientRectGp;
		pDC->DPtoGeoP(&gpRect, &dpRect);
		pDC->FillSolidRect(&gpRect, m_clBackColor[0]);	 		
		backgroundShpDraw(pDC, dZoomFactor);
		
		//래스터, 위성영상, 백터 도시
		long lMapKind, lMapScale;	
		m_ifViewInfo->GetMapKind(&lMapKind);		// 지도의 종류
		m_ifViewInfo->GetMapScale(&lMapScale);	// 지도의 스케일	
		int iZoomRatio = (int)((double)lMapScale / (dZoomFactor/100.0));
		
		if( is_first_drawing == TRUE )
		{		 
			rect.m_R.left = 113.0;
			rect.m_R.right = 140.0;
			rect.m_R.top = 44.0;
			rect.m_R.bottom = 30.0;
			
			//int type_dis_map = 0;
			//type_dis_map = CBAMgr::GetDispMapType();		
			
			shpDraw( pDC, MS_1_2000000, iZoomRatio, rect, iDispMapType, false );
		//	shpDraw( pDC, MS_1_1000000, iZoomRatio, rect, type_dis_map, false );
		//	shpDraw( pDC, MS_1_500000, iZoomRatio, rect, type_dis_map, false );
		//	shpDraw( pDC, MS_1_250000, iZoomRatio, rect, type_dis_map, false );	
			is_first_drawing = FALSE;
		}

		 
		//현재 정보 메모리에 저장 
		{
			BOOL bIsAutoMode;
			BOOL  bIsWorldMapOpen;
			long lMapMode;
			int iMapRatio, iMapType;
			double dleft, dtop, dright, dbottom, dZoomFactor;
			CGRectGP gpSelRect; 	
			
			// 자동축척유/무 get
			m_ifViewInfo->GetAutoScaleMode(&bIsAutoMode);
			// 맵 종류 get
			//값을 가져오긴 하나 사용하지 않는다.
			m_ifViewInfo->GetMapKind(&lMapKind);
			// 맵 scale get
			m_ifViewInfo->GetMapScale(&lMapScale);	
			// 맵 scale, 배율, 확대축소비율 get
			m_ifViewInfo->GetMapScale(&lMapScale);
			m_ifViewInfo->GetZoomFactor(&dZoomFactor);
			iMapRatio = (int)((double)lMapScale / dZoomFactor);
			// 맵 mode get
			m_ifViewInfo->GetMappingMode(&lMapMode);
			// 맵 도시영역 get
			m_ifViewInfo->GetSelectedRange(&dleft, &dtop, &dright, &dbottom);
			// 세계지도 open 유무 get
			bIsWorldMapOpen = CMapDataMgr::IsWorldMapOpen();
			// 지도 종류 get
			iMapType = iDispMapType;		
			
			S_MAP_INFO map_info;
			map_info.bIsAutoMode = bIsAutoMode;
			map_info.bIsWorldMapOpen = bIsWorldMapOpen;
			map_info.lMapMode = lMapMode;
			map_info.lMapScale = lMapScale;
			map_info.iMapRatio = iMapRatio;
			map_info.iMapType = iMapType;			
			map_info.dleft = dleft;
			map_info.dtop = dtop;
			map_info.dright = dright;
			map_info.dbottom = dbottom;			
			map_info.dZoomFactor = dZoomFactor;
			map_info.gpSelRect = gpSelRect;
			map_info.lMapKind = (EGMapKind)iDispMapType;			
			
			CRect rect;
			CPoint point;
			rect = CGLib::GetClientRect();	
			point.x = (rect.right + rect.left) / 2;
			point.y = (rect.bottom + rect.top) / 2;
			CGPointGP geo_pt; 		
			pDC->DPtoGeoP(&geo_pt, &point);	
			
			CString coord;		
			coord = CBAMgr::DegreeToDmsLatEx(geo_pt.m_P.lat);
			coord += CBAMgr::DegreeToDmsLonEx(geo_pt.m_P.lon);		
			map_info.coord_center = coord;						
			CMapDataMgr::save_current_map_info_on_mem( map_info );
		}

		if( dZoomFactor < 67 ) 	return; //5,000,000
				
				
		int iScale = 0;
		EGMapScale nScale = MS_NOTHING;
		switch(lMapScale)
		{
		case 50000 :	nScale = MS_1_50000;	iScale = 50000;		break;
		case 100000 :	nScale = MS_1_100000;	iScale = 100000;	break;
		case 250000 :	nScale = MS_1_250000;	iScale = 250000;	break;
		case 500000 :	nScale = MS_1_500000;	iScale = 500000;	break;
		case 1000000 :	nScale = MS_1_1000000;	iScale = 1000000;	break;
		case 2000000 :	nScale = MS_1_2000000;	iScale = 2000000;	break;
		default: break;
		}		
		
		m_pMapInfo->SetMapScale(nScale);
		m_pMapInfo->SetMapKind((EGMapKind)iDispMapType);
		CMapConfigMgr::SetMapScale(nScale);				
		
		// 레스터 & 위성영상 데이터 처리		
		stRasterMap * pRasterMap = NULL;			
		stSatelliteMapIndex * pIndex = NULL;		
			
		CGRectGP rectGP(left, top, right, bottom);		
		CMapConfigMgr::SetCurrentMapInfo((int)lMapScale, iZoomRatio);
		
		long MapMode;
		//m_ifViewInfo->GetMappingMode(&MapMode);
		UINT uiRasterMapKind = 0;
		BOOL bIsAutoMode;
		m_ifViewInfo->GetEllipsoid(&m_lEllipsoid);		
		m_ifViewInfo->GetAutoScaleMode(&bIsAutoMode);
		uiRasterMapKind = CMapDataMgr::GetRasterMapKind(bIsAutoMode, iZoomRatio);
		
		// 1m 위성영상, 10/5만 육도 도시이면 좌표계를 평면으로 바꾼다.
		if(iDispMapType != 0 && 
			(uiRasterMapKind == 1 || uiRasterMapKind == 50000 || uiRasterMapKind == 100000) && 
			MapMode == GMM_UTM)
		{
			if(m_lEllipsoid == EBES)	m_lEllipsoid = EWGE;
			//CGMessenger::PostToMainframe(WMG_AX_MAP_RESET, GMM_GP, (int)m_lEllipsoid);
		}
		// 벡터도시가 아니고, 1m 위성영상, 10/5만 육도가 아니며 좌표계가 평면이면 투영법으로 바꾼다.
		else if(iDispMapType != 0 && 
			(uiRasterMapKind != 1 && uiRasterMapKind != 50000 && uiRasterMapKind != 100000) && 
			MapMode == GMM_GP)
		{
			if(m_lEllipsoid == EBES)	m_lEllipsoid = EWGE;
			//CGMessenger::PostToMainframe(WMG_AX_MAP_RESET, GMM_UTM, (int)m_lEllipsoid);
		}
		else if(iDispMapType == 0 && MapMode != CMapDataMgr::GetVectorMapMode())
		{
			MapMode = CMapDataMgr::GetVectorMapMode();
			//CGMessenger::PostToMainframe(WMG_AX_MAP_RESET, MapMode, (int)m_lEllipsoid);
		}	
		
		
		// 레스터 지도 도시
		s_flag_raster_view raster_view; 
		raster_view =	CMapDataMgr::get_flag_raster_view();	
		if( raster_view.visible != FALSE )
		//if((lMapScale == 50000 && raster_view.visible_5) == TRUE ||
		//	(lMapScale == 100000 && raster_view.visible_10) == TRUE ||
		//	(lMapScale == 250000 && raster_view.visible_25) == TRUE ||
		//	(lMapScale == 500000 && raster_view.visible_50) == TRUE ||
		//	(lMapScale == 1000000 && raster_view.visible_100) == TRUE ||
		//	(lMapScale == 2000000 && raster_view.visible_200) == TRUE)
		{
			if(CMapDataMgr::ExtractRasterMap(bIsAutoMode, uiRasterMapKind, rectGP, iDispMapType, iZoomRatio))
			{				  				
				BOOL visible_cur_scale;	int iCnt = CMapDataMgr::GetRasterMapCount();
				for(int i=0; i<iCnt; i++)
				{
					pRasterMap = CMapDataMgr::GetRasterMap(i);	
					
					visible_cur_scale = FALSE;
					switch( nScale )
					{
					case MS_1_50000 :		
						visible_cur_scale = raster_view.visible_5;		
						break;
					case MS_1_100000 :		
						visible_cur_scale = raster_view.visible_10;		
						break;
					case MS_1_250000 :		
						visible_cur_scale = raster_view.visible_25;		
						break;
					case MS_1_500000 :		
						visible_cur_scale = raster_view.visible_50;		
						break;
					case MS_1_1000000 :		
						visible_cur_scale = raster_view.visible_100;	
						break;
					case MS_1_2000000 :		
						visible_cur_scale = raster_view.visible_200;	
						break;
					default:				visible_cur_scale = FALSE;						
						break;
					}					
					if( (nScale != pRasterMap->m_enMapScale && MS_1_2000000 != pRasterMap->m_enMapScale)
						|| 0 == visible_cur_scale) 
					{						
						pRasterMap = NULL; 		
						continue;
					}									
					if(pRasterMap->m_bIsRasterDataOpen == TRUE)  
						CMapDataMgr::DrawRasterMap(pDC, pRasterMap);
				}
				
			}
		}
	
		s_flag_satellite_view satellite_view; 
		satellite_view = CMapDataMgr::get_flag_satellite_view();		
		if( satellite_view.visible != FALSE )			
		{
			if(CMapDataMgr::ExtractSatelliteMap(bIsAutoMode, rectGP, iDispMapType, iZoomRatio))
			{		
				int iCnt = CMapDataMgr::GetSatelliteMapIndexCount();				
				int cnt_draw = 0;
				for(int i=0; i<iCnt; i++)
				{
					pIndex = CMapDataMgr::GetSatelliteMapIndex(i);
					if(pIndex->m_pData != NULL)
					{
						CMapDataMgr::DrawSatelliteMap(pDC, pIndex);	++cnt_draw;
					}					
				} 
			}			
		}
		
		// CGLib 에 지도 정보 set
		
		if(iDispMapType == 0)
			CGLib::SetMapKind(iScale, 0);
		else if(iDispMapType == 1)
			CGLib::SetMapKind(0, (int)uiRasterMapKind);
		else
			CGLib::SetMapKind(iScale, (int)uiRasterMapKind);			  
		CGLib::SetMapRect(gpRect.m_R.left, gpRect.m_R.top, gpRect.m_R.right, gpRect.m_R.bottom);		
		 	 
		//if(!((lMapScale == 50000 && raster_view.visible_5) == TRUE ||
		//	(lMapScale == 100000 && raster_view.visible_10) == TRUE ||
		//	(lMapScale == 250000 && raster_view.visible_25) == TRUE ||
		//	(lMapScale == 500000 && raster_view.visible_50) == TRUE ||
		//	(lMapScale == 1000000 && raster_view.visible_100) == TRUE ||
		//	(lMapScale == 2000000 && raster_view.visible_200) == TRUE))
		if( raster_view.visible == FALSE)// && satellite_view.visible == FALSE )			
		{
			CBAMgr::SetDispMapType(0);
			//iDispMapType = 0;
		}

		// 벡터지도 복원도시
		//int type_dis_map = 0;
		//type_dis_map = CBAMgr::GetDispMapType();		
		if( iDispMapType != 1)
		{
			//iZoomRatio
			int mapscale = m_pMapInfo->GetMapScale();	
			if( iZoomRatio <= 2000000 )
				shpDraw( pDC, mapscale, iZoomRatio, rect, iDispMapType, true );	// 백터, 백터+래스터 		 
		}
		

		// select marking image draw
		CGPointGP gpoint;	CGRectGP rectGP2;
		if(CMapDataMgr::GetSelMarkingFlag(gpoint) == TRUE)
			SelMarkingDraw(pDC, dZoomFactor, rectGP, gpoint);
		CMapDataMgr::SetSelMarkingFlag(FALSE, gpoint);
		CMapDataMgr::set_current_map_kind((EGMapKind)iDispMapType);
	}
}
 
void CGMapDisplayMgr::SaveLogFile(CString strmsg)
{  

	CString strWrite = _T("");
	CString strPath  = _T("C:/AFC4I/Bin/log/");
	

	SYSTEMTIME cur_time;
	GetLocalTime( &cur_time );

	CString strTime;
	strTime.Format( "\n[종료시간] %02d시%02d분%02d.%03ld초", cur_time.wHour, cur_time.wMinute, cur_time.wSecond, cur_time.wMilliseconds );

	//CString strTime = COleDateTime::GetCurrentTime().Format("\n[끝] %Y-%m-%d %H:%M:%S");
	//CString strFileName = strPath + COleDateTime::GetCurrentTime().Format("%Y%m%d") + _T(".log");
	CString strFileName = strPath + _T("지도 도시 성능 측정.log");
	
	CFile f;
	CFileException e;
	
	if( !f.Open( strFileName, CFile::modeWrite | CFile::typeBinary, &e ) )
	{ 
		if(!f.Open( strFileName, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary, &e ))
		{
			return;
		}		
	}
	
	f.Seek(0, CFile::end);
	f.Write(strTime, strTime.GetLength());	
	f.Close();
}


void CGMapDisplayMgr::save_current_state_on_mem()
{
	 
}


void CGMapDisplayMgr::DrawMapBoundary(CGDCGeo* pDC)
{
	CRect rt = CGLib::GetClientRect();
	CPoint ptCenter(rt.CenterPoint());
	
	CPoint leftCenter(rt.left, (rt.top+rt.bottom)/2);
	CPoint rightCenter(rt.right,((rt.top+rt.bottom)/2));
	CPoint topCenter((rt.left+rt.right)/2, rt.top);
	CPoint bottomCenter((rt.left+rt.right)/2, rt.bottom);
	int rate = ptCenter.x/3;
	
	CRect rt_b1(ptCenter.x - rate, ptCenter.y - rate, ptCenter.x + rate, ptCenter.y + rate);
	CRect rt_b2(ptCenter.x - (rate*2), ptCenter.y - (rate*2), ptCenter.x + (rate*2), ptCenter.y + (rate*2));
	
	CGRectGP grt_b1, grt_b2, grt;
	pDC->DPtoGeoP(&grt_b1, rt_b1);
	pDC->DPtoGeoP(&grt_b2, rt_b2);
	pDC->DPtoGeoP(&grt, rt);
	CGPointGP gptCenter;
	pDC->DPtoGeoP(&gptCenter, &ptCenter);
	
	CGPointGP gLCenter, gRCenter, gTCenter, gBCenter;
	pDC->DPtoGeoP(&gLCenter, &leftCenter);
	pDC->DPtoGeoP(&gRCenter, &rightCenter);
	pDC->DPtoGeoP(&gTCenter, &topCenter);
	pDC->DPtoGeoP(&gBCenter, &bottomCenter);
	
	LOGBRUSH logBrush1;
	logBrush1.lbColor = RGB(255,0,0);
	logBrush1.lbHatch = 0;
	logBrush1.lbStyle = BS_SOLID;
	LOGBRUSH logBrush2;
	logBrush2.lbColor = RGB(255,0,0);
	logBrush2.lbHatch = 0;
	logBrush2.lbStyle = BS_SOLID;
	DWORD  *pStyle1 = NULL;
	DWORD  *pStyle2 = NULL;
	CSize  size;
	CPen pen1, pen2, *pOldPen;
	size.cx = 1;
	pDC->DPtoLP(&size);
	
	pStyle1 = new DWORD[2];
	pStyle1[0]= size.cx;
	pStyle1[1]= size.cx;   
	pen1.CreatePen(PS_GEOMETRIC|PS_USERSTYLE|PS_ENDCAP_FLAT|PS_JOIN_BEVEL,
		size.cx, &logBrush1 , 2, pStyle1); 
	pStyle2 = new DWORD[2];
	size.cx = 2;
	pDC->DPtoLP(&size);
	pStyle2[0]= size.cx;
	pStyle2[1]= size.cx;   
	pen2.CreatePen(PS_GEOMETRIC|PS_USERSTYLE|PS_ENDCAP_FLAT|PS_JOIN_BEVEL,
		size.cx, &logBrush2 , 2, pStyle2);  
	
	pDC->SetBkMode(TRANSPARENT);
	int nRopMode = pDC->SetROP2(R2_MASKPEN);
	
	pOldPen = pDC->SelectObject(&pen1);
	//CrossLine
	pDC->MoveTo(&gLCenter);
	pDC->LineTo(&gRCenter);
	pDC->MoveTo(&gTCenter);
	pDC->LineTo(&gBCenter);
	
	pDC->SelectObject(&pen2);
	pDC->Ellipse(&grt_b1);
	pDC->Ellipse(&grt_b2);
	
	pDC->SelectObject(pOldPen);
	pDC->SetROP2(nRopMode);
	
	USES_CONVERSION;
	LOGFONT lf;
	::ZeroMemory(&lf, sizeof(LOGFONT));
	
	int nFontSize;
	nFontSize = 14;
	CSize cSize(nFontSize, nFontSize);
	pDC->DPtoLP(&cSize);
	
	lf.lfHeight = (int)(cSize.cx);
	//lf.lfWidth = 0;
	lf.lfEscapement =  0;
	lf.lfOrientation =  0;
	lf.lfWeight = FW_REGULAR;
	lf.lfCharSet = ANSI_CHARSET;  //한글 지원
	CFont font;
	font.CreateFontIndirect(&lf);
	
	CFont *def_font  = pDC->SelectObject(&font);
	int iOldRop2  = 0; iOldRop2 = pDC->SetROP2(R2_MASKPEN);
	int iOldBkMode2 = 0; iOldBkMode2 = pDC->SetBkMode(TRANSPARENT);
	COLORREF oldColor2 = pDC->SetTextColor(RGB(0,0,0));
	
	CPoint txt1(rt_b1.right, rt_b1.CenterPoint().y);
	CGPointGP gTxt1;
	pDC->DPtoGeoP(&gTxt1, &txt1);
	CString strText = "test";
	pDC->SetTextAlign(TA_LEFT);
	pDC->TextOut(&gTxt1, strText);
	
	pDC->SetTextColor(oldColor2);
	pDC->SelectObject(def_font);
	font.DeleteObject();
	
	delete[] pStyle1;
	delete[] pStyle2;
}

void CGMapDisplayMgr::DrawPost(CGDCGeo *pDC, void *pCurSel, int iSelID)
{
	if(pCurSel == NULL)
	{
		if(m_pSelmemDC)
		{
			m_pSelmemDC->DeleteDC();
			delete m_pSelmemDC;
			m_pSelmemDC = NULL;
		}
		return;
	}
	
	if(CBAMgr::GetDispMapType() != 1)
	{
		if(m_pSelmemDC)
		{
			m_pSelmemDC->DeleteDC();
			delete m_pSelmemDC;
			m_pSelmemDC = NULL;
		}
		m_pSelmemDC = new CGDCGeo();
		m_pSelmemDC->CreateCompatibleDC(pDC);
		shpDrawPost(pDC, m_dZoomFactor, pCurSel, iSelID);
	}
}

void CGMapDisplayMgr::SetInitInfo(IGViewInfo* pViewInfo, CGMapInfo* pMapInfo)
{
	m_ifViewInfo = pViewInfo;
	m_pMapInfo = pMapInfo;	
}

void CGMapDisplayMgr::SetParentWnd(CWnd* pWnd)
{
	m_pWnd = pWnd;
}


double CGMapDisplayMgr::GetScreenScale(CGDCGeo* pDC, CGRectGP *rect)
{
	if(!m_pWnd)
		return -1;
	m_dPixelPerMM = GetPixelPerMM(pDC);
	CGPointGP pSrcPtGeo[2];
	pSrcPtGeo[0].m_P.lon = (rect->TopLeft()).m_P.lon;
	pSrcPtGeo[0].m_P.lat = 0;
	pSrcPtGeo[1].m_P.lon = (rect->BottomRight()).m_P.lon;
	pSrcPtGeo[1].m_P.lat = 0;
	
	CPoint pt[2];
	pDC->GeoPtoDP(pt, &pSrcPtGeo[0], 2);
	if(pSrcPtGeo[0].m_P.lon < 0.0)
		return 1.0;
	
	pDC->DPtoLP(pt, 2);
	double dMapWidth = CGLib::GetDistance(pt[0], pt[1]);
	
	if(dMapWidth < 0.0)
		dMapWidth = 1.0;
	
	CRect rect2;
	m_pWnd->GetClientRect(&rect2);
	double dScreenWidth = rect2.Width() / m_dPixelPerMM / 1000;
	double dScreenScale = dMapWidth / dScreenWidth;
	return dScreenScale;
}

double CGMapDisplayMgr::GetPixelPerMM(CGDCGeo* pDC)
{
	int iScreenMMWidth, iScreenMMHeight;
	iScreenMMWidth = pDC->GetDeviceCaps(HORZSIZE);
	iScreenMMHeight = pDC->CDC::GetDeviceCaps(VERTSIZE);
	
	int iScreenPixelWidth, iScreenPixelHeight;
	iScreenPixelWidth = pDC->GetDeviceCaps(HORZRES);
	iScreenPixelHeight = pDC->GetDeviceCaps(VERTRES);
	
	double dPixelPerMM = (double)iScreenPixelWidth / (double)iScreenMMWidth;
	return dPixelPerMM;
}

///////////////////////////////////////////////////////////////////////////////
//
//
//
/////////////////////////////////////////////////////////////////////////////////
void CGMapDisplayMgr::DrawMapBoundary(CDC* pDC)
{
	CCoord m_Coord;
	CCoordMgr*	m_CoordMgr = GP_COORDMGR;
	
	CRect rt;// = CGLib::GetClientRect();
	m_pWnd->GetClientRect(&rt);

	CPoint ptCenter(rt.CenterPoint());
	
	CPoint leftCenter(rt.left, (rt.top+rt.bottom)/2);
	CPoint rightCenter(rt.right,((rt.top+rt.bottom)/2));
	CPoint topCenter((rt.left+rt.right)/2, rt.top);
	CPoint bottomCenter((rt.left+rt.right)/2, rt.bottom);
	int rate = ptCenter.x/3;
	
	CRect rt_b1(ptCenter.x - rate, ptCenter.y - rate, ptCenter.x + rate, ptCenter.y + rate);
	CRect rt_b2(ptCenter.x - (rate*2), ptCenter.y - (rate*2), ptCenter.x + (rate*2), ptCenter.y + (rate*2));
	
	double lx, ly, dx, dy;
	CGRectGP grt_b1, grt_b2, grt;
	//pDC->DPtoGeoP(&grt_b1, rt_b1);
	m_CoordMgr->ScreenXY2WorldXY(rt_b1.left, rt_b1.top, &lx, &ly);
	m_CoordMgr->ScreenXY2WorldXY(rt_b1.right, rt_b1.bottom, &dx, &dy);
	m_Coord.x_y_to_degrees_lat_long(lx, ly, &grt_b1.m_R.top, &grt_b1.m_R.left);
	m_Coord.x_y_to_degrees_lat_long(dx, dy, &grt_b1.m_R.bottom, &grt_b1.m_R.right);

	//pDC->DPtoGeoP(&grt_b2, rt_b2);
	m_CoordMgr->ScreenXY2WorldXY(rt_b2.left, rt_b2.top, &lx, &ly);
	m_CoordMgr->ScreenXY2WorldXY(rt_b2.right, rt_b2.bottom, &dx, &dy);
	m_Coord.x_y_to_degrees_lat_long(lx, ly, &grt_b2.m_R.top, &grt_b2.m_R.left);
	m_Coord.x_y_to_degrees_lat_long(dx, dy, &grt_b2.m_R.bottom, &grt_b2.m_R.right);

	//pDC->DPtoGeoP(&grt, rt);
	m_CoordMgr->ScreenXY2WorldXY(rt.left, rt.top, &lx, &ly);
	m_CoordMgr->ScreenXY2WorldXY(rt.right, rt.bottom, &dx, &dy);
	m_Coord.x_y_to_degrees_lat_long(lx, ly, &grt.m_R.top, &grt.m_R.left);
	m_Coord.x_y_to_degrees_lat_long(dx, dy, &grt.m_R.bottom, &grt.m_R.right);

	CGPointGP gptCenter;
	//pDC->DPtoGeoP(&gptCenter, &ptCenter);
	m_CoordMgr->ScreenXY2WorldXY(ptCenter.x, ptCenter.y, &lx, &ly);
	m_Coord.x_y_to_degrees_lat_long(lx, ly, &gptCenter.m_P.lat, &gptCenter.m_P.lon);
	
	CGPointGP gLCenter, gRCenter, gTCenter, gBCenter;
	//pDC->DPtoGeoP(&gLCenter, &leftCenter);
	m_CoordMgr->ScreenXY2WorldXY(leftCenter.x, leftCenter.y, &lx, &ly);
	m_Coord.x_y_to_degrees_lat_long(lx, ly, &gLCenter.m_P.lat, &gLCenter.m_P.lon);

	//pDC->DPtoGeoP(&gRCenter, &rightCenter);
	m_CoordMgr->ScreenXY2WorldXY(rightCenter.x, rightCenter.y, &lx, &ly);
	m_Coord.x_y_to_degrees_lat_long(lx, ly, &gRCenter.m_P.lat, &gRCenter.m_P.lon);

	//pDC->DPtoGeoP(&gTCenter, &topCenter);
	m_CoordMgr->ScreenXY2WorldXY(topCenter.x, topCenter.y, &lx, &ly);
	m_Coord.x_y_to_degrees_lat_long(lx, ly, &gTCenter.m_P.lat, &gTCenter.m_P.lon);

	//pDC->DPtoGeoP(&gBCenter, &bottomCenter);
	m_CoordMgr->ScreenXY2WorldXY(bottomCenter.x, bottomCenter.y, &lx, &ly);
	m_Coord.x_y_to_degrees_lat_long(lx, ly, &gBCenter.m_P.lat, &gBCenter.m_P.lon);
	
	LOGBRUSH logBrush1;
	logBrush1.lbColor = RGB(255,0,0);
	logBrush1.lbHatch = 0;
	logBrush1.lbStyle = BS_SOLID;
	LOGBRUSH logBrush2;
	logBrush2.lbColor = RGB(255,0,0);
	logBrush2.lbHatch = 0;
	logBrush2.lbStyle = BS_SOLID;
	DWORD  *pStyle1 = NULL;
	DWORD  *pStyle2 = NULL;
	CSize  size;
	CPen pen1, pen2, *pOldPen;
	size.cx = 1;
	pDC->DPtoLP(&size);
	
	pStyle1 = new DWORD[2];
	pStyle1[0]= size.cx;
	pStyle1[1]= size.cx;   
	pen1.CreatePen(PS_GEOMETRIC|PS_USERSTYLE|PS_ENDCAP_FLAT|PS_JOIN_BEVEL,
		size.cx, &logBrush1 , 2, pStyle1); 
	pStyle2 = new DWORD[2];
	size.cx = 2;
	pDC->DPtoLP(&size);
	pStyle2[0]= size.cx;
	pStyle2[1]= size.cx;   
	pen2.CreatePen(PS_GEOMETRIC|PS_USERSTYLE|PS_ENDCAP_FLAT|PS_JOIN_BEVEL,
		size.cx, &logBrush2 , 2, pStyle2);  
	
	pDC->SetBkMode(TRANSPARENT);
	int nRopMode = pDC->SetROP2(R2_MASKPEN);
	
	pOldPen = pDC->SelectObject(&pen1);

	CPoint LCenter, RCenter, TCenter, BCenter;
	LCenter.x = gLCenter.m_P.lon;
	LCenter.y = gLCenter.m_P.lat;

	RCenter.x = gRCenter.m_P.lon;
	RCenter.y = gRCenter.m_P.lat;

	TCenter.x = gTCenter.m_P.lon;
	TCenter.y = gTCenter.m_P.lat;

	BCenter.x = gBCenter.m_P.lon;
	BCenter.y = gBCenter.m_P.lat;

	//CrossLine
	//pDC->MoveTo(&gLCenter);
	//pDC->LineTo(&gRCenter);
	//pDC->MoveTo(&gTCenter);
	//pDC->LineTo(&gBCenter);
	pDC->MoveTo(LCenter);
	pDC->LineTo(RCenter);
	pDC->MoveTo(TCenter);
	pDC->LineTo(BCenter);
	

	pDC->SelectObject(&pen2);

	CRect r1, r2;
	r1.left = grt_b1.m_R.left;
	r1.top = grt_b1.m_R.top;
	r1.right = grt_b1.m_R.right;
	r1.bottom = grt_b1.m_R.bottom;

	r2.left = grt_b1.m_R.left;
	r2.top = grt_b1.m_R.top;
	r2.right = grt_b1.m_R.right;
	r2.bottom = grt_b1.m_R.bottom;

	//pDC->Ellipse(&grt_b1);
	//pDC->Ellipse(&grt_b2);
	pDC->Ellipse(&r1);
	pDC->Ellipse(&r2);
	
	pDC->SelectObject(pOldPen);
	pDC->SetROP2(nRopMode);
	
	USES_CONVERSION;
	LOGFONT lf;
	::ZeroMemory(&lf, sizeof(LOGFONT));
	
	int nFontSize;
	nFontSize = 14;
	CSize cSize(nFontSize, nFontSize);
	pDC->DPtoLP(&cSize);
	
	lf.lfHeight = (int)(cSize.cx);
	//lf.lfWidth = 0;
	lf.lfEscapement =  0;
	lf.lfOrientation =  0;
	lf.lfWeight = FW_REGULAR;
	lf.lfCharSet = ANSI_CHARSET;  //한글 지원
	CFont font;
	font.CreateFontIndirect(&lf);
	
	CFont *def_font  = pDC->SelectObject(&font);
	int iOldRop2  = 0; iOldRop2 = pDC->SetROP2(R2_MASKPEN);
	int iOldBkMode2 = 0; iOldBkMode2 = pDC->SetBkMode(TRANSPARENT);
	COLORREF oldColor2 = pDC->SetTextColor(RGB(0,0,0));
	
	CPoint txt1(rt_b1.right, rt_b1.CenterPoint().y);
	CGPointGP gTxt1;
	//pDC->DPtoGeoP(&gTxt1, &txt1);
	m_CoordMgr->ScreenXY2WorldXY(txt1.x, txt1.y, &lx, &ly);
	m_Coord.x_y_to_degrees_lat_long(lx, ly, &gTxt1.m_P.lat, &gTxt1.m_P.lon);

	CString strText = "test";
	pDC->SetTextAlign(TA_LEFT);
	pDC->TextOut(gTxt1.m_P.lon, gTxt1.m_P.lat, strText);
	
	pDC->SetTextColor(oldColor2);
	pDC->SelectObject(def_font);
	font.DeleteObject();
	
	delete[] pStyle1;
	delete[] pStyle2;
}

void CGMapDisplayMgr::DrawPost(CDC *pDC, void *pCurSel, int iSelID)
{
	if(pCurSel == NULL)
	{
		if(m_pSelmemDC)
		{
			m_pSelmemDC->DeleteDC();
			delete m_pSelmemDC;
			m_pSelmemDC = NULL;
		}
		return;
	}
	
	if(CBAMgr::GetDispMapType() != 1)
	{
		if(m_pSelmemDC)
		{
			m_pSelmemDC->DeleteDC();
			delete m_pSelmemDC;
			m_pSelmemDC = NULL;
		}
		m_pSelmemDC = new CGDCGeo();
		m_pSelmemDC->CreateCompatibleDC((CGDCGeo*)pDC);
		shpDrawPost(pDC, m_dZoomFactor, pCurSel, iSelID);
	}
}


double CGMapDisplayMgr::GetScreenScale(CDC* pDC, CGRectGP *rect)
{
	if(!m_pWnd)
		return -1;
	m_dPixelPerMM = GetPixelPerMM(pDC);
	CGPointGP pSrcPtGeo[2];

	pSrcPtGeo[0].m_P.lon = (rect->TopLeft()).m_P.lon;
	pSrcPtGeo[0].m_P.lat = 0;
	pSrcPtGeo[1].m_P.lon = (rect->BottomRight()).m_P.lon;
	pSrcPtGeo[1].m_P.lat = 0;
	
	CPoint pt[2];
	//pDC->GeoPtoDP(pt, &pSrcPtGeo[0], 2);

	m_pGeoObj->Forward(pt, &pSrcPtGeo[0], 2);
	pDC->LPtoDP(pt, 2);

	if(pSrcPtGeo[0].m_P.lon < 0.0)
		return 1.0;
	
	pDC->DPtoLP(pt, 2);
	double dMapWidth = CGLib::GetDistance(pt[0], pt[1]);
	
	if(dMapWidth < 0.0)
		dMapWidth = 1.0;
	
	CRect rect2;
	m_pWnd->GetClientRect(&rect2);
	double dScreenWidth = rect2.Width() / m_dPixelPerMM / 1000;
	double dScreenScale = dMapWidth / dScreenWidth;
	return dScreenScale;
}

double CGMapDisplayMgr::GetPixelPerMM(CDC* pDC)
{
	int iScreenMMWidth, iScreenMMHeight;
	iScreenMMWidth = pDC->GetDeviceCaps(HORZSIZE);
	iScreenMMHeight = pDC->GetDeviceCaps(VERTSIZE);
	
	int iScreenPixelWidth, iScreenPixelHeight;
	iScreenPixelWidth = pDC->GetDeviceCaps(HORZRES);
	iScreenPixelHeight = pDC->GetDeviceCaps(VERTRES);
	
	double dPixelPerMM = (double)iScreenPixelWidth / (double)iScreenMMWidth;
	return dPixelPerMM;
}

/*
void CGMapDisplayMgr::shpDraw(CGDCGeo* pDC, double dZoomFactor, CGRectGP rectGP)
{
	if(m_pMapInfo == NULL)
		return;
	
	CRect rect;
	GPoint gp, gpOri;
	gpOri.x = 127.42906022388939;
	gpOri.y = 38.068137052285763;
	
	rect = CGLib::GetClientRect();
	dZoomFactor/=100;
	dZoomFactor*=2.5;
	
	double left, top, right, bottom;
	left = rectGP.m_R.left;
	top = rectGP.m_R.top;
	right = rectGP.m_R.right;
	bottom = rectGP.m_R.bottom;
	
	gp.x = gpOri.x - (gpOri.x - left);
	gp.y = gpOri.y - (gpOri.y - top);
	
	EGMapScale nScale;
	int mapscale = m_pMapInfo->GetMapScale();
	switch(mapscale)
	{
	case 0 : nScale = MS_1_25000;
		break;
	case 1 : nScale = MS_1_50000;
		break;
	case 2 : nScale = MS_1_100000;
		break;
	case 3 : nScale = MS_1_250000;
		break;
	case 4 : nScale = MS_1_500000;
		break;
	case 5 : nScale = MS_1_1000000;
		break;
	case 6 : nScale = MS_1_2000000;
		break;
	}
	
	double dViewRatio = 100.0 / m_dZoomFactor;
	if(!CMapDataMgr::ExtractVectorMap(nScale, rectGP, dViewRatio))
	{
		TRACE("info: Extract VectorMap Failed!");
		return;
	}
	clock_t startTimeDraw = clock(); 

	stVObject* pVObject;
	CMapProperty* pMapProperty;
	shpObject* pShpObject;
	CMapPriority* pMapPriority;
	stVectorMap* pVectorMap;
	stMapText* pMapText;
	stTextPriority* pTextPriority;
	CPolylinePriority* pPolylinePriority;
	int iCount = CMapDataMgr::GetTextPriorityCnt();
	
	if(nScale == MS_1_1000000)
	{
		int cnt_map = CMapDataMgr::GetCurrentMapCnt();
		for(int i=0; i<cnt_map; i++)
		{
			pVectorMap = CMapDataMgr::GetCurrentMap(i);
			int cnt_obj = pVectorMap->m_pVObject.GetSize();
			for(int j=0; j<cnt_obj; j++)
			{
				pVObject = (stVObject*)pVectorMap->m_pVObject[j];
				pMapProperty = (CMapProperty*)pVObject->m_pMapProperty;
				pShpObject = (shpObject*)pVObject->m_pObject;
				// 속성 정보 check
				if(pMapProperty == NULL) continue;
				// 지도 data check
				if(pShpObject == NULL) continue;
				
				for(int k=0; k<pVObject->m_paMapPriority.GetSize(); k++)
				{
					pMapPriority = pVObject->m_paMapPriority[k];
					// 우선순위 data check
					if(pMapPriority == NULL) continue;


					// 속성이 unvisible 이면 skip
					if(pMapPriority->m_bIsVisible == FALSE) continue;
					// view ratio check
					if(dViewRatio > (double)pMapPriority->m_iViewRatio) continue;
					


					switch(pMapProperty->m_enMapProperty)
					{
					case MP_POINT : 
						{
							shpDrawPoint(pDC, dZoomFactor, gpOri, rectGP, pMapProperty, pShpObject, pMapPriority);
							break;
						}
					case MP_POLYLINE : 
						{
							// 등고선 처리
							pPolylinePriority = (CPolylinePriority*)pMapPriority;
							if(pPolylinePriority->m_strFileName == "LCA010")
							{
								pShpObject->Draw(pDC, m_lEllipsoid, dZoomFactor, gpOri, rectGP, 63, m_dScreenScale, 
									pPolylinePriority->m_uiType,		
									pPolylinePriority->m_crLineColor,	
									pPolylinePriority->m_uiLineWidth,	
									pPolylinePriority->m_uiType2,		
									pPolylinePriority->m_crLineColor2,	
									pPolylinePriority->m_uiLineWidth2);	
							}
							// 도로 처리
							else if(pPolylinePriority->m_strFileName == "LAP030")
							{
								pShpObject->Draw(pDC, dZoomFactor, gpOri, rectGP, 63, m_dScreenScale, mapscale, 
									pPolylinePriority->m_strName,		
									pPolylinePriority->m_uiType,		
									pPolylinePriority->m_crLineColor,	
									pPolylinePriority->m_uiLineWidth,	
									pPolylinePriority->m_uiType2,		
									pPolylinePriority->m_crLineColor2,	
									pPolylinePriority->m_uiLineWidth2);	
							}
							// 일반
							else
							{
								shpDrawPolyline(pDC, dZoomFactor, gpOri, rectGP, pMapProperty, pShpObject, pMapPriority);
							}
							break;
						}
					case MP_POLYGON : 
						{
							shpDrawPolygon(pDC, dZoomFactor, gpOri, rectGP, pMapProperty, pShpObject, pMapPriority);
							break;
						}
					default : 
						{
							TRACE("error: undeclared map property type!(CGMapDisplayMgr::shpDraw)\n");
							break;
						}
					}	// end switch
				}	// end for k
			}	// end for j
			
			// text data display
			for(j=0; j<iCount; j++)
			{
				pTextPriority = (stTextPriority*)CMapDataMgr::GetTextPriority(j);
				if(nScale == pTextPriority->m_enMapScale && pTextPriority->m_bIsVisible == TRUE)
				{
					dViewRatio = 100.0 / m_dZoomFactor;
					if(dViewRatio > (double)pTextPriority->m_uiViewRatio) continue;
					for(j=0; j<pVectorMap->m_pMapText.GetSize(); j++)
					{
						pMapText = (stMapText*)pVectorMap->m_pMapText[j];
						shpDrawText(pDC, pTextPriority, pMapText);
					}
					break;
				}
			}
		}	// end for i
	}
	else
	{
		UINT cnt_map_list = CMapDataMgr::GetCurrentMapListCnt();
		for(int i=0; i< cnt_map_list; i++)
		{
			pVectorMap = CMapDataMgr::GetCurrentMapList(i);
			int cnt_vec_obj = pVectorMap->m_pVObject.GetSize();
			for(int j=0; j < cnt_vec_obj; j++)
			{
				pVObject = (stVObject*)pVectorMap->m_pVObject[j];
				pMapProperty = (CMapProperty*)pVObject->m_pMapProperty;
				pShpObject = (shpObject*)pVObject->m_pObject;
				// 속성 정보 check
				if(pMapProperty == NULL) continue;
				// 지도 data check
				if(pShpObject == NULL) continue;
				for(int k = 0; k < pVObject->m_paMapPriority.GetSize(); k++)
				{
					pMapPriority = pVObject->m_paMapPriority[k];
					// 우선순위 data check
					if(pMapPriority == NULL) continue;
					// 속성이 unvisible 이면 skip
					if(pMapPriority->m_bIsVisible == FALSE) continue;
					// view ratio check
					if(dViewRatio > (double)pMapPriority->m_iViewRatio) continue;
					
					switch(pMapProperty->m_enMapProperty)
					{
					case MP_POINT :
						{
							shpDrawPoint(pDC, dZoomFactor, gpOri, rectGP, pMapProperty, pShpObject, pMapPriority);
							break;
						}
					case MP_POLYLINE : 
						{
							// 등고선 처리
							pPolylinePriority = (CPolylinePriority*)pMapPriority;
							if(pPolylinePriority->m_strFileName == "LCA010")
							{
								pShpObject->Draw(pDC, m_lEllipsoid, dZoomFactor, gpOri, rectGP, 63, m_dScreenScale, 
									pPolylinePriority->m_uiType, 
									pPolylinePriority->m_crLineColor, 
									pPolylinePriority->m_uiLineWidth, 
									pPolylinePriority->m_uiType2, 
									pPolylinePriority->m_crLineColor2, 
									pPolylinePriority->m_uiLineWidth2);
							}
							// 도로 처리
							else if(pPolylinePriority->m_strFileName == "LAP030")
							{
								pShpObject->Draw(pDC, dZoomFactor, gpOri, rectGP, 63, m_dScreenScale, mapscale, 
									pPolylinePriority->m_strName, 
									pPolylinePriority->m_uiType, 
									pPolylinePriority->m_crLineColor, 
									pPolylinePriority->m_uiLineWidth, 
									pPolylinePriority->m_uiType2, 
									pPolylinePriority->m_crLineColor2, 
									pPolylinePriority->m_uiLineWidth2);
							}
							else
							{
								shpDrawPolyline(pDC, dZoomFactor, gpOri, rectGP, pMapProperty, pShpObject, pMapPriority);
							}
							break;
						}
					case MP_POLYGON : 
						{
							shpDrawPolygon(pDC, dZoomFactor, gpOri, rectGP, pMapProperty, pShpObject, pMapPriority);
							break;
						}
					default : 
						{
							TRACE("error: undeclared map property type!(CGMapDisplayMgr::shpDraw)\n");
							break;
						}
					}	// end switch
				}	// end for k
			}	// end for j
			
			// text data display
			for(j=0; j<iCount; j++)
			{
				pTextPriority = (stTextPriority*)CMapDataMgr::GetTextPriority(j);
				if(nScale == pTextPriority->m_enMapScale && pTextPriority->m_bIsVisible == TRUE)
				{
					dViewRatio = 100.0 / m_dZoomFactor;
					if(dViewRatio > (double)pTextPriority->m_uiViewRatio) continue;
					for(j=0; j<pVectorMap->m_pMapText.GetSize(); j++)
					{
						pMapText = (stMapText*)pVectorMap->m_pMapText[j];
						shpDrawText(pDC, pTextPriority, pMapText);
					}
					break;
				}
			}
		}	// end for i
	}	

	clock_t finishTimeDraw = clock();
 	double drawDuration;
	drawDuration = (double)(finishTimeDraw - startTimeDraw) / CLOCKS_PER_SEC;
	CString strDrawTime;
 	strDrawTime.Format("\nshpDraw 소요시간(%5.3f Sec)", drawDuration);
	TRACE("info: %s\n", strDrawTime);
}
*/

void CGMapDisplayMgr::shpDrawPost(CGDCGeo *pDC, double dZoomFactor, void *pCurSel, int iSelID)
{
	return;
	if(m_pMapInfo == NULL)
		return;
	
	CRect rect;
	GPoint gp, gpOri;
	stVObject *pCurSelObject;
	CMapPriority *pPriority;
	CMapProperty *pProperty;
//	shpObject *pShpObject;
	
	gpOri.x = 127.42906022388939;
	gpOri.y = 38.068137052285763;
	
	rect = CGLib::GetClientRect();
	dZoomFactor/=100;
	dZoomFactor*=2.5;
	
	double left, top, right, bottom;
	//m_ifViewInfo->GetSelectedRange(&left, &top, &right, &bottom);
	
	CGRectGP rectGP(left, top, right, bottom);
	
	gp.x = gpOri.x - (gpOri.x - left);
	gp.y = gpOri.y - (gpOri.y - top);
	
	if(pCurSel != NULL)
	{
		pCurSelObject = (stVObject *)pCurSel;
		pProperty = pCurSelObject->m_pMapProperty;
//		pShpObject = pCurSelObject->m_pObject;
		
		for(int i=0; i<pCurSelObject->m_paMapPriority.GetSize(); i++)
		{
			pPriority = pCurSelObject->m_paMapPriority[i];
//			if(pPriority != NULL && pProperty != NULL && pShpObject != NULL && pPriority->m_bIsVisible == TRUE)
			{
				switch(pProperty->m_enMapProperty)
				{
				case MP_POINT : 
					{
//						shpDrawPoint(pDC, dZoomFactor, gpOri, rectGP, pProperty, pShpObject, pPriority, iSelID);
						break;
					}
				case MP_POLYLINE : 
					{
//						shpDrawPolyline(pDC, dZoomFactor, gpOri, rectGP, pProperty, pShpObject, pPriority, iSelID);
						break;
					}
				case MP_POLYGON : 
					{
//						shpDrawPolygon(pDC, dZoomFactor, gpOri, rectGP, pProperty, pShpObject, pPriority, iSelID);
						break;
					}
				default : 
					{
						TRACE("error: undeclared map property type!(CGMapDisplayMgr::shpDrawPost)\n");
						break;
					}
				}
			}
			break;
		}
	}
	
	if(m_pSelmemDC != NULL)
		pDC->BitBlt(rectGP.m_R.left, rectGP.m_R.top, rectGP.Width(), -rectGP.Height(),
		m_pSelmemDC, rectGP.m_R.left, rectGP.m_R.top, SRCCOPY);
}

// 점 데이터 복원도시(일반)
BOOL CGMapDisplayMgr::shpDrawPoint(CGDCGeo* pDC, double dZoomFactor, GPoint gpOri, CGRectGP rectGP,
								   CMapProperty* pMapProperty, shpObject* pShpObject, CMapPriority* pMapPriority)
{
	CPen pen, *oldPen;
	CBrush brush, *oldbrush;
	CSize size;
	stSymbol* pSymbol;
	CBitmap* pBitmap;
	COLORREF colorTransparent;
	double len;
	
	// 속성정보 적용
	CPointPriority* pPriority = (CPointPriority*)pMapPriority;
	pSymbol = pPriority->m_pSymbol;
	
	// line width (default set)
	size.cx = 1;

	// radius
	len = pPriority->m_uiRadius + 1;
	pDC->DPtoLP(&size);
	
	if(pPriority->m_uiType != 2 && pSymbol == NULL)
	{
		// transparent		
		// object create
		pen.CreatePen(PS_SOLID, size.cx, pPriority->m_crFillColor);
		brush.CreateSolidBrush(pPriority->m_crFillColor);
		oldPen = pDC->SelectObject(&pen);
		oldbrush = pDC->SelectObject(&brush);
	}
	else if(pPriority->m_uiType == 2 && pSymbol != NULL)
	{
		pBitmap = CBitmap::FromHandle(pSymbol->m_hBitmap);
		colorTransparent = RGB(255,255,255);
	}
	
	pShpObject->SetPointType(pPriority->m_uiType, len);
	
	// point type set
	switch(pPriority->m_uiType)
	{
	// circle, square
	case 0 :
	case 1 :
		pShpObject->Draw(pDC, m_lEllipsoid, dZoomFactor, gpOri, rectGP, 63);
		break;
	// symbol
	case 2 :
		if(pSymbol == NULL)
			pShpObject->Draw(pDC, m_lEllipsoid, dZoomFactor, gpOri, rectGP, 63);
		//else if(pPriority->m_uiType == 2 && pSymbol != NULL)
		//	pShpObject->Draw(pDC, m_lEllipsoid, dZoomFactor, gpOri, rectGP, 63, pBitmap, colorTransparent);
		break;
	}
	
	//if(pPriority->m_uiType != 2 && pSymbol == NULL)
	//{
	//	pDC->SelectObject(oldPen);
	//	pDC->SelectObject(oldbrush);
	//	pen.DeleteObject();
	//	brush.DeleteObject();
	//}
	
	return TRUE;
}

// 점 데이터 복원도시(지형요소속성보기)
BOOL CGMapDisplayMgr::shpDrawPoint(CGDCGeo* pDC, double dZoomFactor, GPoint gpOri, CGRectGP rectGP,
								   CMapProperty* pMapProperty, shpObject* pShpObject, CMapPriority* pMapPriority, int iSelID)
{
	CPen pen, *oldPen;
	CBrush brush, *oldBrush;
	LOGBRUSH logBrush;
	::ZeroMemory(&logBrush, sizeof(LOGBRUSH));
	CSize size;
	stSymbol* pSymbol;
	COLORREF crLineColor, crFillColor;
	double len;
	
	// 속성정보 적용
	CPointPriority* pPriority = (CPointPriority*)pMapPriority;
	pSymbol = pPriority->m_pSymbol;
	
	// line width (default set)
	size.cx = 1;
	// radius
	len = pPriority->m_uiRadius + 1;
	pDC->DPtoLP(&size);
	pShpObject->SetPointType(pPriority->m_uiType, len);
	
	crLineColor = RGB(0, 0, 0);
	crFillColor = RGB(0, 210, 210);
	
	pen.CreatePen(PS_SOLID, size.cx, crLineColor);
	brush.CreateSolidBrush(crFillColor);
	oldPen = pDC->SelectObject(&pen);
	oldBrush = pDC->SelectObject(&brush);
	int iOldRop = pDC->SetROP2(R2_NOTXORPEN);
	int iOldBkMode = pDC->SetBkMode(TRANSPARENT);
	pShpObject->DrawSelected(pDC, iSelID);
	pDC->SetROP2(iOldRop);
	pDC->SetBkMode(iOldBkMode);
	pDC->SelectObject(oldPen);
	pDC->SelectObject(oldBrush);
	pen.DeleteObject();
	brush.DeleteObject();
	
	return TRUE;
}

// 선 데이터 복원도시(일반)
BOOL CGMapDisplayMgr::shpDrawPolyline(CGDCGeo* pDC, double dZoomFactor, GPoint gpOri, CGRectGP rectGP,
									  CMapProperty* pMapProperty, shpObject* pShpObject, CMapPriority* pMapPriority)
{
	CPen pen, *oldPen;
	CSize size, size1, size3;
	int iwidth;
	LOGBRUSH logBrush;
	
	// 속성정보 적용
	CPolylinePriority* pItem = (CPolylinePriority*)pMapPriority;
	
	// line width
	size.cx = pItem->m_uiLineWidth;
	pDC->DPtoLP(&size);
	
	// color
	COLORREF color = pItem->m_crLineColor;
	
	logBrush.lbColor = color;
	logBrush.lbHatch = 10;
	logBrush.lbStyle = BS_SOLID;
	DWORD *pStyle = NULL;
	
	// line style
	switch(pItem->m_uiType)
	{
		// solid
	case 0 : pen.CreatePen(PS_SOLID, size.cx, color);
		break;
		// dash
	case 1 : pStyle = new DWORD[2];
		pStyle[0]= size.cx*10;
		pStyle[1]= size.cx*2;
		pen.CreatePen(PS_GEOMETRIC|PS_USERSTYLE|PS_ENDCAP_FLAT|PS_JOIN_BEVEL,
			size.cx, &logBrush, 2, pStyle);
		break;
		// dot
	case 2 : pStyle = new DWORD[2];
		pStyle[0]= size.cx;
		pStyle[1]= size.cx;
		pen.CreatePen(PS_GEOMETRIC|PS_USERSTYLE|PS_ENDCAP_FLAT|PS_JOIN_BEVEL,
			size.cx, &logBrush, 2, pStyle);
		break;
		// dashdot
	case 3 : pStyle = new DWORD[4];
		pStyle[0]= size.cx*10;
		pStyle[1]= size.cx;
		pStyle[2]= size.cx*2;
		pStyle[3]= size.cx;
		pen.CreatePen(PS_GEOMETRIC|PS_USERSTYLE|PS_ENDCAP_FLAT|PS_JOIN_BEVEL,
			size.cx, &logBrush, 4, pStyle);
		break;
		// dashdotdot
	case 4 : pStyle = new DWORD[6];
		pStyle[0]= size.cx*10;
		pStyle[1]= size.cx;
		pStyle[2]= size.cx*2;
		pStyle[3]= size.cx;
		pStyle[4]= size.cx*2;
		pStyle[5]= size.cx;
		pen.CreatePen(PS_GEOMETRIC|PS_USERSTYLE|PS_ENDCAP_FLAT|PS_JOIN_BEVEL,
			size.cx, &logBrush, 6, pStyle);
		break;
		// null line
	case 9 : break;
		// default
	default : pen.CreatePen(PS_SOLID, size.cx, color);
		break;
	}
	
	if(pItem->m_uiType != 9)
	{
		oldPen = pDC->SelectObject(&pen); 
 		pShpObject->Draw(pDC, m_lEllipsoid, dZoomFactor, gpOri, rectGP, 63);
		pDC->SelectObject(oldPen);
		pen.DeleteObject();
		
		delete [] pStyle;
		pStyle = NULL;
	}
	
	// pattern 적용
	// 일반
	if(pItem->m_uiType2 != 999 && pItem->m_uiType2 <= 4)
	{
		::ZeroMemory(&logBrush, sizeof(LOGBRUSH));
		size.cx = pItem->m_uiLineWidth2;
		size.cy = pItem->m_uiLineWidth2;
		pDC->DPtoLP(&size);
		iwidth = (int)(size.cx / m_dScreenScale);
		
		double Widlate = 0.21;
		int lgWidth = (int)((double)iwidth * Widlate);
		
		logBrush.lbStyle = BS_SOLID;
		logBrush.lbColor = pItem->m_crLineColor2;
		
		switch(pItem->m_uiType2)
		{
			// solid
		case 0 : pen.CreatePen(PS_SOLID, lgWidth, pItem->m_crLineColor2);
			break;
			// dash
		case 1 : pStyle = new DWORD[2];
			pStyle[0]= size.cx*10;
			pStyle[1]= size.cx*2;
			pen.CreatePen(PS_GEOMETRIC|PS_USERSTYLE|PS_ENDCAP_FLAT|PS_JOIN_BEVEL,
				lgWidth, &logBrush, 2, pStyle);
			break;
			// dot
		case 2 : pStyle = new DWORD[2];
			pStyle[0]= size.cx;
			pStyle[1]= size.cx;
			pen.CreatePen(PS_GEOMETRIC|PS_USERSTYLE|PS_ENDCAP_FLAT|PS_JOIN_BEVEL,
				lgWidth, &logBrush, 2, pStyle);
			break;
			// dashdot
		case 3 : pStyle = new DWORD[4];
			pStyle[0]= size.cx*10;
			pStyle[1]= size.cx;
			pStyle[2]= size.cx*2;
			pStyle[3]= size.cx;
			pen.CreatePen(PS_GEOMETRIC|PS_USERSTYLE|PS_ENDCAP_FLAT|PS_JOIN_BEVEL,
				lgWidth, &logBrush, 4, pStyle);
			break;
			// dashdotdot
		case 4 : pStyle = new DWORD[6];
			pStyle[0]= size.cx*10;
			pStyle[1]= size.cx;
			pStyle[2]= size.cx*2;
			pStyle[3]= size.cx;
			pStyle[4]= size.cx*2;
			pStyle[5]= size.cx;
			pen.CreatePen(PS_GEOMETRIC|PS_USERSTYLE|PS_ENDCAP_FLAT|PS_JOIN_BEVEL,
				lgWidth, &logBrush, 6, pStyle);
			break;
			// default
		default : pen.CreatePen(PS_SOLID, lgWidth, pItem->m_crLineColor2);
			break;
		}
		
		oldPen = pDC->SelectObject(&pen);
		pShpObject->Draw(pDC, m_lEllipsoid, dZoomFactor, gpOri, rectGP, 63);
		pDC->SelectObject(oldPen);
		pen.DeleteObject();
		
		delete [] pStyle;
		pStyle = NULL;
	}
	// 교량
	else if(pItem->m_uiType2 != 999 && pItem->m_uiType2 == 5)
	{
	}
	// 절토지
	else if(pItem->m_uiType2 != 999 && pItem->m_uiType2 == 6)
	{
		::ZeroMemory(&logBrush, sizeof(LOGBRUSH));
		logBrush.lbStyle = BS_SOLID;
		logBrush.lbColor = pItem->m_crLineColor2;
		
		int iWidth = pItem->m_uiLineWidth;
		size.cx = iWidth * 4.0; // 티크의 길이
		size1.cx = iWidth; // 티크의 굵기
		if(size1.cx <= 0) size1.cx = 1.0;
		
		pDC->DPtoLP(&size);
		pDC->DPtoLP(&size1);
		pStyle = new DWORD[2];
		
		//티크의 간격
		size3.cx = iWidth * 1.2;
		size3.cy = iWidth * 1.2;
		
		pDC->DPtoLP(&size3);
		pStyle[0] = size1.cx;
		
		pStyle[1] = (UINT)(size3.cx * 2.0);
		pen.CreatePen(PS_GEOMETRIC|PS_USERSTYLE|PS_ENDCAP_FLAT|PS_JOIN_BEVEL, size.cx, &logBrush,2,pStyle);
		oldPen = pDC->SelectObject(&pen);
		pShpObject->Draw(pDC, m_lEllipsoid, dZoomFactor, gpOri, rectGP, 63);
		pDC->SelectObject(oldPen);
		pen.DeleteObject();
		delete [] pStyle;
		pStyle = NULL;
	}
	// 철도/제방
	else if(pItem->m_uiType2 != 999 && pItem->m_uiType2 == 7)
	{
		::ZeroMemory(&logBrush, sizeof(LOGBRUSH));
		logBrush.lbStyle = BS_SOLID;
		logBrush.lbColor = pItem->m_crLineColor2;
		
		int iWidth = pItem->m_uiLineWidth;
		size.cx = iWidth * 4.0;
		size1.cx = iWidth;
		
		pDC->DPtoLP(&size);
		pDC->DPtoLP(&size1);
		pStyle = new DWORD[2];
		
		//티크의 간격
		size3.cx = iWidth * 2.0;
		size3.cy = iWidth * 2.0;
		
		pDC->DPtoLP(&size3);
		pStyle[0] = size1.cx;
		pStyle[1] = (UINT)(size3.cx * 2.0);
		pen.CreatePen(PS_GEOMETRIC|PS_USERSTYLE|PS_ENDCAP_FLAT|PS_JOIN_BEVEL, size.cx, &logBrush,2,pStyle);
		oldPen = pDC->SelectObject(&pen);
		pShpObject->Draw(pDC, m_lEllipsoid, dZoomFactor, gpOri, rectGP, 63);
		pDC->SelectObject(oldPen);
		pen.DeleteObject();
		delete [] pStyle;
		pStyle = NULL;
	}
	// 터널
	else if(pItem->m_uiType2 != 999 && pItem->m_uiType2 == 8)
	{
		::ZeroMemory(&logBrush, sizeof(LOGBRUSH));
		size.cx = pItem->m_uiLineWidth2;
		size.cy = pItem->m_uiLineWidth2;
		pDC->DPtoLP(&size);
		iwidth = (int)(size.cx / m_dScreenScale);
		
		double Widlate = 0.21;
		int lgWidth = (int)((double)iwidth * Widlate);
		
		logBrush.lbStyle = BS_SOLID;
		logBrush.lbColor = pItem->m_crLineColor2;
		
		pStyle = new DWORD[2];
		pStyle[0]= size.cx;
		pStyle[1]= size.cx;
		pen.CreatePen(PS_GEOMETRIC|PS_USERSTYLE|PS_ENDCAP_FLAT|PS_JOIN_BEVEL,
			lgWidth, &logBrush, 2, pStyle);
		oldPen = pDC->SelectObject(&pen);
		pShpObject->Draw(pDC, m_lEllipsoid, dZoomFactor, gpOri, rectGP, 63);
		pDC->SelectObject(oldPen);
		pen.DeleteObject();
		
		delete [] pStyle;
		pStyle = NULL;
	}	
	return TRUE;
}

// 선 데이터 복원도시(지형요소속성보기)
BOOL CGMapDisplayMgr::shpDrawPolyline(CGDCGeo* pDC, double dZoomFactor, GPoint gpOri, CGRectGP rectGP,
									  CMapProperty* pMapProperty, shpObject* pShpObject, CMapPriority* pMapPriority, int iSelID)
{
	CPen pen, *oldPen;
	CBrush brush, *oldBrush;
	LOGBRUSH logBrush;
	::ZeroMemory(&logBrush, sizeof(LOGBRUSH));
	CSize size;
	COLORREF crLineColor, crFillColor;
	
	CPolylinePriority* pItem = (CPolylinePriority*)pMapPriority;
	
	// line width
	size.cx = pItem->m_uiLineWidth + 1;
	pDC->DPtoLP(&size);
	
	// color
	crLineColor = RGB(0, 210, 210);
	crFillColor = RGB(0, 210, 210);
	
	pen.CreatePen(PS_SOLID, size.cx, crLineColor);
	brush.CreateSolidBrush(crFillColor);
	oldPen = pDC->SelectObject(&pen);
	oldBrush = pDC->SelectObject(&brush);
	pShpObject->DrawSelected(pDC, iSelID);
	pDC->SelectObject(oldPen);
	pDC->SelectObject(oldBrush);
	pen.DeleteObject();
	brush.DeleteObject();
	
	return TRUE;
}

// 면 데이터 복원도시(일반)
BOOL CGMapDisplayMgr::shpDrawPolygon(CGDCGeo* pDC, double dZoomFactor, GPoint gpOri, CGRectGP rectGP,
									 CMapProperty* pMapProperty, shpObject* pShpObject, CMapPriority* pMapPriority)
{
	CPen pen, *oldPen;
	CBrush brush, *oldBrush;
	LOGBRUSH logBrush;
	::ZeroMemory(&logBrush, sizeof(LOGBRUSH));
	CSize size;
	COLORREF crLineColor, crFillColor;
	CBitmap* pBitmap;
	
	// 속성정보 적용
	CPolygonPriority* pPriority = (CPolygonPriority*)pMapPriority;
	
	stSymbol* pPattern = pPriority->m_pPattern;
	
	// line width
	size.cx = pPriority->m_uiLineWidth;
	pDC->DPtoLP(&size);
	
	crLineColor = pPriority->m_crLineColor;
	crFillColor = pPriority->m_crFillColor;
	
	// type
	switch(pPriority->m_uiType)
	{
		// normal
	case 0 : 
		{
			if(CBAMgr::GetDispMapType() == 0)
			{
				pen.CreatePen(PS_SOLID, size.cx, crLineColor);
				brush.CreateSolidBrush(crFillColor);
				oldPen = pDC->SelectObject(&pen);
				oldBrush = pDC->SelectObject(&brush);
				pShpObject->Draw(pDC, m_lEllipsoid, dZoomFactor, gpOri, rectGP, 63);
				pDC->SelectObject(oldPen);
				pDC->SelectObject(oldBrush);
				pen.DeleteObject();
				brush.DeleteObject();
			}
			else if(CBAMgr::GetDispMapType() == 2)
			{
				pen.CreatePen(PS_SOLID, size.cx, crLineColor);
				oldPen = pDC->SelectObject(&pen);
				pDC->SetBkMode(TRANSPARENT);
				int nRopMode = pDC->SetROP2(R2_MASKPEN);
				pShpObject->Draw(pDC, m_lEllipsoid, dZoomFactor, gpOri, rectGP, 63);
				pDC->SelectObject(oldPen);
				pen.DeleteObject();
			}
		}
		break;
		// pattern
	case 1 : 
		{
			if(pPattern == NULL)
			{
				if(CBAMgr::GetDispMapType() == 0)
				{
					pen.CreatePen(PS_SOLID, size.cx, crLineColor);
					brush.CreateSolidBrush(crFillColor);
					oldPen = pDC->SelectObject(&pen);
					oldBrush = pDC->SelectObject(&brush);
					pShpObject->Draw(pDC, m_lEllipsoid, dZoomFactor, gpOri, rectGP, 63);
					pDC->SelectObject(oldPen);
					pDC->SelectObject(oldBrush);
					pen.DeleteObject();
					brush.DeleteObject();
				}
				else if(CBAMgr::GetDispMapType() == 2)
				{
					pen.CreatePen(PS_SOLID, size.cx, crLineColor);
					oldPen = pDC->SelectObject(&pen);
					pDC->SetBkMode(TRANSPARENT);
					int nRopMode = pDC->SetROP2(R2_MASKPEN);
					pShpObject->Draw(pDC, m_lEllipsoid, dZoomFactor, gpOri, rectGP, 63);
					pDC->SelectObject(oldPen);
					pen.DeleteObject();
				}
			}
			else
			{
				pBitmap = CBitmap::FromHandle(pPattern->m_hBitmap);
				brush.CreatePatternBrush(pBitmap);
				brush.GetLogBrush(&logBrush);
				pen.CreatePen(PS_GEOMETRIC|PS_ENDCAP_FLAT, size.cx, &logBrush);
				oldPen = pDC->SelectObject(&pen);
				oldBrush = pDC->SelectObject(&brush);
				int iOldRop = pDC->SetROP2(R2_MASKPEN);
				int iOldBkMode = pDC->SetBkMode(TRANSPARENT);
				pShpObject->Draw(pDC, m_lEllipsoid, dZoomFactor, gpOri, rectGP, 63);
				pDC->SetROP2(iOldRop);
				pDC->SetBkMode(iOldBkMode);
				pDC->SelectObject(oldPen);
				pDC->SelectObject(oldBrush);
				pen.DeleteObject();
				brush.DeleteObject();
			}
		}
		break;
	}
	
	return TRUE;
}

// 면 데이터 복원도시(지형요소속성보기)
BOOL CGMapDisplayMgr::shpDrawPolygon(CGDCGeo* pDC, double dZoomFactor, GPoint gpOri, CGRectGP rectGP, 
									 CMapProperty* pMapProperty, shpObject* pShpObject, CMapPriority* pMapPriority, int iSelID)
{
	CPen pen, *oldPen;
	CBrush brush, *oldBrush;
	LOGBRUSH logBrush;
	::ZeroMemory(&logBrush, sizeof(LOGBRUSH));
	CSize size;
	DWORD *pStyle = NULL;
	COLORREF crLineColor, crFillColor;
	//	WORD HatchPattern[8] = {0,0,0,0,0,0,0,0};
	//	WORD pPattern2[8] = { 0xaaaa, 0x5555, 0xaaaa, 0x5555, 0xaaaa, 0x5555, 0xaaaa, 0x5555 };
	//	HBITMAP hBitMap;
	//	CBitmap* pBitmap;
	//	for(int i=0; i<8; i++)
	//		HatchPattern[i] |= pPattern2[i];
	//	hBitMap = ::CreateBitmap(8, 8, 1, 1, HatchPattern);
	
	// 속성정보 적용
	CPolygonPriority* pPriority = (CPolygonPriority*)pMapPriority;
	
	stSymbol* pPattern = pPriority->m_pPattern;
	
	// line width
	size.cx = pPriority->m_uiLineWidth;
	pDC->DPtoLP(&size);
	
	crLineColor = RGB(0, 0, 0);
	crFillColor = RGB(0, 210, 210);
	
	pen.CreatePen(PS_SOLID, size.cx, crLineColor);
	brush.CreateSolidBrush(crFillColor);
	oldPen = pDC->SelectObject(&pen);
	oldBrush = pDC->SelectObject(&brush);
	int iOldRop = pDC->SetROP2(R2_NOTXORPEN);
	int iOldBkMode = pDC->SetBkMode(TRANSPARENT);
	pShpObject->DrawSelected(pDC, iSelID);
	pDC->SetROP2(iOldRop);
	pDC->SetBkMode(iOldBkMode);
	pDC->SelectObject(oldPen);
	pDC->SelectObject(oldBrush);
	pen.DeleteObject();
	brush.DeleteObject();
	
	return TRUE;
}

void CGMapDisplayMgr::shpDrawText(CGDCGeo* pDC, stTextPriority* pPriority, stMapText* pMapText)
{
	if(pPriority == NULL || pMapText == NULL)
		return;
	if(!m_ClientRectGp.PtInRect(pMapText->m_gpPoint))
		return;
	if(!pPriority->m_strWorldMapPoint.IsEmpty())
		return;
	
	CString strName = pMapText->m_strName.Mid(0,2);
	BYTE byte[2];
	memset(byte, ' ', 2);
	memcpy(byte, (LPCTSTR)strName, strName.GetLength());
	
	CString strByte;
	strByte.Format("%d", byte[0]);
	int iByte = atoi(strByte);
	BOOL bRes = FALSE;
	
	switch(pPriority->m_uiVisibleType)
	{
		// 한글만 출력
	case 0 : 
		if(iByte >=0 && iByte <= 127) bRes = TRUE;
		break;
		// 영문만 출력
	case 1 : 
		if(iByte > 127) bRes = TRUE;
		break;
		// 한글+영문 출력
	case 2 :
		break;
	default :
		break;
	}
	
	if(bRes) return;
	
	USES_CONVERSION;
	LOGFONT lf;
	::ZeroMemory(&lf, sizeof(LOGFONT));
	
	int nFontSize = pPriority->m_uiFontSize;
	CSize cSize(nFontSize, nFontSize);
	pDC->DPtoLP(&cSize);
	
	lf.lfHeight = (int)(cSize.cx);
	//lf.lfWidth = 0;
	lf.lfEscapement = 0;
	lf.lfOrientation = 0;
	lf.lfWeight = FW_REGULAR;
	
	CString strFaceName;
	stFontType* pFontType = pPriority->m_pFontType;
	if(pFontType == NULL)
	{
		lf.lfCharSet = ANSI_CHARSET;  //한글 지원
		strFaceName = "굴림";
		lstrcpynA(lf.lfFaceName, T2A((LPTSTR)(LPCTSTR)strFaceName), LF_FACESIZE);
	}
	else
	{		
		lf.lfCharSet = pFontType->m_nCharSet;
		lf.lfPitchAndFamily = pFontType->m_nPitchAndFamily;
		strFaceName = pFontType->m_strName;
		lstrcpynA(lf.lfFaceName, T2A((LPTSTR)(LPCTSTR)strFaceName), LF_FACESIZE);
	}
	
	CFont font;
	font.CreateFontIndirect(&lf);
	
	CFont *def_font	= pDC->SelectObject(&font);
	int iOldRop2 = pDC->SetROP2(R2_MASKPEN);
	int iOldBkMode2 = pDC->SetBkMode(TRANSPARENT);
	COLORREF oldColor2 = pDC->SetTextColor(pPriority->m_clFontColor);
	pDC->SetTextAlign(TA_LEFT);
	
	pDC->TextOut(&pMapText->m_gpPoint, pMapText->m_strName);
	
	pDC->SetROP2(iOldRop2);
	pDC->SetBkMode(iOldBkMode2);
	pDC->SetTextColor(oldColor2);
	pDC->SelectObject(def_font);
	font.DeleteObject();
}

void CGMapDisplayMgr::SelMarkingDraw(CGDCGeo *pDC, double dZoomFactor, CGRectGP rectGP, CGPointGP gpoint)
{
	stSymbol* pSymbol = CMapDataMgr::GetSelPointSymbol();
	if(pSymbol == NULL) return;
	
	CRect rect;
	GPoint gpOri;
	gpOri.x = 127.42906022388939;
	gpOri.y = 38.068137052285763;
	
	dZoomFactor/=100;
	dZoomFactor*=2.5;
	
	CBitmap* pBitmap;
	COLORREF colorTransparent;
	CGDCGeo memDC, maskDC, tempDC;
	CBitmap maskBitmap;
	CBitmap bmpImage ;
	BITMAP bm;
	CBitmap *pOldBitmap, *pOldmaskBmp, *pold;
	CSize widthLP, widthDP;
	CPoint Src;
	CGPointGP GpSrc;
	POINT DestLP;
	CGPointGP DestGP;
	
	pBitmap = CBitmap::FromHandle(pSymbol->m_hBitmap);
	colorTransparent = RGB(255,255,255);
	
	if(pBitmap == NULL) return;
	if(pBitmap->m_hObject == NULL) return;
	if(pBitmap->GetBitmap(&bm) == 0) return;
	
	// DC saving..
	int savedDC = pDC->SaveDC();
	
	maskDC.CreateCompatibleDC(pDC);
	memDC.CreateCompatibleDC(pDC);
	tempDC.CreateCompatibleDC(pDC);
	
	pold = tempDC.SelectObject(pBitmap);
	
	bmpImage.CreateCompatibleBitmap(pDC,bm.bmWidth,bm.bmHeight);
	pOldBitmap = memDC.SelectObject(&bmpImage);
	
	memDC.CDC::SetStretchBltMode(STRETCH_ANDSCANS);
	memDC.CDC::StretchBlt(0,0,bm.bmWidth,bm.bmHeight,&tempDC, 0,0,bm.bmWidth,bm.bmHeight,SRCCOPY); 
	
	maskBitmap.CreateBitmap(bm.bmWidth,bm.bmHeight,1,1,NULL);
	pOldmaskBmp=maskDC.SelectObject(&maskBitmap); 
	memDC.CDC::SetBkColor(colorTransparent);
	
	maskDC.CDC::SetStretchBltMode(STRETCH_ANDSCANS);
	maskDC.CDC::StretchBlt(0,0,bm.bmWidth,bm.bmHeight,&memDC, 0,0,bm.bmWidth,bm.bmHeight,SRCCOPY); 
	
	memDC.CDC::SetBkColor(RGB(0,0,0)); 
	memDC.CDC::SetTextColor(RGB(255,255,255));
	memDC.CDC::SetStretchBltMode(STRETCH_ANDSCANS);
	memDC.CDC::StretchBlt(0,0,bm.bmWidth,bm.bmHeight, &maskDC, 0,0,bm.bmWidth,bm.bmHeight,SRCAND);
	
	pDC->SetBkColor(RGB(255,255,255));
	pDC->SetTextColor(RGB(0,0,0));
	
	::SetStretchBltMode(pDC->m_hDC, COLORONCOLOR);
	
	if (dZoomFactor > 0.4f)
		dZoomFactor = 0.4f;
	double dblScale = 2.5f * dZoomFactor;
	
	// now drawing..
	CGPointGP gpLT, gpRB;
	CGRectGP gpRect;
	CPoint point;
	CPoint point1, point2;
	CPoint tpt;
	double dlimit = bm.bmWidth;
	
	pDC->GeoPtoLP(&point, &gpoint);
	
	DestLP.x = point.x;
	DestLP.y = point.y;
	
	CPoint dpt = DestLP;
	CPoint symPos;
	pDC->LPtoDP(&dpt);
	symPos = dpt;
	
	dpt.x -= dlimit;
	dpt.y += dlimit;
	symPos.x -= dlimit/2;
	symPos.y -= dlimit/2;
	
	pDC->DPtoLP(&dpt);
	pDC->DPtoLP(&symPos);
	
	tpt.x = DestLP.x - dpt.x;
	tpt.y = DestLP.y - dpt.y;
	widthLP = tpt;
	
	pDC->CDC::StretchBlt(symPos.x, symPos.y, (INT)(widthLP.cx * dblScale), 
		(INT)(-widthLP.cy * dblScale), &maskDC, 0,0, bm.bmWidth,bm.bmHeight,SRCAND);
	pDC->CDC::StretchBlt(symPos.x, symPos.y, (INT)(widthLP.cx * dblScale), 
		(INT)(-widthLP.cy * dblScale), &memDC, 0,0, bm.bmWidth,bm.bmHeight,SRCPAINT);
	
	tempDC.SelectObject(pold);
	maskDC.SelectObject(pOldmaskBmp);
	memDC.SelectObject(pOldBitmap);
	
	maskDC.MemReset();
	memDC.MemReset();
	tempDC.MemReset();
	
	maskBitmap.DeleteObject();
	bmpImage.DeleteObject();
	
	// DC restoring..
	pDC->RestoreDC(savedDC);
}

void CGMapDisplayMgr::SetChromView(int iHSLSet, int iType, double H, double S, double L)
{
	CMapDataMgr::SetChromView(iHSLSet, iType, H, S, L);
}

static int id_tmp = 1;
void CGMapDisplayMgr::shpDraw(CGDCGeo* pDC, int scale, int ratio, CGRectGP rectGP, int type_dis_map, BOOL draw )
{	
	//shp 파일 경로 불러오기 
	{
		int cnt_path = load_mgr.get_cnt_path(); 
		if( cnt_path == 0 )
		{			
			load_mgr.set_path_file( "C:\\AFC4I\\map\\mapinfo\\path_shp.afccs" ); 
			load_mgr.load_path_shp();			
		}
	}
	
	EGMapScale nScale;		
	std::vector< SFILE_INFO > vec_path_shp_land;	
	//std::vector< SFILE_INFO > vec_path_shp_air;	
	
	switch(5)
	{
	case 0 : nScale = MS_1_25000;
		break;
	case 1 :
		{ 
			nScale = MS_1_50000;
			vec_path_shp_land = load_mgr.search_shp_ex( TYPE_EARTH_5, rectGP.m_R.left, rectGP.m_R.bottom, rectGP.m_R.right, rectGP.m_R.top );
			break;
		}
	case 2 : 
		{
			vec_path_shp_land = load_mgr.search_shp_ex( TYPE_EARTH_10, rectGP.m_R.left, rectGP.m_R.bottom, rectGP.m_R.right, rectGP.m_R.top );
			nScale = MS_1_100000;
			break;
		}
	case 3 : 
		{
			//vec_path_shp_air = load_mgr.search_shp_ex( TYPE_AIR_25, rectGP.m_R.left, rectGP.m_R.bottom, rectGP.m_R.right, rectGP.m_R.top );
			vec_path_shp_land = load_mgr.search_shp_ex( TYPE_EARTH_25, rectGP.m_R.left, rectGP.m_R.bottom, rectGP.m_R.right, rectGP.m_R.top );
			nScale = MS_1_250000;
			break;
		}
	case 4 : 
		{			
			//vec_path_shp_air = load_mgr.search_shp_ex( TYPE_AIR_50, rectGP.m_R.left, rectGP.m_R.bottom, rectGP.m_R.right, rectGP.m_R.top );
			vec_path_shp_land = load_mgr.search_shp_ex( TYPE_EARTH_50, rectGP.m_R.left, rectGP.m_R.bottom, rectGP.m_R.right, rectGP.m_R.top );
			nScale = MS_1_500000;
			break;
		}
	case 5 : 
		{
			//vec_path_shp_air = load_mgr.search_shp_ex( TYPE_AIR_100, rectGP.m_R.left, rectGP.m_R.bottom, rectGP.m_R.right, rectGP.m_R.top );
			vec_path_shp_land = load_mgr.search_shp_ex( TYPE_EARTH_100, rectGP.m_R.left, rectGP.m_R.bottom, rectGP.m_R.right, rectGP.m_R.top );
			nScale = MS_1_1000000;
			break;
		}
	case 6 :
		{
			//vec_path_shp_air = load_mgr.search_shp_ex( TYPE_AIR_200, rectGP.m_R.left, rectGP.m_R.bottom, rectGP.m_R.right, rectGP.m_R.top );
			vec_path_shp_land = load_mgr.search_shp_ex(TYPE_EARTH_200, rectGP.m_R.left, rectGP.m_R.bottom, rectGP.m_R.right, rectGP.m_R.top);
			nScale = MS_1_2000000;
			break;
		}
	}	
	int cnt_path_land = 0;
	int cnt_path_air = 0;
	cnt_path_land = vec_path_shp_land.size();
	//cnt_path_air = vec_path_shp_air.size();	

/*
	//상황도성능개선
	if( scale < 3 )
	{
		m_shp_mgr.close_shpfile_5_10();
		SFILE_INFO file_info;		
		for( int cnt = 0; cnt < cnt_path_land; ++cnt )
		{		
			file_info = vec_path_shp_land.at( cnt );
			m_shp_mgr.add_id_file(file_info.id_file);
			m_shp_mgr.load_shp(  file_info.id_file, file_info.path_file ,file_info.type_map, pDC, nScale );	
		}
	}
	else 
	*/
	{
		//육도를 읽는다.
		SFILE_INFO file_info;
		for( int cnt = 0; cnt < cnt_path_land; ++cnt )
		{		
			file_info = vec_path_shp_land.at( cnt );				
			//열려 있는 파일이 아니라면 
			//if( FALSE == m_shp_mgr.search_id_file( file_info.id_file ) )
			//{
			//	m_shp_mgr.add_id_file( file_info.id_file );	 
			//	m_shp_mgr.load_shp(  file_info.id_file, file_info.path_file ,file_info.type_map, pDC, nScale );				
			//}					
		}
		//공도를 읽는다.
		/*
		for(int cnt = 0; cnt < cnt_path_air; ++cnt )
		{		
			file_info = vec_path_shp_air.at( cnt );			
			//열려 있는 파일이 아니라면 
			//if( FALSE == m_shp_mgr.search_id_file( file_info.id_file ) )
			//{
			//	m_shp_mgr.add_id_file( file_info.id_file ); 
			//	m_shp_mgr.load_shp(  file_info.id_file, file_info.path_file ,
			//						file_info.type_map, pDC, nScale );
			//}		
		} 
		*/
	}		 
	

	
	//m_shp_mgr.close_shpfile();
	//m_shp_mgr.mmap_id.clear();

	//SHP DRAWING
	//if( draw == TRUE )
	//	m_shp_mgr.draw_shp( pDC, rectGP, nScale, ratio, type_dis_map );
	
	//IDENTIFY
	if( CMapDataMgr::is_identify() == TRUE )
	{
		CPoint coord = CMapDataMgr::get_identify_coord();
		CGPointGP  pt_geo_selected;	
		pDC->DPtoGeoP( &pt_geo_selected, &coord );
		
		CMapDataMgr::delete_feature();
//		m_shp_mgr.identify_feature( pDC, rectGP, pt_geo_selected, nScale, ratio );			
	} 
}


void CGMapDisplayMgr::backgroundShpDraw(CGDCGeo* pDC, double dZoomFactor)
{
	if(m_pMapInfo == NULL)
		return;
	
	CRect rect;
	GPoint gp, gpOri;
	gpOri.x = 127.42906022388939;
	gpOri.y = 38.068137052285763;
	
	rect = CGLib::GetClientRect();
	dZoomFactor/=100;
	dZoomFactor*=2.5;
	
	double left, top, right, bottom;
	m_ifViewInfo->GetSelectedRange(&left, &top, &right, &bottom);
	CGRectGP rectGP(left, top, right, bottom);
	
	rectGP.m_R.left -= 1;
	rectGP.m_R.top += 1;
	rectGP.m_R.right += 1;
	rectGP.m_R.bottom -= 1;
	
	gp.x = gpOri.x - (gpOri.x - left);
	gp.y = gpOri.y - (gpOri.y - top);
	
	CPen pen, *oldPen;
	CBrush brush, *oldbrush;
	CSize size;
	size.cx = 1;
	pDC->DPtoLP(&size);
	
	shpObject* shpDraw;
	stSmallMap* smallMap = CMapDataMgr::GetSmallMapHead();
	int cnt_small_map = CMapDataMgr::GetSmallMapCount();
	
	while(smallMap)
	{
		smallMap = CMapDataMgr::GetSmallMapNext();
		if(smallMap == NULL) break;
		shpDraw = smallMap->m_pObject;
		switch(smallMap->m_enType)
		{
		case SM_SMALLMAP :
			{ 
				pDC->FillSolidRect(&rectGP, m_clBackColor[0]);
				pen.CreatePen(PS_SOLID, size.cx, RGB(64,64,64));	 
				brush.CreateSolidBrush(m_clBackColor[1]);
				oldPen = pDC->SelectObject(&pen);
				oldbrush = pDC->SelectObject(&brush);
				shpDraw->Draw(pDC, m_lEllipsoid, m_dZoomFactor, gpOri, rectGP, 63);
				pDC->SelectObject(oldPen);
				pDC->SelectObject(oldbrush);
				pen.DeleteObject();
				brush.DeleteObject();
			}
			break;
		}
	}
}

void CGMapDisplayMgr::worldmapShpDraw(CGDCGeo* pDC, double dZoomFactor, int iViewType)
{
	if(m_pMapInfo == NULL)
		return;
	
	stTextPriority *pTextPriority = NULL;
	stTextPriority *pTextPriority1 = NULL;
	stTextPriority *pTextPriority2 = NULL;
	
	CRect rect;
	GPoint gp, gpOri;
	gpOri.x = 127.42906022388939;
	gpOri.y = 38.068137052285763;
	
	rect = CGLib::GetClientRect();
	dZoomFactor/=100;
	dZoomFactor*=2.5;
	
	int iShpType;
	double left, top, right, bottom;
	m_ifViewInfo->GetSelectedRange(&left, &top, &right, &bottom);
	CGRectGP rectGP(left, top, right, bottom);
	
	gp.x = gpOri.x - (gpOri.x - left);
	gp.y = gpOri.y - (gpOri.y - top);
	
	CPen pen, *oldPen;
	CBrush brush, *oldbrush;
	CSize size;
	
	double dViewRatio;
	CString strFileName;
	int iIndex;
	shpObject* shpDraw;
	stSmallMap* smallMap = CMapDataMgr::GetWorldMapHead();
	while(smallMap)
	{
		smallMap = CMapDataMgr::GetWorldMapNext();
		if(smallMap == NULL) break;
		shpDraw = smallMap->m_pObject;
		
		iShpType = shpDraw->GetSHPType();
		if(iViewType != iShpType) continue;
		
		strFileName = _T("");
		
		switch(iShpType)
		{
		case SHPT_POINT : 
			{
				for(int i=0; i<CMapDataMgr::GetTextPriorityCnt(); i++)
				{
					pTextPriority = CMapDataMgr::GetTextPriority(i);
					if(pTextPriority->m_strWorldMapPoint == "국가명")
						pTextPriority1 = pTextPriority;
					else if(pTextPriority->m_strWorldMapPoint == "행정지역명")
						pTextPriority2 = pTextPriority;
				}
				
				size.cx = 1;
				pDC->DPtoLP(&size);
				pen.CreatePen(PS_SOLID, size.cx, RGB(255,0,0));
				brush.CreateSolidBrush(RGB(255,0,0));
				oldPen = pDC->SelectObject(&pen);
				oldbrush = pDC->SelectObject(&brush);
				
				iIndex = smallMap->m_strFileName.ReverseFind('\\');
				if(iIndex != -1)
					strFileName = smallMap->m_strFileName.Right(smallMap->m_strFileName.GetLength()-(iIndex+1));
				if(strFileName.Left(9) == "eur_point" ||
					strFileName.Left(9) == "noa_point" ||
					strFileName.Left(9) == "sas_point" ||
					strFileName.Left(9) == "soa_point")
				{
					shpDraw->SetPointType(0, 4);
					if(pTextPriority1 != NULL)
					{
						dViewRatio = 100.0 / m_dZoomFactor;
						if(pTextPriority1->m_bIsVisible == FALSE ||
							dViewRatio > (double)pTextPriority1->m_uiViewRatio)
						{
							pDC->SelectObject(oldPen);
							pDC->SelectObject(oldbrush);
							pen.DeleteObject();
							brush.DeleteObject();
							break;
						}
						
						if(pTextPriority1->m_pFontType == NULL)
						{
							((shpPoint*)shpDraw)->SetFontType(ANSI_CHARSET);
							((shpPoint*)shpDraw)->SetFaceName("굴림");
						}
						else
						{
							((shpPoint*)shpDraw)->SetFontType(pTextPriority1->m_pFontType->m_nCharSet);
							((shpPoint*)shpDraw)->SetFaceName(pTextPriority1->m_pFontType->m_strName);
						}
						
						((shpPoint*)shpDraw)->SetFontSize(pTextPriority1->m_uiFontSize);
						((shpPoint*)shpDraw)->SetFontStrikeOut(FALSE);
						((shpPoint*)shpDraw)->SetFontColor(pTextPriority1->m_clFontColor);
						((shpPoint*)shpDraw)->SetVisibleType(pTextPriority1->m_uiVisibleType);
					}
					shpDraw->SetLabel(0);
				}
				else
				{
					shpDraw->SetPointType(0, 2);
					if(pTextPriority2 != NULL)
					{
						dViewRatio = 100.0 / m_dZoomFactor;
						if(pTextPriority2->m_bIsVisible == FALSE ||
							dViewRatio > (double)pTextPriority2->m_uiViewRatio)
						{
							pDC->SelectObject(oldPen);
							pDC->SelectObject(oldbrush);
							pen.DeleteObject();
							brush.DeleteObject();
							break;
						}
						
						if(pTextPriority1->m_pFontType == NULL)
						{
							((shpPoint*)shpDraw)->SetFontType(ANSI_CHARSET);
							((shpPoint*)shpDraw)->SetFaceName("굴림");
						}
						else
						{
							((shpPoint*)shpDraw)->SetFontType(pTextPriority2->m_pFontType->m_nCharSet);
							((shpPoint*)shpDraw)->SetFaceName(pTextPriority2->m_pFontType->m_strName);
						}
						
						((shpPoint*)shpDraw)->SetFontSize(pTextPriority2->m_uiFontSize);
						((shpPoint*)shpDraw)->SetFontStrikeOut(FALSE);
						((shpPoint*)shpDraw)->SetFontColor(pTextPriority2->m_clFontColor);
						((shpPoint*)shpDraw)->SetVisibleType(pTextPriority2->m_uiVisibleType);
					}
					shpDraw->SetLabel(1);
				}
				
				shpDraw->DrawMemoryMap(pDC, m_lEllipsoid, m_dZoomFactor, gpOri, rectGP, 63);
				pDC->SelectObject(oldPen);
				pDC->SelectObject(oldbrush);
				pen.DeleteObject();
				brush.DeleteObject();
			}
			break;
		case SHPT_ARC : 
			{
				iIndex = smallMap->m_strFileName.ReverseFind('\\');
				if(iIndex != -1)
					strFileName = smallMap->m_strFileName.Right(smallMap->m_strFileName.GetLength()-(iIndex+1));
				if(strFileName.Left(6) == "LAP030" ||
					strFileName.Left(6) == "LAP050" ||
					strFileName.Left(6) == "LAN010")
				{
					size.cx = 1;
					pDC->DPtoLP(&size);
					pen.CreatePen(PS_SOLID, size.cx, RGB(171,86,0));
					brush.CreateSolidBrush(RGB(171,86,0));
				}
				else
				{
					size.cx = 1;
					pDC->DPtoLP(&size);
					pen.CreatePen(PS_SOLID, size.cx, RGB(64,64,64));
					brush.CreateSolidBrush(RGB(64,64,64));
				}
				
				oldPen = pDC->SelectObject(&pen);
				oldbrush = pDC->SelectObject(&brush);
				shpDraw->Draw(pDC, m_lEllipsoid, m_dZoomFactor, gpOri, rectGP, 63);
				pDC->SelectObject(oldPen);
				pDC->SelectObject(oldbrush);
				pen.DeleteObject();
				brush.DeleteObject();
			}
			break;
		case SHPT_POLYGON : 
			{
				size.cx = 1;
				pDC->DPtoLP(&size);
				pDC->FillSolidRect(&rectGP, m_clBackColor[0]);
				//				pen.CreatePen(PS_SOLID, size.cx, m_clBackColor[0]);
				pen.CreatePen(PS_SOLID, size.cx, RGB(175,97,97));
				brush.CreateSolidBrush(m_clBackColor[1]);
				oldPen = pDC->SelectObject(&pen);
				oldbrush = pDC->SelectObject(&brush);
				shpDraw->DrawMemoryMap(pDC, m_lEllipsoid, m_dZoomFactor, gpOri, rectGP, 63);
				pDC->SelectObject(oldPen);
				pDC->SelectObject(oldbrush);
				pen.DeleteObject();
				brush.DeleteObject();
			}
			break;
		}
	}
}

void CGMapDisplayMgr::ShapeMap(CDC* pDC)
{
	CCoord m_Coord;
	CCoordMgr*	m_pCoordMgr = GP_COORDMGR;

	CGRectGP rect;
	CRect rc;
	double left, top, right, bottom;

	iDispMapType = MK_VECTOR_LAND;
	long lMapScale = 1000000;
	double dZoomFactor = 1; 
	//double dZoomFactor = pView->GetZoomFactor();

	//dZoomFactor = CGMapLib::GetZoomFactor();
	
	m_dZoomFactor = dZoomFactor;
	dZoomFactor *= 100;

	if( dZoomFactor < 67 ) 
		return;

	int iZoomRatio = (int)((double)lMapScale / (dZoomFactor/100.0));	
	CMapConfigMgr::SetCurrentMapInfo((int)lMapScale, iZoomRatio);


	m_clBackColor[0] = RGB(145, 177, 207);	//배경 색상 설정 
	m_clBackColor[1] = RGB(187, 196, 204);	//배경 색상 설정 

//	m_clBackColor[1] = RGB(255, 255, 255);
	
	if( m_bIsSmallMapOpen != TRUE )	
		CMapDataMgr::smallMapOpen();				// 소축척지도 load
	if( m_bIsSmallWorldMapOpen != TRUE ) 
		CMapDataMgr::smallworldMapOpen();		// 세계지도(소축척) load
	
	if(CMapDataMgr::IsWorldMapOpen())	//세계 지도 도시 
	{	
		worldmapShpDraw(pDC, dZoomFactor, SHPT_POLYGON); 
		worldmapShpDraw(pDC, dZoomFactor, SHPT_ARC);
		worldmapShpDraw(pDC, dZoomFactor, SHPT_POINT);
	}
	else 
	{
		//배경 지도 도시 
		//CRect dpRect = CGLib::GetClientRect();
		
		CGRectGP gpRect;
		//gpRect = m_ClientRectGp;

		m_pWnd->GetClientRect(&rc);
		//pView->ScreenToClient(&rc);
	
		m_pCoordMgr->ScreenXY2WorldXY(rc.left, rc.top, &left, &top);
		m_pCoordMgr->ScreenXY2WorldXY(rc.right, rc.bottom, &right, &bottom);
		m_Coord.x_y_to_degrees_lat_long(left, top, &rect.m_R.top, &rect.m_R.left);
		m_Coord.x_y_to_degrees_lat_long(right, bottom, &rect.m_R.bottom, &rect.m_R.right);

		//pDC->DPtoGeoP(&gpRect, &dpRect);
		CRect clRect;
		m_pCoordMgr->WorldXY2ScreenXY(rect.m_R.top, rect.m_R.left, &clRect.left, &clRect.top);
		m_pCoordMgr->WorldXY2ScreenXY(rect.m_R.bottom, rect.m_R.right, &clRect.right, &clRect.bottom);
		pDC->FillSolidRect(clRect.left, clRect.top, clRect.Width(), clRect.Height(), m_clBackColor[0]);
		//pDC->FillSolidRect(rc.left, rc.top, rc.Width(), rc.Height(), m_clBackColor[0]);

		//pDC->FillSolidRect(&gpRect, m_clBackColor[0]);	 		
		backgroundShpDraw(pDC, dZoomFactor);
		
		//래스터, 위성영상, 백터 도시
		//long lMapKind, lMapScale;	
		//m_ifViewInfo->GetMapKind(&lMapKind);		// 지도의 종류
		//m_ifViewInfo->GetMapScale(&lMapScale);	// 지도의 스케일	
		int iZoomRatio = (int)((double)lMapScale / (dZoomFactor/100.0));
		
		if( is_first_drawing == TRUE )
		{		 
			rect.m_R.left = 113.0;
			rect.m_R.right = 140.0;
			rect.m_R.top = 44.0;
			rect.m_R.bottom = 30.0;
			
			//int type_dis_map = 0;
			//type_dis_map = CBAMgr::GetDispMapType();		
			
			//shpDraw( pDC, MS_1_2000000, iZoomRatio, rect, iDispMapType, false );
			shpDraw( pDC, MS_1_1000000, iZoomRatio, rect, iDispMapType, true );
		//	shpDraw( pDC, MS_1_500000, iZoomRatio, rect, type_dis_map, false );
		//	shpDraw( pDC, MS_1_250000, iZoomRatio, rect, type_dis_map, false );	
			//is_first_drawing = FALSE;
		}
	}
	
}

void CGMapDisplayMgr::RasterMap(CDC* pDC)
{
	CCoord m_Coord;
	CCoordMgr*	m_pCoordMgr = GP_COORDMGR;

	int iScale = 0;
	long MapMode;// = GMM_UTM;  // GMM_GP  GMM_UTM
	long  lMapScale;
	EGMapScale nScale = MS_NOTHING;

	MapMode = m_pGeoObj->GetMapMode();
	pDC->SetMapMode(MapMode);
	lMapScale = 0;
	double zScale = m_pCoordMgr->m_fScale;
	if(zScale >= 2 && zScale <= 4) lMapScale = 2000000;
	else if(zScale > 4 && zScale <= 10) lMapScale = 1000000;
	else if(zScale > 10 && zScale <= 20) lMapScale = 500000;
	else if(zScale > 20 && zScale <= 50) lMapScale = 250000;
	else if(zScale > 50 && zScale <= 100) lMapScale = 100000;
	else if(zScale > 100 ) lMapScale = 50000;
	switch(lMapScale)
	{
	case 50000 :	nScale = MS_1_50000;	iScale = 50000;		break;
	case 100000 :	nScale = MS_1_100000;	iScale = 100000;	break;
	case 250000 :	nScale = MS_1_250000;	iScale = 250000;	break;
	case 500000 :	nScale = MS_1_500000;	iScale = 500000;	break;
	case 1000000 :	nScale = MS_1_1000000;	iScale = 1000000;	break;
	case 2000000 :	nScale = MS_1_2000000;	iScale = 2000000;	break;
	default: return;
	}		

	m_pMapInfo->SetMapScale(nScale);
	m_pMapInfo->SetMapKind((EGMapKind)iDispMapType);
	iDispMapType = MK_VR_LAND;	// MK_RASTER_AIR  MK_VR_LAND
	CMapConfigMgr::SetMapScale(nScale);				

	// 레스터 & 위성영상 데이터 처리		
	stRasterMap * pRasterMap = NULL;		
	CRect rc;
	CGRectGP rect, rectGP;

	double left, top, right, bottom;
	m_pWnd->GetClientRect(&rc);

	m_pCoordMgr->ScreenXY2WorldXY(rc.left, rc.top, &left, &top);
	m_pCoordMgr->ScreenXY2WorldXY(rc.right, rc.bottom, &right, &bottom);
	m_Coord.x_y_to_degrees_lat_long(left, top, &rect.m_R.top, &rect.m_R.left);
	m_Coord.x_y_to_degrees_lat_long(right, bottom, &rect.m_R.bottom, &rect.m_R.right);

	double dZoomFactor = 1; 
	dZoomFactor *= 100;

	int iZoomRatio = (int)((double)lMapScale / (dZoomFactor/100.0));	
	CMapConfigMgr::SetCurrentMapInfo((int)lMapScale, iZoomRatio);

	//long MapMode = GMM_UTM;	// GMM_UTM, GMM_GP
	UINT uiRasterMapKind = 0;
	BOOL bIsAutoMode = TRUE;
	m_lEllipsoid = m_pGeoObj->GetEllipsoid();

	uiRasterMapKind = CMapDataMgr::GetRasterMapKind(bIsAutoMode, iZoomRatio);

	if(iDispMapType != 0 && 
		(uiRasterMapKind == 1 || uiRasterMapKind == 50000 || uiRasterMapKind == 100000) && 
		MapMode == GMM_UTM)
	{
		if(m_lEllipsoid == EBES)	m_lEllipsoid = EWGE;

		m_pGeoObj->SetMapMode(GMM_GP);
		m_pGeoObj->SetEllipsoid(EWGE);
		m_pGeoObj->SetProjBase(129.0, 38.0); // 투영의 기준점을 설정
		m_pGeoObj->SetStdParallel(33.3333333, 38.3333337); // 표준위선 설정
		m_pGeoObj->SetValidity(1); // LP값의 유효숫자(1미터 단위)
	}
	// 벡터도시가 아니고, 1m 위성영상, 10/5만 육도가 아니며 좌표계가 평면이면 투영법으로 바꾼다.
	else if(iDispMapType != 0 && 
		(uiRasterMapKind != 1 && uiRasterMapKind != 50000 && uiRasterMapKind != 100000) && 
		MapMode == GMM_GP)
		{
			if(m_lEllipsoid == EBES)	m_lEllipsoid = EWGE;
	}

	else if(iDispMapType == 0 && MapMode != CMapDataMgr::GetVectorMapMode())
		MapMode = CMapDataMgr::GetVectorMapMode();


	// 레스터 지도 도시
	if(CMapDataMgr::ExtractRasterMap(bIsAutoMode, uiRasterMapKind, rect, iDispMapType, iZoomRatio))
	{				  				
		int iCnt = CMapDataMgr::GetRasterMapCount();
		for(int i=0; i<iCnt; i++)
		{
			pRasterMap = CMapDataMgr::GetRasterMap(i);			
			
			if( uiRasterMapKind == 50000 && MS_1_50000 != pRasterMap->m_enMapScale ) continue;
			else if( uiRasterMapKind == 100000 && MS_1_100000 != pRasterMap->m_enMapScale ) continue;
			else if( uiRasterMapKind == 250000 && MS_1_250000 != pRasterMap->m_enMapScale ) continue;
			else if( uiRasterMapKind == 500000 && MS_1_500000 != pRasterMap->m_enMapScale ) continue;
			else if( uiRasterMapKind == 1000000 && MS_1_1000000 != pRasterMap->m_enMapScale ) continue;
			else if( uiRasterMapKind == 2000000 && MS_1_2000000 != pRasterMap->m_enMapScale ) continue; 
			
			if(pRasterMap->m_bIsRasterDataOpen == TRUE)  
				CMapDataMgr::DrawRasterMap(pDC, pRasterMap);
		}	
	}
}

void CGMapDisplayMgr::SatelliteMap(CDC *pDC)
{
	CCoord m_Coord;
	CCoordMgr*	m_pCoordMgr = GP_COORDMGR;

	int iScale = 0;
	long MapMode;// = GMM_UTM;  // GMM_GP  GMM_UTM
	long  lMapScale;
	EGMapScale nScale = MS_NOTHING;
	
	MapMode = m_pGeoObj->GetMapMode();
	pDC->SetMapMode(MapMode);

	lMapScale = 50000;

	switch(lMapScale)
	{
	case 50000 :	nScale = MS_1_50000;	iScale = 50000;		break;
	case 100000 :	nScale = MS_1_100000;	iScale = 100000;	break;
	case 250000 :	nScale = MS_1_250000;	iScale = 250000;	break;
	case 500000 :	nScale = MS_1_500000;	iScale = 500000;	break;
	case 1000000 :	nScale = MS_1_1000000;	iScale = 1000000;	break;
	case 2000000 :	nScale = MS_1_2000000;	iScale = 2000000;	break;
	default: break;
	}		
	
	//CMapConfigMgr::SetMapScale(nScale);
	m_pMapInfo->SetMapScale(nScale);
	m_pMapInfo->SetMapKind((EGMapKind)iDispMapType);
	iDispMapType = MK_VR_LAND;	// MK_RASTER_AIR  MK_VR_LAND
	//CMapConfigMgr::SetMapScale(nScale);				
	
	// 레스터 & 위성영상 데이터 처리		
	//stRasterMap * pRasterMap = NULL;			
	stSatelliteMapIndex * pIndex = NULL;

	CRect rc;
	CGRectGP rect, rectGP;

	double left, top, right, bottom;

	//CMapViewView *pView=(CMapViewView *)m_pWnd; 
	m_pWnd->GetClientRect(&rc);
	//pView->ScreenToClient(&rc);
	
	m_pCoordMgr->ScreenXY2WorldXY(rc.left, rc.top, &left, &top);
	m_pCoordMgr->ScreenXY2WorldXY(rc.right, rc.bottom, &right, &bottom);
	m_Coord.x_y_to_degrees_lat_long(left, top, &rect.m_R.top, &rect.m_R.left);
	m_Coord.x_y_to_degrees_lat_long(right, bottom, &rect.m_R.bottom, &rect.m_R.right);

	//m_dScreenScale = GetScreenScale(pDC, &rect);

	//CGRectGP rect(left, top, right, bottom);
	double dZoomFactor = 40; 
	//double dZoomFactor = pView->GetZoomFactor();
	dZoomFactor *= 100;

	if( dZoomFactor < 67 ) 
		return;

	int iZoomRatio = (int)((double)lMapScale / (dZoomFactor/100.0));	
	CMapConfigMgr::SetCurrentMapInfo((int)lMapScale, iZoomRatio);
	
	//long MapMode = GMM_UTM;	// GMM_UTM, GMM_GP
	//m_ifViewInfo->GetMappingMode(&MapMode);
	UINT uiRasterMapKind = 0;
	BOOL bIsAutoMode = TRUE;
	//m_lEllipsoid = 2;
	m_lEllipsoid = m_pGeoObj->GetEllipsoid();

	//m_ifViewInfo->GetEllipsoid(&m_lEllipsoid);		
	//m_ifViewInfo->GetAutoScaleMode(&bIsAutoMode);
	uiRasterMapKind = CMapDataMgr::GetRasterMapKind(bIsAutoMode, iZoomRatio);
	
	// 1m 위성영상, 10/5만 육도 도시이면 좌표계를 평면으로 바꾼다.
	if(iDispMapType != 0 && 
		(uiRasterMapKind == 1 || uiRasterMapKind == 50000 || uiRasterMapKind == 100000) && 
		MapMode == GMM_UTM)
	{
		if(m_lEllipsoid == EBES)	m_lEllipsoid = EWGE;
		//{
		//	m_pGeoObj->SetEllipsoid(EWGE);
		//}
		//pDC->SetMapMode(MM_ISOTROPIC);
		m_pGeoObj->SetMapMode(GMM_GP);
		m_pGeoObj->SetEllipsoid(EWGE);
		m_pGeoObj->SetProjBase(129.0, 38.0); // 투영의 기준점을 설정
											 // 표준위선:북위38도20분, 북위38도40분 우리나라를 포함한 영역
		m_pGeoObj->SetStdParallel(33.3333333, 38.3333337); // 표준위선 설정
		m_pGeoObj->SetValidity(1); // LP값의 유효숫자(1미터 단위)
		//m_pGeoObj->SetProjBase(0.0, 0.0);
		//m_pGeoObj->SetValidity( 0.00001 );
		//CGGeoProj proj;
		//proj.
		//CGMessenger::PostToMainframe(WMG_AX_MAP_RESET, GMM_GP, (int)m_lEllipsoid);
	}
	// 벡터도시가 아니고, 1m 위성영상, 10/5만 육도가 아니며 좌표계가 평면이면 투영법으로 바꾼다.
	else if(iDispMapType != 0 && 
		(uiRasterMapKind != 1 && uiRasterMapKind != 50000 && uiRasterMapKind != 100000) && 
		MapMode == GMM_GP)
	{
		if(m_lEllipsoid == EBES)	m_lEllipsoid = EWGE;
		//CGMessenger::PostToMainframe(WMG_AX_MAP_RESET, GMM_UTM, (int)m_lEllipsoid);
	}
	else if(iDispMapType == 0 && MapMode != CMapDataMgr::GetVectorMapMode())
	{
		MapMode = CMapDataMgr::GetVectorMapMode();
		//CGMessenger::PostToMainframe(WMG_AX_MAP_RESET, MapMode, (int)m_lEllipsoid);
	}	

	s_flag_satellite_view satellite_view; 
	satellite_view.visible = true;
	satellite_view.visible_10m = true;
	//satellite_view = CMapDataMgr::get_flag_satellite_view();		
	if( satellite_view.visible != FALSE )			
	{
		if(CMapDataMgr::ExtractSatelliteMap(bIsAutoMode, rect, iDispMapType, iZoomRatio))
		{		
			int iCnt = CMapDataMgr::GetSatelliteMapIndexCount();				
			int cnt_draw = 0;
			for(int i=0; i<iCnt; i++)
			{
				pIndex = CMapDataMgr::GetSatelliteMapIndex(i);
				if(pIndex->m_pData != NULL)
				{
					CMapDataMgr::DrawSatelliteMap(pDC, pIndex);	++cnt_draw;
				}					
			} 
		}			
	}

}

////////////////////////////////////////////////////////////////////////////////////////////////
//
//		
//
//////////////////////////////////////////////////////////////////////////////////////////////
/*
void CGMapDisplayMgr::Draw(CDC* pDC)
{		
	TCHAR   inBuf[80];
	::GetPrivateProfileString (TEXT("AFCCS_COP"), 
		TEXT("TYPE_BUSINESS"), 
		TEXT("Error: GPPS failed"),
		inBuf , 
		80, 
		TEXT("afccs_cop_type.ini")); 

	CString type_business = inBuf;
	
	iDispMapType = CBAMgr::GetDispMapType();		
	if( _T("ATO") ==  type_business)
	{		
		if(!m_ifViewInfo)
		{			
			return; 		
		}

		long MapMode;
		m_ifViewInfo->GetMappingMode(&MapMode);			// 현재 사용되고 있는 좌표체계
														// 경위도, utm, ups, tm, 등
		if( m_lEllipsoid != 18 || MapMode != GMM_GP)
		{
			m_lEllipsoid = EWGE;
			//CGMessenger::PostToMainframe(WMG_AX_MAP_RESET, GMM_GP, (int)m_lEllipsoid);
		}
	
		HRESULT hRes;	
		double left, top, right, bottom;
		double dZoomFactor;
		hRes = m_ifViewInfo->GetSelectedRange(&left, &top, &right, &bottom);	
		
		CGRectGP rect(left, top, right, bottom);
		m_ClientRectGp = rect;	
		m_dScreenScale = GetScreenScale(pDC, &rect);
		hRes = m_ifViewInfo->GetZoomFactor(&dZoomFactor);	
		m_dZoomFactor = dZoomFactor;
		dZoomFactor *= 100;
		
		m_clBackColor[0] = RGB(145, 177, 190);	//배경 색상 설정 
		m_clBackColor[1] = RGB(187, 196, 190);	//배경 색상 설정 
		
		if( m_bIsSmallMapOpen != TRUE )	CMapDataMgr::smallMapOpen();				// 소축척지도 load 		
		//배경 지도 도시 
		CRect dpRect = CGLib::GetClientRect();
		CGRectGP gpRect, gpRect1;
		//pDC->DPtoGeoP(&gpRect, &dpRect);
		
		m_CoordMgr->ScreenXY2WorldXY(gpRect.m_R.left, gpRect.m_R.top, (double*)dpRect.left, (double*)dpRect.top);
		m_CoordMgr->ScreenXY2WorldXY(gpRect.m_R.right, gpRect.m_R.bottom, (double*)dpRect.right, (double*)dpRect.bottom);
		m_Coord.x_y_to_degrees_lat_long(dpRect.left, dpRect.top, &gpRect1.m_R.left, &gpRect1.m_R.top);
		m_Coord.x_y_to_degrees_lat_long(dpRect.right, dpRect.bottom, &gpRect1.m_R.right, &gpRect1.m_R.bottom);


		//pDC->FillSolidRect(&gpRect, m_clBackColor[0]);	 		
		pDC->FillSolidRect(gpRect1.m_R.left, gpRect1.m_R.top, gpRect1.m_R.right, gpRect1.m_R.bottom, m_clBackColor[0]);	 		
		backgroundShpDraw(pDC, dZoomFactor);
		
		long  lMapScale;	
		m_ifViewInfo->GetMapScale(&lMapScale);	
		int iScale = 0;
		EGMapScale nScale = MS_NOTHING;
		

		switch(lMapScale)
		{
		case 50000 :	nScale = MS_1_50000;	iScale = 50000;		break;
		case 100000 :	nScale = MS_1_100000;	iScale = 100000;	break;
		case 250000 :	nScale = MS_1_250000;	iScale = 250000;	break;
		case 500000 :	nScale = MS_1_500000;	iScale = 500000;	break;
		case 1000000 :	nScale = MS_1_1000000;	iScale = 1000000;	break;
		case 2000000 :	nScale = MS_1_2000000;	iScale = 2000000;	break;
		default: break;
		}
		

			//현재 정보 메모리에 저장 
		{
			long lMapKind, lMapScale;				
			BOOL bIsAutoMode;
			BOOL  bIsWorldMapOpen;
			long lMapMode;
			int iMapRatio, iMapType;
			double dleft, dtop, dright, dbottom, dZoomFactor;
			CGRectGP gpSelRect; 	
			
			// 자동축척유/무 get
			m_ifViewInfo->GetAutoScaleMode(&bIsAutoMode);
			// 맵 종류 get
			//값을 가져오긴 하나 사용하지 않는다.
			m_ifViewInfo->GetMapKind(&lMapKind);
			// 맵 scale get
			m_ifViewInfo->GetMapScale(&lMapScale);	
			// 맵 scale, 배율, 확대축소비율 get
			//m_ifViewInfo->GetMapScale(&lMapScale);
			m_ifViewInfo->GetZoomFactor(&dZoomFactor);
			iMapRatio = (int)((double)lMapScale / dZoomFactor);
			// 맵 mode get
			m_ifViewInfo->GetMappingMode(&lMapMode);
			// 맵 도시영역 get
			m_ifViewInfo->GetSelectedRange(&dleft, &dtop, &dright, &dbottom);
			// 세계지도 open 유무 get
			bIsWorldMapOpen = CMapDataMgr::IsWorldMapOpen();
			// 지도 종류 get
			iMapType = iDispMapType;		
			
			S_MAP_INFO map_info;
			map_info.bIsAutoMode = bIsAutoMode;
			map_info.bIsWorldMapOpen = bIsWorldMapOpen;
			map_info.lMapMode = lMapMode;
			map_info.lMapScale = lMapScale;
			map_info.iMapRatio = iMapRatio;
			map_info.iMapType = iMapType;			
			map_info.dleft = dleft;
			map_info.dtop = dtop;
			map_info.dright = dright;
			map_info.dbottom = dbottom;			
			map_info.dZoomFactor = dZoomFactor;
			map_info.gpSelRect = gpSelRect;
			map_info.lMapKind = (EGMapKind)iDispMapType;			
			
			CRect rect;
			CPoint point;
			rect = CGLib::GetClientRect();	
			point.x = (rect.right + rect.left) / 2;
			point.y = (rect.bottom + rect.top) / 2;
			CGPointGP geo_pt; 		
			pDC->DPtoGeoP(&geo_pt, &point);	
			
			CString coord;		
			coord = CBAMgr::DegreeToDmsLatEx(geo_pt.m_P.lat);
			coord += CBAMgr::DegreeToDmsLonEx(geo_pt.m_P.lon);		
			map_info.coord_center = coord;						
			CMapDataMgr::save_current_map_info_on_mem( map_info );
		}


	//	if( dZoomFactor < 500 )				    //1만 이상은 패스
		if( dZoomFactor < 2000 )				    //1만 이상은 패스
			return;
				
		int iCnt;		
		stSatelliteMapIndex *pIndex = NULL;
		int iZoomRatio = (int)((double)lMapScale / (dZoomFactor/100.0));
		CGRectGP rectGP(left, top, right, bottom);		
		CMapConfigMgr::SetCurrentMapInfo((int)lMapScale, iZoomRatio);
		

		UINT uiRasterMapKind = 0;
		BOOL bIsAutoMode;

		m_lEllipsoid = m_geoObject.GetEllipsoid();
		//m_ifViewInfo->GetEllipsoid(&m_lEllipsoid);		
		m_ifViewInfo->GetAutoScaleMode(&bIsAutoMode);
		uiRasterMapKind = CMapDataMgr::GetRasterMapKind(bIsAutoMode, iZoomRatio);
		 
		
		if(CMapDataMgr::ExtractSatelliteMap(bIsAutoMode, rectGP, iDispMapType, iZoomRatio))
		{
			iCnt = CMapDataMgr::GetSatelliteMapIndexCount();				
			int cnt_draw = 0;
			
			for(int i=0; i<iCnt; i++)
			{
				pIndex = CMapDataMgr::GetSatelliteMapIndex(i);
				if(pIndex->m_pData != NULL)
				{
					CMapDataMgr::DrawSatelliteMap(pDC, pIndex);
					++cnt_draw;
				}					
			}
		}
		TRACE("ATO상황도 도시 완료");
		return;
	}
////////////////////////////////////////////////////////////////////////////////////////////////

	
//	clock_t startTimeDraw = clock(); 	
	
	if(!m_ifViewInfo) return; 		
	HRESULT hRes;	
	double left, top, right, bottom;
	double dZoomFactor;
	hRes = m_ifViewInfo->GetSelectedRange(&left, &top, &right, &bottom);	
	CGRectGP rect(left, top, right, bottom);
	m_ClientRectGp = rect;	
	m_dScreenScale = GetScreenScale(pDC, &rect);
	hRes = m_ifViewInfo->GetZoomFactor(&dZoomFactor);	
	m_dZoomFactor = dZoomFactor;
	dZoomFactor *= 100;
	
	m_clBackColor[0] = RGB(145, 177, 207);	//배경 색상 설정 
	m_clBackColor[1] = RGB(187, 196, 204);	//배경 색상 설정 

//	m_clBackColor[1] = RGB(255, 255, 255);
	
	if( m_bIsSmallMapOpen != TRUE )	
		CMapDataMgr::smallMapOpen();				// 소축척지도 load
	if( m_bIsSmallWorldMapOpen != TRUE ) 
		CMapDataMgr::smallworldMapOpen();		// 세계지도(소축척) load
	
	if(CMapDataMgr::IsWorldMapOpen())	//세계 지도 도시 
	{	
		worldmapShpDraw(pDC, dZoomFactor, SHPT_POLYGON); 
		worldmapShpDraw(pDC, dZoomFactor, SHPT_ARC);
		worldmapShpDraw(pDC, dZoomFactor, SHPT_POINT);
	}
	else 
	{
		//배경 지도 도시 
		CRect dpRect = CGLib::GetClientRect();
		CGRectGP gpRect;
		gpRect = m_ClientRectGp;
		pDC->DPtoGeoP(&gpRect, &dpRect);
		pDC->FillSolidRect(&gpRect, m_clBackColor[0]);	 		
		backgroundShpDraw(pDC, dZoomFactor);
		
		//래스터, 위성영상, 백터 도시
		long lMapKind, lMapScale;	
		m_ifViewInfo->GetMapKind(&lMapKind);		// 지도의 종류
		m_ifViewInfo->GetMapScale(&lMapScale);	// 지도의 스케일	
		int iZoomRatio = (int)((double)lMapScale / (dZoomFactor/100.0));
		
		if( is_first_drawing == TRUE )
		{		 
			rect.m_R.left = 113.0;
			rect.m_R.right = 140.0;
			rect.m_R.top = 44.0;
			rect.m_R.bottom = 30.0;
			
			//int type_dis_map = 0;
			//type_dis_map = CBAMgr::GetDispMapType();		
			
			shpDraw( pDC, MS_1_2000000, iZoomRatio, rect, iDispMapType, false );
		//	shpDraw( pDC, MS_1_1000000, iZoomRatio, rect, type_dis_map, false );
		//	shpDraw( pDC, MS_1_500000, iZoomRatio, rect, type_dis_map, false );
		//	shpDraw( pDC, MS_1_250000, iZoomRatio, rect, type_dis_map, false );	
			is_first_drawing = FALSE;
		}

		 
		//현재 정보 메모리에 저장 
		{
			BOOL bIsAutoMode;
			BOOL  bIsWorldMapOpen;
			long lMapMode;
			int iMapRatio, iMapType;
			double dleft, dtop, dright, dbottom, dZoomFactor;
			CGRectGP gpSelRect; 	
			
			// 자동축척유/무 get
			m_ifViewInfo->GetAutoScaleMode(&bIsAutoMode);
			// 맵 종류 get
			//값을 가져오긴 하나 사용하지 않는다.
			m_ifViewInfo->GetMapKind(&lMapKind);
			// 맵 scale get
			m_ifViewInfo->GetMapScale(&lMapScale);	
			// 맵 scale, 배율, 확대축소비율 get
			//m_ifViewInfo->GetMapScale(&lMapScale);
			m_ifViewInfo->GetZoomFactor(&dZoomFactor);
			iMapRatio = (int)((double)lMapScale / dZoomFactor);
			// 맵 mode get
			m_ifViewInfo->GetMappingMode(&lMapMode);
			// 맵 도시영역 get
			m_ifViewInfo->GetSelectedRange(&dleft, &dtop, &dright, &dbottom);
			// 세계지도 open 유무 get
			bIsWorldMapOpen = CMapDataMgr::IsWorldMapOpen();
			// 지도 종류 get
			iMapType = iDispMapType;		
			
			S_MAP_INFO map_info;
			map_info.bIsAutoMode = bIsAutoMode;
			map_info.bIsWorldMapOpen = bIsWorldMapOpen;
			map_info.lMapMode = lMapMode;
			map_info.lMapScale = lMapScale;
			map_info.iMapRatio = iMapRatio;
			map_info.iMapType = iMapType;			
			map_info.dleft = dleft;
			map_info.dtop = dtop;
			map_info.dright = dright;
			map_info.dbottom = dbottom;			
			map_info.dZoomFactor = dZoomFactor;
			map_info.gpSelRect = gpSelRect;
			map_info.lMapKind = (EGMapKind)iDispMapType;			
			
			CRect rect;
			CPoint point;
			rect = CGLib::GetClientRect();	
			point.x = (rect.right + rect.left) / 2;
			point.y = (rect.bottom + rect.top) / 2;
			CGPointGP geo_pt; 		
			pDC->DPtoGeoP(&geo_pt, &point);	
			
			CString coord;		
			coord = CBAMgr::DegreeToDmsLatEx(geo_pt.m_P.lat);
			coord += CBAMgr::DegreeToDmsLonEx(geo_pt.m_P.lon);		
			map_info.coord_center = coord;						
			CMapDataMgr::save_current_map_info_on_mem( map_info );
		}

		if( dZoomFactor < 67 ) 	return; //5,000,000
				
				
		int iScale = 0;
		EGMapScale nScale = MS_NOTHING;
		switch(lMapScale)
		{
		case 50000 :	nScale = MS_1_50000;	iScale = 50000;		break;
		case 100000 :	nScale = MS_1_100000;	iScale = 100000;	break;
		case 250000 :	nScale = MS_1_250000;	iScale = 250000;	break;
		case 500000 :	nScale = MS_1_500000;	iScale = 500000;	break;
		case 1000000 :	nScale = MS_1_1000000;	iScale = 1000000;	break;
		case 2000000 :	nScale = MS_1_2000000;	iScale = 2000000;	break;
		default: break;
		}		
		
		m_pMapInfo->SetMapScale(nScale);
		m_pMapInfo->SetMapKind((EGMapKind)iDispMapType);
		CMapConfigMgr::SetMapScale(nScale);				
		
		// 레스터 & 위성영상 데이터 처리		
		stRasterMap * pRasterMap = NULL;			
		stSatelliteMapIndex * pIndex = NULL;		
			
		CGRectGP rectGP(left, top, right, bottom);		
		CMapConfigMgr::SetCurrentMapInfo((int)lMapScale, iZoomRatio);
		
		long MapMode;
		m_ifViewInfo->GetMappingMode(&MapMode);
		UINT uiRasterMapKind = 0;
		BOOL bIsAutoMode;
		m_ifViewInfo->GetEllipsoid(&m_lEllipsoid);		
		m_ifViewInfo->GetAutoScaleMode(&bIsAutoMode);
		uiRasterMapKind = CMapDataMgr::GetRasterMapKind(bIsAutoMode, iZoomRatio);
		
		// 1m 위성영상, 10/5만 육도 도시이면 좌표계를 평면으로 바꾼다.
		if(iDispMapType != 0 && 
			(uiRasterMapKind == 1 || uiRasterMapKind == 50000 || uiRasterMapKind == 100000) && 
			MapMode == GMM_UTM)
		{
			if(m_lEllipsoid == EBES)	m_lEllipsoid = EWGE;
			//CGMessenger::PostToMainframe(WMG_AX_MAP_RESET, GMM_GP, (int)m_lEllipsoid);
		}
		// 벡터도시가 아니고, 1m 위성영상, 10/5만 육도가 아니며 좌표계가 평면이면 투영법으로 바꾼다.
		else if(iDispMapType != 0 && 
			(uiRasterMapKind != 1 && uiRasterMapKind != 50000 && uiRasterMapKind != 100000) && 
			MapMode == GMM_GP)
		{
			if(m_lEllipsoid == EBES)	m_lEllipsoid = EWGE;
			//CGMessenger::PostToMainframe(WMG_AX_MAP_RESET, GMM_UTM, (int)m_lEllipsoid);
		}
		else if(iDispMapType == 0 && MapMode != CMapDataMgr::GetVectorMapMode())
		{
			MapMode = CMapDataMgr::GetVectorMapMode();
			//CGMessenger::PostToMainframe(WMG_AX_MAP_RESET, MapMode, (int)m_lEllipsoid);
		}	
		
		
		// 레스터 지도 도시
		s_flag_raster_view raster_view; 
		raster_view =	CMapDataMgr::get_flag_raster_view();	
		if( raster_view.visible != FALSE )
		//if((lMapScale == 50000 && raster_view.visible_5) == TRUE ||
		//	(lMapScale == 100000 && raster_view.visible_10) == TRUE ||
		//	(lMapScale == 250000 && raster_view.visible_25) == TRUE ||
		//	(lMapScale == 500000 && raster_view.visible_50) == TRUE ||
		//	(lMapScale == 1000000 && raster_view.visible_100) == TRUE ||
		//	(lMapScale == 2000000 && raster_view.visible_200) == TRUE)
		{
			if(CMapDataMgr::ExtractRasterMap(bIsAutoMode, uiRasterMapKind, rectGP, iDispMapType, iZoomRatio))
			{				  				
				BOOL visible_cur_scale;	int iCnt = CMapDataMgr::GetRasterMapCount();
				for(int i=0; i<iCnt; i++)
				{
					pRasterMap = CMapDataMgr::GetRasterMap(i);	
					
					visible_cur_scale = FALSE;
					switch( nScale )
					{
					case MS_1_50000 :		
						visible_cur_scale = raster_view.visible_5;		
						break;
					case MS_1_100000 :		
						visible_cur_scale = raster_view.visible_10;		
						break;
					case MS_1_250000 :		
						visible_cur_scale = raster_view.visible_25;		
						break;
					case MS_1_500000 :		
						visible_cur_scale = raster_view.visible_50;		
						break;
					case MS_1_1000000 :		
						visible_cur_scale = raster_view.visible_100;	
						break;
					case MS_1_2000000 :		
						visible_cur_scale = raster_view.visible_200;	
						break;
					default:				visible_cur_scale = FALSE;						break;
					}					
					if( (nScale != pRasterMap->m_enMapScale && MS_1_2000000 != pRasterMap->m_enMapScale)
						|| 0 == visible_cur_scale) 
					{						
						pRasterMap = NULL; 		
						continue;
					}									
					if(pRasterMap->m_bIsRasterDataOpen == TRUE)  
						CMapDataMgr::DrawRasterMap(pDC, pRasterMap);
				}
				
			}
		}
	
		s_flag_satellite_view satellite_view; 
		satellite_view = CMapDataMgr::get_flag_satellite_view();		
		if( satellite_view.visible != FALSE )			
		{
			if(CMapDataMgr::ExtractSatelliteMap(bIsAutoMode, rectGP, iDispMapType, iZoomRatio))
			{		
				int iCnt = CMapDataMgr::GetSatelliteMapIndexCount();				
				int cnt_draw = 0;
				for(int i=0; i<iCnt; i++)
				{
					pIndex = CMapDataMgr::GetSatelliteMapIndex(i);
					if(pIndex->m_pData != NULL)
					{
						CMapDataMgr::DrawSatelliteMap(pDC, pIndex);	++cnt_draw;
					}					
				} 
			}			
		}
		
		// CGLib 에 지도 정보 set
		
		if(iDispMapType == 0)
			CGLib::SetMapKind(iScale, 0);
		else if(iDispMapType == 1)
			CGLib::SetMapKind(0, (int)uiRasterMapKind);
		else
			CGLib::SetMapKind(iScale, (int)uiRasterMapKind);			  
		CGLib::SetMapRect(gpRect.m_R.left, gpRect.m_R.top, gpRect.m_R.right, gpRect.m_R.bottom);		
		 	 
		//if(!((lMapScale == 50000 && raster_view.visible_5) == TRUE ||
		//	(lMapScale == 100000 && raster_view.visible_10) == TRUE ||
		//	(lMapScale == 250000 && raster_view.visible_25) == TRUE ||
		//	(lMapScale == 500000 && raster_view.visible_50) == TRUE ||
		//	(lMapScale == 1000000 && raster_view.visible_100) == TRUE ||
		//	(lMapScale == 2000000 && raster_view.visible_200) == TRUE))
		if( raster_view.visible == FALSE)// && satellite_view.visible == FALSE )			
		{
			CBAMgr::SetDispMapType(0);
			//iDispMapType = 0;
		}

		// 벡터지도 복원도시
		//int type_dis_map = 0;
		//type_dis_map = CBAMgr::GetDispMapType();		
		if( iDispMapType != 1)
		{
			//iZoomRatio
			int mapscale = m_pMapInfo->GetMapScale();	
			if( iZoomRatio <= 2000000 )
				shpDraw( pDC, mapscale, iZoomRatio, rect, iDispMapType, true );	// 백터, 백터+래스터 		 
		}
		

		// select marking image draw
		CGPointGP gpoint;	CGRectGP rectGP2;
		if(CMapDataMgr::GetSelMarkingFlag(gpoint) == TRUE)
			SelMarkingDraw(pDC, dZoomFactor, rectGP, gpoint);
		CMapDataMgr::SetSelMarkingFlag(FALSE, gpoint);
		CMapDataMgr::set_current_map_kind((EGMapKind)iDispMapType);
	}
}
*/

/*
void CGMapDisplayMgr::OnPrepareDC(CWnd *pWnd, CDC *pDC, int nEllipsoid)
{
	CGDCGeo* pGeoDC = (CGDCGeo*)pDC;

	// UTM 좌표계를 사용하기 위한 지리객체 생성
	// 중복 생성 방지
	if(m_pGeoObj == NULL)
	{
		m_pGeoObj = (CGGeoObject*)new CGGeoObjGP;
		m_pGeoObj->SetMapMode(GMM_GP);			// 경위도 좌표
		m_pGeoObj->SetEllipsoid(nEllipsoid);	// 타원체를 WGS84로 설정(default)
		m_pGeoObj->SetValidity(0.00001);		// 유효숫자로 지정
	}

// (지리, UTM)좌표와 화면좌표 변환을 위한 매핑모드 설정
	pGeoDC->SetMapMode(m_pGeoObj);

	CRect rect;
	CRgn rgn;

	pWnd->GetClientRect(&rect);
	CPoint org((rect.right+rect.left)/2, (rect.top+rect.bottom)/2);

	CSize ext(rect.Width(), -rect.Height());
	rgn.CreateRectRgnIndirect(&rect);
	pGeoDC->SelectClipRgn(&rgn);

	if(m_WinOrg.m_P.lat <= 0.0) 
	{
		m_WinOrg.m_P.lon = 125.8;
		m_WinOrg.m_P.lat = 38.45;
	}

	//pGeoDC->SetWindowOrg(&m_WinOrg);	// 윈도우의 지리좌표 원점을 설정(지리좌표)
	pGeoDC->SetWindowExt(&m_WinExt);	// 윈도우의 지리좌표 범위를 설정(지리좌표)
	pGeoDC->SetViewportOrg(org);		// 화면좌표의 원점을 설정(장치좌표)
	pGeoDC->SetViewportExt(ext);		// 화면좌표의 범위를 설정(장치좌표)

	if(m_viewportOrg.x != 0 || m_viewportOrg.y != 0)
	{
		pGeoDC->DPtoGeoP(&m_WinOrg, &m_viewportOrg);
		pGeoDC->SetWindowOrg(&m_WinOrg);
		m_viewportOrg.x = 0;
		m_viewportOrg.y = 0;
	}

	rgn.DeleteObject();
}
*/

void CGMapDisplayMgr::shpDrawPost(CDC *pDC, double dZoomFactor, void *pCurSel, int iSelID)
{
	//return;
	if(m_pMapInfo == NULL)
		return;

	CCoord m_Coord;
	CCoordMgr*	m_CoordMgr = GP_COORDMGR;

	CRect rect;
	GPoint gp, gpOri;
	stVObject *pCurSelObject;
	CMapPriority *pPriority;
	CMapProperty *pProperty;
	shpObject *pShpObject;
	
	gpOri.x = 127.42906022388939;
	gpOri.y = 38.068137052285763;
	
	//rect = CGLib::GetClientRect();
	m_pWnd->GetClientRect(&rect);

	dZoomFactor/=100;
	dZoomFactor*=2.5;
	
	double left, top, right, bottom;
	//m_ifViewInfo->GetSelectedRange(&left, &top, &right, &bottom);
	
	//CGRectGP rectGP(left, top, right, bottom);

	CGRectGP rectGP;//(left, top, right, bottom);

	m_CoordMgr->ScreenXY2WorldXY(rect.left, rect.top, &left, &top);
	m_CoordMgr->ScreenXY2WorldXY(rect.right, rect.bottom, &right, &bottom);
	m_Coord.x_y_to_degrees_lat_long(left, top, &rectGP.m_R.top, &rectGP.m_R.left);
	m_Coord.x_y_to_degrees_lat_long(right, bottom, &rectGP.m_R.bottom, &rectGP.m_R.right);

	
	gp.x = gpOri.x - (gpOri.x - left);
	gp.y = gpOri.y - (gpOri.y - top);
	
	if(pCurSel != NULL)
	{
		pCurSelObject = (stVObject *)pCurSel;
		pProperty = pCurSelObject->m_pMapProperty;
		pShpObject = pCurSelObject->m_pObject;
		
		for(int i=0; i<pCurSelObject->m_paMapPriority.GetSize(); i++)
		{
			pPriority = pCurSelObject->m_paMapPriority[i];
			if(pPriority != NULL && pProperty != NULL && pShpObject != NULL && pPriority->m_bIsVisible == TRUE)
			{
				switch(pProperty->m_enMapProperty)
				{
				case MP_POINT : 
					{
						shpDrawPoint(pDC, dZoomFactor, gpOri, rectGP, pProperty, pShpObject, pPriority, iSelID);
						break;
					}
				case MP_POLYLINE : 
					{
						shpDrawPolyline(pDC, dZoomFactor, gpOri, rectGP, pProperty, pShpObject, pPriority, iSelID);
						break;
					}
				case MP_POLYGON : 
					{
						shpDrawPolygon(pDC, dZoomFactor, gpOri, rectGP, pProperty, pShpObject, pPriority, iSelID);
						break;
					}
				default : 
					{
						TRACE("error: undeclared map property type!(CGMapDisplayMgr::shpDrawPost)\n");
						break;
					}
				}
			}
			break;
		}
	}
	
	if(m_pSelmemDC != NULL)
		pDC->BitBlt(rectGP.m_R.left, rectGP.m_R.top, rectGP.Width(), -rectGP.Height(),
		m_pSelmemDC, rectGP.m_R.left, rectGP.m_R.top, SRCCOPY);
}

// 점 데이터 복원도시(일반)
BOOL CGMapDisplayMgr::shpDrawPoint(CDC* pDC, double dZoomFactor, GPoint gpOri, CGRectGP rectGP,
								   CMapProperty* pMapProperty, shpObject* pShpObject, CMapPriority* pMapPriority)
{
	CPen pen, *oldPen;
	CBrush brush, *oldbrush;
	CSize size;
	stSymbol* pSymbol;
	CBitmap* pBitmap;
	COLORREF colorTransparent;
	double len;
	
	// 속성정보 적용
	CPointPriority* pPriority = (CPointPriority*)pMapPriority;
	pSymbol = pPriority->m_pSymbol;
	
	// line width (default set)
	size.cx = 1;
	// radius
	len = pPriority->m_uiRadius + 1;
	pDC->DPtoLP(&size);
	
	if(pPriority->m_uiType != 2 && pSymbol == NULL)
	{
		// transparent		
		// object create
		pen.CreatePen(PS_SOLID, size.cx, pPriority->m_crFillColor);
		brush.CreateSolidBrush(pPriority->m_crFillColor);
		oldPen = pDC->SelectObject(&pen);
		oldbrush = pDC->SelectObject(&brush);
	}
	else if(pPriority->m_uiType == 2 && pSymbol != NULL)
	{
		pBitmap = CBitmap::FromHandle(pSymbol->m_hBitmap);
		colorTransparent = RGB(255,255,255);
	}
	
	pShpObject->SetPointType(pPriority->m_uiType, len);
	
	// point type set
	switch(pPriority->m_uiType)
	{
	// circle, square
	case 0 :
	case 1 :
		pShpObject->Draw(pDC, m_lEllipsoid, dZoomFactor, gpOri, rectGP, 63);
		break;
	// symbol
	case 2 :
		if(pSymbol == NULL)
			pShpObject->Draw(pDC, m_lEllipsoid, dZoomFactor, gpOri, rectGP, 63);
		//else if(pPriority->m_uiType == 2 && pSymbol != NULL)
		//	pShpObject->Draw(pDC, m_lEllipsoid, dZoomFactor, gpOri, rectGP, 63, pBitmap, colorTransparent);
		break;
	}
	
	//if(pPriority->m_uiType != 2 && pSymbol == NULL)
	//{
	//	pDC->SelectObject(oldPen);
	//	pDC->SelectObject(oldbrush);
	//	pen.DeleteObject();
	//	brush.DeleteObject();
	//}
	
	return TRUE;
}

// 점 데이터 복원도시(지형요소속성보기)
BOOL CGMapDisplayMgr::shpDrawPoint(CDC* pDC, double dZoomFactor, GPoint gpOri, CGRectGP rectGP,
								   CMapProperty* pMapProperty, shpObject* pShpObject, CMapPriority* pMapPriority, int iSelID)
{
	CPen pen, *oldPen;
	CBrush brush, *oldBrush;
	LOGBRUSH logBrush;
	::ZeroMemory(&logBrush, sizeof(LOGBRUSH));
	CSize size;
	stSymbol* pSymbol;
	COLORREF crLineColor, crFillColor;
	double len;
	
	// 속성정보 적용
	CPointPriority* pPriority = (CPointPriority*)pMapPriority;
	pSymbol = pPriority->m_pSymbol;
	
	// line width (default set)
	size.cx = 1;
	// radius
	len = pPriority->m_uiRadius + 1;
	pDC->DPtoLP(&size);
	pShpObject->SetPointType(pPriority->m_uiType, len);
	
	crLineColor = RGB(0, 0, 0);
	crFillColor = RGB(0, 210, 210);
	
	pen.CreatePen(PS_SOLID, size.cx, crLineColor);
	brush.CreateSolidBrush(crFillColor);
	oldPen = pDC->SelectObject(&pen);
	oldBrush = pDC->SelectObject(&brush);
	int iOldRop = pDC->SetROP2(R2_NOTXORPEN);
	int iOldBkMode = pDC->SetBkMode(TRANSPARENT);
	pShpObject->DrawSelected(pDC, iSelID);
	pDC->SetROP2(iOldRop);
	pDC->SetBkMode(iOldBkMode);
	pDC->SelectObject(oldPen);
	pDC->SelectObject(oldBrush);
	pen.DeleteObject();
	brush.DeleteObject();
	
	return TRUE;
}

// 선 데이터 복원도시(일반)
BOOL CGMapDisplayMgr::shpDrawPolyline(CDC* pDC, double dZoomFactor, GPoint gpOri, CGRectGP rectGP,
									  CMapProperty* pMapProperty, shpObject* pShpObject, CMapPriority* pMapPriority)
{
	CPen pen, *oldPen;
	CSize size, size1, size3;
	int iwidth;
	LOGBRUSH logBrush;
	
	// 속성정보 적용
	CPolylinePriority* pItem = (CPolylinePriority*)pMapPriority;
	
	// line width
	size.cx = pItem->m_uiLineWidth;
	pDC->DPtoLP(&size);
	
	// color
	COLORREF color = pItem->m_crLineColor;
	
	logBrush.lbColor = color;
	logBrush.lbHatch = 10;
	logBrush.lbStyle = BS_SOLID;
	DWORD *pStyle = NULL;
	
	// line style
	switch(pItem->m_uiType)
	{
		// solid
	case 0 : pen.CreatePen(PS_SOLID, size.cx, color);
		break;
		// dash
	case 1 : pStyle = new DWORD[2];
		pStyle[0]= size.cx*10;
		pStyle[1]= size.cx*2;
		pen.CreatePen(PS_GEOMETRIC|PS_USERSTYLE|PS_ENDCAP_FLAT|PS_JOIN_BEVEL,
			size.cx, &logBrush, 2, pStyle);
		break;
		// dot
	case 2 : pStyle = new DWORD[2];
		pStyle[0]= size.cx;
		pStyle[1]= size.cx;
		pen.CreatePen(PS_GEOMETRIC|PS_USERSTYLE|PS_ENDCAP_FLAT|PS_JOIN_BEVEL,
			size.cx, &logBrush, 2, pStyle);
		break;
		// dashdot
	case 3 : pStyle = new DWORD[4];
		pStyle[0]= size.cx*10;
		pStyle[1]= size.cx;
		pStyle[2]= size.cx*2;
		pStyle[3]= size.cx;
		pen.CreatePen(PS_GEOMETRIC|PS_USERSTYLE|PS_ENDCAP_FLAT|PS_JOIN_BEVEL,
			size.cx, &logBrush, 4, pStyle);
		break;
		// dashdotdot
	case 4 : pStyle = new DWORD[6];
		pStyle[0]= size.cx*10;
		pStyle[1]= size.cx;
		pStyle[2]= size.cx*2;
		pStyle[3]= size.cx;
		pStyle[4]= size.cx*2;
		pStyle[5]= size.cx;
		pen.CreatePen(PS_GEOMETRIC|PS_USERSTYLE|PS_ENDCAP_FLAT|PS_JOIN_BEVEL,
			size.cx, &logBrush, 6, pStyle);
		break;
		// null line
	case 9 : break;
		// default
	default : pen.CreatePen(PS_SOLID, size.cx, color);
		break;
	}
	
	if(pItem->m_uiType != 9)
	{
		oldPen = pDC->SelectObject(&pen); 
 		pShpObject->Draw(pDC, m_lEllipsoid, dZoomFactor, gpOri, rectGP, 63);
		pDC->SelectObject(oldPen);
		pen.DeleteObject();
		
		delete [] pStyle;
		pStyle = NULL;
	}
	
	// pattern 적용
	// 일반
	if(pItem->m_uiType2 != 999 && pItem->m_uiType2 <= 4)
	{
		::ZeroMemory(&logBrush, sizeof(LOGBRUSH));
		size.cx = pItem->m_uiLineWidth2;
		size.cy = pItem->m_uiLineWidth2;
		pDC->DPtoLP(&size);
		iwidth = (int)(size.cx / m_dScreenScale);
		
		double Widlate = 0.21;
		int lgWidth = (int)((double)iwidth * Widlate);
		
		logBrush.lbStyle = BS_SOLID;
		logBrush.lbColor = pItem->m_crLineColor2;
		
		switch(pItem->m_uiType2)
		{
			// solid
		case 0 : pen.CreatePen(PS_SOLID, lgWidth, pItem->m_crLineColor2);
			break;
			// dash
		case 1 : pStyle = new DWORD[2];
			pStyle[0]= size.cx*10;
			pStyle[1]= size.cx*2;
			pen.CreatePen(PS_GEOMETRIC|PS_USERSTYLE|PS_ENDCAP_FLAT|PS_JOIN_BEVEL,
				lgWidth, &logBrush, 2, pStyle);
			break;
			// dot
		case 2 : pStyle = new DWORD[2];
			pStyle[0]= size.cx;
			pStyle[1]= size.cx;
			pen.CreatePen(PS_GEOMETRIC|PS_USERSTYLE|PS_ENDCAP_FLAT|PS_JOIN_BEVEL,
				lgWidth, &logBrush, 2, pStyle);
			break;
			// dashdot
		case 3 : pStyle = new DWORD[4];
			pStyle[0]= size.cx*10;
			pStyle[1]= size.cx;
			pStyle[2]= size.cx*2;
			pStyle[3]= size.cx;
			pen.CreatePen(PS_GEOMETRIC|PS_USERSTYLE|PS_ENDCAP_FLAT|PS_JOIN_BEVEL,
				lgWidth, &logBrush, 4, pStyle);
			break;
			// dashdotdot
		case 4 : pStyle = new DWORD[6];
			pStyle[0]= size.cx*10;
			pStyle[1]= size.cx;
			pStyle[2]= size.cx*2;
			pStyle[3]= size.cx;
			pStyle[4]= size.cx*2;
			pStyle[5]= size.cx;
			pen.CreatePen(PS_GEOMETRIC|PS_USERSTYLE|PS_ENDCAP_FLAT|PS_JOIN_BEVEL,
				lgWidth, &logBrush, 6, pStyle);
			break;
			// default
		default : pen.CreatePen(PS_SOLID, lgWidth, pItem->m_crLineColor2);
			break;
		}
		
		oldPen = pDC->SelectObject(&pen);
		pShpObject->Draw(pDC, m_lEllipsoid, dZoomFactor, gpOri, rectGP, 63);
		pDC->SelectObject(oldPen);
		pen.DeleteObject();
		
		delete [] pStyle;
		pStyle = NULL;
	}
	// 교량
	else if(pItem->m_uiType2 != 999 && pItem->m_uiType2 == 5)
	{
	}
	// 절토지
	else if(pItem->m_uiType2 != 999 && pItem->m_uiType2 == 6)
	{
		::ZeroMemory(&logBrush, sizeof(LOGBRUSH));
		logBrush.lbStyle = BS_SOLID;
		logBrush.lbColor = pItem->m_crLineColor2;
		
		int iWidth = pItem->m_uiLineWidth;
		size.cx = iWidth * 4.0; // 티크의 길이
		size1.cx = iWidth; // 티크의 굵기
		if(size1.cx <= 0) size1.cx = 1.0;
		
		pDC->DPtoLP(&size);
		pDC->DPtoLP(&size1);
		pStyle = new DWORD[2];
		
		//티크의 간격
		size3.cx = iWidth * 1.2;
		size3.cy = iWidth * 1.2;
		
		pDC->DPtoLP(&size3);
		pStyle[0] = size1.cx;
		
		pStyle[1] = (UINT)(size3.cx * 2.0);
		pen.CreatePen(PS_GEOMETRIC|PS_USERSTYLE|PS_ENDCAP_FLAT|PS_JOIN_BEVEL, size.cx, &logBrush,2,pStyle);
		oldPen = pDC->SelectObject(&pen);
		pShpObject->Draw(pDC, m_lEllipsoid, dZoomFactor, gpOri, rectGP, 63);
		pDC->SelectObject(oldPen);
		pen.DeleteObject();
		delete [] pStyle;
		pStyle = NULL;
	}
	// 철도/제방
	else if(pItem->m_uiType2 != 999 && pItem->m_uiType2 == 7)
	{
		::ZeroMemory(&logBrush, sizeof(LOGBRUSH));
		logBrush.lbStyle = BS_SOLID;
		logBrush.lbColor = pItem->m_crLineColor2;
		
		int iWidth = pItem->m_uiLineWidth;
		size.cx = iWidth * 4.0;
		size1.cx = iWidth;
		
		pDC->DPtoLP(&size);
		pDC->DPtoLP(&size1);
		pStyle = new DWORD[2];
		
		//티크의 간격
		size3.cx = iWidth * 2.0;
		size3.cy = iWidth * 2.0;
		
		pDC->DPtoLP(&size3);
		pStyle[0] = size1.cx;
		pStyle[1] = (UINT)(size3.cx * 2.0);
		pen.CreatePen(PS_GEOMETRIC|PS_USERSTYLE|PS_ENDCAP_FLAT|PS_JOIN_BEVEL, size.cx, &logBrush,2,pStyle);
		oldPen = pDC->SelectObject(&pen);
		pShpObject->Draw(pDC, m_lEllipsoid, dZoomFactor, gpOri, rectGP, 63);
		pDC->SelectObject(oldPen);
		pen.DeleteObject();
		delete [] pStyle;
		pStyle = NULL;
	}
	// 터널
	else if(pItem->m_uiType2 != 999 && pItem->m_uiType2 == 8)
	{
		::ZeroMemory(&logBrush, sizeof(LOGBRUSH));
		size.cx = pItem->m_uiLineWidth2;
		size.cy = pItem->m_uiLineWidth2;
		pDC->DPtoLP(&size);
		iwidth = (int)(size.cx / m_dScreenScale);
		
		double Widlate = 0.21;
		int lgWidth = (int)((double)iwidth * Widlate);
		
		logBrush.lbStyle = BS_SOLID;
		logBrush.lbColor = pItem->m_crLineColor2;
		
		pStyle = new DWORD[2];
		pStyle[0]= size.cx;
		pStyle[1]= size.cx;
		pen.CreatePen(PS_GEOMETRIC|PS_USERSTYLE|PS_ENDCAP_FLAT|PS_JOIN_BEVEL,
			lgWidth, &logBrush, 2, pStyle);
		oldPen = pDC->SelectObject(&pen);
		pShpObject->Draw(pDC, m_lEllipsoid, dZoomFactor, gpOri, rectGP, 63);
		pDC->SelectObject(oldPen);
		pen.DeleteObject();
		
		delete [] pStyle;
		pStyle = NULL;
	}	
	return TRUE;
}

// 선 데이터 복원도시(지형요소속성보기)
BOOL CGMapDisplayMgr::shpDrawPolyline(CDC* pDC, double dZoomFactor, GPoint gpOri, CGRectGP rectGP,
									  CMapProperty* pMapProperty, shpObject* pShpObject, CMapPriority* pMapPriority, int iSelID)
{
	CPen pen, *oldPen;
	CBrush brush, *oldBrush;
	LOGBRUSH logBrush;
	::ZeroMemory(&logBrush, sizeof(LOGBRUSH));
	CSize size;
	COLORREF crLineColor, crFillColor;
	
	CPolylinePriority* pItem = (CPolylinePriority*)pMapPriority;
	
	// line width
	size.cx = pItem->m_uiLineWidth + 1;
	pDC->DPtoLP(&size);
	
	// color
	crLineColor = RGB(0, 210, 210);
	crFillColor = RGB(0, 210, 210);
	
	pen.CreatePen(PS_SOLID, size.cx, crLineColor);
	brush.CreateSolidBrush(crFillColor);
	oldPen = pDC->SelectObject(&pen);
	oldBrush = pDC->SelectObject(&brush);
	pShpObject->DrawSelected(pDC, iSelID);
	pDC->SelectObject(oldPen);
	pDC->SelectObject(oldBrush);
	pen.DeleteObject();
	brush.DeleteObject();
	
	return TRUE;
}

// 면 데이터 복원도시(일반)
BOOL CGMapDisplayMgr::shpDrawPolygon(CDC* pDC, double dZoomFactor, GPoint gpOri, CGRectGP rectGP,
									 CMapProperty* pMapProperty, shpObject* pShpObject, CMapPriority* pMapPriority)
{
	CPen pen, *oldPen;
	CBrush brush, *oldBrush;
	LOGBRUSH logBrush;
	::ZeroMemory(&logBrush, sizeof(LOGBRUSH));
	CSize size;
	COLORREF crLineColor, crFillColor;
	CBitmap* pBitmap;
	
	// 속성정보 적용
	CPolygonPriority* pPriority = (CPolygonPriority*)pMapPriority;
	
	stSymbol* pPattern = pPriority->m_pPattern;
	
	// line width
	size.cx = pPriority->m_uiLineWidth;
	pDC->DPtoLP(&size);
	
	crLineColor = pPriority->m_crLineColor;
	crFillColor = pPriority->m_crFillColor;
	
	// type
	switch(pPriority->m_uiType)
	{
		// normal
	case 0 : 
		{
			if(CBAMgr::GetDispMapType() == 0)
			{
				pen.CreatePen(PS_SOLID, size.cx, crLineColor);
				brush.CreateSolidBrush(crFillColor);
				oldPen = pDC->SelectObject(&pen);
				oldBrush = pDC->SelectObject(&brush);
				pShpObject->Draw(pDC, m_lEllipsoid, dZoomFactor, gpOri, rectGP, 63);
				pDC->SelectObject(oldPen);
				pDC->SelectObject(oldBrush);
				pen.DeleteObject();
				brush.DeleteObject();
			}
			else if(CBAMgr::GetDispMapType() == 2)
			{
				pen.CreatePen(PS_SOLID, size.cx, crLineColor);
				oldPen = pDC->SelectObject(&pen);
				pDC->SetBkMode(TRANSPARENT);
				int nRopMode = pDC->SetROP2(R2_MASKPEN);
				pShpObject->Draw(pDC, m_lEllipsoid, dZoomFactor, gpOri, rectGP, 63);
				pDC->SelectObject(oldPen);
				pen.DeleteObject();
			}
		}
		break;
		// pattern
	case 1 : 
		{
			if(pPattern == NULL)
			{
				if(CBAMgr::GetDispMapType() == 0)
				{
					pen.CreatePen(PS_SOLID, size.cx, crLineColor);
					brush.CreateSolidBrush(crFillColor);
					oldPen = pDC->SelectObject(&pen);
					oldBrush = pDC->SelectObject(&brush);
					pShpObject->Draw(pDC, m_lEllipsoid, dZoomFactor, gpOri, rectGP, 63);
					pDC->SelectObject(oldPen);
					pDC->SelectObject(oldBrush);
					pen.DeleteObject();
					brush.DeleteObject();
				}
				else if(CBAMgr::GetDispMapType() == 2)
				{
					pen.CreatePen(PS_SOLID, size.cx, crLineColor);
					oldPen = pDC->SelectObject(&pen);
					pDC->SetBkMode(TRANSPARENT);
					int nRopMode = pDC->SetROP2(R2_MASKPEN);
					pShpObject->Draw(pDC, m_lEllipsoid, dZoomFactor, gpOri, rectGP, 63);
					pDC->SelectObject(oldPen);
					pen.DeleteObject();
				}
			}
			else
			{
				pBitmap = CBitmap::FromHandle(pPattern->m_hBitmap);
				brush.CreatePatternBrush(pBitmap);
				brush.GetLogBrush(&logBrush);
				pen.CreatePen(PS_GEOMETRIC|PS_ENDCAP_FLAT, size.cx, &logBrush);
				oldPen = pDC->SelectObject(&pen);
				oldBrush = pDC->SelectObject(&brush);
				int iOldRop = pDC->SetROP2(R2_MASKPEN);
				int iOldBkMode = pDC->SetBkMode(TRANSPARENT);
				pShpObject->Draw(pDC, m_lEllipsoid, dZoomFactor, gpOri, rectGP, 63);
				pDC->SetROP2(iOldRop);
				pDC->SetBkMode(iOldBkMode);
				pDC->SelectObject(oldPen);
				pDC->SelectObject(oldBrush);
				pen.DeleteObject();
				brush.DeleteObject();
			}
		}
		break;
	}
	
	return TRUE;
}

// 면 데이터 복원도시(지형요소속성보기)
BOOL CGMapDisplayMgr::shpDrawPolygon(CDC* pDC, double dZoomFactor, GPoint gpOri, CGRectGP rectGP, 
									 CMapProperty* pMapProperty, shpObject* pShpObject, CMapPriority* pMapPriority, int iSelID)
{
	CPen pen, *oldPen;
	CBrush brush, *oldBrush;
	LOGBRUSH logBrush;
	::ZeroMemory(&logBrush, sizeof(LOGBRUSH));
	CSize size;
	DWORD *pStyle = NULL;
	COLORREF crLineColor, crFillColor;
	//	WORD HatchPattern[8] = {0,0,0,0,0,0,0,0};
	//	WORD pPattern2[8] = { 0xaaaa, 0x5555, 0xaaaa, 0x5555, 0xaaaa, 0x5555, 0xaaaa, 0x5555 };
	//	HBITMAP hBitMap;
	//	CBitmap* pBitmap;
	//	for(int i=0; i<8; i++)
	//		HatchPattern[i] |= pPattern2[i];
	//	hBitMap = ::CreateBitmap(8, 8, 1, 1, HatchPattern);
	
	// 속성정보 적용
	CPolygonPriority* pPriority = (CPolygonPriority*)pMapPriority;
	
	stSymbol* pPattern = pPriority->m_pPattern;
	
	// line width
	size.cx = pPriority->m_uiLineWidth;
	pDC->DPtoLP(&size);
	
	crLineColor = RGB(0, 0, 0);
	crFillColor = RGB(0, 210, 210);
	
	pen.CreatePen(PS_SOLID, size.cx, crLineColor);
	brush.CreateSolidBrush(crFillColor);
	oldPen = pDC->SelectObject(&pen);
	oldBrush = pDC->SelectObject(&brush);
	int iOldRop = pDC->SetROP2(R2_NOTXORPEN);
	int iOldBkMode = pDC->SetBkMode(TRANSPARENT);
	pShpObject->DrawSelected(pDC, iSelID);
	pDC->SetROP2(iOldRop);
	pDC->SetBkMode(iOldBkMode);
	pDC->SelectObject(oldPen);
	pDC->SelectObject(oldBrush);
	pen.DeleteObject();
	brush.DeleteObject();
	
	return TRUE;
}

void CGMapDisplayMgr::shpDrawText(CDC* pDC, stTextPriority* pPriority, stMapText* pMapText)
{
	if(pPriority == NULL || pMapText == NULL)
		return;
	if(!m_ClientRectGp.PtInRect(pMapText->m_gpPoint))
		return;
	if(!pPriority->m_strWorldMapPoint.IsEmpty())
		return;
	
	CString strName = pMapText->m_strName.Mid(0,2);
	BYTE byte[2];
	memset(byte, ' ', 2);
	memcpy(byte, (LPCTSTR)strName, strName.GetLength());
	
	CString strByte;
	strByte.Format("%d", byte[0]);
	int iByte = atoi(strByte);
	BOOL bRes = FALSE;
	
	switch(pPriority->m_uiVisibleType)
	{
		// 한글만 출력
	case 0 : 
		if(iByte >=0 && iByte <= 127) bRes = TRUE;
		break;
		// 영문만 출력
	case 1 : 
		if(iByte > 127) bRes = TRUE;
		break;
		// 한글+영문 출력
	case 2 :
		break;
	default :
		break;
	}
	
	if(bRes) return;
	
	USES_CONVERSION;
	LOGFONT lf;
	::ZeroMemory(&lf, sizeof(LOGFONT));
	
	int nFontSize = pPriority->m_uiFontSize;
	CSize cSize(nFontSize, nFontSize);
	pDC->DPtoLP(&cSize);
	
	lf.lfHeight = (int)(cSize.cx);
	//lf.lfWidth = 0;
	lf.lfEscapement = 0;
	lf.lfOrientation = 0;
	lf.lfWeight = FW_REGULAR;
	
	CString strFaceName;
	stFontType* pFontType = pPriority->m_pFontType;
	if(pFontType == NULL)
	{
		lf.lfCharSet = ANSI_CHARSET;  //한글 지원
		strFaceName = "굴림";
		lstrcpynA(lf.lfFaceName, T2A((LPTSTR)(LPCTSTR)strFaceName), LF_FACESIZE);
	}
	else
	{		
		lf.lfCharSet = pFontType->m_nCharSet;
		lf.lfPitchAndFamily = pFontType->m_nPitchAndFamily;
		strFaceName = pFontType->m_strName;
		lstrcpynA(lf.lfFaceName, T2A((LPTSTR)(LPCTSTR)strFaceName), LF_FACESIZE);
	}
	
	CFont font;
	font.CreateFontIndirect(&lf);
	
	CFont *def_font	= pDC->SelectObject(&font);
	int iOldRop2 = pDC->SetROP2(R2_MASKPEN);
	int iOldBkMode2 = pDC->SetBkMode(TRANSPARENT);
	COLORREF oldColor2 = pDC->SetTextColor(pPriority->m_clFontColor);
	pDC->SetTextAlign(TA_LEFT);
	
	//pDC->TextOut(&pMapText->m_gpPoint, pMapText->m_strName);
	pDC->TextOut(pMapText->m_gpPoint.m_P.lat, pMapText->m_gpPoint.m_P.lon, pMapText->m_strName);
	
	pDC->SetROP2(iOldRop2);
	pDC->SetBkMode(iOldBkMode2);
	pDC->SetTextColor(oldColor2);
	pDC->SelectObject(def_font);
	font.DeleteObject();
}

void CGMapDisplayMgr::SelMarkingDraw(CDC *pDC, double dZoomFactor, CGRectGP rectGP, CGPointGP gpoint)
{
	CCoord m_Coord;
	CCoordMgr*	m_CoordMgr = GP_COORDMGR;

	stSymbol* pSymbol = CMapDataMgr::GetSelPointSymbol();
	if(pSymbol == NULL) return;
	
	CRect rect;
	GPoint gpOri;
	gpOri.x = 127.42906022388939;
	gpOri.y = 38.068137052285763;
	
	dZoomFactor/=100;
	dZoomFactor*=2.5;
	
	CBitmap* pBitmap;
	COLORREF colorTransparent;
	CDC memDC, maskDC, tempDC;
	CBitmap maskBitmap;
	CBitmap bmpImage ;
	BITMAP bm;
	CBitmap *pOldBitmap, *pOldmaskBmp, *pold;
	CSize widthLP, widthDP;
	CPoint Src;
	CGPointGP GpSrc;
	POINT DestLP;
	CGPointGP DestGP;
	
	pBitmap = CBitmap::FromHandle(pSymbol->m_hBitmap);
	colorTransparent = RGB(255,255,255);
	
	if(pBitmap == NULL) return;
	if(pBitmap->m_hObject == NULL) return;
	if(pBitmap->GetBitmap(&bm) == 0) return;
	
	// DC saving..
	int savedDC = pDC->SaveDC();
	
	maskDC.CreateCompatibleDC(pDC);
	memDC.CreateCompatibleDC(pDC);
	tempDC.CreateCompatibleDC(pDC);
	
	pold = tempDC.SelectObject(pBitmap);
	
	bmpImage.CreateCompatibleBitmap(pDC,bm.bmWidth,bm.bmHeight);
	pOldBitmap = memDC.SelectObject(&bmpImage);
	
	memDC.CDC::SetStretchBltMode(STRETCH_ANDSCANS);
	memDC.CDC::StretchBlt(0,0,bm.bmWidth,bm.bmHeight,&tempDC, 0,0,bm.bmWidth,bm.bmHeight,SRCCOPY); 
	
	maskBitmap.CreateBitmap(bm.bmWidth,bm.bmHeight,1,1,NULL);
	pOldmaskBmp=maskDC.SelectObject(&maskBitmap); 
	memDC.CDC::SetBkColor(colorTransparent);
	
	maskDC.CDC::SetStretchBltMode(STRETCH_ANDSCANS);
	maskDC.CDC::StretchBlt(0,0,bm.bmWidth,bm.bmHeight,&memDC, 0,0,bm.bmWidth,bm.bmHeight,SRCCOPY); 
	
	memDC.CDC::SetBkColor(RGB(0,0,0)); 
	memDC.CDC::SetTextColor(RGB(255,255,255));
	memDC.CDC::SetStretchBltMode(STRETCH_ANDSCANS);
	memDC.CDC::StretchBlt(0,0,bm.bmWidth,bm.bmHeight, &maskDC, 0,0,bm.bmWidth,bm.bmHeight,SRCAND);
	
	pDC->SetBkColor(RGB(255,255,255));
	pDC->SetTextColor(RGB(0,0,0));
	
	::SetStretchBltMode(pDC->m_hDC, COLORONCOLOR);
	
	if (dZoomFactor > 0.4f)
		dZoomFactor = 0.4f;
	double dblScale = 2.5f * dZoomFactor;
	
	// now drawing..
	CGPointGP gpLT, gpRB;
	CGRectGP gpRect;
	CPoint point;
	CPoint point1, point2;
	CPoint tpt;
	double dlimit = bm.bmWidth;
	double lx, ly;
	
	//pDC->GeoPtoLP(&point, &gpoint);
	m_Coord.degrees_lat_long_to_x_y (gpoint.m_P.lat, gpoint.m_P.lon, &lx, &ly); 
	m_CoordMgr->WorldXY2ScreenXY(lx, ly, &point.x, &point.y);

	
	DestLP.x = point.x;
	DestLP.y = point.y;
	
	CPoint dpt = DestLP;
	CPoint symPos;
	pDC->LPtoDP(&dpt);
	symPos = dpt;
	
	dpt.x -= dlimit;
	dpt.y += dlimit;
	symPos.x -= dlimit/2;
	symPos.y -= dlimit/2;
	
	pDC->DPtoLP(&dpt);
	pDC->DPtoLP(&symPos);
	
	tpt.x = DestLP.x - dpt.x;
	tpt.y = DestLP.y - dpt.y;
	widthLP = tpt;
	
	pDC->CDC::StretchBlt(symPos.x, symPos.y, (INT)(widthLP.cx * dblScale), 
		(INT)(-widthLP.cy * dblScale), &maskDC, 0,0, bm.bmWidth,bm.bmHeight,SRCAND);
	pDC->CDC::StretchBlt(symPos.x, symPos.y, (INT)(widthLP.cx * dblScale), 
		(INT)(-widthLP.cy * dblScale), &memDC, 0,0, bm.bmWidth,bm.bmHeight,SRCPAINT);
	
	tempDC.SelectObject(pold);
	maskDC.SelectObject(pOldmaskBmp);
	memDC.SelectObject(pOldBitmap);
	
	//maskDC.MemReset();
	//memDC.MemReset();
	//tempDC.MemReset();
	
	maskBitmap.DeleteObject();
	bmpImage.DeleteObject();
	
	// DC restoring..
	pDC->RestoreDC(savedDC);
}

//static int id_tmp = 1;
void CGMapDisplayMgr::shpDraw(CDC* pDC, int scale, int ratio, CGRectGP rectGP, int type_dis_map, BOOL draw )
{	
	CCoord m_Coord;
	CCoordMgr*	m_CoordMgr = GP_COORDMGR;

	//shp 파일 경로 불러오기 
	{
		int cnt_path = load_mgr.get_cnt_path(); 
		if( cnt_path == 0 )
		{			
			load_mgr.set_path_file( "C:\\AFC4I\\map\\mapinfo\\path_shp.afccs" ); 
			load_mgr.load_path_shp();			
		}
	}
	
	EGMapScale nScale;		
	std::vector< SFILE_INFO > vec_path_shp_land;	
	//std::vector< SFILE_INFO > vec_path_shp_air;	
	
	switch(scale)
	{
	case 0 : nScale = MS_1_25000;
		break;
	case 1 :
		{ 
			nScale = MS_1_50000;
			vec_path_shp_land = load_mgr.search_shp_ex( TYPE_EARTH_5, rectGP.m_R.left, rectGP.m_R.bottom, rectGP.m_R.right, rectGP.m_R.top );
			break;
		}
	case 2 : 
		{
			vec_path_shp_land = load_mgr.search_shp_ex( TYPE_EARTH_10, rectGP.m_R.left, rectGP.m_R.bottom, rectGP.m_R.right, rectGP.m_R.top );
			nScale = MS_1_100000;
			break;
		}
	case 3 : 
		{
			//vec_path_shp_air = load_mgr.search_shp_ex( TYPE_AIR_25, rectGP.m_R.left, rectGP.m_R.bottom, rectGP.m_R.right, rectGP.m_R.top );
			vec_path_shp_land = load_mgr.search_shp_ex( TYPE_EARTH_25, rectGP.m_R.left, rectGP.m_R.bottom, rectGP.m_R.right, rectGP.m_R.top );
			nScale = MS_1_250000;
			break;
		}
	case 4 : 
		{			
			//vec_path_shp_air = load_mgr.search_shp_ex( TYPE_AIR_50, rectGP.m_R.left, rectGP.m_R.bottom, rectGP.m_R.right, rectGP.m_R.top );
			vec_path_shp_land = load_mgr.search_shp_ex( TYPE_EARTH_50, rectGP.m_R.left, rectGP.m_R.bottom, rectGP.m_R.right, rectGP.m_R.top );
			nScale = MS_1_500000;
			break;
		}
	case 5 : 
		{
			//vec_path_shp_air = load_mgr.search_shp_ex( TYPE_AIR_100, rectGP.m_R.left, rectGP.m_R.bottom, rectGP.m_R.right, rectGP.m_R.top );
			vec_path_shp_land = load_mgr.search_shp_ex( TYPE_EARTH_100, rectGP.m_R.left, rectGP.m_R.bottom, rectGP.m_R.right, rectGP.m_R.top );
			nScale = MS_1_1000000;
			break;
		}
	case 6 :
		{
			//vec_path_shp_air = load_mgr.search_shp_ex( TYPE_AIR_200, rectGP.m_R.left, rectGP.m_R.bottom, rectGP.m_R.right, rectGP.m_R.top );
			nScale = MS_1_2000000;
			break;
		}
	}	
	int cnt_path_land = 0;
	//int cnt_path_air = 0;
	cnt_path_land = vec_path_shp_land.size();
	//cnt_path_air = vec_path_shp_air.size();	

/*
	//상황도성능개선
	if( scale < 3 )
	{
		m_shp_mgr.close_shpfile_5_10();
		SFILE_INFO file_info;		
		for( int cnt = 0; cnt < cnt_path_land; ++cnt )
		{		
			file_info = vec_path_shp_land.at( cnt );
			m_shp_mgr.add_id_file(file_info.id_file);
			m_shp_mgr.load_shp(  file_info.id_file, file_info.path_file ,file_info.type_map, pDC, nScale );	
		}
	}
	else 
	*/
	{
		//육도를 읽는다.
		SFILE_INFO file_info;
		for( int cnt = 0; cnt < cnt_path_land; ++cnt )
		{		
			file_info = vec_path_shp_land.at( cnt );				
			//열려 있는 파일이 아니라면 
			//if( FALSE == m_shp_mgr.search_id_file( file_info.id_file ) )
			//{
			//	m_shp_mgr.add_id_file( file_info.id_file );	 
			//	m_shp_mgr.load_shp( file_info.id_file, file_info.path_file ,file_info.type_map, pDC, nScale );				
			//}					
		}
		//공도를 읽는다.
		/*
		for(int cnt = 0; cnt < cnt_path_air; ++cnt )
		{		
			file_info = vec_path_shp_air.at( cnt );			
			//열려 있는 파일이 아니라면 
			//if( FALSE == m_shp_mgr.search_id_file( file_info.id_file ) )
			//{
			//	m_shp_mgr.add_id_file( file_info.id_file ); 
			//	m_shp_mgr.load_shp( file_info.id_file, file_info.path_file, file_info.type_map, pDC, nScale );
			//}		
		} 
		*/
	}		 
	

	
	//m_shp_mgr.close_shpfile();
	//m_shp_mgr.mmap_id.clear();

	//SHP DRAWING
	//if( draw == TRUE )
	//	m_shp_mgr.draw_shp( pDC, rectGP, nScale, ratio, type_dis_map );
	
	//IDENTIFY
	if( CMapDataMgr::is_identify() == TRUE )
	{
		CPoint coord = CMapDataMgr::get_identify_coord();
		CGPointGP  pt_geo_selected, pt_geo_selected1;	
		//pDC->DPtoGeoP( &pt_geo_selected, &coord );
		m_CoordMgr->ScreenXY2WorldXY(coord.x, coord.y, &pt_geo_selected.m_P.lon, &pt_geo_selected.m_P.lat);
		m_Coord.x_y_to_degrees_lat_long(pt_geo_selected.m_P.lat, pt_geo_selected.m_P.lon, 
														&pt_geo_selected1.m_P.lat, &pt_geo_selected1.m_P.lon);

		
		CMapDataMgr::delete_feature();
		//m_shp_mgr.identify_feature( pDC, rectGP, pt_geo_selected1, nScale, ratio );			
	} 
}

void CGMapDisplayMgr::backgroundShpDraw(CDC* pDC, double dZoomFactor)
{
	if(m_pMapInfo == NULL)
		return;

	CCoord m_Coord;
	CCoordMgr*	m_CoordMgr = GP_COORDMGR;

	CRect clRect;
	CRect rect;
	GPoint gp, gpOri;
	gpOri.x = 127.42906022388939;
	gpOri.y = 38.068137052285763;
	
	//rect = CGLib::GetClientRect();
	m_pWnd->GetClientRect(&rect);

	dZoomFactor/=100;
	dZoomFactor*=2.5;
	
	double left, top, right, bottom;
	//HRESULT hRes = m_ifViewInfo->GetSelectedRange(&left, &top, &right, &bottom);
	CGRectGP rectGP;//(left, top, right, bottom);

	m_CoordMgr->ScreenXY2WorldXY(rect.left, rect.top, &left, &top);
	m_CoordMgr->ScreenXY2WorldXY(rect.right, rect.bottom, &right, &bottom);
	m_Coord.x_y_to_degrees_lat_long(left, top, &rectGP.m_R.top, &rectGP.m_R.left);
	m_Coord.x_y_to_degrees_lat_long(right, bottom, &rectGP.m_R.bottom, &rectGP.m_R.right);

	
	
	rectGP.m_R.left -= 1;
	rectGP.m_R.top += 1;
	rectGP.m_R.right += 1;
	rectGP.m_R.bottom -= 1;
	
	gp.x = gpOri.x - (gpOri.x - left);
	gp.y = gpOri.y - (gpOri.y - top);
	
	CPen pen, *oldPen;
	CBrush brush, *oldbrush;
	CSize size;
	size.cx = 1;
	pDC->DPtoLP(&size);
	
	shpObject* shpDraw;
	stSmallMap* smallMap = CMapDataMgr::GetSmallMapHead();
	int cnt_small_map = CMapDataMgr::GetSmallMapCount();
	
	while(smallMap)
	{
		smallMap = CMapDataMgr::GetSmallMapNext();
		if(smallMap == NULL) break;
		shpDraw = smallMap->m_pObject;
		switch(smallMap->m_enType)
		{
		case SM_SMALLMAP :
			{ 
				//pDC->FillSolidRect(&rectGP, m_clBackColor[0]);
				m_CoordMgr->WorldXY2ScreenXY(rectGP.m_R.top, rectGP.m_R.left, &clRect.left, &clRect.top);
				m_CoordMgr->WorldXY2ScreenXY(rectGP.m_R.bottom, rectGP.m_R.right, &clRect.right, &clRect.bottom);
				pDC->FillSolidRect(clRect.left, clRect.top, clRect.Width(), clRect.Height(), m_clBackColor[0]);

				//pDC->FillSolidRect(rectGP.m_R.left, rectGP.m_R.top, rectGP.Width(), rectGP.Height(), m_clBackColor[0]);

				pen.CreatePen(PS_SOLID, size.cx, RGB(64,64,64));	 
				brush.CreateSolidBrush(m_clBackColor[1]);
				oldPen = pDC->SelectObject(&pen);
				oldbrush = pDC->SelectObject(&brush);
				shpDraw->Draw(pDC, m_lEllipsoid, m_dZoomFactor, gpOri, rectGP, 63);
				pDC->SelectObject(oldPen);
				pDC->SelectObject(oldbrush);
				pen.DeleteObject();
				brush.DeleteObject();
			}
			break;
		}
	}
}

void CGMapDisplayMgr::worldmapShpDraw(CDC* pDC, double dZoomFactor, int iViewType)
{
	CCoord m_Coord;
	CCoordMgr*	m_pCoordMgr = GP_COORDMGR;

	if(m_pMapInfo == NULL)
		return;
	
	stTextPriority *pTextPriority = NULL;
	stTextPriority *pTextPriority1 = NULL;
	stTextPriority *pTextPriority2 = NULL;
	
	//CRect rect;
	CRect rc;
	GPoint gp, gpOri;
	gpOri.x = 127.42906022388939;
	gpOri.y = 38.068137052285763;
	
	//rect = CGLib::GetClientRect();
	dZoomFactor/=100;
	dZoomFactor*=2.5;
	
	int iShpType;
	double left, top, right, bottom;
	//m_ifViewInfo->GetSelectedRange(&left, &top, &right, &bottom);
	//CGRectGP(left, top, right, bottom);
	
	CGRectGP rectGP;// (left, top, right, bottom);

	m_pWnd->GetClientRect(&rc);
	//pView->ScreenToClient(&rc);
	
	m_pCoordMgr->ScreenXY2WorldXY(rc.left, rc.top, &left, &top);
	m_pCoordMgr->ScreenXY2WorldXY(rc.right, rc.bottom, &right, &bottom);
	m_Coord.x_y_to_degrees_lat_long(left, top, &rectGP.m_R.top, &rectGP.m_R.left);
	m_Coord.x_y_to_degrees_lat_long(right, bottom, &rectGP.m_R.bottom, &rectGP.m_R.right);

	
	gp.x = gpOri.x - (gpOri.x - rectGP.m_R.left);
	gp.y = gpOri.y - (gpOri.y - rectGP.m_R.top);
	
	CPen pen, *oldPen;
	CBrush brush, *oldbrush;
	CSize size;
	
	double dViewRatio;
	CString strFileName;
	int iIndex;
	shpObject* shpDraw;
	stSmallMap* smallMap = CMapDataMgr::GetWorldMapHead();
	while(smallMap)
	{
		smallMap = CMapDataMgr::GetWorldMapNext();
		if(smallMap == NULL) break;
		shpDraw = smallMap->m_pObject;
		
		iShpType = shpDraw->GetSHPType();
		if(iViewType != iShpType) continue;
		
		strFileName = _T("");
		
		switch(iShpType)
		{
		case SHPT_POINT : 
			{
				for(int i=0; i<CMapDataMgr::GetTextPriorityCnt(); i++)
				{
					pTextPriority = CMapDataMgr::GetTextPriority(i);
					if(pTextPriority->m_strWorldMapPoint == "국가명")
						pTextPriority1 = pTextPriority;
					else if(pTextPriority->m_strWorldMapPoint == "행정지역명")
						pTextPriority2 = pTextPriority;
				}
				
				size.cx = 1;
				pDC->DPtoLP(&size);
				pen.CreatePen(PS_SOLID, size.cx, RGB(255,0,0));
				brush.CreateSolidBrush(RGB(255,0,0));
				oldPen = pDC->SelectObject(&pen);
				oldbrush = pDC->SelectObject(&brush);
				
				iIndex = smallMap->m_strFileName.ReverseFind('\\');
				if(iIndex != -1)
					strFileName = smallMap->m_strFileName.Right(smallMap->m_strFileName.GetLength()-(iIndex+1));
				if(strFileName.Left(9) == "eur_point" ||
					strFileName.Left(9) == "noa_point" ||
					strFileName.Left(9) == "sas_point" ||
					strFileName.Left(9) == "soa_point")
				{
					shpDraw->SetPointType(0, 4);
					if(pTextPriority1 != NULL)
					{
						dViewRatio = 100.0 / m_dZoomFactor;
						if(pTextPriority1->m_bIsVisible == FALSE ||
							dViewRatio > (double)pTextPriority1->m_uiViewRatio)
						{
							pDC->SelectObject(oldPen);
							pDC->SelectObject(oldbrush);
							pen.DeleteObject();
							brush.DeleteObject();
							break;
						}
						
						if(pTextPriority1->m_pFontType == NULL)
						{
							((shpPoint*)shpDraw)->SetFontType(ANSI_CHARSET);
							((shpPoint*)shpDraw)->SetFaceName("굴림");
						}
						else
						{
							((shpPoint*)shpDraw)->SetFontType(pTextPriority1->m_pFontType->m_nCharSet);
							((shpPoint*)shpDraw)->SetFaceName(pTextPriority1->m_pFontType->m_strName);
						}
						
						((shpPoint*)shpDraw)->SetFontSize(pTextPriority1->m_uiFontSize);
						((shpPoint*)shpDraw)->SetFontStrikeOut(FALSE);
						((shpPoint*)shpDraw)->SetFontColor(pTextPriority1->m_clFontColor);
						((shpPoint*)shpDraw)->SetVisibleType(pTextPriority1->m_uiVisibleType);
					}
					shpDraw->SetLabel(0);
				}
				else
				{
					shpDraw->SetPointType(0, 2);
					if(pTextPriority2 != NULL)
					{
						dViewRatio = 100.0 / m_dZoomFactor;
						if(pTextPriority2->m_bIsVisible == FALSE ||
							dViewRatio > (double)pTextPriority2->m_uiViewRatio)
						{
							pDC->SelectObject(oldPen);
							pDC->SelectObject(oldbrush);
							pen.DeleteObject();
							brush.DeleteObject();
							break;
						}
						
						if(pTextPriority1->m_pFontType == NULL)
						{
							((shpPoint*)shpDraw)->SetFontType(ANSI_CHARSET);
							((shpPoint*)shpDraw)->SetFaceName("굴림");
						}
						else
						{
							((shpPoint*)shpDraw)->SetFontType(pTextPriority2->m_pFontType->m_nCharSet);
							((shpPoint*)shpDraw)->SetFaceName(pTextPriority2->m_pFontType->m_strName);
						}
						
						((shpPoint*)shpDraw)->SetFontSize(pTextPriority2->m_uiFontSize);
						((shpPoint*)shpDraw)->SetFontStrikeOut(FALSE);
						((shpPoint*)shpDraw)->SetFontColor(pTextPriority2->m_clFontColor);
						((shpPoint*)shpDraw)->SetVisibleType(pTextPriority2->m_uiVisibleType);
					}
					shpDraw->SetLabel(1);
				}
				
				shpDraw->DrawMemoryMap(pDC, m_lEllipsoid, m_dZoomFactor, gpOri, rectGP, 63);
				pDC->SelectObject(oldPen);
				pDC->SelectObject(oldbrush);
				pen.DeleteObject();
				brush.DeleteObject();
			}
			break;
		case SHPT_ARC : 
			{
				iIndex = smallMap->m_strFileName.ReverseFind('\\');
				if(iIndex != -1)
					strFileName = smallMap->m_strFileName.Right(smallMap->m_strFileName.GetLength()-(iIndex+1));
				if(strFileName.Left(6) == "LAP030" ||
					strFileName.Left(6) == "LAP050" ||
					strFileName.Left(6) == "LAN010")
				{
					size.cx = 1;
					pDC->DPtoLP(&size);
					pen.CreatePen(PS_SOLID, size.cx, RGB(171,86,0));
					brush.CreateSolidBrush(RGB(171,86,0));
				}
				else
				{
					size.cx = 1;
					pDC->DPtoLP(&size);
					pen.CreatePen(PS_SOLID, size.cx, RGB(64,64,64));
					brush.CreateSolidBrush(RGB(64,64,64));
				}
				
				oldPen = pDC->SelectObject(&pen);
				oldbrush = pDC->SelectObject(&brush);
				shpDraw->Draw(pDC, m_lEllipsoid, m_dZoomFactor, gpOri, rectGP, 63);
				pDC->SelectObject(oldPen);
				pDC->SelectObject(oldbrush);
				pen.DeleteObject();
				brush.DeleteObject();
			}
			break;
		case SHPT_POLYGON : 
			{
				size.cx = 1;
				pDC->DPtoLP(&size);
				//pDC->FillSolidRect(&rectGP, m_clBackColor[0]);
				pDC->FillSolidRect(rectGP.m_R.left, rectGP.m_R.top, rectGP.Width(), rectGP.Height(), m_clBackColor[0]);

				//				pen.CreatePen(PS_SOLID, size.cx, m_clBackColor[0]);
				pen.CreatePen(PS_SOLID, size.cx, RGB(175,97,97));
				brush.CreateSolidBrush(m_clBackColor[1]);
				oldPen = pDC->SelectObject(&pen);
				oldbrush = pDC->SelectObject(&brush);
				shpDraw->DrawMemoryMap(pDC, m_lEllipsoid, m_dZoomFactor, gpOri, rectGP, 63);
				pDC->SelectObject(oldPen);
				pDC->SelectObject(oldbrush);
				pen.DeleteObject();
				brush.DeleteObject();
			}
			break;
		}
	}
}