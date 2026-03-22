// DlgOverlayEidtObjectPoint.cpp : implementation file
//

#include "pch.h"
#include "C4IMap.h"
#include "DlgOverlayEidtObjectPoint.h"
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
// CDlgOverlayEditObjectPoint dialog

IMPLEMENT_DYNAMIC(CDlgOverlayEditObjectPoint, CDialogEx)

CDlgOverlayEditObjectPoint::CDlgOverlayEditObjectPoint(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgOverlayEditObjectPoint::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgOverlayEditObjectPoint)
	// NOTE: the ClassWizard will add member initialization here
	// 위경도
	m_strlat = _T("");
	m_strlong = _T("");
	m_latmm = _T("");
	m_latdd = _T("");
	m_latss = _T("");
	m_longdd = _T("");
	m_longmm = _T("");
	m_longss = _T("");
	
	// MGRS
	m_strMGRS = _T("");
	
	// GEOREF
	m_strGEOREF = _T("");
	
	m_nObjectID = 0;
	m_nPointID = 0;
	
	m_bDegree = TRUE;
	m_bDMS = FALSE;
	//}}AFX_DATA_INIT
}


void CDlgOverlayEditObjectPoint::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgOverlayEditObjectPoint)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	// 위경도.
	DDX_Check(pDX, IDC_RADIO_OVERLAY_EDIT_OBJECT_POINT_SELECT_LAT_LNG_DEGREE, m_bDegree);
	DDX_Check(pDX, IDC_RADIO_OVERLAY_EDIT_OBJECT_POINT_SELECT_LAT_LNG_DMS, m_bDMS);
	
	DDX_Text(pDX, IDC_EDT_OVERLAY_EDIT_OBJECT_POINT_DEGREE_LAT, m_strlat);
	DDX_Text(pDX, IDC_EDT_OVERLAY_EDIT_OBJECT_POINT_DEGREE_LNG, m_strlong);	
	DDX_Text(pDX, IDC_EDT_OVERLAY_EDIT_OBJECT_POINT_DMS_LAT_D, m_latdd);
	DDV_MaxChars(pDX, m_latdd, 2);
	DDX_Text(pDX, IDC_EDT_OVERLAY_EDIT_OBJECT_POINT_DMS_LAT_M, m_latmm);
	DDV_MaxChars(pDX, m_latmm, 2);
	DDX_Text(pDX, IDC_EDT_OVERLAY_EDIT_OBJECT_POINT_DMS_LAT_S, m_latss);
	DDV_MaxChars(pDX, m_latss, 2);
	DDX_Text(pDX, IDC_EDT_OVERLAY_EDIT_OBJECT_POINT_DMS_LNG_D, m_longdd);
	DDV_MaxChars(pDX, m_longdd, 3);
	DDX_Text(pDX, IDC_EDT_OVERLAY_EDIT_OBJECT_POINT_DMS_LNG_M, m_longmm);
	DDV_MaxChars(pDX, m_longmm, 2);
	DDX_Text(pDX, IDC_EDT_OVERLAY_EDIT_OBJECT_POINT_DMS_LNG_S, m_longss);
	DDV_MaxChars(pDX, m_longss, 2);
		
	// MGRS	
	DDX_Text(pDX, IDC_EDIT_OVERLAY_OBJECT_POINT_MGRS, m_strMGRS);
	
	// GEOREF
	DDX_Text(pDX, IDC_EDIT_OVERLAY_OBJECT_POINT_GEOREF, m_strGEOREF);
	
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgOverlayEditObjectPoint, CDialogEx)
//{{AFX_MSG_MAP(CDlgOverlayEditObjectPoint)
ON_BN_CLICKED(IDC_BTN_OVERLAY_EDIT_OBJECT_POINT_MGRS, OnBtnOverlayEditObjectMgrs)
ON_BN_CLICKED(IDC_BTN_OVERLAY_EDIT_OBJECT_POINT_LAT_LNG, OnBtnOverlayEditObjectLatLng)
ON_BN_CLICKED(IDC_BTN_OVERLAY_EDIT_OBJECT_POINT_GEOREF, OnBtnOverlayEditObjectGeoref)

