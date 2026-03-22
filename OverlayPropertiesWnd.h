// 이 MFC 샘플 소스 코드는 MFC Microsoft Office Fluent 사용자 인터페이스("Fluent UI")를 
// 사용하는 방법을 보여 주며, MFC C++ 라이브러리 소프트웨어에 포함된 
// Microsoft Foundation Classes Reference 및 관련 전자 문서에 대해 
// 추가적으로 제공되는 내용입니다.  
// Fluent UI를 복사, 사용 또는 배포하는 데 대한 사용 약관은 별도로 제공됩니다.  
// Fluent UI 라이선싱 프로그램에 대한 자세한 내용은 
// http://msdn.microsoft.com/officeui를 참조하십시오.
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

#pragma once
#include "MapXOVL/IMXOVerlayDefine.h"
#include "MapXOVL/IMXOverlayBase.h"
#include "CommonBaseClass/BasePropertyGrid.h"

// 속성 창
class COverlayPropertiesWnd : public CDockablePane
{
// 생성입니다.
public:
	COverlayPropertiesWnd();
	virtual ~COverlayPropertiesWnd();

	// Property Kind 구별자
	enum emOverlayPropertyWndKind
	{
		SHAPEPROPERTY_COMMON,				// 공통
		SHAPEPROPERTY_NAME,					// 공통 - 명칭
		SHAPEPROPERTY_DESCRIPT,				// 공통 - 설명(비고)
		SHAPEPROPERTY_EDIT,					// 도형편집
		SHAPEPROPERTY_EDIT_LINEWIDTH,		// 도형편집 - 선 굵기
		SHAPEPROPERTY_EDIT_LINECOLOR,		// 도형편집 - 선 색상
		SHAPEPROPERTY_EDIT_LINEKIND,		// 도형편집 - 선 종류
		SHAPEPROPERTY_EDIT_ROTATE,			// 도형편집 - 회전
		SHAPEPROPERTY_EDIT_FILLUSE,			// 도형편집 - 채움여부
		SHAPEPROPERTY_EDIT_FILLCOLOR,		// 도형편집 - 채움색상
		SHAPEPROPERTY_EDIT_TRANSPARENT,		// 도형편집 - 반투명
		SHAPEPROPERTY_EDIT_PATTERNUSE,		// 도형편집 - 패턴사용여부
		SHAPEPROPERTY_EDIT_PATTERNCOLOR,	// 도형편집 - 패턴색상
		SHAPEPROPERTY_EDIT_PATTERNKIND,		// 도형편집 - 패턴종류
		SHAPEPROPERTY_LINE,					// 선속성
		SHAPEPROPERTY_LINE_STRAIGHT,		// 선속성 - 직선/곡선
		SHAPEPROPERTY_LINE_STARTARROW,		// 선속성 - 시작화살표
		SHAPEPROPERTY_LINE_ENDARROW,		// 선속성 - 종료화살표
		SHAPEPROPERTY_LINE_CLOSED,			// 선속성 - 시작점끝점연결여부
		SHAPEPROPERTY_RECT,					// 사각형속성
		SHAPEPROPERTY_RECT_ROUND,			// 사각형속성 - 곡률
		SHAPEPROPERTY_NGON,					// 다각형속성
		SHAPEPROPERTY_NGON_COUNT,			// 다각형속성 - 점 수
		SHAPEPROPERTY_ARROW,				// 화살표
		SHAPEPROPERTY_ARROW_TAIL,			// 화살표 - 끝모양
		SHAPEPROPERTY_ARROW_WIDTH,			// 화살표 - 너비값
		SHAPEPROPERTY_ARROW_LINE,			// 화살표 - 선모양
		SHAPEPROPERTY_TEXT,					// 텍스트
		SHAPEPROPERTY_TEXT_SIZE,			// 텍스트 - 크기
		SHAPEPROPERTY_TEXT_COLOR,			// 텍스트 - 색상
		SHAPEPROPERTY_TEXT_KIND,			// 텍스트 - 종류
		SHAPEPROPERTY_TEXT_ALIGN,			// 텍스트 - 정렬
		SHAPEPROPERTY_TEXT_STRING,			// 텍스트 - 내용
		SHAPEPROPERTY_TEXT_WEIGHT,			// 텍스트 - 굵게
		SHAPEPROPERTY_TEXT_ITALIC,			// 텍스트 - 기울임
		SHAPEPROPERTY_TEXT_UNDERLINE,		// 텍스트 - 하단 라인
		SHAPEPROPERTY_TEXT_STRIKEOUT,		// 텍스트 - 취소선

