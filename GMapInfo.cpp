// GMapInfo.cpp : implementation file
//

#include "pch.h"
#include "GStatusFlag.h"
#include "GMapInfo.h"
#include "GEnvVar.h"
#include "GFileUtil.h"

//#include "GFileFind.h"
//#include "..\\MapDlg\\S57OBJData.h" //MapDlg에 있는 해도관련 헤더파일을 참조한다.

// 지도자료 유뮤 확인시 사용
#include <io.h>
#include <direct.h>
#include <errno.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGMapInfo

CGMapInfo::CGMapInfo()
{
	InitInstance();
}

CGMapInfo::~CGMapInfo()
{
//	if( m_pMap3d ) {
//		delete m_pMap3d;
//		m_pMap3d = NULL;
//	}
	
	if( m_pPalette )
	{
		m_pPalette->DeleteObject();
		delete m_pPalette;
		m_pPalette = NULL;
	}
	if( m_pClientDC->GetSafeHdc() )
	{
		delete m_pClientDC;
		m_pClientDC = NULL;
	}
	POSITION	pos;
	pos = m_MapInfoList.GetHeadPosition();
	while(pos)
	{
		CGMapSetInfo *Info = (CGMapSetInfo *) m_MapInfoList.GetNext(pos);
		delete Info;
	}
	m_MapInfoList.RemoveAll();

	m_SencRectList.RemoveAll();
	m_SencRectInfoList.RemoveAll();
	m_SencScaleList.RemoveAll();
}

CString	CGMapInfo::GetBasePath()
{
	if (m_strBasePath.IsEmpty()) 
	{
		return m_strBasePath;
	}

	CGEnvVar	env("MAP_HOME");
	CString		strBase;

	strBase = env.GetValue();
	if (m_strBasePath.GetAt(0)!='\\')
	{
		strBase += "\\";
	}
	strBase += m_strBasePath;

	return strBase;
}

void CGMapInfo::SetBasePath(CString basePath)
{
	CGEnvVar	envFileBase("SGIS_FILE_BASE");
	CGEnvVar	envSgisSv("SGIS-SV");
	CString		strBase;
	CString		strTmp;

	// 로컬 영역인지 확인
	strBase = envFileBase.GetValue() + "\\map";
	strTmp = basePath.Left(strBase.GetLength());
	strBase.MakeUpper();
	strTmp.MakeUpper();
	if (strBase==strTmp)
	{
		m_strBasePath = basePath.Right(basePath.GetLength() - strBase.GetLength());
		return;
	}

	// 서버 영역인지 확인
	strBase = envSgisSv.GetValue() + "\\map";
	strTmp = basePath.Left(strBase.GetLength());
	strBase.MakeUpper();
	strTmp.MakeUpper();
	if (strBase==strTmp) 
	{
		m_strBasePath = basePath.Right(basePath.GetLength() - strBase.GetLength());
		return;
	}

	// 임의의 영역
	m_strBasePath = basePath;
}

void CGMapInfo::InitInstance()
{	
	m_nMapKind = MK_VECTOR_LAND;	// Nothing
	m_msMapScale = MS_1_1000000;	// 1:100만
	m_ctCoordType = CT_GP;	// 경위도 좌표계
//	m_strBasePath.Empty();
//	m_strLibName.Empty();
	m_strBasePath = _T("");
	m_strLibName = _T("");

	m_Color.InitInstance();
	m_Feature.InitInstance();
	m_Cvg.InitInstance();
	m_Revert.InitInstance();
	m_Revertable.InitInstance();

	m_bColorModified = FALSE;
	m_bFeatureModified = FALSE;
	m_bCvgModified = FALSE;
	m_bRevModified = FALSE;
	m_bRevTableModified = FALSE;

	m_pPalette = NULL;
	
	m_nOldMapKind = MK_NOTHING;
	m_nOldMapScale = MS_NOTHING;
//	m_n3DModel = M3DT_WIRE2;

	m_bBlankMsgOK = TRUE;

	m_MapPoint = NULL;
	m_MapHandle = 0 ;

	m_bDrawed = FALSE;		// 지도 선택 유무
	m_pClientDC = NULL;		// 3차원 객체 생성시에 할당되고 3차원 객체가 죽을 때 해제 된다.
	m_bReplaceMap = FALSE;	// 지도 전환 여부

	// 항만도 초기 위치좌표 설정용
	m_HcrfPointLT = CGPointGP(0,0);
	m_HcrfExt = CGSizeGP(0,0);

	m_bIsYodoRaster = FALSE;

	m_bEncBackDisp = FALSE;
	m_bNeighbDisp = FALSE;
	m_ChartSort = 22;
	m_nEncScale = 0;


	for(int i=0  ; i < 167; i++)
		m_CheckedItemList[i] = 'T';

//2003. 05 일부 주석처리함.
/*	for(i=0; i<m_NumMin; i++)
	{
		m_CheckedItemList[(m_MinCheckItem[i])]='F';
	}
	for(i=0; i<m_NumBase; i++) 
	{//기본 선택 개체는 56 개..
		m_CheckedItemList[(m_BaseCheckItem[i])]='F';
	} //m_BaseCheckItem[i] :: 기본 선택 아이템의 리스트번호.  
	for(i=0; i<m_NumSTD; i++)  
	{//표준 선택 개체는 93 개..
		m_CheckedItemList[(m_STDCheckItem[i])]='F';
	} //m_STDCheckItem[i] :: 표준 선택 아이템의 리스트번호.  
	m_SelectedRadio = 1; 
	int j=0;
	for(i=0;i<NUM_ITEM;i++)
	{
		if(m_CheckedItemList[i]=='F')
		{
			mp_ItemCode[j] = (WORD)atoi(_gszObj[i][1]);//지형요소들의 코드번호. 		
			j++;
		}
	}*/

	int j=0;
	m_nSelCodeNum = j;
	mp_ItemCode[j] = 0;  
	m_bObjSelChange = FALSE;
	m_nMapViewType = 0; // 상세보기

	m_HighlightShallowRadio = 1;	
	m_TwoShadeRadio = 1 ;	
	m_PlainBoundRadio = 0 ;	
	m_SimpleSymbolRadio = 1;
	m_ShowMagvarCheck = TRUE ;	
	m_ShowTextCheck = TRUE ;	
	m_UseScaminCheck = TRUE ;	
	m_ShowTopmark = FALSE;	
	m_ShowIsoDanger = FALSE;
	m_DeepContourEdit = 100.0f ;	  
	m_SafeDepthEdit = 3.0f ;	     
	m_SafetyContourEdit = 10.0f;	  
	m_ShallowContourEdit = 3.0f;	
	m_bIfDisp = TRUE;	// 지도 배율에 따라 제한

	m_bTerrainChk = FALSE;

	m_bSimple = FALSE;
//	m_pMap3d = NULL;

	LoadDefaultInformations();
}

