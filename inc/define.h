#pragma once


#define TYPE_EARTH_200	"EAR_200"
#define TYPE_EARTH_100	"EAR_100"
#define TYPE_EARTH_50	"EAR_50"
#define TYPE_EARTH_25	"EAR_25"
#define TYPE_EARTH_10	"EAR_10"
#define TYPE_EARTH_5	"EAR_5"

#define TYPE_AIR_200	"AIR_200"
#define TYPE_AIR_100	"AIR_100"
#define TYPE_AIR_50		"AIR_50"
#define TYPE_AIR_25		"AIR_25"


#define	CLOAD_PATH_SHPS		500001
#define	CWRITE_PATH_SHPS	500002

/*
typedef enum {
	MS_NOTHING = -1,	// 지정되지 않음
	MS_1_25000 = 0,	// 1:2.5만(공도에서 사용)
	MS_1_50000,		// 1:5만
	MS_1_100000,	// 1:10만
	MS_1_250000,	// 1:25만
	MS_1_500000,	// 1:50만
	MS_1_1000000,	// 1:100만
	MS_1_2000000,	// 1:200만
	MS_1_1,			// 1m급 위성영상
	MS_1_10,		// 10m급 위성영상
	MS_1_20,		// 20m급 위성영상
	MS_ERROR,		// error
} EGMapScale;

// 지도 종류
typedef enum { 
	MK_NOTHING     =-1,	// 지정되지 않음
	MK_VECTOR_LAND =0 ,	// 벡터 육도
	MK_RASTER_AIR  =1,  // 레스터 공도
	MK_VR_LAND     =2,	// 벡터+래스터
	MK_VECTOR_AIR  =3 ,	// 벡터 공도
	MK_VECTOR_SEA  =4,	// 벡터 해도
} EGMapKind;
*/

enum MAP_PRIMITIVETYPE
{
	MAP_PRIMITIVE_UNKNOWN = 0,				/**< 미지원 */
	MAP_PRIMITIVE_POINT = 1,				/**< 점 */
	MAP_PRIMITIVE_LINE = 2,				/**< 선 */
	MAP_PRIMITIVE_POLYGON = 3,				/**< 다각형 */
	MAP_PRIMITIVE_RASTER = 4,				/**< Raster (CIB, JPG, CADRG 등 )*/
	MAP_PRIMITIVE_DEM = 5,				/**< DEM 고도 데이타 등*/
	MAP_PRIMITIVE_COUNT
};
