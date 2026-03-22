// DlgOverlayEditObject.cpp : implementation file
//

#include "pch.h"
#include "C4IMap.h"
#include "DlgOverlayEditObject.h"
#include "C4IMapView.h"
#include "ObjectThread.h"
#include "../LIBSrc/GeoDC/CoordMgr.h"
#include "../LIBSrc/Overlay/Mathematics.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgOverlayEditObject dialog

IMPLEMENT_DYNAMIC(CDlgOverlayEditObject, CDialogEx)

CDlgOverlayEditObject::CDlgOverlayEditObject(CWnd* pParent /*=NULL*/)
: CDialogEx(CDlgOverlayEditObject::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgOverlayEditObject)
	// NOTE: the ClassWizard will add member initialization here
	// 위경도
	m_strlat_left_top = _T("");
	m_strlong_left_top = _T("");
	m_latmm_left_top = _T("");
	m_latdd_left_top = _T("");
	m_latss_left_top = _T("");
	m_longdd_left_top = _T("");
	m_longmm_left_top = _T("");
	m_longss_left_top = _T("");
	
	m_dRotation = 0.0f;
	// MGRS
	m_strMGRS_left_top = _T("");
	m_strMGRS_right_bottom = _T("");
	
	// GEOREF
	m_strGEOREF_left_top = _T("");
	m_strGEOREF_right_bottom = _T("");
	
	m_nObjectID =0;
	
	m_bDegree = TRUE;
	m_bDMS = FALSE;
	//}}AFX_DATA_INIT

	m_pView = pParent;
}


void CDlgOverlayEditObject::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgOverlayEditObject)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	// 위경도.
	DDX_Check(pDX, IDC_RADIO_OVERLAY_EDIT_OBJECT_SELECT_LAT_LNG_DEGREE, m_bDegree);
	DDX_Check(pDX, IDC_RADIO_OVERLAY_EDIT_OBJECT_SELECT_LAT_LNG_DMS, m_bDMS);
	
	DDX_Text(pDX, IDC_EDT_OVERLAY_EDIT_OBJECT_LEFT_TOP_DEGREE_LAT, m_strlat_left_top);
	DDX_Text(pDX, IDC_EDT_OVERLAY_EDIT_OBJECT_LEFT_TOP_DEGREE_LNG, m_strlong_left_top);	
	DDX_Text(pDX, IDC_EDT_OVERLAY_EDIT_OBJECT_LEFT_TOP_DMS_LAT_D, m_latdd_left_top);
	DDV_MaxChars(pDX, m_latdd_left_top, 2);
	DDX_Text(pDX, IDC_EDT_OVERLAY_EDIT_OBJECT_LEFT_TOP_DMS_LAT_M, m_latmm_left_top);
	DDV_MaxChars(pDX, m_latmm_left_top, 2);
	DDX_Text(pDX, IDC_EDT_OVERLAY_EDIT_OBJECT_LEFT_TOP_DMS_LAT_S, m_latss_left_top);
	DDV_MaxChars(pDX, m_latss_left_top, 2);
	DDX_Text(pDX, IDC_EDT_OVERLAY_EDIT_OBJECT_LEFT_TOP_DMS_LNG_D, m_longdd_left_top);
	DDV_MaxChars(pDX, m_longdd_left_top, 3);
	DDX_Text(pDX, IDC_EDT_OVERLAY_EDIT_OBJECT_LEFT_TOP_DMS_LNG_M, m_longmm_left_top);
	DDV_MaxChars(pDX, m_longmm_left_top, 2);
	DDX_Text(pDX, IDC_EDT_OVERLAY_EDIT_OBJECT_LEFT_TOP_DMS_LNG_S, m_longss_left_top);
	DDV_MaxChars(pDX, m_longss_left_top, 2);
	
	DDX_Text(pDX, IDC_EDT_OVERLAY_EDIT_OBJECT_RIGHT_BOTTOM_LAT_DEGREE, m_strlat_right_bottom);
	DDX_Text(pDX, IDC_EDT_OVERLAY_EDIT_OBJECT_RIGHT_BOTTOM_LNG_DEGREE, m_strlong_right_bottom);
	DDX_Text(pDX, IDC_EDT_OVERLAY_EDIT_OBJECT_RIGHT_BOTTOM_DMS_LAT_D, m_latdd_right_bottom);
	DDV_MaxChars(pDX, m_latdd_right_bottom, 2);
	DDX_Text(pDX, IDC_EDT_OVERLAY_EDIT_OBJECT_RIGHT_BOTTOM_DMS_LAT_M, m_latmm_right_bottom);
	DDV_MaxChars(pDX, m_latmm_right_bottom, 2);
	DDX_Text(pDX, IDC_EDT_OVERLAY_EDIT_OBJECT_RIGHT_BOTTOM_DMS_LAT_S, m_latss_right_bottom);
	DDV_MaxChars(pDX, m_latss_right_bottom, 2);
	DDX_Text(pDX, IDC_EDT_OVERLAY_EDIT_OBJECT_RIGHT_BOTTOM_DMS_LNG_D, m_longdd_right_bottom);
	DDV_MaxChars(pDX, m_longdd_right_bottom, 3);
	DDX_Text(pDX, IDC_EDT_OVERLAY_EDIT_OBJECT_RIGHT_BOTTOM_DMS_LNG_M, m_longmm_right_bottom);
	DDV_MaxChars(pDX, m_longmm_right_bottom, 2);
	DDX_Text(pDX, IDC_EDT_OVERLAY_EDIT_OBJECT_RIGHT_BOTTOM_DMS_LNG_S, m_longss_right_bottom);
	DDV_MaxChars(pDX, m_longss_right_bottom, 2);
	
	DDX_Text(pDX, IDC_EDT_OVERLAY_EDIT_OBJECT_ROTATION, m_dRotation);
	//	DDV_MaxChars(pDX, m_dRotation, 3);
	
	// MGRS	
	DDX_Text(pDX, IDC_EDIT_OVERLAY_OBJECT_LEFT_TOP_MGRS, m_strMGRS_left_top);
	DDX_Text(pDX, IDC_EDIT_OVERLAY_OBJECT_RIGHT_BOTTOM_MGRS, m_strMGRS_right_bottom);
	
	// GEOREF
	DDX_Text(pDX, IDC_EDIT_OVERLAY_OBJECT_LEFT_TOP_GEOREF, m_strGEOREF_left_top);
	DDX_Text(pDX, IDC_EDIT_OVERLAY_OBJECT_RIGHT_BOTTOM_GEOREF, m_strGEOREF_right_bottom);
	
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgOverlayEditObject, CDialogEx)
//{{AFX_MSG_MAP(CDlgOverlayEditObject)
ON_BN_CLICKED(IDC_BTN_OVERLAY_EDIT_OBJECT_MGRS, OnBtnOverlayEditObjectMgrs)
ON_BN_CLICKED(IDC_BTN_OVERLAY_EDIT_OBJECT_LAT_LNG, OnBtnOverlayEditObjectLatLng)
ON_BN_CLICKED(IDC_BTN_OVERLAY_EDIT_OBJECT_GEOREF, OnBtnOverlayEditObjectGeoref)