BOOL CGMapInfo::IsValid()
{
	BOOL	bValid =
			(m_nMapKind!=MK_NOTHING)
			&& (m_msMapScale!=MS_NOTHING)
			&& (m_ctCoordType!=CT_NOTHING)
			;
	return bValid;
}

void CGMapInfo::Serialize(CArchive& ar)
{
	int			i;
	long		tmp = 0;
	POINTGP		pntGP;
	SIZEGP		sizeGP;
	CGEnvVar	envM("MAP_HOME");
//	CString		strM;
//	CString		strTmp;
	CString		strBaseTmp;
//	int			baseLen;
//	int			envLen;

//	strM = envM.GetValue();
//	strM.MakeUpper();
//	envLen = strM.GetLength();

	if(ar.IsStoring()) {
		ar.Write(&m_nMapKind, sizeof(EGMapKind));
		ar.Write(&m_msMapScale, sizeof(EGMapScale));
		ar.Write(&m_ctCoordType, sizeof(EGCoordType));
		ar.Write(&m_bDrawed, sizeof(BOOL));
		ar.Write(&m_bReplaceMap, sizeof(BOOL));
		ar.Write(&m_bIsYodoRaster, sizeof(BOOL));
		ar.Write(&m_nEncScale, sizeof(int));
		ar.Write(&m_nDispScale, sizeof(int));

		memcpy(&pntGP, &(m_HcrfPointLT.m_P), sizeof(POINTGP));
		memcpy(&sizeGP, &(m_HcrfExt.m_S), sizeof(SIZEGP));
		ar.Write(&pntGP, sizeof(POINTGP));
		ar.Write(&sizeGP, sizeof(SIZEGP));

		ar.Write(&m_ChartSort, sizeof(UINT));
		ar.Write(&m_SelectedRadio, sizeof(UINT));
		ar.Write(&m_nSelCodeNum, sizeof(int));

		for(i = 0; i < m_nSelCodeNum ; i++) {
			ar.Write(&mp_ItemCode[i], sizeof(WORD));
		}

		for(i=0 ; i < 167 ;  i++  ) {
			ar.Write(&m_CheckedItemList[i], sizeof(char) );
		}

		ar.Write(&m_bEncBackDisp, sizeof(BOOL));
		ar.Write(&m_bNeighbDisp, sizeof(BOOL));
		ar.Write(&m_HighlightShallowRadio, sizeof(int));
		ar.Write(&m_TwoShadeRadio,sizeof(int));
		ar.Write(&m_PlainBoundRadio,sizeof(int));
		ar.Write(&m_SimpleSymbolRadio,sizeof(int));
		ar.Write(&m_ShowMagvarCheck, sizeof(BOOL));
		ar.Write(&m_ShowTextCheck, sizeof(BOOL));
		ar.Write(&m_UseScaminCheck, sizeof(BOOL));
		ar.Write(&m_ShowTopmark, sizeof(BOOL));
		ar.Write(&m_ShowIsoDanger, sizeof(BOOL));
		ar.Write(&m_DeepContourEdit, sizeof(float));
		ar.Write(&m_SafeDepthEdit, sizeof(float));
		ar.Write(&m_SafetyContourEdit, sizeof(float));
		ar.Write(&m_ShallowContourEdit, sizeof(float));

		ar.Write(&m_nMapViewType, sizeof(SHORT));

		//
		// 지도 디렉토리인 경우 path에서 MAP_HOME 부분을 제거하고 저장한다.
		// 일반 디렉토리인 경우 그냥 저장한다.
//		baseLen = m_strBasePath.GetLength();
//		strBaseTmp = m_strBasePath;
		strBaseTmp = m_strBasePath;
		strBaseTmp.MakeUpper();

//		if (baseLen<envLen) {
//			strTmp = m_strBasePath;
//		}
//		else if (strBaseTmp.Left(envLen)==strM) {	// 지도 디렉토리
//			strTmp = m_strBasePath.Right(baseLen - envLen);
//		}
//		else {		// 일반 디렉토리
//			strTmp = m_strBasePath;
//		}
//		CGFileUtil::WriteString(ar, strTmp);
		CGFileUtil::WriteString(ar, strBaseTmp);
		CGFileUtil::WriteString(ar, m_strLibName);
	}
	else {
		InitInstance();

		ar.Read(&m_nMapKind, sizeof(EGMapKind));
		ar.Read(&m_msMapScale, sizeof(EGMapScale));
		ar.Read(&m_ctCoordType, sizeof(EGCoordType));
		ar.Read(&m_bDrawed, sizeof(BOOL));
		ar.Read(&m_bReplaceMap, sizeof(BOOL));
		ar.Read(&m_bIsYodoRaster, sizeof(BOOL));
		ar.Read(&m_nEncScale, sizeof(int));
		ar.Read(&m_nDispScale, sizeof(int));

		ar.Read(&pntGP, sizeof(POINTGP));
		ar.Read(&sizeGP, sizeof(SIZEGP));
		m_HcrfPointLT.SetPoint(&pntGP);
		m_HcrfExt.SetSize(&sizeGP);

		ar.Read(&m_ChartSort, sizeof(UINT));
		ar.Read(&m_SelectedRadio, sizeof(UINT));
		ar.Read(&m_nSelCodeNum, sizeof(int));	
		
		for(i = 0; i < m_nSelCodeNum ; i++) {
			ar.Read(&mp_ItemCode[i], sizeof(WORD));
		}
		mp_ItemCode[i] = 0;  

		for(i=0 ; i < 167 ;  i++  ) {
			ar.Read(&m_CheckedItemList[i] , sizeof(char));
		}

		ar.Read(&m_bEncBackDisp, sizeof(BOOL));
		ar.Read(&m_bNeighbDisp, sizeof(BOOL));
		ar.Read(&m_HighlightShallowRadio, sizeof(int));
		ar.Read(&m_TwoShadeRadio,sizeof(int));
		ar.Read(&m_PlainBoundRadio,sizeof(int));
		ar.Read(&m_SimpleSymbolRadio,sizeof(int));
		ar.Read(&m_ShowMagvarCheck, sizeof(BOOL));
		ar.Read(&m_ShowTextCheck, sizeof(BOOL));
		ar.Read(&m_UseScaminCheck, sizeof(BOOL));
		ar.Read(&m_ShowTopmark, sizeof(BOOL));
		ar.Read(&m_ShowIsoDanger, sizeof(BOOL));
		ar.Read(&m_DeepContourEdit, sizeof(float));
		ar.Read(&m_SafeDepthEdit, sizeof(float));
		ar.Read(&m_SafetyContourEdit, sizeof(float));
		ar.Read(&m_ShallowContourEdit, sizeof(float));

		ar.Read(&m_nMapViewType, sizeof(SHORT));
		
//		CGFileUtil::ReadString(ar, strTmp);
		CGFileUtil::ReadString(ar, m_strBasePath);
		CGFileUtil::ReadString(ar, m_strLibName);

		// Full path인 경우 일반 디렉토리이므로 그대로 사용하고
		// 아닐 경우 지도 디렉토리이므로 MAP_HOME을 앞에 붙여서 사용한다.

//		if(strTmp.IsEmpty()) {
//			// 디렉토리가 지정되지 않은 것은 지도 도시를 하지 않음
//		}
//		else if (strTmp.GetLength()>2 && strTmp.GetAt(1)==':') {
//			// 일반 디렉토리
//		}
//		else {
//			// 지도 디렉토리
//			strTmp = strM + strTmp;
//		}
//		m_strBasePath = strTmp;
	}

	m_Color.Serialize(ar);		// 색상정보
	m_Feature.Serialize(ar);	// Feature On/Off
	m_Cvg.Serialize(ar);		// 커버리지 On/Off
	m_Revert.Serialize(ar);		//
	m_Revertable.Serialize(ar);	//

	m_bColorModified = (m_Color.GetSize()>0);
	m_bFeatureModified = (m_Feature.GetSize()>0);
	m_bCvgModified = (m_Cvg.GetSize()<=0);
	m_bRevModified = (m_Revert.GetSize()<=0);
	m_bRevTableModified = (m_Revertable.GetSize()<=0);
}

