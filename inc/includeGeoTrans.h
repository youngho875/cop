#include "../LibSrc/GeoTrans/GGeoObjStruct.h"

#include "../LibSrc/GeoTrans/GGeoAbst.h"

#include "../LibSrc/GeoTrans/GGeoGP.h"
#include "../LibSrc/GeoTrans/GGeoGK.h"
#include "../LibSrc/GeoTrans/GGeoUPS.h"
#include "../LibSrc/GeoTrans/GGeoUTM.h"

#include "../LibSrc/GeoTrans/GStringGeo.h"

#include "../LibSrc/GeoTrans/GStringGP.h"			// 지리좌표
#include "../LibSrc/GeoTrans/GStringGEOREF.h"		// GEOREF 좌표
#include "../LibSrc/GeoTrans/GStringGK.h"			// GK 좌표
#include "../LibSrc/GeoTrans/GStringMGRS.h"			// MGRS 좌표
#include "../LibSrc/GeoTrans/GStringUPS.h"			// UPS 좌표
#include "../LibSrc/GeoTrans/GStringNE.h"			// NE 좌표 

#include "../LibSrc/GeoTrans/GDatum.h"				// 데이텀
#include "../LibSrc/GeoTrans/GDatumTrans.h"			// 데이텀 변환
#include "../LibSrc/GeoTrans/GEllipsoid.h"			// 타원체

#include "../LibSrc/GeoTrans/GGeoProj.h"			// 투영법 에 관련된 클래스
#include "../LibSrc/GeoTrans/GGeoObject.h"			// 
#include "../LibSrc/GeoTrans/GGeoObjGP.h"			// 지리좌표 모드를 정의한 클래스
#include "../LibSrc/GeoTrans/GGeoObjUTM.h"			// UTM 좌표 모드를 정의한 클래스

#include "../LibSrc/GeoTrans/GGeoTrans.h"			// 좌표변환 클래스

#include "../LibSrc/GeoTrans/GCalcDistance.h"		// 거리계산 클래스