		LAYERPROPERTY_TITLE,				// 레이어 제목
		LAYERPROPERTY_NAME,					// 레이어 명칭

		MILITARYPROP_POINT,		// 군대부호 점형
		MILITARYPROP_POINT_ADDINFO,
		MILITARYPROP_EXT,		// 군대부호 선,면형
		MILITARYPROP_EXT_ADDINFO,
		MILITARYPROP_AIR,		// 군대부호 비행회랑

		MILITARY_USER_COLOR,	// 사용자 정의 색상 사용 여부
		MILITARY_COLOR,			// 군대부호 사용자 정의 색상, 피아에 관계 없이 설정을 할 경우 
		MILITARY_SIZE,			// 군대부호 크기

		MILITARY_CODE_FULL,		// 군대부호 코드
		MILITARY_CODE_PIA,		// 군대부호 피아			- All
		MILITARY_CODE_UNIT11,	// 군대부호 추가첩보[11]
		MILITARY_CODE_UNIT12,	// 군대부호 추가접보[12]
		MILITARY_CODE_NATION,	// 군대부호 국가코드
		MILITARY_CODE_OB,		// 군대부호 전투서열

		MILITARY_ADDINFO_VIEW,	// 추가정보 도시 On/Off
		MILITARY_ADDINFO_COLOR,	// 추가정보 색상
		// 기본군대부호 
		MILITARY_ADDINFO_A,		// G 기본부호지정, 코드 선택시 자동 설정																	[부o,장o,시o,신o,기o]
		MILITARY_ADDINFO_B,		// G 부대단위, 코드 선택 시 자동 설정																		[부o,장x,시x,신x,기o]
		MILITARY_ADDINFO_C,		// T 장비수량, 장비에 대한 심볼인 경우 장비의 수량을 숫자로 표현한다. 9의 3승?							[부x,장o,시x,신x,기x]
		MILITARY_ADDINFO_D,		// G 기동부대, 식별 코드 선택 시 자동 설정																	[부o,장x,시x,신x,기o]
		MILITARY_ADDINFO_E,		// G 외형수식, 코드 선택 시 자동 설정																		[부o,장o,시o,신x,기o]
		MILITARY_ADDINFO_F,		// T 부대증감, 부대의 증감을 표현한다. (+)증가, (-)감소, (+-)증감	20크기제한								[부o,장x,시x,신x,기o]
		MILITARY_ADDINFO_G,		// T 군 및 국가구분, 부호가 나타내는 객체의 소속 군 및 국가를 표기한다. 예) 육군-미국	20 크기제한	[부o,장o,시o,신o,기o]
		MILITARY_ADDINFO_H,		// T 추가사항, 활동상황 등과 같이 객체의 상태 또는 기타 부가적인 설명을 기재한다.						[부o,장o,시o,신o,기o]
		MILITARY_ADDINFO_J1,	// T 신뢰도, 부호가 표현하는 정보에 대한 신뢰도 등급을 선택한다.										[부o,장o,시o,신o,기o]
								// A 완전 신뢰, B 신뢰도 높음, C 보통, D 신뢰도 낮음, E 신뢰할 수 없음, F 판단 불가
		MILITARY_ADDINFO_J2,	// T 정확성, 부호가 표현하는 정보에 대한 정확성 등급을 선택한다.										[부o,장o,시o,신o,기o]
								// 1 다른 출처를 통해 확인됨, 2 정확성 높음, 3 정확성 있음, 4 정확성 낮음, 5 정확성 희박, 6 확인 불가
		MILITARY_ADDINFO_K,		// T 전투력, 부대 및 화기의 전투력 또는 시설의 능력을 백분율로 설정한다.								[부o,장x,시o,신x,기o]
		MILITARY_ADDINFO_L,		// T 전자장비, 전자장비로 탐지 가능한 적 장비에 대하여 !로 표현한다.	탐지 가능						[부x,장o,시x,신o,기x]
		MILITARY_ADDINFO_M,		// T 상급제대, 상급제대 명을 숫자 또는 문자로 표현한다.		21자리										[부o,장x,시x,신x,기o]
		MILITARY_ADDINFO_N,		// T 적군표시, 적 장비임을 표현하지 위하여 적 또는 ENY를 표시한다.		표시, 미표시					[부x,장o,시x,신x,기x]
								//   장비 부호에 외형 사용을 하지 않은 경우를 위해서 인데 우리는 외형 Off를 사용하지 않으니 사실 필요 없음
		MILITARY_ADDINFO_P,		// T 피아식별 모드 및 코드, 피아식별(IFF/SIF) 모드와 식별된 코드를 표기한다.							[부o,장o,시o,신x,기o]
		MILITARY_ADDINFO_Q,		// T 이동방향, 부대, 장비 및 시설에 대한 이동 또는 예상 이동 방향을 표현한다.							[부o,장o,시o,신x,기o]
								// 
		MILITARY_ADDINFO_R,		// G 이동수단, 코드 선택 시 자동 설정 장비의 이동수단을 표시한다.										[부x,장o,시x,신x,기x]
		MILITARY_ADDINFO_R2,	// T 전자장비 이동성, 전자장비의 기동성을 표시한다.	M 기동가능, S 고정, U 불확실						[부x,장x,시x,신o,기x]		
		MILITARY_ADDINFO_S,		// G 지휘소 표시/실제위치표시  코드 선택 시 자동 설정														[부o,장o,시o,신x,기o]
								//   지휘소를 나타내거나 또는 실제 위치로부터 떨어진 객체를 표시하기 위해 사용되는 부호				
		MILITARY_ADDINFO_T,		// T 고유명칭,																				21자리				[부o,장o,시o,신o,기o]
								//   고유식별자로 부대인 경우 부대명, 시설인 경우 시설명, 장비인 경우 장비의 고유번호를 표기한다.		
		MILITARY_ADDINFO_V,		// T 장비종류, 장비명 또는 장비종류명을 기입한다. 예) 105 곡사포 -> 105 표기			24자리				[부x,장o,시x,신o,기x]
		MILITARY_ADDINFO_W,		// T 활동시각, 날짜와 시각을 나타낸다. 형식) YYMMDDHHmmSS									20자리			[부o,장o,시o,신o,기o]
		MILITARY_ADDINFO_X,		// T 고도/심도 고도 단위는 미터(m)기본으로 하며 다른 단위의 경우 단위 표기를 한다.	6자리			[부o,장o,시o,신x,기o]
		MILITARY_ADDINFO_Y,		// T 위치, 경위도를 기본으로 하며 다른 좌표일 경우 첫 문자로 좌표 형식 식별자를 표기한다.	19자리	[부o,장o,시o,신x,기o]
								//	 경위도G, UTM U, MGRS M, NE N, GEOREF F
		MILITARY_ADDINFO_Z,		// T 속도,																					8자리				[부o,장o,시o,신x,기o]
								//   이동속도 표시가 필요한 부대, 장비 및 시설의 속도를 표기한다. 기본단위는 Km/h 이며 다를 경우 단위를 표기한다.
		MILITARY_ADDINFO_AA,	// T 지휘소명, 지휘소에 해당하는 부대는 지휘소명을 표기한다.							9자리				[부o,장x,시x,신x,기o]
		MILITARY_ADDINFO_AB,	// G 가장/가상 식별부호, 추가첩보 코드 선택으로 자동 설정												[부o,장o,시o,신x,기o]
		MILITARY_ADDINFO_AC,	// G 시설, 추가첩보 코드 선택으로 자동 설정																[부o,장o,시o,신x,기o]
		MILITARY_ADDINFO_AD,	// T 기반형태, 전자전 장비의 플랫폼 형태를 ELNOT 또는 CENOT로 표기한다. (기타영역에 표시)		6자리	[부x,장x,시x,신o,기x]
		MILITARY_ADDINFO_AE,	// T 장비분해시간, 전자전 장비 분해 시간을 분 단위로 표기한다. (기타영역에 표시)				3자리	[부x,장x,시x,신o,기x]
		MILITARY_ADDINFO_AF,	// T 공통명칭, 장비에 대해 일반적으로 사용되는 이름을 표기한다.									12자리	[부x,장x,시x,신o,기x]
		MILITARY_ADDINFO_AG,	// T 보조장비식별부호, 예인소나와 같은 보조장비가 있는 장비의 경우 이를 표시한다.						[부x,장o,시x,신x,기x]