BOOL CGMapInfo::SetMapKind(EGMapKind mapKind)
{
	m_nMapKind = mapKind;
	if(mapKind==MK_NOTHING) 
	{
		m_Color.InitInstance();
		m_Feature.InitInstance();
		m_Cvg.InitInstance();
		return TRUE;
	}

	if(!LoadDefaultInformations()) 
	{
		return FALSE;
	}

	return TRUE;
}

const CGMapSetInfo &CGMapSetInfo::operator=(const CGMapSetInfo &right)
{
	if(&right!=this) 
	{
		m_nMapKind = right.m_nMapKind;
		m_msMapScale = right.m_msMapScale;

		m_strBasePath = right.m_strBasePath;
		m_strLibName = right.m_strLibName;

		m_Color = right.m_Color;
		m_Feature = right.m_Feature;
		m_Cvg = right.m_Cvg;
		m_Revert = right.m_Revert;
		m_Revertable = right.m_Revertable;
	}

	return *this;
}

const CGMapInfo &CGMapInfo::operator=(const CGMapInfo &right)
{
	if(&right!=this) 
	{
		m_nMapKind = right.m_nMapKind;
		m_msMapScale = right.m_msMapScale;
		m_ctCoordType = right.m_ctCoordType;
		m_bDrawed = right.m_bDrawed;			// 도시설정
		m_bReplaceMap = right.m_bReplaceMap;	// 지도전환여부

		m_strBasePath = right.m_strBasePath;
		m_strLibName = right.m_strLibName;

		m_Color = right.m_Color;			// 색상정보
		m_Feature = right.m_Feature;		// feature on/off
		m_Cvg = right.m_Cvg;				// coverage on/off
		m_Revert = right.m_Revert;			
		m_Revertable = right.m_Revertable;
		//도로망 분석용
		m_MapPoint = right.m_MapPoint;
		m_MapHandle = right.m_MapHandle;

		POSITION	pos;
		pos = right.m_MapInfoList.GetHeadPosition();
		while(pos) 
		{
			CGMapSetInfo *tmp;
			BOOL bExist = FALSE;
			
			CGMapSetInfo *rInfo = (CGMapSetInfo *) right.m_MapInfoList.GetNext(pos);

			// 이미 동일 지도및 축척의 값이 존재하는지 검사.
			POSITION pos1 = m_MapInfoList.GetHeadPosition();
			while (pos1) 
			{
				CGMapSetInfo *pInfo = (CGMapSetInfo *) m_MapInfoList.GetNext(pos1);
				if((rInfo->m_nMapKind == pInfo->m_nMapKind) && (rInfo->m_msMapScale == pInfo->m_msMapScale)) {	// 존재함 
					pInfo->m_Color = rInfo->m_Color;	// 색상과 피쳐세팅만 바꾼다
					pInfo->m_Feature = rInfo->m_Feature;
					pInfo->m_Cvg = rInfo->m_Cvg;
					pInfo->m_Revert = rInfo->m_Revert;
					pInfo->m_Revertable = rInfo->m_Revertable;
					bExist = TRUE;
					break;
				}
			}
			if(bExist) break;	// 존재

			if (IsDrawed()) 
			{
				tmp = new CGMapSetInfo;
				*tmp = *rInfo;
				m_MapInfoList.AddTail(tmp);
			}
		}
		
		m_nMapViewType = right.m_nMapViewType;

		//*m_pPalette = *(right.m_pPalette);

		m_nOldMapKind = right.m_nOldMapKind;
		m_nOldMapScale = right.m_nOldMapScale;
		m_bBlankMsgOK = right.m_bBlankMsgOK;

		m_bIsYodoRaster = right.m_bIsYodoRaster;

		m_bColorModified = right.m_bColorModified;			// 색상정보를 변경 했는가.
		m_bFeatureModified = right.m_bFeatureModified;		// Feature 상태를 변경 했는가.
		m_bCvgModified = right.m_bCvgModified;				// Coverage 상태를 변경 했는가.
		m_bRevModified = right.m_bRevModified;				//
		m_bRevTableModified = right.m_bRevTableModified;	//

		//*m_pClientDC = *(right.m_pClientDC);

		m_nDispScale = right.m_nDispScale;			// 화면스케일
		m_nEncScale = right.m_nEncScale;			// 해도,항만도의 원시축척
		m_bEncBackDisp = right.m_bEncBackDisp;		//배경도 
		m_bNeighbDisp = right.m_bNeighbDisp;		//연속도시
		m_ChartSort = right.m_ChartSort;			//도엽 종류
		m_bObjSelChange = right.m_bObjSelChange;
		m_HighlightShallowRadio = right.m_HighlightShallowRadio;
		m_TwoShadeRadio = right.m_TwoShadeRadio;
		m_PlainBoundRadio = right.m_PlainBoundRadio;
		m_ShowMagvarCheck = right.m_ShowMagvarCheck;
		m_ShowTopmark = right.m_ShowTopmark;
		m_ShowIsoDanger = right.m_ShowIsoDanger;
		m_DeepContourEdit = right.m_DeepContourEdit;
		m_SafeDepthEdit = right.m_SafeDepthEdit;
		m_SafetyContourEdit = right.m_SafetyContourEdit;
		m_SimpleSymbolRadio = right.m_SimpleSymbolRadio;
		m_ShowTextCheck = right.m_ShowTextCheck;
		m_UseScaminCheck = right.m_UseScaminCheck;
		m_ShallowContourEdit = right.m_ShallowContourEdit;
		// 호성추가
		for( int i = 0; i < 168; i++ )
			mp_ItemCode[i] = right.mp_ItemCode[i];

		for(int i = 0; i < 167; i++ )
			m_CheckedItemList[i] = right.m_CheckedItemList[i];

		m_nSelCodeNum = right.m_nSelCodeNum;
		m_SelectedRadio = right.m_SelectedRadio;
		


		m_HcrfPointLT = right.m_HcrfPointLT;	// 항만도 초기의 좌상좌표
		m_HcrfExt = right.m_HcrfExt;
		m_bIfDisp = right.m_bIfDisp;

		//SencRectInfo
		POSITION spos;
		CGRectGP   rct;
		CString    StrSenc;
		int  SencScale;
		m_SencRectList.RemoveAll();
		spos = right.m_SencRectList.GetHeadPosition();
		while(spos != NULL)
		{			
			rct = right.m_SencRectList.GetNext(spos);
			m_SencRectList.AddTail( rct );
		}

		m_SencRectInfoList.RemoveAll();
		spos = right.m_SencRectInfoList.GetHeadPosition();
		while(spos != NULL)
		{			
			StrSenc = right.m_SencRectInfoList.GetNext(spos);
			m_SencRectInfoList.AddTail( StrSenc );
		}

		m_SencScaleList.RemoveAll();
		spos = right.m_SencScaleList.GetHeadPosition();
		while(spos != NULL)
		{			
			SencScale = right.m_SencScaleList.GetNext(spos);
			m_SencScaleList.AddTail( SencScale );
		}
		
		m_KrRect = right.m_KrRect;

	}

	return *this;
}