// 위경도.	
ON_BN_CLICKED(IDC_RADIO_OVERLAY_EDIT_OBJECT_SELECT_LAT_LNG_DEGREE, OnRadioLatLngDegree)
ON_BN_CLICKED(IDC_RADIO_OVERLAY_EDIT_OBJECT_SELECT_LAT_LNG_DMS, OnRadioLatLngDMS)
ON_EN_UPDATE(IDC_EDT_OVERLAY_EDIT_OBJECT_LEFT_TOP_DMS_LAT_D, OnUpdateLeftTopLatdd)
ON_EN_UPDATE(IDC_EDT_OVERLAY_EDIT_OBJECT_LEFT_TOP_DMS_LAT_M, OnUpdateLeftTopLatmm)
ON_EN_UPDATE(IDC_EDT_OVERLAY_EDIT_OBJECT_LEFT_TOP_DMS_LNG_D, OnUpdateLeftTopLongdd)
ON_EN_UPDATE(IDC_EDT_OVERLAY_EDIT_OBJECT_LEFT_TOP_DMS_LAT_S, OnUpdateLeftTopLatss)
ON_EN_UPDATE(IDC_EDT_OVERLAY_EDIT_OBJECT_LEFT_TOP_DMS_LNG_M, OnUpdateLeftTopLongmm)

ON_EN_UPDATE(IDC_EDT_OVERLAY_EDIT_OBJECT_RIGHT_BOTTOM_DMS_LAT_D, OnUpdateRightBottomLatdd)
ON_EN_UPDATE(IDC_EDT_OVERLAY_EDIT_OBJECT_RIGHT_BOTTOM_DMS_LAT_M, OnUpdateRightBottomLatmm)
ON_EN_UPDATE(IDC_EDT_OVERLAY_EDIT_OBJECT_RIGHT_BOTTOM_DMS_LNG_D, OnUpdateRightBottomLongdd)
ON_EN_UPDATE(IDC_EDT_OVERLAY_EDIT_OBJECT_RIGHT_BOTTOM_DMS_LAT_S, OnUpdateRightBottomLatss)
ON_EN_UPDATE(IDC_EDT_OVERLAY_EDIT_OBJECT_RIGHT_BOTTOM_DMS_LNG_M, OnUpdateRightBottomLongmm)
ON_EN_UPDATE(IDC_EDT_OVERLAY_EDIT_OBJECT_ROTATION, OnUpdateObjectRoatation)
ON_WM_CLOSE()

//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgOverlayEditObject message handlers
void CDlgOverlayEditObject::OnBtnOverlayEditObjectMgrs() 
{	
	CC4IMapView *pView=(CC4IMapView*)(((CMainFrame*)AfxGetMainWnd())->GetActiveView());
	if(pView==NULL)
		return;
	OVERLAY::OBJECT* pObject = pView->m_pOverlayThread->GetOverlayMain()->SelectedObject(m_nObjectID);
	if(pObject==NULL)
		return;
	
	CString strMGRS_left_top=m_strMGRS_left_top;
	CString strMGRS_right_bottom=m_strMGRS_right_bottom;
	UpdateData(TRUE);
	
	// 조건처리 
	if(m_strMGRS_left_top.GetLength() != 11)
	{
		AfxMessageBox("MGRS 형식에 맞지 않습니다.");		
		m_strMGRS_left_top = strMGRS_left_top;
		m_strMGRS_right_bottom = strMGRS_right_bottom;
		UpdateData(FALSE);
		return;
	}
	
	//일정 시야 밖의 범위 임력시 에러처리.	
	char num[2];
	num[0] = m_strMGRS_left_top.GetAt(0);
	num[1] = m_strMGRS_left_top.GetAt(1);
	int MgrsValidNum;
	MgrsValidNum= atoi(num);
	
	if(MgrsValidNum < 49 || MgrsValidNum > 54)
	{
		AfxMessageBox("일정범위의 값을 넣어주세요");		
		m_strMGRS_left_top = strMGRS_left_top;
		m_strMGRS_right_bottom = strMGRS_right_bottom;
		UpdateData(FALSE);
		return;
	}
	
	for(int i=0; i<2 ; i++)
	{
		if(m_strMGRS_left_top.GetAt(i) < '0' || m_strMGRS_left_top.GetAt(i) > '9')
		{
			AfxMessageBox("MGRS 형식에 맞지 않습니다.");		
			m_strMGRS_left_top = strMGRS_left_top;
			m_strMGRS_right_bottom = strMGRS_right_bottom;
			UpdateData(FALSE);
			return;
		}
	}
	
	for(int i=2; i<5 ; i++)
	{
		if(m_strMGRS_left_top.GetAt(i) < 'A' || m_strMGRS_left_top.GetAt(i) > 'z')
		{
			AfxMessageBox("MGRS 형식에 맞지 않습니다.");		
			m_strMGRS_left_top = strMGRS_left_top;
			m_strMGRS_right_bottom = strMGRS_right_bottom;
			UpdateData(FALSE);
			return;
		}
	}
	
	for(int i=5; i<11 ; i++)
	{
		if(m_strMGRS_left_top.GetAt(i) < '0' || m_strMGRS_left_top.GetAt(i) > '9')
		{
			AfxMessageBox("MGRS 형식에 맞지 않습니다.");		
			m_strMGRS_left_top = strMGRS_left_top;
			m_strMGRS_right_bottom = strMGRS_right_bottom;
			UpdateData(FALSE);
			return;
		}
	}
	
	////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////
	// 조건처리 
	if(m_strMGRS_right_bottom.GetLength() != 11)
	{
		AfxMessageBox("MGRS 형식에 맞지 않습니다.");		
		m_strMGRS_left_top = strMGRS_left_top;
		m_strMGRS_right_bottom = strMGRS_right_bottom;
		UpdateData(FALSE);
		return;
	}
	
	num[0] = m_strMGRS_right_bottom.GetAt(0);
	num[1] = m_strMGRS_right_bottom.GetAt(1);
	
	MgrsValidNum= atoi(num);
	
	if(MgrsValidNum < 49 || MgrsValidNum > 54)
	{
		AfxMessageBox("일정범위의 값을 넣어주세요");		
		m_strMGRS_left_top = strMGRS_left_top;
		m_strMGRS_right_bottom = strMGRS_right_bottom;
		UpdateData(FALSE);
		return;
	}
	
	for(int i=0; i<2 ; i++)
	{
		if(m_strMGRS_right_bottom.GetAt(i) < '0' || m_strMGRS_right_bottom.GetAt(i) > '9')
		{
			AfxMessageBox("MGRS 형식에 맞지 않습니다.");		
			m_strMGRS_left_top = strMGRS_left_top;
			m_strMGRS_right_bottom = strMGRS_right_bottom;
			UpdateData(FALSE);
			return;
		}
	}
	
	for(int i=2; i<5 ; i++)
	{
		if(m_strMGRS_right_bottom.GetAt(i) < 'A' || m_strMGRS_right_bottom.GetAt(i) > 'z')
		{
			AfxMessageBox("MGRS 형식에 맞지 않습니다.");					
			m_strMGRS_left_top = strMGRS_left_top;
			m_strMGRS_right_bottom = strMGRS_right_bottom;
			UpdateData(FALSE);
			return;
		}
	}
	
	for(int i=5; i<11 ; i++)
	{
		if(m_strMGRS_right_bottom.GetAt(i) < '0' || m_strMGRS_right_bottom.GetAt(i) > '9')
		{
			AfxMessageBox("MGRS 형식에 맞지 않습니다.");					
			m_strMGRS_left_top = strMGRS_left_top;
			m_strMGRS_right_bottom = strMGRS_right_bottom;
			UpdateData(FALSE);
			return;
		}
	}
	
	/////////////////////////////////////////
	// Update	
	char* temp_string;
	char* temp_string2;	
	
	m_strMGRS_left_top.MakeUpper();   // 대소문자 
	m_strMGRS_right_bottom.MakeUpper();   // 대소문자 처리
	temp_string = m_strMGRS_left_top.GetBuffer(11);
	temp_string2 = m_strMGRS_right_bottom.GetBuffer(11);
	
	double lat_rad, lng_rad;
	double lat_rad2, lng_rad2;
	pView->m_Coord.utm_to_radians_lat_long(temp_string, &lat_rad, &lng_rad);
	pView->m_Coord.utm_to_radians_lat_long(temp_string2, &lat_rad2, &lng_rad2);

	// double lat_degree[2]; // unused
	double lng_degree[2];

	lng_degree[0] = lat_rad * OVERLAY::MATH_RADIAN_TO_DEGREE;
	lng_degree[0] = lng_rad * OVERLAY::MATH_RADIAN_TO_DEGREE;

	lng_degree[1] = lat_rad2 * OVERLAY::MATH_RADIAN_TO_DEGREE;
	lng_degree[1] = lng_rad2 * OVERLAY::MATH_RADIAN_TO_DEGREE;
// 	for(i=0; i<2; i++)
// 	{
// 		if(lat_degree[i] < 20 || lat_degree[i] > 50 )
// 		{
// 			AfxMessageBox("다음의 범위로 입력하여 주십시오.\n북위:20~50 동경:100~170 "); 	
// 			m_strMGRS_left_top = strMGRS_left_top;
// 			m_strMGRS_right_bottom = strMGRS_right_bottom;
// 			UpdateData(FALSE);
// 			return;
// 		}
// 		if(lng_degree[i] < 100 || lng_degree[i] >170)
// 		{
// 			AfxMessageBox("다음의 범위로 입력하여 주십시오.\n북위:20~50 동경:100~170 "); 
// 			m_strMGRS_left_top = strMGRS_left_top;
// 			m_strMGRS_right_bottom = strMGRS_right_bottom;
// 			UpdateData(FALSE);
// 			return;
// 		}
// 	}
	UpdateData(FALSE);
	
	pObject->pt[0].SetLatLong(lat_rad,lng_rad);	
	pObject->pt[1].SetLatLong(lat_rad2,lng_rad2);

	//삼각형인 경우 꼭지점도 set 해준다.
	if(pObject->GetType() == OVERLAY::TRIANGLE)
		pObject->pt[2].SetLatLong(lat_rad,lng_rad);

	pView->m_pOverlayThread->GetOverlayMain()->DrawEditlayer();
	pView->m_pOverlayThread->GetOverlayView()->ReleaseSelectObject();
//	pView->DrawSituation(FALSE,FALSE,TRUE, TRUE);
	UpdatePositionData(pObject);
	
}