		// 작전활동부호 추가식별
		MILITARY_EXTINFO_T,		// 고유명칭(T)
		MILITARY_EXTINFO_T1,	// 고유명칭(T1)
		MILITARY_EXTINFO_H,		// 추가정보(H)
		MILITARY_EXTINFO_H1,	// 추가정보(H1)
		MILITARY_EXTINFO_H2,	// 추가정보(H2)
		MILITARY_EXTINFO_W,		// 활동시각(W)
		MILITARY_EXTINFO_W1,	// 활동시각(W1)
		MILITARY_EXTINFO_C,		// 핵용량(C)
		MILITARY_EXTINFO_V,		// 장비종류(V)
		MILITARY_EXTINFO_N,		// 적군표시(N) True, False
		MILITARY_EXTINFO_B,		// 부대단위(B)
		MILITARY_EXTINFO_R,		// 회전각(R)
		MILITARY_EXTINFO_Y,		// 위치(Y)
	};

	void AdjustLayout();

	// 구현입니다.
public:
	// 속성창 내용 초기화
	void ClearPropList();

	// 도형 정보 표기
	void SetPropertyShape(CShape* pShape, CString strOverlayGuid);
	// 레이어 정보 표기
	void SetPropertiesAppLayer(CString strName, CString strGuid);