// 위경도.	
ON_BN_CLICKED(IDC_RADIO_OVERLAY_EDIT_OBJECT_POINT_SELECT_LAT_LNG_DEGREE, OnRadioLatLngDegree)
ON_BN_CLICKED(IDC_RADIO_OVERLAY_EDIT_OBJECT_POINT_SELECT_LAT_LNG_DMS, OnRadioLatLngDMS)

ON_EN_UPDATE(IDC_EDT_OVERLAY_EDIT_OBJECT_POINT_DMS_LAT_D, OnUpdateLatdd)
ON_EN_UPDATE(IDC_EDT_OVERLAY_EDIT_OBJECT_POINT_DMS_LAT_M, OnUpdateLatmm)
ON_EN_UPDATE(IDC_EDT_OVERLAY_EDIT_OBJECT_POINT_DMS_LNG_D, OnUpdateLongdd)
ON_EN_UPDATE(IDC_EDT_OVERLAY_EDIT_OBJECT_POINT_DMS_LAT_S, OnUpdateLatss)
ON_EN_UPDATE(IDC_EDT_OVERLAY_EDIT_OBJECT_POINT_DMS_LNG_M, OnUpdateLongmm)

ON_WM_CLOSE()

//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgOverlayEditObjectPoint message handlers
void CDlgOverlayEditObjectPoint::OnBtnOverlayEditObjectMgrs() 
{	
	CC4IMapView *pView=(CC4IMapView*)(((CMainFrame*)AfxGetMainWnd())->GetActiveView());
	if(pView==NULL)
		return;

	OVERLAY::OBJECT* pObject = pView->m_pOverlayThread->GetOverlayMain()->SelectedObject(m_nObjectID);
	if(pObject==NULL)
		return;

	if(m_nPointID>=pObject->GetAddedPointsNum())
		return;
	
	CString strMGRS=m_strMGRS;
	UpdateData(TRUE);
	
	// 조건처리 
	if(m_strMGRS.GetLength() != 11)
	{
		AfxMessageBox("MGRS 형식에 맞지 않습니다.");
		m_strMGRS = strMGRS;
		UpdateData(FALSE);
		return;
	}
	
	//일정 시야 밖의 범위 임력시 에러처리.	
	char num[2];
	num[0] = m_strMGRS.GetAt(0);
	num[1] = m_strMGRS.GetAt(1);
	int MgrsValidNum;
	MgrsValidNum= atoi(num);
	
	if(MgrsValidNum < 49 || MgrsValidNum > 54)
	{
		AfxMessageBox("일정범위의 값을 넣어주세요");
		m_strMGRS = strMGRS;
		UpdateData(FALSE);
		return;
	}
	
	for(int i=0; i<2 ; i++)
	{
		if(m_strMGRS.GetAt(i) < '0' || m_strMGRS.GetAt(i) > '9')
		{
			AfxMessageBox("MGRS 형식에 맞지 않습니다.");
			m_strMGRS = strMGRS;
			UpdateData(FALSE);
			return;
		}
	}
	
	for(int i=2; i<5 ; i++)
	{
		if(m_strMGRS.GetAt(i) < 'A' || m_strMGRS.GetAt(i) > 'z')
		{
			AfxMessageBox("MGRS 형식에 맞지 않습니다.");
			m_strMGRS = strMGRS;
			UpdateData(FALSE);
			return;
		}
	}
	
	for(int i=5; i<11 ; i++)
	{
		if(m_strMGRS.GetAt(i) < '0' || m_strMGRS.GetAt(i) > '9')
		{
			AfxMessageBox("MGRS 형식에 맞지 않습니다.");
			m_strMGRS = strMGRS;
			UpdateData(FALSE);
			return;
		}
	}	
	
	/////////////////////////////////////////
	// Update	
	char* temp_string;
	
	m_strMGRS.MakeUpper();   // 대소문자 
	temp_string = m_strMGRS.GetBuffer(11);
	
	double lat_rad, lng_rad;
	pView->m_Coord.utm_to_radians_lat_long(temp_string, &lat_rad, &lng_rad);

	double lat_degree = lat_rad * MATH_RADIAN_TO_DEGREE; 
	double lng_degree = lng_rad * MATH_RADIAN_TO_DEGREE;

	if(lat_degree < 20 || lat_degree > 50 || lng_degree < 100 || lng_degree >170)
	{
		AfxMessageBox("다음의 범위로 입력하여 주십시오.\n북위:20~50 동경:100~170 ");		
		m_strMGRS = strMGRS;
		UpdateData(FALSE);
		return;
	}
	UpdateData(FALSE);
	pObject->pt[m_nPointID].SetLatLong(lat_rad,lng_rad);	
	pView->m_pOverlayThread->GetOverlayMain()->DrawEditlayer();
//	pView->DrawSituation(FALSE,FALSE,TRUE, TRUE);
	UpdatePositionData(pObject, m_nPointID);
	
	//-------------------------
	pView->m_pOverlayThread->GetOverlayView()->ReleaseSelectObject();
	pView->m_pOverlayThread->GetOverlayMain()->DrawOnScreen();
//	pView->DrawSituation(FALSE,FALSE,FALSE, TRUE);
}

