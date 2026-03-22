// CaptureWindow.cpp : implementation file
//

#include "pch.h"
#include "C4IMap.h"
#include "FilePath.h"
#include "CaptureWindow.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCaptureWindow

CCaptureWindow::CCaptureWindow()
{
	m_hSelectorCursor = AfxGetApp()->LoadCursor(IDC_ARROW);	// 커서 모양은 나중에 바꾸기로 유영호
	m_hLastHWND		= NULL;
	hWindow = NULL;
}

CCaptureWindow::~CCaptureWindow()
{
	::DeleteObject(m_hSelectorCursor);

	if(m_bmpCapture.m_hObject)	m_bmpCapture.DeleteObject();
}



// 특정 윈도우의 외곽에 테두리를 그리는 함수
void CCaptureWindow::InvertWindowEdge(HWND hWnd)
{
	RECT stRect;

	// 해당 윈도우의 위치를 구한다
	::GetWindowRect(hWnd, &stRect);

	// 해당 윈도우의 윈도우 DC를 구한다
	HDC hDC = ::GetWindowDC(hWnd);

	// 역상 출력 속성을 설정한다
	SetROP2(hDC, R2_NOT);

	// 시스템의 테두리 두께의 5배 굵기로 검은색 펜을 생성한다
	HPEN hPen = CreatePen(PS_INSIDEFRAME, 5 * GetSystemMetrics(SM_CXBORDER), RGB(0,0,0));

	// 생성한 펜을 설정한다
	HPEN   hOldPen   = (HPEN)SelectObject(hDC, hPen);
	// 투명 브러쉬를 설정한다
	HBRUSH hOldBrush = (HBRUSH)SelectObject(hDC, GetStockObject(NULL_BRUSH));

	// 윈도우의 외곽에 사각 테두리를 출력한다
	Rectangle(hDC, 0, 0, stRect.right - stRect.left, stRect.bottom - stRect.top);

	// 이전 객체들을 되돌린다
	SelectObject(hDC, hOldBrush);
	SelectObject(hDC, hOldPen);

	// DC를 반환한다
	::ReleaseDC(hWnd, hDC);

	// 펜을 제거한다
	DeleteObject(hPen);
}

void CCaptureWindow::Capture()
{

    BITMAPFILEHEADER          bifileHdr;
    BYTE*                     lptrData;

			// 이전에 캡춰된 비트맵을 제거한다
			if(m_bmpCapture.m_hObject)
				m_bmpCapture.DeleteObject();

			// 선택된 윈도우의 영역을 구한다
			CRect rcWindow;
			::GetWindowRect(hWindow, &rcWindow);

			// 선택된 윈도우의 DC를 구한다
			HDC hDC = ::GetWindowDC(hWindow);
			CDC dc, MemDC;

			// 메모리 DC를 생성한다
			dc.Attach(hDC);
			MemDC.CreateCompatibleDC(&dc);

			// 캡춰되는 화면을 저장할 비트맵을 생성한다
			m_bmpCapture.CreateCompatibleBitmap(&dc, rcWindow.Width(), rcWindow.Height());
			CBitmap *pOldBmp = MemDC.SelectObject(&m_bmpCapture);

			// 윈도우의 내용을 비트맵에 저장한다
			MemDC.BitBlt(0, 0, rcWindow.Width(), rcWindow.Height(),
				&dc, 0, 0, SRCCOPY);

			// 클립보드에 캡춰한 비트맵을 설정한다.

			OpenClipboard(hWindow);        // 현재 클립보드를 연다.
			EmptyClipboard();        // 현재 클립보드의 내용을 모두 제거한다.
			SetClipboardData(CF_BITMAP, m_bmpCapture.GetSafeHandle()); // Capture한 비트맵 설정
			CloseClipboard();
			MemDC.SelectObject(pOldBmp);
			m_bmpCapture.Detach();        // 비트맵 객체 제거
			OpenClipboard(hWindow);

			//클립보드로 카피된게 DIB 형식이 아니면 바로 리턴.
			if(!IsClipboardFormatAvailable(CF_DIB)) 
			{
				AfxMessageBox("Image file save Fail");
				return;
			}

			HANDLE hDIB = ::GetClipboardData(CF_DIB);

			if(!hDIB)
			{
				AfxMessageBox("Image file save Fail");
				CloseClipboard();
				return;
			}    
			CloseClipboard();

			//비트맵 헤더의 크기를 계산하고
			bifileHdr.bfType        = 0x4D42;
			bifileHdr.bfSize        = sizeof(BITMAPFILEHEADER) + GlobalSize(hDIB);
			bifileHdr.bfReserved1    = 0;
			bifileHdr.bfReserved2    = 0;
			bifileHdr.bfOffBits        = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

		    lptrData = (BYTE *)GlobalLock(hDIB);

			// 2. 화일 이름 만들기
			COleDateTime dtCurrent;
			dtCurrent = COleDateTime::GetCurrentTime();
			CString strFilePath;
			CString strFileName;
			CString strFileExt;
			strFileName.Format("%s%s", CFilePath::GetRoot(), FILEPATH_CAPTURE);
			strFileName += dtCurrent.Format("%Y년%m월%d일%H시%M분%S초 화면");	
			 
			CHAR	lpPath[512]={0,};
			GetCurrentDirectory(512,lpPath);
			// 3. 화면 저장 다이얼로그 열기	
			CFileDialog dlg(FALSE, "*.bmp", strFileName, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, 
				"비트맵 파일(*.bmp)|*.bmp|JPEG 파일(*.jpg;*.jpeg)|*.jpg;*.jpeg|PNG 파일(*.png)|*.png|GIF 파일|*.gif|", NULL);	
			if(dlg.DoModal() == IDOK) {
				
				// 실제 파일로 저장		
				strFileName = dlg.GetFileName();
				strFileExt	= dlg.GetFileExt();
				strFileExt.MakeUpper();
      
                CFile file;
                file.Open(dlg.GetPathName(), CFile::modeCreate | CFile::modeWrite);     
                file.Write(&bifileHdr, sizeof(BITMAPFILEHEADER));
                file.Write(lptrData, GlobalSize(hDIB));
                file.Close();       
			}
			
			SetCurrentDirectory(lpPath);

           GlobalUnlock(hDIB);
           GlobalFree(hDIB);  


		hWindow = NULL; 

}