void CDlgOverlayEditObject::OnBtnOverlayEditObjectLatLng() 
{
	CC4IMapView *pView=(CC4IMapView*)(((CMainFrame*)AfxGetMainWnd())->GetActiveView());
	if(pView==NULL)
		return;
	
	OVERLAY::OBJECT* pObject = pView->m_pOverlayThread->GetOverlayMain()->SelectedObject(m_nObjectID);
	if(pObject==NULL)
		return;
		
	CString strlat_left_top=m_strlat_left_top;
	CString strlong_left_top=m_strlong_left_top;

	CString	strlat_right_bottom=m_strlat_right_bottom;
	CString	strlong_right_bottom=m_strlong_right_bottom;

	CString	latdd_left_top=m_latdd_left_top;
	CString	latmm_left_top=m_latmm_left_top;
	CString	latss_left_top=m_latss_left_top;
	CString	longdd_left_top=m_longdd_left_top;
	CString	longmm_left_top=m_longmm_left_top;
	CString	longss_left_top=m_longss_left_top;

	CString	latdd_right_bottom =m_latdd_right_bottom;
	CString	latmm_right_bottom=m_latmm_right_bottom;
	CString	latss_right_bottom=m_latss_right_bottom;
	CString	longdd_right_bottom=m_longdd_right_bottom;
	CString	longmm_right_bottom=m_longmm_right_bottom;
	CString	longss_right_bottom=m_longss_right_bottom;

	UpdateData(TRUE);
	
	double input_lat = 0.0;
	double input_long = 0.0;
	
	if(m_bDegree)   // 실수 입력
	{
		input_lat = atof( m_strlat_left_top.GetBuffer(7) );
		input_long = atof( m_strlong_left_top.GetBuffer(8) );	  
		
		//에러처리. 우리나라 지도부근으로 범위를 한정.
		if(input_lat < 20 || input_lat > 50 || input_long < 100 || input_long >1170)
		{
			AfxMessageBox("다음의 범위로 입력하여 주십시오.\n북위:20~50 동경:100~170 ");   
			
			m_strlat_left_top=strlat_left_top;
			m_strlong_left_top=strlong_left_top;
			m_strlat_right_bottom=strlat_right_bottom;
			m_strlong_right_bottom=strlong_right_bottom;

			m_latdd_left_top	=latdd_left_top;
			m_latmm_left_top	=latmm_left_top;
			m_latss_left_top	=latss_left_top;
			m_longdd_left_top	=longdd_left_top;
			m_longmm_left_top	=longmm_left_top;
			m_longss_left_top	=longss_left_top;

			m_latdd_right_bottom	=latdd_right_bottom;
			m_latmm_right_bottom	=latmm_right_bottom;			
			m_latss_right_bottom	=latss_right_bottom;
			m_longdd_right_bottom	=longdd_right_bottom;
			m_longmm_right_bottom	=longmm_right_bottom;
			m_longss_right_bottom	=longss_right_bottom;
			UpdateData(FALSE);
			return;
		}
	}
	else    // 도분초 입력 처리
	{
		
		if(m_latmm_left_top.GetLength()==1)
			m_latmm_left_top= '0'+m_latmm_left_top;
		if(m_latss_left_top.GetLength()==1)
			m_latss_left_top= '0'+m_latss_left_top;
		if(m_longmm_left_top.GetLength()==1)
			m_longmm_left_top= '0'+m_longmm_left_top;
		if(m_longss_left_top.GetLength()==1)
			m_longss_left_top= '0'+m_longss_left_top;
		
		m_strlat_left_top = m_latdd_left_top;
		m_strlat_left_top+= m_latmm_left_top;
		m_strlat_left_top+=m_latss_left_top;
		m_strlat_left_top+="N";
		
		m_strlong_left_top=m_longdd_left_top;
		m_strlong_left_top+=m_longmm_left_top;
		m_strlong_left_top+=m_longss_left_top;
		m_strlong_left_top+="E";
		
		pView->m_Coord.dms_to_dec_deg(m_strlat_left_top.GetBuffer(7), &input_lat); 
		pView->m_Coord.dms_to_dec_deg(m_strlong_left_top.GetBuffer(8), &input_long);
		
		//일정범위 벗어나면 에러처리.  우리나라를 중심으로 범위 한정. 
		if(input_lat < 20 || input_lat > 50 || input_long < 100 || input_long >170)
		{
			AfxMessageBox("다음의 범위로 입력하여 주십시오.\n북위:20~50 동경:100~170 "); 
			
			m_strlat_left_top=strlat_left_top;
			m_strlong_left_top=strlong_left_top;
			m_strlat_right_bottom=strlat_right_bottom;
			m_strlong_right_bottom=strlong_right_bottom;

			m_latdd_left_top	=latdd_left_top;
			m_latmm_left_top	=latmm_left_top;
			m_latss_left_top	=latss_left_top;
			m_longdd_left_top	=longdd_left_top;
			m_longmm_left_top	=longmm_left_top;
			m_longss_left_top	=longss_left_top;

			m_latdd_right_bottom	=latdd_right_bottom;
			m_latmm_right_bottom	=latmm_right_bottom;			
			m_latss_right_bottom	=latss_right_bottom;
			m_longdd_right_bottom	=longdd_right_bottom;
			m_longmm_right_bottom	=longmm_right_bottom;
			m_longss_right_bottom	=longss_right_bottom;
			UpdateData(FALSE);
			return;
		}
		if(m_latmm_left_top > "59" || m_latss_left_top > "59" || m_longmm_left_top >"59" || m_longss_left_top >"59")
		{
			AfxMessageBox("분 또는 초의 입력값이 잘못되었습니다.");
			
			m_strlat_left_top=strlat_left_top;
			m_strlong_left_top=strlong_left_top;
			m_strlat_right_bottom=strlat_right_bottom;
			m_strlong_right_bottom=strlong_right_bottom;

			m_latdd_left_top	=latdd_left_top;
			m_latmm_left_top	=latmm_left_top;
			m_latss_left_top	=latss_left_top;
			m_longdd_left_top	=longdd_left_top;
			m_longmm_left_top	=longmm_left_top;
			m_longss_left_top	=longss_left_top;

			m_latdd_right_bottom	=latdd_right_bottom;
			m_latmm_right_bottom	=latmm_right_bottom;			
			m_latss_right_bottom	=latss_right_bottom;
			m_longdd_right_bottom	=longdd_right_bottom;
			m_longmm_right_bottom	=longmm_right_bottom;
			m_longss_right_bottom	=longss_right_bottom;
			UpdateData(FALSE);
			return;
		}	
	}
	////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////
	double input_lat2=0.0, input_lng2=0.0;
	if(m_bDegree)   // 실수 입력
	{
		input_lat2 = atof( m_strlat_right_bottom.GetBuffer(7) );
		input_lng2 = atof( m_strlong_right_bottom.GetBuffer(8) );	  
		
		//에러처리. 우리나라 지도부근으로 범위를 한정.		
		if(input_lat2 < 20 || input_lat2 > 50 || input_lng2 < 100 || input_lng2 >170)
		{
			AfxMessageBox("다음의 범위로 입력하여 주십시오.\n북위:20~50 동경:100~170 "); 
			m_strlat_left_top=strlat_left_top;
			m_strlong_left_top=strlong_left_top;
			m_strlat_right_bottom=strlat_right_bottom;
			m_strlong_right_bottom=strlong_right_bottom;

			m_latdd_left_top	=latdd_left_top;
			m_latmm_left_top	=latmm_left_top;
			m_latss_left_top	=latss_left_top;
			m_longdd_left_top	=longdd_left_top;
			m_longmm_left_top	=longmm_left_top;
			m_longss_left_top	=longss_left_top;

			m_latdd_right_bottom	=latdd_right_bottom;
			m_latmm_right_bottom	=latmm_right_bottom;			
			m_latss_right_bottom	=latss_right_bottom;
			m_longdd_right_bottom	=longdd_right_bottom;
			m_longmm_right_bottom	=longmm_right_bottom;
			m_longss_right_bottom	=longss_right_bottom;
			UpdateData(FALSE);
			return;
		}
	}
	else    // 도분초 입력 처리
	{
		
		if(m_latmm_right_bottom.GetLength()==1)
			m_latmm_right_bottom= '0'+m_latmm_right_bottom;
		if(m_latss_right_bottom.GetLength()==1)
			m_latss_right_bottom= '0'+m_latss_right_bottom;
		if(m_longmm_right_bottom.GetLength()==1)
			m_longmm_right_bottom= '0'+m_longmm_right_bottom;
		if(m_longss_right_bottom.GetLength()==1)
			m_longss_right_bottom= '0'+m_longss_right_bottom;
		
		m_strlat_right_bottom = m_latdd_right_bottom;
		m_strlat_right_bottom+= m_latmm_right_bottom;
		m_strlat_right_bottom+=m_latss_right_bottom;
		m_strlat_right_bottom+="N";
		
		m_strlong_right_bottom=m_longdd_right_bottom;
		m_strlong_right_bottom+=m_longmm_right_bottom;
		m_strlong_right_bottom+=m_longss_right_bottom;
		m_strlong_right_bottom+="E";
		
		pView->m_Coord.dms_to_dec_deg(m_strlat_right_bottom.GetBuffer(7), &input_lat2); 
		pView->m_Coord.dms_to_dec_deg(m_strlong_right_bottom.GetBuffer(8), &input_lng2);
		
		//일정범위 벗어나면 에러처리.  우리나라를 중심으로 범위 한정. 	
		if(input_lat2 < 20 || input_lat2 > 50 || input_lng2 < 100 || input_lng2 >170)
		{
			AfxMessageBox("다음의 범위로 입력하여 주십시오.\n북위:20~50 동경:100~170 ");
			
			m_strlat_left_top=strlat_left_top;
			m_strlong_left_top=strlong_left_top;
			m_strlat_right_bottom=strlat_right_bottom;
			m_strlong_right_bottom=strlong_right_bottom;

			m_latdd_left_top	=latdd_left_top;
			m_latmm_left_top	=latmm_left_top;
			m_latss_left_top	=latss_left_top;
			m_longdd_left_top	=longdd_left_top;
			m_longmm_left_top	=longmm_left_top;
			m_longss_left_top	=longss_left_top;

			m_latdd_right_bottom	=latdd_right_bottom;
			m_latmm_right_bottom	=latmm_right_bottom;			
			m_latss_right_bottom	=latss_right_bottom;
			m_longdd_right_bottom	=longdd_right_bottom;
			m_longmm_right_bottom	=longmm_right_bottom;
			m_longss_right_bottom	=longss_right_bottom;

			UpdateData(FALSE);

			return;
		}
		if(m_latmm_right_bottom > "59" || m_latss_right_bottom > "59" || m_longmm_right_bottom >"59" || m_longss_right_bottom >"59")
		{
			AfxMessageBox("분 또는 초의 입력값이 잘못되었습니다.");
			
			m_strlat_left_top=strlat_left_top;
			m_strlong_left_top=strlong_left_top;
			m_strlat_right_bottom=strlat_right_bottom;
			m_strlong_right_bottom=strlong_right_bottom;

			m_latdd_left_top	=latdd_left_top;
			m_latmm_left_top	=latmm_left_top;
			m_latss_left_top	=latss_left_top;
			m_longdd_left_top	=longdd_left_top;
			m_longmm_left_top	=longmm_left_top;
			m_longss_left_top	=longss_left_top;

			m_latdd_right_bottom	=latdd_right_bottom;
			m_latmm_right_bottom	=latmm_right_bottom;			
			m_latss_right_bottom	=latss_right_bottom;
			m_longdd_right_bottom	=longdd_right_bottom;
			m_longmm_right_bottom	=longmm_right_bottom;
			m_longss_right_bottom	=longss_right_bottom;

			UpdateData(FALSE);

			return;
		}
	}
	pObject->pt[0].SetLatLong(input_lat * OVERLAY::MATH_DEGREE_TO_RADIAN, input_long * OVERLAY::MATH_DEGREE_TO_RADIAN);
	pObject->pt[1].SetLatLong(input_lat2 * OVERLAY::MATH_DEGREE_TO_RADIAN, input_lng2 * OVERLAY::MATH_DEGREE_TO_RADIAN);

	//삼각형인 경우 꼭지점도 set 해준다.
	if(pObject->GetType() == OVERLAY::TRIANGLE)
		pObject->pt[2].SetLatLong(input_lat * OVERLAY::MATH_DEGREE_TO_RADIAN, input_long * OVERLAY::MATH_DEGREE_TO_RADIAN);

		
	pView->m_pOverlayThread->GetOverlayMain()->DrawEditlayer();
	pView->m_pOverlayThread->GetOverlayView()->ReleaseSelectObject();
	//pView->m_pOverlayThread->GetOverlayMain()->DrawOnScreen();
//	pView->DrawSituation(FALSE,FALSE,TRUE, TRUE);
	UpdatePositionData(pObject);
}