	void SetShapePropertyLineColor(COLORREF clrLine);		// 선 색상
	void SetShapePropertyLineWidth(int nLineWidth);			// 선 굵기
	void SetShapePropertyLineStyle(int nLineStyle);			// 도형 선 모양
	void SetShapePropertyFillUse(BOOL bFillUse);			// 도형 채움 사용 여부
	void SetShapePropertyFillColor(COLORREF clrFill);		// 도형 채움 색상
	void SetShapePropertyFillTrance(BOOL bTranceUse);		// 도형 채움 반투명 여부
	void SetShapePropertyPatternUse(BOOL bPatternUse);		// 도형 패턴 사용 여부
	void SetShapePropertyPatternColr(COLORREF clrPattern);	// 도형 패턴 색상
	void SetShapePropertyPatternStyle(int nPatternStyle);	// 도형 패턴 모양
	void SetShapePropertyFontName(CString strFontName);		// 문자 글꼴
	void SetShapePropertyFontSize(int nFontSize);			// 문자 크기
	void SetShapePropertyFontColor(COLORREF clrFont);		// 문자 색상
	void SetShapePropertyFontBold(BOOL bBold);				// 문자 굵게
	void SetShapePropertyFontStrikeOut(BOOL bStrikeOut);	// 문자 취소선
	void SetShapePropertyFontItalic(BOOL bItalic);			// 문자 기울이기
	void SetShapePropertyFontUnderLine(BOOL bUnderLine);	// 문자 밑줄
	void SetShapePropertyFontAlign(FONT_ALIGN_MODE emAlignMode);	// 문자 정렬

private:
	// 투명도 속성 그룹 초기화
	void InitOverlayPropertyGroup();

