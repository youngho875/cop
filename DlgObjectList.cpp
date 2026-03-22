// DlgObjectList.cpp: 구현 파일
//

#include "pch.h"
#include "C4IMap.h"
#include "DlgObjectList.h"
#include "afxdialogex.h"
#include "ObjectThread.h"

char* strObjectType_[] =
{
	"NULL_TYPE",
	"CURVE",
	"POINT_TYPE",
	"LINE",
	"TRIANGLE",
	"RECT_TYPE",
	"DIAMOND",
	"ELLIPSE",
	"POLYGON",
	"PIE",
	"POLYLINE",
	"ROUND_RECT",
	"TEXT",
	"PENTAGON",
	"SIX_ANGLE",
	"EIGHT_ANGLE",
	"IMAGE",
	"ARC",
	"LINKED_LINE",
	"LINKED_MULTI_LINE",
	"LINKED_CURVE",
	"LINKED_MULTI_CURVE",
	"OUTLINE"
};


// CDlgObjectList 대화 상자

IMPLEMENT_DYNAMIC(CDlgObjectList, CDialogEx)

CDlgObjectList::CDlgObjectList(CWnd* pParent /*=nullptr*/)
	: CDialogEx(CDlgObjectList::IDD, pParent)
	, m_pOverlayMain(NULL)
	, m_nSelectedLayer(0)
	, m_nSelectedItem(-1)
	, m_strSelectedObjectType(_T(""))
	, m_strSelectedObjectID(_T(""))
{

}

CDlgObjectList::~CDlgObjectList()
{
}
void CDlgObjectList::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_OVERLAY_OBJECTS, m_ObjectList);
	DDX_Text(pDX, IDC_STATIC_OVERLAY_OBJECT_SELECTED_OBJECT_TYPE, m_strSelectedObjectType);
	DDX_Text(pDX, IDC_STATIC_OVERLAY_OBJECT_SELECTED_OBJECT_ID, m_strSelectedObjectID);
}


BEGIN_MESSAGE_MAP(CDlgObjectList, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_OBJECT_MOVE_TOP, CDlgObjectList::OnBnClickedBtnObjectMoveTop)
	ON_BN_CLICKED(IDC_BTN_OBJECT_MOVE_UP, CDlgObjectList::OnBnClickedBtnObjectMoveUp)
	ON_BN_CLICKED(IDC_BTN_OBJECT_MOVE_DOWN, CDlgObjectList::OnBnClickedBtnObjectMoveDown)
	ON_BN_CLICKED(IDC_BTN_OBJECT_MOVE_BOTTOM, CDlgObjectList::OnBnClickedBtnObjectMoveBottom)
	ON_BN_CLICKED(IDC_BTN_OBJECT_SELECTED_OBJECT_DELETE, CDlgObjectList::OnBnClickedBtnObjectDeleteObject)
	ON_NOTIFY(NM_CLICK, IDC_LIST_OVERLAY_OBJECTS, CDlgObjectList::OnHdnItemclickListObjectList)
	ON_WM_SHOWWINDOW()

END_MESSAGE_MAP()


// CDlgObjectList 메시지 처리기
BOOL CDlgObjectList::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	InitColumn();
	
	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CDlgObjectList::OnBnClickedBtnObjectMoveTop()
{
	if (m_ObjectList.GetItemCount() < 1)
		return;
	if (m_nSelectedItem < 0)
		return;

	CLayerStructure* pLayer = m_pOverlayMain->GetLayer(m_nSelectedLayer);
	if (m_pOverlayMain != NULL && pLayer != NULL)
	{
		pLayer->MoveToBottom(m_ObjectList.GetItemCount() - m_nSelectedItem - 1);
		m_pOverlayMain->DrawObjects();
		m_pOverlayMain->DrawEditlayer();
		UpdateListItem();
	}
}

void CDlgObjectList::OnBnClickedBtnObjectMoveUp()
{
	if (m_ObjectList.GetItemCount() < 1)
		return;
	if (m_nSelectedItem < 0)
		return;

	CLayerStructure* pLayer = m_pOverlayMain->GetLayer(m_nSelectedLayer);
	if (m_pOverlayMain != NULL && pLayer != NULL)
	{
		pLayer->MoveToDown(m_ObjectList.GetItemCount() - m_nSelectedItem - 1);
		m_pOverlayMain->DrawObjects();
		m_pOverlayMain->DrawEditlayer();
		UpdateListItem();
	}
}

void CDlgObjectList::OnBnClickedBtnObjectMoveDown()
{
	if (m_ObjectList.GetItemCount() < 1)
		return;
	if (m_nSelectedItem < 0)
		return;

	CLayerStructure* pLayer = m_pOverlayMain->GetLayer(m_nSelectedLayer);
	if (m_pOverlayMain != NULL && pLayer != NULL)
	{
		pLayer->MoveToUp(m_ObjectList.GetItemCount() - m_nSelectedItem - 1);
		m_pOverlayMain->DrawObjects();
		m_pOverlayMain->DrawEditlayer();
		UpdateListItem();
	}
}