void CDlgOverlayEditObject::OnBtnOverlayEditObjectGeoref()
{
	CC4IMapView *pView=(CC4IMapView*)(((CMainFrame*)AfxGetMainWnd())->GetActiveView());
	if(pView==NULL)
		return;
	
	OVERLAY::OBJECT* pObject = pView->m_pOverlayThread->GetOverlayMain()->SelectedObject(m_nObjectID);
	if(pObject==NULL)
		return;
	
	CString strGEOREF_left_top = m_strGEOREF_left_top;
	CString strGEOREF_right_bottom = m_strGEOREF_right_bottom;

	UpdateData(TRUE);

	double input_lat=0.0, input_long=0;
	double input_lat2=0.0, input_lng2=0;

	
	m_strGEOREF_left_top.MakeUpper();
	m_strGEOREF_right_bottom.MakeUpper();

	CCoord coord;
	coord.georef_to_radians_lat_long(m_strGEOREF_left_top.GetBuffer(0), &input_lat, &input_long);
	coord.georef_to_radians_lat_long(m_strGEOREF_right_bottom.GetBuffer(0), &input_lat2, &input_lng2);

	
	double lat_degree[2];
	double lng_degree[2];

	lat_degree[0] = input_lat * OVERLAY::MATH_RADIAN_TO_DEGREE;
	lng_degree[0] = input_long * OVERLAY::MATH_RADIAN_TO_DEGREE;
	
	lat_degree[1] = input_lat2 * OVERLAY::MATH_RADIAN_TO_DEGREE;
	lng_degree[1] = input_lng2 * OVERLAY::MATH_RADIAN_TO_DEGREE;
	for(int i=0; i<2; i++)
	{
		if(lat_degree[i] < 20 || lat_degree[i] > 50 )//|| input_lng2 < 100 || input_lng2 >170)
		{
			AfxMessageBox("다음의 범위로 입력하여 주십시오.\n북위:20~50 동경:100~170 ");
	/*	if(lat_degree[i]<-90 || lat_degree[i]> 90)
		{
			AfxMessageBox("좌표입력이 형식과 다릅니다.");*/			
			m_strGEOREF_left_top = strGEOREF_left_top;
			m_strGEOREF_right_bottom = strGEOREF_right_bottom;
			UpdateData(FALSE);
			return;
		}
		if(lng_degree[i] < 100 || lng_degree[i] >170)
		{
			AfxMessageBox("다음의 범위로 입력하여 주십시오.\n북위:20~50 동경:100~170 "); 
		/*if(lat_degree[i]<-180 || lat_degree[i]> 180)
		{*/
			//AfxMessageBox("좌표입력이 형식과 다릅니다.");
			m_strGEOREF_left_top = strGEOREF_left_top;
			m_strGEOREF_right_bottom = strGEOREF_right_bottom;
			UpdateData(FALSE);
			return;
		}
	}

	pObject->pt[0].SetLatLong(input_lat,input_long);				
	pObject->pt[1].SetLatLong(input_lat2,input_lng2);	

	//삼각형인 경우 꼭지점도 set 해준다.
	if(pObject->GetType() == OVERLAY::TRIANGLE)
		pObject->pt[2].SetLatLong(input_lat,input_long);
	pView->m_pOverlayThread->GetOverlayMain()->DrawEditlayer();
	pView->m_pOverlayThread->GetOverlayView()->ReleaseSelectObject();
//	pView->DrawSituation(FALSE,FALSE,TRUE, TRUE);
	UpdatePositionData(pObject);
}

