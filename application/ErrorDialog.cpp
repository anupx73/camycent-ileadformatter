// ErrorDialog.cpp : implementation file
//

#include "stdafx.h"
#include "iFormater.h"
#include "ErrorDialog.h"
#include "iFormaterConvertDlg.h"

// CErrorDialog dialog
CRect rcErrorTextArea(72, 42, 360, 110);
CRect rcErrorTextTitle(10, 6, 200, 21);
CRect rcButtonArea(289, 111, 372, 131);
extern CiFormaterConvertDlg *g_pConvertDlg;

IMPLEMENT_DYNAMIC(CErrorDialog, CDialog)

CErrorDialog::CErrorDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CErrorDialog::IDD, pParent)
{
  m_cstrErrorText = _T("");
  m_cstrTitleText = _T("");
}

CErrorDialog::~CErrorDialog()
{
}

void CErrorDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CErrorDialog, CDialog)
  ON_WM_PAINT()
  ON_WM_LBUTTONDOWN()
  ON_WM_LBUTTONUP()
END_MESSAGE_MAP()


// CErrorDialog message handlers
BOOL CErrorDialog::OnInitDialog()
{
  CDialog::OnInitDialog();

	BITMAP bm = {0};
	m_BmpImage[0].LoadBitmap(IDB_BITMAP11);
	m_BmpImage[0].GetBitmap(&bm);
	m_SizBmpImage[0] = CSize(bm.bmWidth, bm.bmHeight);

	//resiz winodow
	SetWindowPos(NULL, 0, 0, bm.bmWidth, bm.bmHeight, SWP_NOMOVE);
  CenterWindow(g_pConvertDlg->GetSafeOwner());

  return TRUE;
}

void CErrorDialog::OnPaint()
{
  CPaintDC dc(this); // device context for painting

	CDC pdc;
	pdc.CreateCompatibleDC(&dc);
	pdc.SelectObject(&m_BmpImage[0]);
	dc.BitBlt(0, 0, m_SizBmpImage[0].cx, m_SizBmpImage[0].cy, &pdc, 0, 0, SRCCOPY);

  CFont newFont;
	newFont.CreateFont(14,						 // nHeight
		0,                        		 // nWidth
		0,                        		 // nEscapement
		0,                        		 // nOrientation
		FW_NORMAL,										 // nWeight
		FALSE,                    		 // bItalic
		FALSE,                    		 // bUnderline
		0,                        		 // cStrikeOut
		ANSI_CHARSET,             		 // nCharSet
		OUT_DEFAULT_PRECIS,       		 // nOutPrecision
		CLIP_DEFAULT_PRECIS,      		 // nClipPrecision
		DEFAULT_QUALITY,          		 // nQuality
		DEFAULT_PITCH | FF_DONTCARE,   // nPitchAndFamily
		_T("Serif"));

  dc.SelectObject(&newFont);
  dc.SetBkMode(TRANSPARENT);
  dc.SetTextColor(RGB(192, 192, 192));
  dc.DrawText(m_cstrTitleText, m_cstrTitleText.GetLength(), rcErrorTextTitle, DT_LEFT);

  dc.SetTextColor(RGB(255, 255, 255));
  dc.DrawText(m_cstrErrorText, m_cstrErrorText.GetLength(), rcErrorTextArea, DT_LEFT);
}

void CErrorDialog::OnLButtonDown(UINT nFlags, CPoint point)
{
	//top area to move window
  if((point.x >= 0 && point.x <= m_SizBmpImage[0].cx) && (point.y >= 0 && point.y <= 22))
	{
		PostMessage(WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(point.x, point.y));
	}
}

void CErrorDialog::OnLButtonUp(UINT nFlags, CPoint point)
{
	//CString cstr;
	//cstr.Format(_T("X: %d, Y: %d"), point.x, point.y);
	//AfxMessageBox(cstr);

  if((point.x >= rcButtonArea.left && point.x <= rcButtonArea.right) && 
     (point.y >= rcButtonArea.top && point.y <= rcButtonArea.bottom))
  {
    CloseErrorDialog();
  }

  CDialog::OnLButtonDown(nFlags, point);
}

void CErrorDialog::UpdateErrorText(CString cstrText, bool bShowAbout)
{
  g_pConvertDlg->SetErrorDialogVisible(true);
  if(bShowAbout == false)
  {
    m_cstrTitleText = _T("iLFormater - Error");
    int iLen = cstrText.GetLength();
    for(int i=0; i<iLen; i++)
    {
      if(i == 60 || i == 120 || i == 180)
        cstrText.Insert(i, _T("\r\n"));
    }
   
    PlaySound((LPCSTR)SND_ALIAS_SYSTEMEXCLAMATION, NULL, SND_ALIAS_ID | SND_ASYNC);
  }
  else
  {
    m_cstrTitleText = _T("iLFormater - About");
  }
  m_cstrErrorText = cstrText;
  ShowWindow(TRUE);
  Invalidate();
  UpdateWindow();
  SetForegroundWindow();
}

LRESULT CErrorDialog::OnNcHitTest(UINT uParam, LONG lParam)
{
	int xPos = LOWORD(lParam);
	int yPos = HIWORD(lParam);
	UINT nHitTest = CDialog::OnNcHitTest(CSize(xPos, yPos));

	return (nHitTest == HTCLIENT) ? HTCAPTION : nHitTest;
}

BOOL CErrorDialog::PreTranslateMessage(MSG* pMsg)
{
  if(pMsg->message == WM_KEYDOWN || pMsg->message == WM_KEYUP)
  {
    if(pMsg->wParam == VK_RETURN)
    {
      CloseErrorDialog();
      return TRUE;
    }
    else if(pMsg->wParam == VK_ESCAPE)
    {
      return TRUE;
    }
  }
  else if(pMsg->message == WM_SYSKEYDOWN  && pMsg->wParam == VK_F4)
  {
	  return TRUE;
  }

  return CDialog::PreTranslateMessage(pMsg);
}

void CErrorDialog::CloseErrorDialog()
{
  g_pConvertDlg->SetErrorDialogVisible(false);
  ShowWindow(FALSE);
}