	void ModifyLayerProperty(CMFCPropertyGridProperty* pLayerProperty);				// 레이어 속성
	
	void AddShapeCommonProperty(CString strShapeName, CString strShapeDescription);	/**< 명칭 및 설명 */
	void AddShapeEditProperty(CAttrShape attrShape);								/**< 도형 편집 속성 */
	void AddShapeSplineProperty(CAttrShapeSpline attrShapeSpline);					/**< 선 속성 */
	void AddShapeNgonProperty(CAttrShapeNgon attrShapeNgon);						/**< 다각형 속성 */
	void AddShapeRectProperty(CAttrShapeRect attrShapeRect);						/**< 사각형 속성 */
	void AddShapeArrowProperty(CAttrShapeArrow attrShapeArrow);						/**< 화살표 속성 */
	//void AddShapeTextProperty(CAttrShapePointText attrShapePointText);				/**< 텍스트 속성 */

#ifdef _MILITARY
	/*void AddShapeMilPointProperty(CAttrShapePoint attrShapeMilitaryPt, int nShapeKind);				// 군대부호 점형 속성
	void AddShapeMilExtProperty(CAttrShapeExt attrShapeMilitaryExt);				// 군대부호 선,면형 속성
	void AddShapeMilAirProperty(OVEREXTAIRINFO attrShapeExtAir);				// 비행회랑 속성*/
#endif

	void AddShapeMilPtAddInfoC(CMFCPropertyGridProperty* pGroupProp);	// T 장비수량
	void AddShapeMilPtAddInfoF(CMFCPropertyGridProperty* pGroupProp);	// T 부대증감
	void AddShapeMilPtAddInfoG(CMFCPropertyGridProperty* pGroupProp);	// T 군 및 국가구분
	void AddShapeMilPtAddInfoH(CMFCPropertyGridProperty* pGroupProp);	// T 추가사항
	void AddShapeMilPtAddInfoJ1(CMFCPropertyGridProperty* pGroupProp);	// T 신뢰도
	void AddShapeMilPtAddInfoJ2(CMFCPropertyGridProperty* pGroupProp);	// T 정확성
	void AddShapeMilPtAddInfoK(CMFCPropertyGridProperty* pGroupProp);	// T 전투력
	void AddShapeMilPtAddInfoL(CMFCPropertyGridProperty* pGroupProp);	// T 전자장비
	void AddShapeMilPtAddInfoM(CMFCPropertyGridProperty* pGroupProp);	// T 상급제대 
	void AddShapeMilPtAddInfoN(CMFCPropertyGridProperty* pGroupProp);	// T 적군표시
	void AddShapeMilPtAddInfoP(CMFCPropertyGridProperty* pGroupProp);	// T 피아식별 모드 및 코드
	void AddShapeMilPtAddInfoQ(CMFCPropertyGridProperty* pGroupProp);	// T 이동방향
	void AddShapeMilPtAddInfoR2(CMFCPropertyGridProperty* pGroupProp);	// T 전자장비 이동성
	void AddShapeMilPtAddInfoT(CMFCPropertyGridProperty* pGroupProp);	// T 고유명칭
	void AddShapeMilPtAddInfoV(CMFCPropertyGridProperty* pGroupProp);	// T 장비종류
	void AddShapeMilPtAddInfoW(CMFCPropertyGridProperty* pGroupProp);	// T 활동시각
	void AddShapeMilPtAddInfoX(CMFCPropertyGridProperty* pGroupProp);	// T 고도/심도
	void AddShapeMilPtAddInfoY(CMFCPropertyGridProperty* pGroupProp);	// T 위치
	void AddShapeMilPtAddInfoZ(CMFCPropertyGridProperty* pGroupProp);	// T 속도
	void AddShapeMilPtAddInfoAA(CMFCPropertyGridProperty* pGroupProp);	// T 지휘소명
	void AddShapeMilPtAddInfoAD(CMFCPropertyGridProperty* pGroupProp);	// T 기반형태
	void AddShapeMilPtAddInfoAE(CMFCPropertyGridProperty* pGroupProp);	// T 장비분해시간
	void AddShapeMilPtAddInfoAF(CMFCPropertyGridProperty* pGroupProp);	// T 공통명칭
	void AddShapeMilPtAddInfoAG(CMFCPropertyGridProperty* pGroupProp);	// T 보조장비식별부호