void CDlgOverlayEditObjectPoint::OnBtnOverlayEditObjectLatLng() 
{
	CC4IMapView* pView=(CC4IMapView*)(((CMainFrame*)AfxGetMainWnd())->GetActiveView());
	if(pView==NULL)
		return;
	
	OVERLAY::OBJECT* pObject = pView->m_pOverlayThread->GetOverlayMain()->SelectedObject(m_nObjectID);
	if(pObject==NULL)
		return;

	if(m_nPointID>=pObject->GetAddedPointsNum())
		return;
	
	CString strlat = m_strlat;
	CString strlong = m_strlong;
	UpdateData(TRUE);
	
	double input_lat = 0.0;
	double input_long = 0.0;
	
	if(m_bDegree)   // 실수 입력
	{
		input_lat = atof( m_strlat.GetBuffer(7) );
		input_long = atof( m_strlong.GetBuffer(8) );	  
		
		//에러처리. 우리나라 지도부근으로 범위를 한정.
		if(input_lat < 20 || input_lat > 50 || input_long < 100 || input_long >170)
		{
			AfxMessageBox("다음의 범위로 입력하여 주십시오.\n북위:20~50 동경:100~170 ");
			m_strlat = strlat;
			m_strlong = strlong;
			UpdateData(FALSE);
			return;
		}
	}
	else    // 도분초 입력 처리
	{
		
		if(m_latmm.GetLength()==1)
			m_latmm= '0'+m_latmm;
		if(m_latss.GetLength()==1)
			m_latss= '0'+m_latss;
		if(m_longmm.GetLength()==1)
			m_longmm= '0'+m_longmm;
		if(m_longss.GetLength()==1)
			m_longss= '0'+m_longss;
		
		m_strlat = m_latdd;
		m_strlat+= m_latmm;
		m_strlat+=m_latss;
		m_strlat+="N";
		
		m_strlong=m_longdd;
		m_strlong+=m_longmm;
		m_strlong+=m_longss;
		m_strlong+="E";
		
		pView->m_Coord.dms_to_dec_deg(m_strlat.GetBuffer(7), &input_lat); 
		pView->m_Coord.dms_to_dec_deg(m_strlong.GetBuffer(8), &input_long);
		
		//일정범위 벗어나면 에러처리.  우리나라를 중심으로 범위 한정. 
		if(input_lat < 20 || input_lat > 50 || input_long < 110 || input_long >150)
		{
			AfxMessageBox("다음의 범위로 입력하여 주십시오.\n북위:20~50 동경:100~170 "); 
			m_strlat = strlat;
			m_strlong = strlong;
			UpdateData(FALSE);
			return;
		}
		if(m_latmm > "59" || m_latss > "59" || m_longmm >"59" || m_longss >"59")
		{
			AfxMessageBox("분 또는 초의 입력값이 잘못되었습니다.");
			m_strlat = strlat;
			m_strlong = strlong;
			UpdateData(FALSE);
			return;
		}	
	}

	if(input_lat < 20 || input_lat > 50 || input_long < 100 || input_long >170)
	{
		AfxMessageBox("다음의 범위로 입력하여 주십시오.\n북위:20~50 동경:100~170 ");
		m_strlat = strlat;
		m_strlong = strlong;
		return;
	}

	
	UpdateData(FALSE);

	pObject->pt[m_nPointID].SetLatLong(input_lat * OVERLAY::MATH_DEGREE_TO_RADIAN, input_long * OVERLAY::MATH_DEGREE_TO_RADIAN);
	pView->m_pOverlayThread->GetOverlayMain()->DrawEditlayer();
//	pView->DrawSituation(FALSE,FALSE,TRUE, TRUE);
	UpdatePositionData(pObject, m_nPointID);

	//-------------------------
	pView->m_pOverlayThread->GetOverlayView()->ReleaseSelectObject();
	pView->m_pOverlayThread->GetOverlayMain()->DrawOnScreen();
//	pView->DrawSituation(FALSE,FALSE,FALSE, TRUE);
}