BOOL CGMapInfo::LoadDefaultInformations()
{
	return TRUE;
	ASSERT(m_nMapKind!=MK_NOTHING);

	CGEnvVar	envVar("AFC4I_SGIS_MAP_INFO");
	CString		basePath;
	CString		colorName;
	CString		featureName;
	CString		cvgName;
	CString		revertName;
	CString		revTableName;

	CGEnvVar	mapVar;	// basepath, libname을 세팅하기 위한 환경변수(육도 및 공도에 대해서만 유용)
//	CGDBListDef listdef;

	basePath = envVar.GetValue();
	if(basePath.IsEmpty()) 
	{
		// 환경변수 ~를 찾을 수 없습니다.
		//CGMessenger::SetMessage(IDS_WGP_0017, CGMessenger::ToDialog, "AFC4I_SGIS_MAP_INFO"     );
		basePath = "M:\\data";
	}
	//basePath += "\\Info\\";

	// 상황도 서버접속 불량
//	if(!CSgisUtil::ConnectServers(SV_SGIS)) {
//			CGMessenger::SetMessage(IDS_EGN_0001, CGMessenger::ToDialog, ""     );
//		return FALSE;
//	}

	// file extension : cit : color information table
	switch(m_nMapKind) 
	{
	case MK_VECTOR_LAND :
		switch(m_msMapScale) 
		{
		case MS_1_25000:
			mapVar.SetEnvVar("MAP_ARMY25K");
			// set libname
			m_strLibName = CString("lib3");

			colorName = basePath + "\\army25k.cit";
			featureName = basePath + "\\army25kft.sft";
			cvgName = basePath + "\\army25kcvg.sft";
			revertName = basePath + "\\army25krt.sft";
			revTableName = basePath + "\\army25krta.sft";
			break;	
		case MS_1_50000:
			// set basepath
			mapVar.SetEnvVar("MAP_ARMY50K");
			// set libname
			m_strLibName = CString("lib3");

			colorName = basePath + "\\army50k.cit";
			featureName = basePath + "\\army50kft.sft";
			cvgName = basePath + "\\army50kcvg.sft";
			revertName = basePath + "\\army50krt.sft";
			revTableName = basePath + "\\army50krta.sft";
			break;
		case MS_1_100000:
			// set basepath
			mapVar.SetEnvVar("MAP_ARMY100K");
			// set libname
			m_strLibName = CString("lib3");

			colorName = basePath + "\\army100k.cit";
			featureName = basePath + "\\army100kft.sft";
			cvgName = basePath + "\\army100kcvg.sft";
			revertName = basePath + "\\army100krt.sft";
			revTableName = basePath + "\\army100krta.sft";
			break;
		case MS_1_250000:
			// set basepath
			mapVar.SetEnvVar("MAP_ARMY250K");
			// set libname
			m_strLibName = CString("klib");

			colorName = basePath + "\\army250k.cit";
			featureName = basePath + "\\army250kft.sft";
			cvgName = basePath + "\\army250kcvg.sft";
			revertName = basePath + "\\army250krt.sft";
			revTableName = basePath + "\\army250krta.sft";
			break;
		case MS_1_500000:
			// set basepath
			mapVar.SetEnvVar("MAP_ARMY500K");
			// set libname
			m_strLibName = CString("klib");

			colorName = basePath + "\\army500k.cit";
			featureName = basePath + "\\army500kft.sft";
			cvgName = basePath + "\\army500kcvg.sft";
			revertName = basePath + "\\army500krt.sft";
			revTableName = basePath + "\\army500krta.sft";
			break;
		case MS_1_1000000:
			// set basepath
			mapVar.SetEnvVar("MAP_ARMY1000K");
			// set libname
			m_strLibName = CString("klib");

			colorName = basePath + "\\army1000k.cit";
			featureName = basePath + "\\army1000kft.sft";
			cvgName = basePath + "\\army1000kcvg.sft";
			revertName = basePath + "\\army1000krt.sft";
			revTableName = basePath + "\\army1000krta.sft";
			break;
		case MS_1_2000000:
			// set basepath
			mapVar.SetEnvVar("MAP_AIR2000K");
			// set libname
			m_strLibName = CString("klib");

			colorName = basePath + "\\air2000k.cit";
			featureName = basePath + "\\air2000kft.sft";
			cvgName = basePath + "\\air2000kcvg.sft";
			revertName = basePath + "\\air2000krt.sft";
			revTableName = basePath + "\\air2000krta.sft";
			break;


		}
//		m_strBasePath += listdef.GetBasePath((int) m_nMapKind, (int) m_msMapScale);
//		m_strBasePath = mapVar.GetValue();
		SetBasePath(mapVar.GetValue());
/*
		//m_strBasePath += listdef.GetBasePath((int) m_nMapKind, (int) m_msMapScale);
//		m_strBasePath = mapVar.GetValue();
		SetBasePath(mapVar.GetValue());
		*/
		break;
	case MK_VECTOR_AIR :
		//mapVar.SetEnvVar("MAP_HOME");
		switch(m_msMapScale) 
		{
		case MS_1_250000:
			mapVar.SetEnvVar("MAP_AIR250K");
			// set libname
			m_strLibName = CString("klib");

			colorName = basePath + "\\air250k.cit";
			featureName = basePath + "\\air250kft.sft";
			cvgName = basePath + "\\air250kcvg.sft";
			revertName = basePath + "\\air250krt.sft";
			revTableName = basePath + "\\air250krta.sft";
			break;
		case MS_1_500000:
			// set basepath
			mapVar.SetEnvVar("MAP_AIR500K");
			// set libname
			m_strLibName = CString("klib");

			colorName = basePath + "\\air500k.cit";
			featureName = basePath + "\\air500kft.sft";
			cvgName = basePath + "\\air500kcvg.sft";
			revertName = basePath + "\\air500krt.sft";
			revTableName = basePath + "\\air500krta.sft";
			break;
		case MS_1_1000000:
			// set basepath
			mapVar.SetEnvVar("MAP_AIR1000K");
			// set libname
			m_strLibName = CString("klib");

			colorName = basePath + "\\air1000k.cit";
			featureName = basePath + "\\air1000kft.sft";
			cvgName = basePath + "\\air1000kcvg.sft";
			revertName = basePath + "\\air1000krt.sft";
			revTableName = basePath + "\\air1000krta.sft";
			break;
		case MS_1_2000000:
			// set basepath
			mapVar.SetEnvVar("MAP_AIR2000K");
			// set libname
			m_strLibName = CString("klib");

			colorName = basePath + "\\air2000k.cit";
			featureName = basePath + "\\air2000kft.sft";
			cvgName = basePath + "\\air2000kcvg.sft";
			revertName = basePath + "\\air2000krt.sft";
			revTableName = basePath + "\\air2000krta.sft";
			break;
		}
		//m_strBasePath += listdef.GetBasePath((int) m_nMapKind, (int) m_msMapScale);
//		m_strBasePath = mapVar.GetValue();
		SetBasePath(mapVar.GetValue());
		break;
	case MK_VECTOR_SEA :
	/*	switch( m_msMapScale ) {
		case MS_1_100000:	// 연근해
			colorName = basePath + "\\DncA0108170.cit";
			featureName = basePath + "\\DncA0108170ft.sft";
			cvgName = basePath + "\\DncA0108170Cvg.sft";
			revertName = basePath + "\\DncA0108170rt.sft";
			revTableName = basePath + "\\DncA0108170rta.sft";
			break;
		case MS_1_50000:	// 항만
			colorName = basePath + "\\DncH0108280.cit";
			featureName = basePath + "\\DncH0108280ft.sft";
			cvgName = basePath + "\\DncH0108280cvg.sft";
			revertName = basePath + "\\DncH0108280rt.sft";
			revTableName = basePath + "\\DncH0108280rta.sft";
			break;
		case MS_1_250000:	// 연안
			colorName = basePath + "\\DncCOA01.cit";
			featureName = basePath + "\\DncCOA01ft.sft";
			cvgName = basePath + "\\DncCOA01Cvg.sft";
			revertName = basePath + "\\DncCOA01rt.sft";
			revTableName = basePath + "\\DncCOA01rta.sft";
			break;
		case MS_1_500000:	// 일반
			colorName = basePath + "\\DncGEN01.cit";
			featureName = basePath + "\\DncGEN01ft.sft";
			cvgName = basePath + "\\DncGEN01Cvg.sft";
			revertName = basePath + "\\DncGEN01rt.sft";
			revTableName = basePath + "\\DncGEN01rta.sft";
			break; 
		default: */
//			CSgisUtil::DisconnectServers(SV_SGIS);
			return TRUE;
		//}
	//	break;
//	case MK_VECTOR_PORT :
//		break;
	default :
//		CSgisUtil::DisconnectServers(SV_SGIS);
		return FALSE;
	}

	if(!m_Color.Load(colorName)) 
	{
		m_bColorModified = FALSE;
//		CSgisUtil::DisconnectServers(SV_SGIS);
		return FALSE;
	}
	if(!m_Feature.Load(featureName)) 
	{
		m_bFeatureModified = FALSE;
//		CSgisUtil::DisconnectServers(SV_SGIS);
		return FALSE;
	}
	if(!m_Cvg.Load(cvgName))
	{
		m_bCvgModified = FALSE;
//		CSgisUtil::DisconnectServers(SV_SGIS);
		return FALSE;
	}
	if(!m_Revert.Load(revertName))
	{
		m_bRevModified = FALSE;
//		CSgisUtil::DisconnectServers(SV_SGIS);
		return FALSE;
	}
	if(!m_Revertable.Load(revTableName))
	{
		m_bRevTableModified = FALSE;
//		CSgisUtil::DisconnectServers(SV_SGIS);
		return FALSE;
	}

//	CSgisUtil::DisconnectServers(SV_SGIS);
	return TRUE;
}
// MapInfo가 기존에 세팅되어 있는지를 알아본다
BOOL CGMapInfo::IsMapExist(EGMapKind mapKind, EGMapScale mapScale)
{
	POSITION pos = m_MapInfoList.GetHeadPosition();

	while (pos)
	{
		CGMapSetInfo *Info = (CGMapSetInfo *) m_MapInfoList.GetNext(pos);
		if(!Info) return FALSE;
		// 지도종류와 축척이 기존에 세팅되어있다면
		if((mapKind == Info->m_nMapKind) && (mapScale == Info->m_msMapScale)) 
			return TRUE;
	}
	return FALSE;
}

