#if !defined(AFX_CIDATA_H__47D085A7_7A7D_4BA7_B975_6D20817278A8__INCLUDED_)
#define AFX_CIDATA_H__47D085A7_7A7D_4BA7_B975_6D20817278A8__INCLUDED_


class CIRect
{
public:
	double left;
	double top;
	double right;
	double bottom;
};
	
class CCoord_
{
public:
	double X;
	double Y;
};

//By Xman94 2009-08-24
typedef struct  
{
	int nGroup_Index; //group
	int nSubGroup_Index; //sub group
	int nScale_Index; //scale
	int nData_Index; // index
}BASELAYER_ITEMINDEX;


//	상황도 데이타 관리	channy	20051014
// 2005/11/08 No1...
//class CBA_RawData
//{
//public:
//	CString			sTypeCI;	//CI식별
//	CString			beNumber;	//BE번호
//	CString			taNumber;	//접미번호
//	CString			coord;		//경도,위도
//	CString			symType;	//심볼타입
//	CString			visible;	//도시여부
//	CString			targetName;	//표적명칭
//	CString			dispType;	//명칭도시타입
//	CString			order;		//색상(우선순위)
//	CString			minArea;	//최소 축척
//	CString			maxArea;	//최대 축척
//};
//
//class Shape_RawData
//{
//public:
//	CString			ciType;			//각 CI 식별 
//	CString			key;			//식별 고유값 
//	CString			subKey;
//	CString			shapeType;		//Shape Type	// 0:원 1:사각형 
//	CString			center;			//중심좌표
//	CString			rad;			//반경
//	CString			lineColor;		//라인 색상
//	CString			lineWidth;		//라인 두께
//	CString			lineType;		//라인 타입 
//	CString			fillType;		//채우기 타입
//	CString			fillColor;		//채우기 색상
//	CString			bVisibleShape;	//도시여부 
//	CString			name;			//명칭 
//	CString			bVisibleName;	//명칭 도시여부
//};
//
//class CKillBox_RawData
//{
//public:
//	CString	grid;					//	격자
//	CString	area;					//	구역
//	CString	control;				//	상태
//	CString	validtime;				//	유효화시간
//	CString	invalidtime;			//	무효화시간
//	CString	controlheight;			//	통제고도
//	CString	controllevel;			//	통제수준
//};
//	channy~

#endif // !defined(AFX_CIDATA_H__47D085A7_7A7D_4BA7_B975_6D20817278A8__INCLUDED_)

//CString grid,CString area,CString control,CString validtime,CString invalidtime,double controlheight,int controllevel