// 위경도
void CDlgOverlayEditObject::OnRadioLatLngDegree() 
{	
	m_bDegree = TRUE;
	m_bDMS = FALSE;
	
	GetDlgItem(IDC_EDT_OVERLAY_EDIT_OBJECT_LEFT_TOP_DEGREE_LAT)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_EDT_OVERLAY_EDIT_OBJECT_LEFT_TOP_DEGREE_LNG)->ShowWindow(SW_SHOW);
	
	GetDlgItem(IDC_EDT_OVERLAY_EDIT_OBJECT_LEFT_TOP_DMS_LAT_D)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_LEFT_TOP_LATDD)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_EDT_OVERLAY_EDIT_OBJECT_LEFT_TOP_DMS_LAT_M)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_LEFT_TOP_LATMM)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_EDT_OVERLAY_EDIT_OBJECT_LEFT_TOP_DMS_LAT_S)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_LEFT_TOP_LATSS)->ShowWindow(SW_HIDE);
	
	GetDlgItem(IDC_EDT_OVERLAY_EDIT_OBJECT_LEFT_TOP_DMS_LNG_D)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_LEFT_TOP_LONGDD)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_EDT_OVERLAY_EDIT_OBJECT_LEFT_TOP_DMS_LNG_M)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_LEFT_TOP_LONGMM)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_EDT_OVERLAY_EDIT_OBJECT_LEFT_TOP_DMS_LNG_S)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_LEFT_TOP_LONGSS)->ShowWindow(SW_HIDE);
	
	////////////////////////////////////////////////////////////////
	GetDlgItem(IDC_EDT_OVERLAY_EDIT_OBJECT_RIGHT_BOTTOM_LAT_DEGREE)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_EDT_OVERLAY_EDIT_OBJECT_RIGHT_BOTTOM_LNG_DEGREE)->ShowWindow(SW_SHOW);
	
	GetDlgItem(IDC_EDT_OVERLAY_EDIT_OBJECT_RIGHT_BOTTOM_DMS_LAT_D)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_RIGHT_BOTTOM_LATDD)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_EDT_OVERLAY_EDIT_OBJECT_RIGHT_BOTTOM_DMS_LAT_M)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_RIGHT_BOTTOM_LATMM)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_EDT_OVERLAY_EDIT_OBJECT_RIGHT_BOTTOM_DMS_LAT_S)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_RIGHT_BOTTOM_LATSS)->ShowWindow(SW_HIDE);
	
	GetDlgItem(IDC_EDT_OVERLAY_EDIT_OBJECT_RIGHT_BOTTOM_DMS_LNG_D)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_RIGHT_BOTTOM_LONGDD)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_EDT_OVERLAY_EDIT_OBJECT_RIGHT_BOTTOM_DMS_LNG_M)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_RIGHT_BOTTOM_LONGMM)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_EDT_OVERLAY_EDIT_OBJECT_RIGHT_BOTTOM_DMS_LNG_S)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_RIGHT_BOTTOM_LONGSS)->ShowWindow(SW_HIDE);
	
	UpdateData(FALSE);
}

