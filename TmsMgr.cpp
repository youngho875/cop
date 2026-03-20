#include "pch.h"
#include "TmsMgr.h"

#include "../LIBSrc/GeoDC/Coord.h"
#include "../LIBSrc/GeoDC/CoordMgr.h"
#include <math.h>
#include <iostream>

CTmsMgr::CTmsMgr(void)
{
	mCoordMgr = GP_COORDMGR;
	mCoord.initialize_adaptation_data();
	mDirectoryPath = _T("d:/cesium/Source/Assets/Textures/NaturalEarthII");

}

CTmsMgr::~CTmsMgr(void)
{
	mImages.clear();
}

long CTmsMgr::CalcLevel(double Geoleft, double Geotop, double Georight, double Geobottom, CRect ClinetRect)
{
	DOUBLE GeoWidth = (Georight - Geoleft)<0 ? -(Georight - Geoleft) : (Georight - Geoleft); 

	DOUBLE ScreenWidth = ClinetRect.right - ClinetRect.left; 

	double UnitPerPixel = GeoWidth / ScreenWidth;
	double u = log10(180.0 / (256 * UnitPerPixel));
	double d = log10(2.0);   

	return  (u / d);  
}

long CTmsMgr::CalcIndexX(long Level, double Longitude) const
{
	return pow(2.0, Level) * ((Longitude + 180) / 180); 
}

long CTmsMgr::CalcIndexY(long Level, double Latitude) const
{
	return pow(2.0, Level) * ((Latitude + 90) / 180); 
}

double CTmsMgr::CalcMapX(long Level, long IndexX) const
{
	return CalcResolution(Level) * 256 * IndexX - 180;
}

double CTmsMgr::CalcMapY(long Level, long IndexY) const
{
	return CalcResolution(Level) * 256 * IndexY - 90;
}

double CTmsMgr::CalcResolution(long Level) const
{
	return (180.0 / 256) / pow(2.0, Level);
}

void CTmsMgr::Initalize()
{
	mCoordMgr = GP_COORDMGR;
	mCoord.initialize_adaptation_data(); 
	mDirectoryPath = _T("d:/cesium/Source/Assets/Textures/NaturalEarthII");
	//mDirectoryPath = "D:/mapdata/maptile";
}

void CTmsMgr::Load(CRect ClinetRect)
{
	DOUBLE MinWorldX = 0;
	DOUBLE MinWorldY = 0;
	DOUBLE MaxWorldX = 0;
	DOUBLE MaxWorldY = 0;

	double XYLeft, XYRight, XYTop, XYBottom;
	double GeoLeft, GeoRight, GeoTop, GeoBottom;

	mCoordMgr->ScreenXY2WorldXY( ClinetRect.left, ClinetRect.top, &XYLeft, &XYTop );
	mCoordMgr->ScreenXY2WorldXY( ClinetRect.right, ClinetRect.bottom, &XYRight, &XYBottom );
	mCoord.x_y_to_degrees_lat_long( XYLeft, XYTop, &GeoTop, &GeoLeft );
	mCoord.x_y_to_degrees_lat_long( XYRight, XYBottom, &GeoBottom, &GeoRight );

	long Level = CalcLevel(GeoLeft, GeoTop, GeoRight, GeoBottom, ClinetRect);
	long MinIndexX = CalcIndexX(Level, GeoLeft);
	long MinIndexY = CalcIndexY(Level, GeoBottom);
	long MaxIndexX = CalcIndexX(Level, GeoRight);
	long MaxIndexY = CalcIndexY(Level, GeoTop);


	MapImagesType Images;
	for( long IdxY = MinIndexY; IdxY <= MaxIndexY; ++IdxY )
	{
		for( long IdxX = MinIndexX; IdxX <= MaxIndexX; ++IdxX )
		{
			CString TileLevelStr; TileLevelStr.Format("%d", Level);
			CString IndexXStr; IndexXStr.Format("%d", IdxX);
			CString IndexYStr; IndexYStr.Format("%d", IdxY);
			  
			CString TileKey = TileLevelStr;
			TileKey +="_";
			TileKey += IndexXStr;
			TileKey +="_";
			TileKey += IndexYStr;

			MapImagesType::iterator iter = mImages.find(TileKey);
			
			if(iter == mImages.end())
			{
				CString Path = mDirectoryPath;
				CString FileName = IndexYStr;

				Path += "/" + TileLevelStr + "/" + IndexXStr + "/" + FileName + ".jpg";
			
				std::cout << Path << std::endl;
			
		/*		if(! find(Path) )
				{
					continue;
				}*/	 

				CImage * pTileImage = new CImage();

				HRESULT ck = pTileImage->Load(Path.GetString());
				if(ck != S_OK)
				{
					delete pTileImage;
					continue;
				}
				
				Images.insert(std::make_pair(TileKey ,pTileImage));
			}
			else
			{
				Images.insert(std::make_pair(TileKey, iter->second));
				mImages.erase(TileKey);
			}
		}
	}

	mImages.swap(Images);


	for(MapImagesType::iterator itr = Images.begin();itr != Images.end();itr++)///임시... 메모리해제 
	{
		delete (CImage*)itr->second;
	}
}

