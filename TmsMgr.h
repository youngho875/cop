#pragma once
//#include <vector>
#include <map>
#include "DIBSectionLite.h"
//#include "../../INC/rtree.h"
//#include "../../inc/graphic/bmglib/BMGGeoTIFF.h"

//typedef struct DataImage
//{	
//	CImage nImage;
//	long nWidth;
//	long nHeight;
//}S_DataImage;

class CTmsMgr
{
public:
	CTmsMgr(void);
	~CTmsMgr(void);


	long CalcLevel(double Geoleft, double Geotop, double Georight, double Geobottom, CRect ClinetRect);
	long CalcIndexX(long Level, double Longitude) const;
	long CalcIndexY(long Level, double Latitude) const;
	double CalcMapX(long Level, long IndexX) const;
	double CalcMapY(long Level, long IndexY) const;
	double CalcResolution(long Level) const;
	//typedef std::map<CString, CDIBSectionLite * > MapImagesType;
	typedef std::map<CString, CImage * > MapImagesType;

	MapImagesType mImages;
	void Initalize();
	void Load(CRect ClinetRect);
	void UnLoad();
	void NeedLoad();
	void Draw(CDC *pDC);
		
private:
	CCoordMgr* mCoordMgr;
	CCoord mCoord;

	CString mDirectoryPath;

};