void CDlgOverlayEditObject::OnRadioLatLngDMS() 
{
	m_bDegree = FALSE;
	m_bDMS = TRUE;	
	
	GetDlgItem(IDC_EDT_OVERLAY_EDIT_OBJECT_LEFT_TOP_DEGREE_LAT)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_EDT_OVERLAY_EDIT_OBJECT_LEFT_TOP_DEGREE_LNG)->ShowWindow(SW_HIDE);
	
	GetDlgItem(IDC_EDT_OVERLAY_EDIT_OBJECT_LEFT_TOP_DMS_LAT_D)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_STATIC_LEFT_TOP_LATDD)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_EDT_OVERLAY_EDIT_OBJECT_LEFT_TOP_DMS_LAT_M)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_STATIC_LEFT_TOP_LATMM)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_EDT_OVERLAY_EDIT_OBJECT_LEFT_TOP_DMS_LAT_S)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_STATIC_LEFT_TOP_LATSS)->ShowWindow(SW_SHOW);
	
	GetDlgItem(IDC_EDT_OVERLAY_EDIT_OBJECT_LEFT_TOP_DMS_LNG_D)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_STATIC_LEFT_TOP_LONGDD)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_EDT_OVERLAY_EDIT_OBJECT_LEFT_TOP_DMS_LNG_M)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_STATIC_LEFT_TOP_LONGMM)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_EDT_OVERLAY_EDIT_OBJECT_LEFT_TOP_DMS_LNG_S)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_STATIC_LEFT_TOP_LONGSS)->ShowWindow(SW_SHOW);
	///////////////////////////////////////////////////////
	GetDlgItem(IDC_EDT_OVERLAY_EDIT_OBJECT_RIGHT_BOTTOM_LAT_DEGREE)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_EDT_OVERLAY_EDIT_OBJECT_RIGHT_BOTTOM_LNG_DEGREE)->ShowWindow(SW_HIDE);
	
	GetDlgItem(IDC_EDT_OVERLAY_EDIT_OBJECT_RIGHT_BOTTOM_DMS_LAT_D)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_STATIC_RIGHT_BOTTOM_LATDD)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_EDT_OVERLAY_EDIT_OBJECT_RIGHT_BOTTOM_DMS_LAT_M)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_STATIC_RIGHT_BOTTOM_LATMM)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_EDT_OVERLAY_EDIT_OBJECT_RIGHT_BOTTOM_DMS_LAT_S)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_STATIC_RIGHT_BOTTOM_LATSS)->ShowWindow(SW_SHOW);
	
	GetDlgItem(IDC_EDT_OVERLAY_EDIT_OBJECT_RIGHT_BOTTOM_DMS_LNG_D)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_STATIC_RIGHT_BOTTOM_LONGDD)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_EDT_OVERLAY_EDIT_OBJECT_RIGHT_BOTTOM_DMS_LNG_M)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_STATIC_RIGHT_BOTTOM_LONGMM)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_EDT_OVERLAY_EDIT_OBJECT_RIGHT_BOTTOM_DMS_LNG_S)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_STATIC_RIGHT_BOTTOM_LONGSS)->ShowWindow(SW_SHOW);
	
	UpdateData(FALSE);
}

BOOL CDlgOverlayEditObject::OnInitDialog() 
{
	CDialogEx::OnInitDialog();
	GetDlgItem(IDC_EDT_OVERLAY_EDIT_OBJECT_LEFT_TOP_DMS_LAT_D)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_LEFT_TOP_LATDD)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_EDT_OVERLAY_EDIT_OBJECT_LEFT_TOP_DMS_LAT_M)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_LEFT_TOP_LATMM)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_EDT_OVERLAY_EDIT_OBJECT_LEFT_TOP_DMS_LAT_S)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_LEFT_TOP_LATSS)->ShowWindow(SW_HIDE);
	
	GetDlgItem(IDC_EDT_OVERLAY_EDIT_OBJECT_LEFT_TOP_DMS_LNG_D)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_LEFT_TOP_LONGDD)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_EDT_OVERLAY_EDIT_OBJECT_LEFT_TOP_DMS_LNG_M)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_LEFT_TOP_LONGMM)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_EDT_OVERLAY_EDIT_OBJECT_LEFT_TOP_DMS_LNG_S)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_LEFT_TOP_LONGSS)->ShowWindow(SW_HIDE);
	
	GetDlgItem(IDC_EDT_OVERLAY_EDIT_OBJECT_RIGHT_BOTTOM_DMS_LAT_D)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_RIGHT_BOTTOM_LATDD)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_EDT_OVERLAY_EDIT_OBJECT_RIGHT_BOTTOM_DMS_LAT_M)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_RIGHT_BOTTOM_LATMM)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_EDT_OVERLAY_EDIT_OBJECT_RIGHT_BOTTOM_DMS_LAT_S)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_RIGHT_BOTTOM_LATSS)->ShowWindow(SW_HIDE);
	
	GetDlgItem(IDC_EDT_OVERLAY_EDIT_OBJECT_RIGHT_BOTTOM_DMS_LNG_D)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_RIGHT_BOTTOM_LONGDD)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_EDT_OVERLAY_EDIT_OBJECT_RIGHT_BOTTOM_DMS_LNG_M)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_RIGHT_BOTTOM_LONGMM)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_EDT_OVERLAY_EDIT_OBJECT_RIGHT_BOTTOM_DMS_LNG_S)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_RIGHT_BOTTOM_LONGSS)->ShowWindow(SW_HIDE);
	CheckNum =0;
	
	return TRUE;  
}

