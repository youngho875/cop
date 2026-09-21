#pragma once
#pragma pack(1)


#include <map>
#include <vector>
#include <string>
#include "GPoint.h"
#include "define.h"


using namespace std;

class CGPointGP;

typedef struct Smbr_
{
	double xmin;
	double ymin;
	double xmax;
	double ymax;
}MBR;

typedef struct Spoint_object_
{
	double x;
	double y;
}GeoPoint;


typedef struct Spoly_object_
{
	MBR m_MBR;
	int m_nNumParts;
	int m_nNumPoints;
	int * m_pParts;
	GeoPoint * m_pPoints;
}PolyObject;

typedef struct{
	int				m_nID;
	MBR				m_MBR;
	int				m_nNumParts;
	int				m_nNumPoints;
	int				*m_iParts;
	CGPointGP		*m_gpPoints;
	CString			m_strCode;
} polyObject;


typedef struct Sdbf_header3_
{
	unsigned char		version;
	unsigned char		year;
	unsigned char		month;
	unsigned char		day;
	unsigned long		recordn;
	unsigned short int	header_length;
	unsigned short int	record_length;
	unsigned char		ver10_reserved[20];
}DBF_FILEHEADER;



typedef struct Sfield_descriptor32_
{
	unsigned char		name[11];
	unsigned char		type;
	unsigned long		field_data_address;
	unsigned char		field_length;
	unsigned char		decimal_count;
	unsigned char		ver10_reserved[14];
}DBF_FILEDDESCRIPTOR;


typedef struct Slabel_object_
{
	double screen_x;
	double screen_y;
	CString label;
}LABEL_OBJECT;


typedef struct _s_point
{
	double x;
	double y;
}S_POINT;



typedef struct s_feature_info
{
	unsigned short id_file;
	CString id_layer;
	CString file_name;
	int type_shape;	 
	int cnt_field; 	
	int idx_feature;
	
}S_FEATURE_INFO;


struct MAP_STATUS_INFO
{
	BOOL bUseStatusBar;			// 사용 여부
	CString strStatusScale;		// 상태바 축척 정보
	CString strStatusCoord;		// 상태바 좌표 정보

	// 상태바 마우스 위치, 고도 정보
	double pGeo_x;
	double pGeo_y;
	double pGeo_z;

	MAP_STATUS_INFO()
	{
		bUseStatusBar = FALSE;
		strStatusScale = _T("");
		strStatusCoord = _T("");
		pGeo_x = 0.0;
		pGeo_y = 0.0;
		pGeo_z = 0.0;
	}
};

// 인덱스 + 데이터 사용 공통 구조체
struct COMMON_INDEX_STRING
{
	int nIndex;
	CString strData;
};

// 지도레이어 선택정보 및 타이틀
struct MAPSEARCH_LAYER_SELECT
{
	int nIndex;
	CString strTitle;
};


// 지도레이어 정보
struct MAPSEARCH_LAYER_INFO
{
	CString strTitle;	// 제목
	CString strContent;	// 내용
};

// 지도레이어 검색 결과
struct MAPSEARCH_LAYER_RESULT
{
	int nIndex;
	vector<MAPSEARCH_LAYER_INFO> vInfo;
};

// 지도레이어 검색 기능
struct MAPSEARCH_LAYER
{
	CString strLayerName;
	CString strDSName;
	CString strDBFName;
	CString strSearch;
};

// 지도 레이어 도시 순서 변경 구조
struct MAP_OBJECT_ORDER_LAYERCHANGE
{
	int nLayerIndex;   /**< 현재 인덱스 */
	int nTargetIndex;  /**< 변경 할 인덱스 */

	MAP_OBJECT_ORDER_LAYERCHANGE()
	{
		nLayerIndex = -1;
		nTargetIndex = -1;
	}
};

/**
* @struct LAYER_ATTRIBUTE
* @brief  Layer의 속성 정보를 저장하는 구조체
*/
struct LAYER_ATTRIBUTE
{
	int			nIndex;				/**< Layer Index */
	MAP_PRIMITIVETYPE	type;		/**< Layer 프리미티브 타입 */
	std::string layerName;			/**< Layer alias 명칭 */
	std::string source;				/**< Layer source 명칭 */
	std::string layerCategory;		/**< Layer 카탈로그  */
	BOOL		bVisible;			/**< Layer 도시 여부  */
};

// 색상 변경 요청
struct MAPVIEW_PROP_COLOR
{
	int nLayerIndex;
	int nFeatureIndex;
	COLORREF clr;

	MAPVIEW_PROP_COLOR()
	{
		nLayerIndex = -1;
		nFeatureIndex = -1;
		clr = RGB(0, 0, 0);
	}
};

/**
* @class FEATUREGROUP_ATTRIBUTE
* @brief Feature 의 속성 정보를 저장하는 구조체
*/
struct FEATUREGROUP_ATTRIBUTE
{
	int			nIndex;					/**< FEATURE GROUP Index */
	std::string	 featureName;			/**< FEATURE GROUP 명칭 */
	MAP_PRIMITIVETYPE			type;					/**< FEATURE type */
	BOOL		bVisible;				/**< 도시 여부 */

};