void CDlgOverlayEditObjectPoint::OnBtnOverlayEditObjectGeoref()
{
	CC4IMapView*pView=(CC4IMapView*)(((CMainFrame*)AfxGetMainWnd())->GetActiveView());
	if(pView==NULL)
		return;
	
	OVERLAY::OBJECT* pObject = pView->m_pOverlayThread->GetOverlayMain()->SelectedObject(m_nObjectID);
	if(pObject==NULL)
		return;
	
	if(m_nPointID>=pObject->GetAddedPointsNum())
		return;

	
	CString strGEOREF = m_strGEOREF;
	UpdateData(TRUE);

	double input_lat=0.0, input_long=0;

	m_strGEOREF.MakeUpper();

	CCoord coord;
	coord.georef_to_radians_lat_long(m_strGEOREF.GetBuffer(0),&input_lat,&input_long);

	double lat_degree = input_lat * MATH_RADIAN_TO_DEGREE; 
	double lng_degree = input_long * MATH_RADIAN_TO_DEGREE;

	if(lat_degree < 20 || lat_degree > 50 || lng_degree < 100 || lng_degree >170)
	{
		AfxMessageBox("다음의 범위로 입력하여 주십시오.\n북위:20~50 동경:100~170 ");		
		m_strGEOREF = strGEOREF;
		UpdateData(FALSE);
		return;
	}
	pObject->pt[m_nPointID].SetLatLong(input_lat,input_long);	
	pView->m_pOverlayThread->GetOverlayMain()->DrawEditlayer();
//	pView->DrawSituation(FALSE,FALSE,TRUE, TRUE);
	UpdatePositionData(pObject, m_nPointID);

	//-------------------------
	pView->m_pOverlayThread->GetOverlayView()->ReleaseSelectObject();
	pView->m_pOverlayThread->GetOverlayMain()->DrawOnScreen();
//	pView->DrawSituation(FALSE,FALSE,FALSE, TRUE);	
}

// 위경도
void CDlgOverlayEditObjectPoint::OnRadioLatLngDegree() 
{	
	m_bDegree = TRUE;
	m_bDMS = FALSE;
	
	GetDlgItem(IDC_EDT_OVERLAY_EDIT_OBJECT_POINT_DEGREE_LAT)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_EDT_OVERLAY_EDIT_OBJECT_POINT_DEGREE_LNG)->ShowWindow(SW_SHOW);
	
	GetDlgItem(IDC_EDT_OVERLAY_EDIT_OBJECT_POINT_DMS_LAT_D)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_POINT_LATDD)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_EDT_OVERLAY_EDIT_OBJECT_POINT_DMS_LAT_M)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_POINT_LATMM)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_EDT_OVERLAY_EDIT_OBJECT_POINT_DMS_LAT_S)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_POINT_LATSS)->ShowWindow(SW_HIDE);
	
	GetDlgItem(IDC_EDT_OVERLAY_EDIT_OBJECT_POINT_DMS_LNG_D)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_POINT_LONGDD)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_EDT_OVERLAY_EDIT_OBJECT_POINT_DMS_LNG_M)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_POINT_LONGMM)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_EDT_OVERLAY_EDIT_OBJECT_POINT_DMS_LNG_S)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_POINT_LONGSS)->ShowWindow(SW_HIDE);
	
	UpdateData(FALSE);
}