	void AddShapeMilExtInfoT(CMFCPropertyGridProperty* pGroupProp);		// 고유명칭(T)
	void AddShapeMilExtInfoT1(CMFCPropertyGridProperty* pGroupProp);	// 고유명칭(T1)
	void AddShapeMilExtInfoH(CMFCPropertyGridProperty* pGroupProp);		// 추가정보(H)
	void AddShapeMilExtInfoH1(CMFCPropertyGridProperty* pGroupProp);	// 추가정보(H1)
	//void AddShapeMilExtInfoH2(CMFCPropertyGridProperty* pGroupProp);	// 추가정보(H2)
	void AddShapeMilExtInfoW(CMFCPropertyGridProperty* pGroupProp);		// 활동시각(W)
	void AddShapeMilExtInfoW1(CMFCPropertyGridProperty* pGroupProp);	// 활동시각(W1)
	void AddShapeMilExtInfoC(CMFCPropertyGridProperty* pGroupProp);		// 핵용량(C)
	void AddShapeMilExtInfoV(CMFCPropertyGridProperty* pGroupProp);		// 장비종류(V)
	void AddShapeMilExtInfoN(CMFCPropertyGridProperty* pGroupProp);		// 적군표시(N) True, False
	void AddShapeMilExtInfoB(CMFCPropertyGridProperty* pGroupProp);		// 부대단위(B)
	void AddShapeMilExtInfoR(CMFCPropertyGridProperty* pGroupProp);		// 회전각(R)
	void AddShapeMilExtInfoY(CMFCPropertyGridProperty* pGroupProp);		// 위치(Y)

	void ModifyShapeCommonProperty(CMFCPropertyGridProperty* pCommonProperty);	/**< 명칭 및 설명 */
	void ModifyShapeEditProperty(CMFCPropertyGridProperty* pEditProperty);		/**< 도형 편집 속성 */
	void ModifyShapeSplineProperty(CMFCPropertyGridProperty* pSplineProperty);	/**< 선 속성 */
	void ModifyShapeNgonProperty(CMFCPropertyGridProperty* pNgonProperty);		/**< 다각형 속성 */
	void ModifyShapeRectProperty(CMFCPropertyGridProperty* pRectProperty);		/**< 사각형 속성 */
	void ModifyShapeArrowProperty(CMFCPropertyGridProperty* pArrowProperty);	/**< 화살표 속성 */
	void ModifyShapeTextProperty(CMFCPropertyGridProperty* pPointText);			/**< 텍스트 속성 */

	void ModifyShapeMilPtProperty(CMFCPropertyGridProperty* pMilPtProperty);

	void UpdateShapeEditProperties();	// 공통 속성 UI 설정
	void UpdateShapeSplineProperties();	// 라인 속성 UI 설정
	