// 지도가 현재 리스트에 없다면 현재 CGMapInfo에 있는 내용을 List에 추가한다.
void CGMapInfo::InsertMapInfoList(EGMapKind mapKind, EGMapScale mapScale)
{
	if (!IsDrawed()) 
	{
		return;
	}

	POSITION pos = m_MapInfoList.GetHeadPosition();
	while (pos)
	{
		CGMapSetInfo *pInfo = (CGMapSetInfo *) m_MapInfoList.GetNext(pos);
		if((mapKind == pInfo->m_nMapKind) && (mapScale == pInfo->m_msMapScale)) {	// 존재함 
			pInfo->m_Color = m_Color;	// 색상과 피쳐세팅만 바꾼다
			pInfo->m_Feature = m_Feature;
			pInfo->m_Cvg = m_Cvg;
			pInfo->m_Revert = m_Revert;
			pInfo->m_Revertable = m_Revertable;
			return;		
		}
	}
	CGMapSetInfo *newInfo = new CGMapSetInfo;
	newInfo->m_nMapKind = m_nMapKind;
	newInfo->m_msMapScale = m_msMapScale;
	newInfo->m_strBasePath = m_strBasePath;
	newInfo->m_strLibName = m_strLibName;
	newInfo->m_Color = m_Color;
	newInfo->m_Feature = m_Feature;
	newInfo->m_Cvg = m_Cvg;
	newInfo->m_Revert = m_Revert;
	newInfo->m_Revertable = m_Revertable;
	m_MapInfoList.AddTail(newInfo);
}