void CDlgOverlayEditObjectPoint::OnRadioLatLngDMS() 
{
	m_bDegree = FALSE;
	m_bDMS = TRUE;	
	
	GetDlgItem(IDC_EDT_OVERLAY_EDIT_OBJECT_POINT_DEGREE_LAT)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_EDT_OVERLAY_EDIT_OBJECT_POINT_DEGREE_LNG)->ShowWindow(SW_HIDE);
	
	GetDlgItem(IDC_EDT_OVERLAY_EDIT_OBJECT_POINT_DMS_LAT_D)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_STATIC_POINT_LATDD)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_EDT_OVERLAY_EDIT_OBJECT_POINT_DMS_LAT_M)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_STATIC_POINT_LATMM)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_EDT_OVERLAY_EDIT_OBJECT_POINT_DMS_LAT_S)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_STATIC_POINT_LATSS)->ShowWindow(SW_SHOW);
	
	GetDlgItem(IDC_EDT_OVERLAY_EDIT_OBJECT_POINT_DMS_LNG_D)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_STATIC_POINT_LONGDD)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_EDT_OVERLAY_EDIT_OBJECT_POINT_DMS_LNG_M)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_STATIC_POINT_LONGMM)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_EDT_OVERLAY_EDIT_OBJECT_POINT_DMS_LNG_S)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_STATIC_POINT_LONGSS)->ShowWindow(SW_SHOW);

	UpdateData(FALSE);
}

BOOL CDlgOverlayEditObjectPoint::OnInitDialog() 
{
	CDialogEx::OnInitDialog();
	GetDlgItem(IDC_EDT_OVERLAY_EDIT_OBJECT_POINT_DMS_LAT_D)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_POINT_LATDD)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_EDT_OVERLAY_EDIT_OBJECT_POINT_DMS_LAT_M)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_POINT_LATMM)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_EDT_OVERLAY_EDIT_OBJECT_POINT_DMS_LAT_S)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_POINT_LATSS)->ShowWindow(SW_HIDE);
	
	GetDlgItem(IDC_EDT_OVERLAY_EDIT_OBJECT_POINT_DMS_LNG_D)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_POINT_LONGDD)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_EDT_OVERLAY_EDIT_OBJECT_POINT_DMS_LNG_M)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_POINT_LONGMM)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_EDT_OVERLAY_EDIT_OBJECT_POINT_DMS_LNG_S)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_POINT_LONGSS)->ShowWindow(SW_HIDE);
	
	CheckNum =0;
	
	return TRUE;  
}

void CDlgOverlayEditObjectPoint::OnUpdateLatdd() 
{
	int CheckNum;
	CString str;
	CheckNum=GetDlgItemText(IDC_EDT_OVERLAY_EDIT_OBJECT_POINT_DMS_LAT_D,str);
	if(CheckNum == 2)
	{
		GetDlgItem(IDC_EDT_OVERLAY_EDIT_OBJECT_POINT_DMS_LAT_M)->SetFocus();
	}
}

void CDlgOverlayEditObjectPoint::OnUpdateLatmm() 
{
	int CheckNum;
	CString str;
	CheckNum=GetDlgItemText(IDC_EDT_OVERLAY_EDIT_OBJECT_POINT_DMS_LAT_M,str);
	if(CheckNum ==2 )
	{
		GetDlgItem(IDC_EDT_OVERLAY_EDIT_OBJECT_POINT_DMS_LAT_S)->SetFocus();
	}
}

void CDlgOverlayEditObjectPoint::OnUpdateLatss() 
{
	int CheckNum;
	CString str;
	CheckNum=GetDlgItemText(IDC_EDT_OVERLAY_EDIT_OBJECT_POINT_DMS_LAT_S,str);
	if(CheckNum ==2 )
	{
		GetDlgItem(IDC_EDT_OVERLAY_EDIT_OBJECT_POINT_DMS_LNG_D)->SetFocus();
	}
}

void CDlgOverlayEditObjectPoint::OnUpdateLongdd() 
{
	int CheckNum;
	CString str;
	CheckNum=GetDlgItemText(IDC_EDT_OVERLAY_EDIT_OBJECT_POINT_DMS_LNG_D,str);
	if(CheckNum ==3 )
	{
		GetDlgItem(IDC_EDT_OVERLAY_EDIT_OBJECT_POINT_DMS_LNG_M)->SetFocus();
	}
}