void CTmsMgr::UnLoad()
{
	mImages.clear();
}

void CTmsMgr::NeedLoad()
{
   
}

void CTmsMgr::Draw(CDC *pDC)
{
	BOOL bResult = FALSE;
	pDC->SetStretchBltMode(COLORONCOLOR);	 
	 

	MapImagesType::iterator iter_beg = mImages.begin();
	MapImagesType::iterator iter_end = mImages.end();
	for( ; iter_beg != iter_end; ++iter_beg)
	{
		CString TileKey = iter_beg->first;
				 	
		CDC bitCDC;
		CBitmap m_pBitmapTemp;
		bitCDC.CreateCompatibleDC(pDC); 
		m_pBitmapTemp.CreateCompatibleBitmap(pDC,iter_beg->second->GetWidth(),iter_beg->second->GetHeight());
		bitCDC.SelectObject(&m_pBitmapTemp);
		bitCDC.FillSolidRect(0,0,iter_beg->second->GetWidth(),iter_beg->second->GetHeight(),RGB(255,255,255));
		 
			
		CString seperator = "_";
		int position = 0; 
 		CString Level = TileKey.Tokenize(seperator, position);
		CString IndexXStr = TileKey.Tokenize(seperator, position);
		CString IndexYStr = TileKey.Tokenize(seperator, position);

		double MinX = CalcMapX(atoi(Level.GetString()), atoi(IndexXStr.GetString()));
		double MaxY = CalcMapY(atoi(Level.GetString()), atoi(IndexYStr.GetString()));
		double MaxX = CalcMapX(atoi(Level.GetString()), atoi(IndexXStr.GetString()) + 1);
		double MinY = CalcMapY(atoi(Level.GetString()), atoi(IndexYStr.GetString()) + 1);

		long scrmX,scrmY,scrMX,scrMY;
		mCoordMgr->WorldLonLat2ScreenXY(MinX,MinY,&scrmX,&scrmY,mCoordMgr->m_fCenterX,mCoordMgr->m_fCenterY,mCoordMgr->m_fScale);
		mCoordMgr->WorldLonLat2ScreenXY(MaxX,MaxY,&scrMX,&scrMY,mCoordMgr->m_fCenterX,mCoordMgr->m_fCenterY,mCoordMgr->m_fScale);

		iter_beg->second->StretchBlt(pDC->GetSafeHdc(), scrmX, scrmY, scrMX - scrmX, scrMY - scrmY, 0, 0,iter_beg->second->GetWidth(),iter_beg->second->GetHeight());
		//::StretchDIBits(pDC->GetSafeHdc(),scrmX,scrmY,scrMX-scrmX,scrMY-scrmY,0,0,iter_beg->second->GetWidth(),iter_beg->second->GetHeight()
		//	,iter_beg->second->GetBits(), iter_beg->second->get(),DIB_RGB_COLORS, SRCCOPY);
		//pDC->StretchBlt(scrmX,scrmY,scrMX-scrmX,scrMY-scrmY,&bitCDC,0,0,iter_beg->second->GetWidth(),iter_beg->second->GetHeight(),SRCCOPY);
		//CComPtr<IScreenPoint> sp_min_screen_point;
		//sp_map_info->WorldToScreen(sp_en_world_point, &sp_min_screen_point);

		//double min_screen_x = 0;
		//double min_screen_y = 0;
		//sp_min_screen_point->GetX(&min_screen_x);
		//sp_min_screen_point->GetY(&min_screen_y);


		//double adjusted_min_screen_x = min_screen_x + 0.5;
		//double adjusted_min_screen_y = min_screen_y + 0.5;
		//double adjusted_max_screen_x = max_screen_x + 0.5;
		//double adjusted_max_screen_y = max_screen_y + 0.5;
		//   

		//pDC->BitBlt(adjusted_min_screen_x ,adjusted_min_screen_y,screen_width,screen_height,&bitCDC,0,0,SRCCOPY);
	}
}