void CDlgObjectList::OnBnClickedBtnObjectMoveBottom()
{
	if (m_ObjectList.GetItemCount() < 1)
		return;
	if (m_nSelectedItem < 0)
		return;

	CLayerStructure* pLayer = m_pOverlayMain->GetLayer(m_nSelectedLayer);
	if (m_pOverlayMain != NULL && pLayer != NULL)
	{
		pLayer->MoveToTop(m_ObjectList.GetItemCount() - m_nSelectedItem - 1);
		m_pOverlayMain->DrawObjects();
		m_pOverlayMain->DrawEditlayer();
		UpdateListItem();
	}
}

void CDlgObjectList::OnBnClickedBtnObjectDeleteObject()
{
	if (m_ObjectList.GetItemCount() < 1)
		return;
	if (m_ObjectList.GetSelectedCount() < 1)
		return;
	if (m_nSelectedLayer < 0)
		return;
	if (m_nSelectedItem < 0)
		return;

	UpdateData(TRUE);

	UINT nObjectIndex = 0;
	nObjectIndex = m_ObjectList.GetItemData(m_nSelectedItem);

	CLayerStructure* pLayer = m_pOverlayMain->GetLayer(m_nSelectedLayer);
	if (pLayer != NULL)
	{
		pLayer->DeleteObject(nObjectIndex);
		m_pOverlayMain->DrawObjects();
		m_pOverlayMain->DrawEditlayer();
		m_strSelectedObjectType.Format(_T(""));
		m_strSelectedObjectID.Format(_T(""));
	}

	UpdateListItem();
	UpdateData(FALSE);
}

void CDlgObjectList::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	ClearList();

	if (bShow == TRUE)
	{
		UpdateListItem();
	}
	UpdateData(FALSE);
}

void CDlgObjectList::BindOverlay(COverlayMain* pOverlay)
{
	if (pOverlay != NULL)
	{
		m_pOverlayMain = pOverlay;
	}
}

void CDlgObjectList::InitColumn()
{
	int nIndex = 0;
	m_ObjectList.InsertColumn(nIndex, _T("ID"));
	m_ObjectList.SetColumnWidth(nIndex++, 80);

	m_ObjectList.InsertColumn(nIndex, _T("Layer Name"));
	m_ObjectList.SetColumnWidth(nIndex++, 200);

	m_ObjectList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
}

void CDlgObjectList::SetLayer(UINT nLayerIndex)
{
	ClearList();
	m_nSelectedLayer = nLayerIndex;
	UpdateData(FALSE);
}

void CDlgObjectList::AddRaw(OVERLAY::OBJECT* pObject)
{
	CString str;
	UINT nIndex = 0;
	str.Format(_T("%d"), pObject->nID);
	m_ObjectList.InsertItem(nIndex, str);

	str.Format(_T("%s"), strObjectType_[pObject->GetType()]);
	m_ObjectList.SetItemText(nIndex, 1, str);

	m_ObjectList.SetItemData(nIndex, pObject->nID);
}

void CDlgObjectList::ClearList()
{
	m_ObjectList.DeleteAllItems();
	m_strSelectedObjectType.Format(_T(""));
	m_strSelectedObjectID.Format(_T(""));
	m_nSelectedItem = -1;
	UpdateData(FALSE);
}

void CDlgObjectList::OnHdnItemclickListObjectList(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	m_nSelectedItem = phdr->iItem;

	*pResult = 0;

	if (m_ObjectList.GetItemCount() < 1)
		return;
	if (m_nSelectedItem < 0)
		return;

	UINT nID = 0;

	nID = m_ObjectList.GetItemData(m_nSelectedItem);
	CLayerStructure* pLayer = m_pOverlayMain->GetLayer(m_nSelectedLayer);
	if (pLayer != NULL)
	{
		OVERLAY::OBJECT* pObject;
		pObject = pLayer->get_object(nID);

		if (pObject != NULL)
		{
			m_strSelectedObjectType.Format(_T("%s"), strObjectType_[pObject->GetType()]);
			m_strSelectedObjectID.Format(_T("%d"), pObject->nID);
		}
		else
		{
			m_strSelectedObjectType.Format(_T(""));
			m_strSelectedObjectID.Format(_T(""));
		}

	}
	UpdateData(FALSE);
}

void CDlgObjectList::UpdateListItem()
{
	if (m_nSelectedLayer < 0)
		return;

	ClearList();

	CLayerStructure* pLayer = m_pOverlayMain->GetLayer(m_nSelectedLayer);
	if (pLayer != NULL)
	{
		OVERLAY::OBJECT* pObject;
		pObject = pLayer->get_first_object();
		for (INT j = 0; j < pLayer->NumberOfObjectInLayer(); j++)
		{
			if (pObject != NULL)
			{
				AddRaw(pObject);
				pObject = pLayer->get_next_object();
			}
		}
	}
}