void CDlgOverlayEditObjectPoint::OnUpdateLongmm() 
{
	int CheckNum;
	CString str;
	CheckNum=GetDlgItemText(IDC_EDT_OVERLAY_EDIT_OBJECT_POINT_DMS_LNG_M,str);
	if(CheckNum ==2 )
	{
		GetDlgItem(IDC_EDT_OVERLAY_EDIT_OBJECT_POINT_DMS_LNG_S)->SetFocus();
	}
}

void CDlgOverlayEditObjectPoint::ShowWindowEditDlg(UINT nObjectID, UINT nPointID)
{
	m_nObjectID = nObjectID;
	m_nPointID = nPointID;
	
	CC4IMapView*pView=(CC4IMapView*)(((CMainFrame*)AfxGetMainWnd())->GetActiveView());
	if(pView==NULL)
		return;
	OVERLAY::OBJECT* pObject = pView->m_pOverlayThread->GetOverlayMain()->SelectedObject(nObjectID);
	if(pObject==NULL)
		return;
		
	UpdatePositionData(pObject, nPointID);
	
	UpdateData(FALSE);
	CDialogEx::ShowWindow(SW_SHOW);
}

void CDlgOverlayEditObjectPoint::OnClose() 
{
	m_nObjectID = 0;
	CDialogEx::ShowWindow(SW_HIDE);
	CDialogEx::OnClose();
}

void CDlgOverlayEditObjectPoint::UpdatePositionData(OVERLAY::OBJECT* pObject, UINT nPointID)
{
	if(pObject==NULL)
		return;
	
	if(m_nPointID>=pObject->GetAddedPointsNum())
		return;
	
	m_strlat.Format(_T("%f"),pObject->pt[nPointID].Lat() * MATH_RADIAN_TO_DEGREE);
	m_strlong.Format(_T("%f"),pObject->pt[nPointID].Lng() * MATH_RADIAN_TO_DEGREE);
	
	CCoordMgr* pCoordMgr  = GP_COORDMGR;
	CC4IMapView* pView=(CC4IMapView*)(((CMainFrame*)AfxGetMainWnd())->GetActiveView());
	
	if(pView!=NULL)
	{		
		CCoord coord;
		char szUtm[20] ={0,};
		// MGRS
		coord.radians_lat_long_to_utm_str(pObject->pt[nPointID].Lat(), pObject->pt[nPointID].Lng(), szUtm);
		m_strMGRS.Format(_T("%s"),szUtm);
		
		// GEOREF
		char szGeoref[20] ={0,};
		coord.radians_lat_long_to_georef(pObject->pt[nPointID].Lat(), pObject->pt[nPointID].Lng(), szGeoref);
		m_strGEOREF.Format(_T("%s"),szGeoref);

		// Lat, Long
		char szDMS[50] ={0,};
		coord.declat_dms(pObject->pt[nPointID].Lat() * MATH_RADIAN_TO_DEGREE, szDMS);
		if(strlen(szDMS) >= 8)
		{
			szDMS[7] = 0;
			m_latss.Format(_T("%s"), &szDMS[5]);
			szDMS[5] = 0;
			m_latmm.Format(_T("%s"), &szDMS[3]);
			szDMS[3] = 0;
			m_latdd.Format(_T("%s"), szDMS);
		}
		else
		{
			szDMS[6] = 0;
			m_latss.Format(_T("%s"), &szDMS[4]);
			szDMS[4] = 0;
			m_latmm.Format(_T("%s"), &szDMS[2]);
			szDMS[2] = 0;
			m_latdd.Format(_T("%s"), szDMS);
		}
		
		coord.declon_dms(pObject->pt[nPointID].Lng()*MATH_RADIAN_TO_DEGREE, szDMS);		
		if(strlen(szDMS) >= 8)
		{
			szDMS[7] = 0;
			m_longss.Format(_T("%s"), &szDMS[5]);
			szDMS[5] = 0;
			m_longmm.Format(_T("%s"), &szDMS[3]);
			szDMS[3] = 0;
			m_longdd.Format(_T("%s"), szDMS);
		}
		else
		{
			szDMS[6] = 0;
			m_longss.Format(_T("%s"), &szDMS[4]);
			szDMS[4] = 0;
			m_longmm.Format(_T("%s"), &szDMS[2]);
			szDMS[2] = 0;
			m_longdd.Format(_T("%s"), strlen);
		}		
	}
	
	UpdateData(FALSE);
}