// 새로변경한 축척은 기존에 한번 도시된 지도 세팅이다.
void CGMapInfo::ReplaceMapInfo(EGMapKind mapKind, EGMapScale mapScale) 
{
	POSITION pos = m_MapInfoList.GetHeadPosition();

	while (pos) 
	{
		CGMapSetInfo *Info = (CGMapSetInfo *) m_MapInfoList.GetNext(pos);
		// 지도종류와 축척이 기존에 세팅되어있다면
		if((mapKind == Info->m_nMapKind) && (mapScale == Info->m_msMapScale))
		{
			m_nMapKind = Info->m_nMapKind;
			m_msMapScale = Info->m_msMapScale;
			m_strBasePath = Info->m_strBasePath;
			m_strLibName = Info->m_strLibName;
			m_Color = Info->m_Color;
			m_Feature = Info->m_Feature;
			m_Cvg = Info->m_Cvg;
			m_Revert = Info->m_Revert;
			m_Revertable = Info->m_Revertable;	
			break;
		}
	}
}


// 지도종류별 축척에 자료가 있는지 없는지의 여부를 판단
//	return value 
//		1 : 지도종류내에 해당축척 지도자료가 있다.
//		0 : 지도종류내에 해당축척 지도자료가 없다.
//		-1 : 지도종류 자체가 없다.
int CGMapInfo::IsHaveMapData()
{
	return IsHaveMapData( m_nMapKind, m_msMapScale );
}