void CDlgOverlayEditObject::OnUpdateLeftTopLatdd() 
{
	int CheckNum;
	CString str;
	CheckNum=GetDlgItemText(IDC_EDT_OVERLAY_EDIT_OBJECT_LEFT_TOP_DMS_LAT_D,str);
	if(CheckNum == 2)
	{
		GetDlgItem(IDC_EDT_OVERLAY_EDIT_OBJECT_LEFT_TOP_DMS_LAT_M)->SetFocus();
	}
}

void CDlgOverlayEditObject::OnUpdateLeftTopLatmm() 
{
	int CheckNum;
	CString str;
	CheckNum=GetDlgItemText(IDC_EDT_OVERLAY_EDIT_OBJECT_LEFT_TOP_DMS_LAT_M,str);
	if(CheckNum ==2 )
	{
		GetDlgItem(IDC_EDT_OVERLAY_EDIT_OBJECT_LEFT_TOP_DMS_LAT_S)->SetFocus();
	}
}

void CDlgOverlayEditObject::OnUpdateLeftTopLatss() 
{
	int CheckNum;
	CString str;
	CheckNum=GetDlgItemText(IDC_EDT_OVERLAY_EDIT_OBJECT_LEFT_TOP_DMS_LAT_S,str);
	if(CheckNum ==2 )
	{
		GetDlgItem(IDC_EDT_OVERLAY_EDIT_OBJECT_LEFT_TOP_DMS_LNG_D)->SetFocus();
	}
}

void CDlgOverlayEditObject::OnUpdateLeftTopLongdd() 
{
	int CheckNum;
	CString str;
	CheckNum=GetDlgItemText(IDC_EDT_OVERLAY_EDIT_OBJECT_LEFT_TOP_DMS_LNG_D,str);
	if(CheckNum ==3 )
	{
		GetDlgItem(IDC_EDT_OVERLAY_EDIT_OBJECT_LEFT_TOP_DMS_LNG_M)->SetFocus();
	}
}

void CDlgOverlayEditObject::OnUpdateLeftTopLongmm() 
{
	int CheckNum;
	CString str;
	CheckNum=GetDlgItemText(IDC_EDT_OVERLAY_EDIT_OBJECT_LEFT_TOP_DMS_LNG_M,str);
	if(CheckNum ==2 )
	{
		GetDlgItem(IDC_EDT_OVERLAY_EDIT_OBJECT_LEFT_TOP_DMS_LNG_S)->SetFocus();
	}
}

/////////////////////////////


void CDlgOverlayEditObject::OnUpdateRightBottomLatdd() 
{
	int CheckNum;
	CString str;
	CheckNum=GetDlgItemText(IDC_EDT_OVERLAY_EDIT_OBJECT_RIGHT_BOTTOM_DMS_LAT_D,str);
	if(CheckNum == 2)
	{
		GetDlgItem(IDC_EDT_OVERLAY_EDIT_OBJECT_RIGHT_BOTTOM_DMS_LAT_M)->SetFocus();
	}
}

void CDlgOverlayEditObject::OnUpdateRightBottomLatmm() 
{
	int CheckNum;
	CString str;
	CheckNum=GetDlgItemText(IDC_EDT_OVERLAY_EDIT_OBJECT_RIGHT_BOTTOM_DMS_LAT_M,str);
	if(CheckNum ==2 )
	{
		GetDlgItem(IDC_EDT_OVERLAY_EDIT_OBJECT_RIGHT_BOTTOM_DMS_LAT_S)->SetFocus();
	}
}

void CDlgOverlayEditObject::OnUpdateRightBottomLatss() 
{
	int CheckNum;
	CString str;
	CheckNum=GetDlgItemText(IDC_EDT_OVERLAY_EDIT_OBJECT_RIGHT_BOTTOM_DMS_LAT_S,str);
	if(CheckNum ==2 )
	{
		GetDlgItem(IDC_EDT_OVERLAY_EDIT_OBJECT_RIGHT_BOTTOM_DMS_LNG_D)->SetFocus();
	}
}

void CDlgOverlayEditObject::OnUpdateRightBottomLongdd() 
{
	int CheckNum;
	CString str;
	CheckNum=GetDlgItemText(IDC_EDT_OVERLAY_EDIT_OBJECT_RIGHT_BOTTOM_DMS_LNG_D,str);
	if(CheckNum ==3 )
	{
		GetDlgItem(IDC_EDT_OVERLAY_EDIT_OBJECT_RIGHT_BOTTOM_DMS_LNG_M)->SetFocus();
	}
}

void CDlgOverlayEditObject::OnUpdateRightBottomLongmm() 
{
	int CheckNum;
	CString str;
	CheckNum=GetDlgItemText(IDC_EDT_OVERLAY_EDIT_OBJECT_RIGHT_BOTTOM_DMS_LNG_M,str);
	if(CheckNum ==2 )
	{
		GetDlgItem(IDC_EDT_OVERLAY_EDIT_OBJECT_RIGHT_BOTTOM_DMS_LNG_S)->SetFocus();
	}
}

void CDlgOverlayEditObject::OnUpdateObjectRoatation()
{
	CC4IMapView* pView=(CC4IMapView*)(((CMainFrame*)AfxGetMainWnd())->GetActiveView()); 
	if(pView==NULL)
		return;
	
	OVERLAY::OBJECT* pObject = pView->m_pOverlayThread->GetOverlayMain()->SelectedObject(m_nObjectID);
	if(pObject==NULL)
		return;
	
	UpdateData(TRUE);
	pObject->Property.dAngle = m_dRotation;
}