	CMFCPropertyGridProperty*			m_pShapePropertyEditLineWidth;		// 선굵기
	CMFCPropertyGridColorProperty*	m_pShapePropertyEditLineColor;		// 선색상
	CMFCPropertyGridProperty*		m_pShapePropertyEditLineStyle;		// 선 모양
	CMFCPropertyGridProperty*		m_pShapePropertyEditFillUse;		// 채움 사용
	CMFCPropertyGridColorProperty*	m_pShapePropertyEditFillColor;		// 채움 색상
	CMFCPropertyGridProperty*		m_pShapePropertyEditFillTrance;		// 반투명 사용
	CMFCPropertyGridProperty*		m_pShapePropertyEditPatternUse;		// 패턴 사용
	CMFCPropertyGridColorProperty*	m_pShapePropertyEditPatternColor;	// 패턴 색상
	CMFCPropertyGridProperty*		m_pShapePropertyEditPatternStyle;	// 패턴 모양
	CMFCPropertyGridProperty*			m_pShapePropertyEditRotate;			// 회전값
	CMFCPropertyGridProperty*		m_pLineShapeStartArrow;				// 시작 화살표
	CMFCPropertyGridProperty*		m_pLineShapeEndArrow;				// 끝 화살표
	CMFCPropertyGridProperty*		m_pShapePropertyTextName;			// 글꼴
	CMFCPropertyGridProperty*			m_pShapePropertyTextSize;			// 텍스트 크기
	CMFCPropertyGridColorProperty*	m_pShapePropertyTextColor;			// 텍스트 색상
	CMFCPropertyGridProperty*		m_pShapePropertyTextBold;			// 굵게 사용 여부
	CMFCPropertyGridProperty*		m_pShapePropertyTextItalic;			// 기울이기
	CMFCPropertyGridProperty*		m_pShapePropertyTextUnderline;		// 밑줄
	CMFCPropertyGridProperty*		m_pShapePropertyTextStrikeOut;		// 취소선
	CMFCPropertyGridProperty*		m_pShapePropertyTextAlign;			// 정렬

	CAttrShape			m_AttrShape;			// 도형편집속성
	CAttrShapeSpline	m_AttrShapeSpline;		// 선 속성
	CAttrShapeNgon		m_AttrShapeNgon;		// 다각형 속성
	CAttrShapeRect		m_AttrShapeRect;		// 사각형 속성	
	CAttrShapeArrow		m_AttrShapeArrow;		// 화살표 속성	
	//CAttrShapePointText m_AttrShapePointText;	// 텍스트 속성

#ifdef _MILITARY
	CAttrShapePoint		m_AttrShapeMilitaryPt;	// 군대부호 점형 속성
	CAttrShapeExt		m_AttrShapeMilitaryExt;	// 군대부호 선,면형 속성
#endif

	CString m_strShapeExtModifier;					/**< 군대부호 선 면형 추가정보 사용 정보 */
	CString m_strShapePointModifier;				/**< 군대부호 점형 추가정보 사용 정보 */

	//OVEREXTAIRINFO m_pAttrShapeExtAir;				/**< 비행회랑 추가정보 */

	CString m_strShapeDescription;	// 설명/비고			
	CString m_strShapeName;			// 투명도 명칭
	CString m_strOverLayerGuid;		// 투명도 식별 Guid
	CString m_strShapeGuid;			// 객제 식별 Guid
	int		m_nShapeKind;			// 객체 종류
	BOOL	m_bUseEditFillProperty; // 선속성만 사용 여부

// 특성입니다.
public:
	void SetVSDotNetLook(BOOL bSet)
	{
		m_wndPropList.SetVSDotNetLook(bSet);
		m_wndPropList.SetGroupNameFullWidth(bSet);
	}

protected:
	CFont m_fntPropList;
	CMFCToolBar m_wndToolBar;
	CMFCPropertyGridCtrl m_wndPropList;

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnExpandAllProperties();
	afx_msg void OnUpdateExpandAllProperties(CCmdUI* pCmdUI);
	afx_msg void OnSortProperties();
	afx_msg void OnUpdateSortProperties(CCmdUI* pCmdUI);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	LRESULT OnPropertyChanged( WPARAM wparam, LPARAM lparam);

	DECLARE_MESSAGE_MAP()

	void InitPropList();
	void SetPropListFont();
};