int CGMapInfo::IsHaveMapData( EGMapKind nMapKind, EGMapScale nMapScale )
{
	//////////	지도종류에 해당하는 기본 파일경로 및 축척에 해당하는 디렉토리명을 얻는다.
	CString		strBasePath;	// 기본 파일경로
	CString		strScaleName = _T("");	// 지도축척에 해당하는 디렉토리
	CGEnvVar	env;
	//CGDBListDef	listdef;

	switch( nMapKind )
	{
		case	MK_VECTOR_LAND :	// 벡터 육도
//			env.SetEnvVar("MAP_HOME");
			switch(nMapScale) {
			case MS_1_25000:
				env.SetEnvVar("MAP_ARMY25K");
				break;
			case MS_1_50000:
				env.SetEnvVar("MAP_ARMY50K");
				break;
			case MS_1_100000:
				env.SetEnvVar("MAP_ARMY100K");
				break;
			case MS_1_250000:
				env.SetEnvVar("MAP_ARMY250K");
				break;
			case MS_1_500000:
				env.SetEnvVar("MAP_ARMY500K");
				break;
			case MS_1_1000000:
				env.SetEnvVar("MAP_ARMY1000K");
				break;
			}
			//strBasePath += listdef.GetBasePath((int)nMapKind, (int)nMapScale);
			strBasePath = env.GetValue();
			break;
		case MK_VECTOR_AIR:
			//env.SetEnvVar("MAP_HOME");
			switch(nMapScale) {
			case MS_1_250000:
				env.SetEnvVar("MAP_AIR250K");
				break;
			case MS_1_500000:
				env.SetEnvVar("MAP_AIR500K");
				break;
			case MS_1_1000000:
				env.SetEnvVar("MAP_AIR1000K");
				break;
			case MS_1_2000000:
				env.SetEnvVar("MAP_AIR2000K");
				break;
			}
			//strBasePath += listdef.GetBasePath((int)nMapKind, (int)nMapScale);
			strBasePath = env.GetValue();
			break;	
		case	MK_VECTOR_SEA :		// 벡터 해도
			env.SetEnvVar("MAP_HOME");
			strBasePath = env.GetValue();
			strBasePath += "\\DNC01";
			break;
	}

	CString path;

	// vector
		path = strBasePath;
	

	CFileFind	ff;
	BOOL find = ff.FindFile(path);
	if(find != 0) 
	{
		return 1;
	}
	else 
	{
		return 0;
	}
}

int CGMapInfo::IsHaveMapData( CString path )
{
	//////////	설정된 경로로 DB를 읽어서 지도자료의 유무 확인
/*	int		nRet;
	char	currPath[_MAX_PATH];
	char	dataPath[_MAX_PATH];

	_getdcwd( _getdrive(), currPath, _MAX_PATH );
	int		currDrive = currPath[0] - 'A' + 1;

	_chdir( path );
	if( _getdcwd( _getdrive(), dataPath, _MAX_PATH ) == NULL ) nRet = -1;	// -1 : 디렉토리 자체가 없다.
	else nRet = 1;

	_chdrive( currDrive );
	_chdir( currPath );

	return nRet;
	*/
	CFileFind	ff;
	BOOL find = ff.FindFile(path);
	if(find != 0) return 1;
	else return 0;
}