void CDlgOverlayEditObject::ShowWindowEditDlg(UINT nObjectID)
{
	m_nObjectID = nObjectID;
	
	CC4IMapView *pView=(CC4IMapView*)(((CMainFrame*)AfxGetMainWnd())->GetActiveView());
	if(pView==NULL)
		return;
	OVERLAY::OBJECT* pObject = pView->m_pOverlayThread->GetOverlayMain()->SelectedObject(nObjectID);
	if(pObject==NULL)
		return;
	
	m_dRotation = pObject->Property.dAngle;
	
	//선(LINE)이면 각도 조절창이 안나오도록 함 kjs 091201
	if(pObject->GetType() ==OVERLAY::LINE)
		GetDlgItem(IDC_EDT_OVERLAY_EDIT_OBJECT_ROTATION)->EnableWindow(FALSE); 
	else
		GetDlgItem(IDC_EDT_OVERLAY_EDIT_OBJECT_ROTATION)->EnableWindow(TRUE); 

	
	UpdatePositionData(pObject);
	
	UpdateData(FALSE);
	CDialogEx::ShowWindow(SW_SHOW);
}

void CDlgOverlayEditObject::OnClose() 
{
	m_nObjectID = 0;
	CDialogEx::ShowWindow(SW_HIDE);
	CDialogEx::OnClose();
	
	//-------------------------
	CC4IMapView* pView = (CC4IMapView*)(((CMainFrame*)AfxGetMainWnd())->GetActiveView());
	if(pView==NULL)
		return;

	pView->m_pOverlayThread->GetOverlayView()->ReleaseSelectObject();
	pView->m_pOverlayThread->GetOverlayMain()->DrawOnScreen();
//	pView->DrawSituation(FALSE,FALSE,FALSE, TRUE);	
}

void CDlgOverlayEditObject::UpdatePositionData(OVERLAY::OBJECT* pObject)
{
	if(pObject==NULL)
		return;
	
	m_strlat_left_top.Format(_T("%f"),pObject->pt[0].Lat() * OVERLAY::MATH_RADIAN_TO_DEGREE);
	m_strlong_left_top.Format(_T("%f"),pObject->pt[0].Lng() * OVERLAY::MATH_RADIAN_TO_DEGREE);
	
	m_strlat_right_bottom.Format(_T("%f"),pObject->pt[1].Lat() * OVERLAY::MATH_RADIAN_TO_DEGREE);
	m_strlong_right_bottom.Format(_T("%f"),pObject->pt[1].Lng() * OVERLAY::MATH_RADIAN_TO_DEGREE);
	
	CCoordMgr* pCoordMgr  = GP_COORDMGR;
	CC4IMapView*pView=(CC4IMapView*)(((CMainFrame*)AfxGetMainWnd())->GetActiveView());
	
	if(pView!=NULL)
	{
		
		CCoord coord;
		char szUtm[20] ={0,};
		// MGRS
		coord.radians_lat_long_to_utm_str(pObject->pt[0].Lat(), pObject->pt[0].Lng(), szUtm);
		m_strMGRS_left_top.Format(_T("%s"),szUtm);
		coord.radians_lat_long_to_utm_str(pObject->pt[1].Lat(), pObject->pt[1].Lng(), szUtm);
		m_strMGRS_right_bottom.Format(_T("%s"),szUtm);
		
		// GEOREF
		char szGeoref[20] ={0,};
		coord.radians_lat_long_to_georef(pObject->pt[0].Lat(), pObject->pt[0].Lng(), szGeoref);
		m_strGEOREF_left_top.Format(_T("%s"),szGeoref);
		coord.radians_lat_long_to_georef(pObject->pt[1].Lat(), pObject->pt[1].Lng(), szGeoref);
		m_strGEOREF_right_bottom.Format(_T("%s"),szGeoref);

		// Lat, Long
		char szDMS[50] ={0,};
		coord.declat_dms(pObject->pt[0].Lat() * OVERLAY::MATH_RADIAN_TO_DEGREE, szDMS);
		if(strlen(szDMS)>=8)
		{
			szDMS[7] =0;
			m_latss_left_top.Format(_T("%s"), &szDMS[5]);
			szDMS[5] = 0;
			m_latmm_left_top.Format(_T("%s"), &szDMS[3]);
			szDMS[3] = 0;
			m_latdd_left_top.Format(_T("%s"), szDMS);
		}
		else
		{
			szDMS[6] =0;
			m_latss_left_top.Format(_T("%s"), &szDMS[4]);
			szDMS[4] = 0;
			m_latmm_left_top.Format(_T("%s"), &szDMS[2]);
			szDMS[2] = 0;
			m_latdd_left_top.Format(_T("%s"), szDMS);
		}
		
		coord.declon_dms(pObject->pt[0].Lng() * OVERLAY::MATH_RADIAN_TO_DEGREE, szDMS);
		if(strlen(szDMS)>=8)
		{
			szDMS[7] =0;
			m_longss_left_top.Format(_T("%s"), &szDMS[5]);
			szDMS[5] = 0;
			m_longmm_left_top.Format(_T("%s"), &szDMS[3]);
			szDMS[3] = 0;
			m_longdd_left_top.Format(_T("%s"), szDMS);
		}
		else
		{
			szDMS[6] =0;
			m_longss_left_top.Format(_T("%s"), &szDMS[4]);
			szDMS[4] = 0;
			m_longmm_left_top.Format(_T("%s"), &szDMS[2]);
			szDMS[2] = 0;
			m_longdd_left_top.Format(_T("%s"), strlen);
		}
		
		// Lat, Long
		coord.declat_dms(pObject->pt[1].Lat() * OVERLAY::MATH_RADIAN_TO_DEGREE, szDMS);
		if(strlen(szDMS)>=8)
		{
			szDMS[7] =0;
			m_latss_right_bottom.Format(_T("%s"), &szDMS[5]);
			szDMS[5] = 0;
			m_latmm_right_bottom.Format(_T("%s"), &szDMS[3]);
			szDMS[3] = 0;
			m_latdd_right_bottom.Format(_T("%s"), szDMS);
		}
		else
		{
			szDMS[6] =0;
			m_latss_right_bottom.Format(_T("%s") ,&szDMS[4]);
			szDMS[4] = 0;
			m_latmm_right_bottom.Format(_T("%s") ,&szDMS[2]);
			szDMS[2] = 0;
			m_latdd_right_bottom.Format(_T("%s") ,szDMS);
		}
		
		coord.declon_dms(pObject->pt[1].Lng() * OVERLAY::MATH_RADIAN_TO_DEGREE, szDMS);
		if(strlen(szDMS)>=8)
		{
			szDMS[7] =0;
			m_longss_right_bottom.Format(_T("%s"), &szDMS[5]);
			szDMS[5] = 0;
			m_longmm_right_bottom.Format(_T("%s"), &szDMS[3]);
			szDMS[3] = 0;
			m_longdd_right_bottom.Format(_T("%s"), szDMS);
		}
		else
		{
			szDMS[6] =0;
			m_longss_right_bottom.Format(_T("s"), &szDMS[4]);
			szDMS[4] = 0;
			m_longmm_right_bottom.Format(_T("s"), &szDMS[2]);
			szDMS[2] = 0;
			m_longdd_right_bottom.Format(_T("s"), szDMS);
		}
	}
	
	UpdateData(FALSE);
}