//	지도축척에 해당하는 디렉토리명을 설정한다
// 이 함수는 벡터에서 쓰이면 안된다.(라이브러리가 변할수 있다)
CString	CGMapInfo::GetScaleName( EGMapKind nMapKind, EGMapScale nMapScale )
{
	CString		strScaleName;

	// 지도축척 디렉토리명 Setting
	switch(nMapScale)
	{
		case -1 :	// MS_NOTHING
			strScaleName = _T("");
			break;
		case 0 :	// MS_1_25000
			strScaleName = CString("Air25k");
			break;
		case 1 :	// Ms_1_50000
			strScaleName = CString("Air50k");
			break;
		case 2 :	// MS_1_100000
			strScaleName = CString("Air100k");
			break;
		case 3 :	// MS_1_250000
			strScaleName = CString("Air250k");
			break;
		case 4 :	// MS_1_500000
			strScaleName = CString("Air500k");
			break;
		case 5 :	// MS_1_1000000
			strScaleName = CString("Air1000k");
			break;
		case 6 :	// MS_1_2000000
			strScaleName = CString("Air2000k");
			break;
	}

	return strScaleName;
}

CString CGMapInfo::GetScaleName(int nMapScale)
{
	CString	scale[8] = {
			"1:2만5천",
			"1:5만",
			"1:10만",
			"1:25만",
			"1:50만",
			"1:100만",
			"1:200만",
			"1:3100만",
	};

	if( nMapScale == -1 )  
	{
		CString str;
		str.Format("1:%d", m_nEncScale);
		return str;
	}
	else
		return scale[nMapScale];}

CString CGMapInfo::GetMapKindName(int nMapKind)
{
	CString mapKind[13] = {
			"벡터 육도",
			"벡터 공도",
			"벡터 해도",			
			"3차원 지도"
	};
	return mapKind[nMapKind];
}


BOOL  CGMapInfo::SetSelObjCodeList( WORD *  pCodeList , int CodeNum)
{
	int ni;
	for(ni = 0; ni <  CodeNum ; ni ++)
	{
		mp_ItemCode[ni] = pCodeList[ni];
	}
	mp_ItemCode[ni] = 0 ;
	m_nSelCodeNum = CodeNum;
	return TRUE;
}

WORD *  CGMapInfo::GetSelObjCodeList( )
{
	return  mp_ItemCode ;
}

BOOL  CGMapInfo::SetCheckedItemList( char *  pCheckList )
{
	for( int ni = 0; ni < 226  ; ni ++)
	{
		m_CheckedItemList[ni] = pCheckList[ni];
	}

	return  TRUE;
}
char *   CGMapInfo::GetCheckedItemList( )
{
	return  m_CheckedItemList;
}


void CGMapInfo::SetSencRectInfo( CString  str)
{
		m_SencRectInfoList.AddTail(str);
}

void CGMapInfo::SetSencRect( CGRectGP  rct)
{
		m_SencRectList.AddTail(rct);
}

void CGMapInfo::SetSencScaleInfo( int  scale)
{
		m_SencScaleList.AddTail( scale );
}


CGRectGP CGMapInfo::GetAlignRect()
{
	return m_AlignRect;
}

void CGMapInfo::SetAlignRect(CGRectGP rectgp)
{
	m_AlignRect = rectgp;
}

void CGMapInfo::SetRecovery(BOOL bIs)
{
	m_bSimple = bIs;
}

BOOL CGMapInfo::GetRecovery()
{
	return m_bSimple;	// 단순도시(TRUE), 복원도시(FALSE)
}

BOOL CGMapInfo::GetTerrainCheck()
{
	return 	m_bTerrainChk;	// 지형중첩 설정
}

void CGMapInfo::SetTerrainCheck(BOOL bTerrain)
{
	m_bTerrainChk = bTerrain;
}

EGMapScale CGMapInfo::GetMapScale()
{
	return m_msMapScale;
};


void CGMapInfo::SetMapScale(EGMapScale mapScale)
{
	m_msMapScale = mapScale;
};


long CGMapInfo::GetMapScaleValue()
{
	EGMapKind	mapKind = GetMapKind();
	EGMapScale	mapScale = GetMapScale();
	long		scaleValue=100000;	// 전술 기본지도 축척

	switch (mapScale) 
	{
	case MS_NOTHING:
		scaleValue = GetEncScale();
		break;
	case MS_1_25000:
		scaleValue = 25000;
		break;
	case MS_1_50000:	
		scaleValue = 50000;
		break;
	case MS_1_100000:	
		scaleValue = 100000;
		break;
	case MS_1_250000:	
		scaleValue = 250000;
		break;
	case MS_1_500000:
		scaleValue = 500000;
		break;
	case MS_1_1000000:
		scaleValue = 1000000;
		break;
	case MS_1_2000000:
		scaleValue = 2000000;
		break;
	}

	return scaleValue;
}


void CGMapInfo::SetMapScaleValue(long nScale)
{
	EGMapKind	mapKind = GetMapKind();
	EGMapScale	mapScale = GetMapScale();

	if (mapScale==MS_NOTHING) 
	{
//		SetEncScaleValue(nScale);
		return;
	}

	mapScale = MS_1_100000;	// 전술 기본지도 축척
	if (nScale<=25000)
	{
		mapScale = MS_1_25000;
	}
	else if (nScale<=50000) {
			mapScale = MS_1_50000;
	}
	else if (nScale<=100000) {
			mapScale = MS_1_100000;
	}
	else if (nScale<=250000) {
			mapScale = MS_1_250000;
	}
	else if (nScale<=500000) {
			mapScale = MS_1_500000;
	}
	else if (nScale<=1000000) {
			mapScale = MS_1_1000000;
	}
	else if (nScale<=2000000) {
			mapScale = MS_1_2000000;
	}

	SetMapScale(mapScale);
}

