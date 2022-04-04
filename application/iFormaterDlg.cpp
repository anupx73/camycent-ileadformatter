// iFormaterDlg.cpp : implementation file
//

#include "stdafx.h"
#include "iFormater.h"
#include "iFormaterDlg.h"
#include "iFormaterConvertDlg.h"
#include "..\common\Registry.h"
#include "..\common\Logger.h"
#include "md5.h"
#include "CryptString.h"

#define ENCRYPT_PASS _T("tail&29")
#define UWM_AUTH_COMPLETED WM_USER + 100

CRect rcLoginStat(30, 290, 250, 322);
CRect rcLoginButton(25, 326, 252, 378);
CRect rcVer(200, 398, 273, 422);
CRect rcForgotPass(32, 379, 116, 394);
CRect rcLowerArea(20, 139, 258, 381);
CRect rcEditBox1(29, 151, 29+221, 151+35);  //only left, top used for plotting image
CRect rcEditBox2(29, 218, 29+221, 218+35);  //only left, top used for plotting image
CRect rcUsernameText(52, 136, 120, 154);
CRect rcPassText(52, 202, 120, 220);  //18
CRect rcRememPass(48, 260, 48+120, 278);

CiFormaterDlg *g_pLoginDlg = NULL;

CiFormaterDlg::CiFormaterDlg(CWnd* pParent /*=NULL*/)
: CDialog(CiFormaterDlg::IDD, pParent)
{
  m_bLoginAreaPressed = false;
  m_eLoginErr = ERROR_NOERROR;
  m_cstrGIFLoader = _T("");
  m_bProcessingLogin = false;
  m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
  m_bHoverOnForgotPass = false;
}

CiFormaterDlg::~CiFormaterDlg()
{
  m_UerPassFont.DeleteObject();
  m_LoginFont.DeleteObject();
  m_ForgotPassFont.DeleteObject();
  m_ForgotPassUFont.DeleteObject();
  //m_VerFont.DeleteObject();
}

void CiFormaterDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_EDIT1, m_Edit);
  DDX_Control(pDX, IDC_EDIT2, m_Edit2);
  DDX_Control(pDX, IDC_LOGINING, m_LoggingIn);
  DDX_Control(pDX, IDC_CHECK_REMEM, m_CheckBoxRemem);
}

BEGIN_MESSAGE_MAP(CiFormaterDlg, CDialog)
  ON_WM_PAINT()
  ON_WM_LBUTTONDOWN()
  ON_WM_LBUTTONUP()
  ON_WM_MOUSEMOVE()
  ON_WM_SETCURSOR()
  ON_MESSAGE(UWM_AUTH_COMPLETED, OnDBAuthComplete)
  ON_EN_CHANGE(IDC_EDIT1, OnChangeUsername)
  ON_EN_CHANGE(IDC_EDIT2, OnChangePassword)
END_MESSAGE_MAP()


// CiFormaterDlg message handlers

BOOL CiFormaterDlg::OnInitDialog()
{
  CDialog::OnInitDialog();

  g_Logger.GetLoggingPath();

  SetIcon(m_hIcon, TRUE);			// Set big icon
  SetIcon(m_hIcon, FALSE);		// Set small icon

  CreateRequiredFonts();

  //Run updater
  CWinThread *pUpdateThread = AfxBeginThread(UpdateCheckerThread, NULL, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
  if(pUpdateThread != NULL)
    pUpdateThread->ResumeThread();

  //Loading gif file create
  TCHAR tempPath[MAX_PATH] = _T("");
  GetTempPath(MAX_PATH, tempPath);
  _tcscat_s(tempPath, _countof(tempPath), _T("loader.gif"));
  if(LoadResFile(IDR_RCDATA1, tempPath) == true)
    m_cstrGIFLoader = tempPath;

  g_pLoginDlg = this;

  BITMAP bm = {0};
  //Back img
  m_BmpImg[0].LoadBitmapA(IDB_BITMAP3);
  m_BmpImg[0].GetBitmap(&bm);
  m_SizBmpImg[0] = CSize(bm.bmWidth, bm.bmHeight);
  SetWindowPos(NULL, 0, 0, bm.bmWidth, bm.bmHeight, SWP_NOMOVE);

  //Normal img
  m_BmpImg[1].LoadBitmapA(IDB_BITMAP1);
  m_BmpImg[1].GetBitmap(&bm);
  m_SizBmpImg[1] = CSize(bm.bmWidth, bm.bmHeight);

  //Pressed img
  m_BmpImg[2].LoadBitmapA(IDB_BITMAP2);
  m_BmpImg[2].GetBitmap(&bm);
  m_SizBmpImg[2] = CSize(bm.bmWidth, bm.bmHeight);

  //Edit box enabled img
  m_BmpImg[3].LoadBitmapA(IDB_BITMAP9);
  m_BmpImg[3].GetBitmap(&bm);
  m_SizBmpImg[3] = CSize(bm.bmWidth, bm.bmHeight);

  //Edit box disabled img
  m_BmpImg[4].LoadBitmapA(IDB_BITMAP10);
  m_BmpImg[4].GetBitmap(&bm);
  m_SizBmpImg[4] = CSize(bm.bmWidth, bm.bmHeight);

  SetWindowText(_T("iLFormater"));
  ModifyStyle(0, WS_SYSMENU | WS_MINIMIZEBOX);

  g_Logger.LogEvent("Loading screen initiated", 0);

  ProcessAutoLogin();

  return TRUE;  // return TRUE  unless you set the focus to a control
}

void CiFormaterDlg::OnPaint()
{
  CPaintDC dc(this);

  CString cstrText = _T("");
  CDC pdc;
  pdc.CreateCompatibleDC(&dc);
  CBitmap *oldBmp = pdc.SelectObject(&m_BmpImg[0]);
  dc.BitBlt(0, 0, m_SizBmpImg[0].cx, m_SizBmpImg[0].cy, &pdc, 0, 0, SRCCOPY);
  dc.SelectObject(&oldBmp);

  dc.SetBkMode(TRANSPARENT);
  dc.SelectObject(&m_UerPassFont);

  //username, password
  dc.SetTextColor(RGB(255, 255, 255));
  cstrText = _T("Username");
  dc.DrawText(cstrText, cstrText.GetLength(), rcUsernameText, DT_LEFT);
  cstrText = _T("Password");
  dc.DrawText(cstrText, cstrText.GetLength(), rcPassText, DT_LEFT);

  dc.SetTextColor(RGB(192, 192, 192));
  //error messages
  if(m_eLoginErr == ERROR_BLANK)
  {
    cstrText = _T("Username / Password field is blank");
    CRect rcTemp = rcLoginStat;
    rcTemp.top += 4;
    rcTemp.bottom -= 12;
    //dc.FillSolidRect(rcLoginStat, RGB(255, 255, 255));
    dc.DrawText(cstrText, cstrText.GetLength(), rcTemp, DT_CENTER);
  }
  else if(m_eLoginErr == ERROR_INVALID_USER_PASS)
  {
    cstrText = _T("Invalid Username / Password");
    CRect rcTemp = rcLoginStat;
    rcTemp.top += 4;
    rcTemp.bottom -= 12;
    dc.DrawText(cstrText, cstrText.GetLength(), rcTemp, DT_CENTER);
  }
  else if(m_eLoginErr == ERROR_ACC_SUSPENDED)
  {
    cstrText = _T("Please check your email\r\nto activate your Account");
    dc.DrawText(cstrText, cstrText.GetLength(), rcLoginStat, DT_CENTER);
  }
  else if(m_eLoginErr == ERROR_ACC_DEACTIVATED)
  {
    cstrText = _T("Account deactivated, Please pay your \r\ndues to activate your account");
    dc.DrawText(cstrText, cstrText.GetLength(), rcLoginStat, DT_CENTER);
  }
  else if(m_eLoginErr == ERROR_CONNECTIVITY)
  {
    cstrText = _T("Connection problem, Please check \r\nyour Internet connectivity");
    dc.DrawText(cstrText, cstrText.GetLength(), rcLoginStat, DT_CENTER);
  }

  //remember pass
  dc.SetTextColor(RGB(192, 192, 192));
  dc.SelectObject(&m_ForgotPassFont);
  cstrText = _T("Remember Password");
  dc.DrawText(cstrText, cstrText.GetLength(), rcRememPass, DT_LEFT);

  //forgot pass
  dc.SetTextColor(RGB(192, 192, 192));
  (m_bHoverOnForgotPass == true) ? dc.SelectObject(&m_ForgotPassUFont) : dc.SelectObject(&m_ForgotPassFont);
  cstrText = _T("Forgot Password");
  //dc.FillSolidRect(rcForgotPass, RGB(0, 120, 102));
  dc.DrawText(cstrText, cstrText.GetLength(), rcForgotPass, DT_LEFT);

  //version font
  //dc.SelectObject(&m_VerFont);
  //dc.SetTextColor(RGB(147, 147, 147));
  //cstrText = _T("v");
  //cstrText.Append(APP_VER);
  //dc.DrawText(cstrText, cstrText.GetLength(), rcVer, DT_RIGHT);

  //Login font
  dc.SelectObject(&m_LoginFont);
  dc.SetTextColor(RGB(255, 255, 255));

  //Normal / Button Up state
  if(m_bLoginAreaPressed == false)
  {
    CDC pLogindc;
    pLogindc.CreateCompatibleDC(&dc);
    pLogindc.SelectObject(&m_BmpImg[1]);
    dc.BitBlt(rcLoginButton.left, rcLoginButton.top, m_SizBmpImg[1].cx, m_SizBmpImg[1].cy, &pLogindc, 0, 0, SRCCOPY);
  }
  //Down state
  else
  {
    CDC pLoginPressdc;
    pLoginPressdc.CreateCompatibleDC(&dc);
    pLoginPressdc.SelectObject(&m_BmpImg[2]);
    dc.BitBlt(rcLoginButton.left, rcLoginButton.top, m_SizBmpImg[2].cx, m_SizBmpImg[2].cy, &pLoginPressdc, 0, 0, SRCCOPY);
  }

  CDC pEditBoxdc;
  pEditBoxdc.CreateCompatibleDC(&dc);

  if(m_bProcessingLogin == false)
  {
    pEditBoxdc.SelectObject(&m_BmpImg[3]);
    dc.BitBlt(rcEditBox1.left, rcEditBox1.top, m_SizBmpImg[3].cx, m_SizBmpImg[3].cy, &pEditBoxdc, 0, 0, SRCCOPY);
    dc.BitBlt(rcEditBox2.left, rcEditBox2.top, m_SizBmpImg[3].cx, m_SizBmpImg[3].cy, &pEditBoxdc, 0, 0, SRCCOPY);

    cstrText = _T("Login");
  }
  else
  {
    pEditBoxdc.SelectObject(&m_BmpImg[4]);
    dc.BitBlt(rcEditBox1.left, rcEditBox1.top, m_SizBmpImg[4].cx, m_SizBmpImg[4].cy, &pEditBoxdc, 0, 0, SRCCOPY);
    dc.BitBlt(rcEditBox2.left, rcEditBox2.top, m_SizBmpImg[4].cx, m_SizBmpImg[4].cy, &pEditBoxdc, 0, 0, SRCCOPY);

    dc.SetTextColor(RGB(192, 192, 192));
    cstrText = _T("Logging in..");
  }

  dc.DrawText(cstrText, cstrText.GetLength(), CRect(30, 343, 250, 363), DT_CENTER);

  //CDialog::OnPaint();
}

void CiFormaterDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
  //CString cstr;
  //cstr.Format(_T("X: %d, Y: %d"), point.x, point.y);
  //AfxMessageBox(cstr);

  //Login button area
  if((point.x >= rcLoginButton.left && point.x <= rcLoginButton.right) && (point.y >= rcLoginButton.top && point.y <= rcLoginButton.bottom) && (m_bProcessingLogin == false))
  {
    m_bLoginAreaPressed = true;
    InvalidateRect(rcLoginButton);
    InvalidateRect(rcLoginStat);
    UpdateWindow();
  }

  //Min area
  else if((point.x >= 240 && point.x <= 259) && (point.y >= 2 && point.y <= 13))
  {
    ShowWindow(SW_MINIMIZE);
  }

  //Close area
  else if((point.x >= 260 && point.x <= 279) && (point.y >= 2 && point.y <= 13))
  {
    DestroyWindow();
    PostQuitMessage(0);
    return;
  }

  //top area for dragging
  else if((point.x >= 0 && point.x <= 236) && (point.y >= 0 && point.y <= 16))
  {
    PostMessage(WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(point.x, point.y));
  }

  CDialog::OnLButtonDown(nFlags, point);
}

void CiFormaterDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
  //Login button area
  if((point.x >= rcLoginButton.left && point.x <= rcLoginButton.right) && (point.y >= rcLoginButton.top && point.y <= rcLoginButton.bottom) && (m_bProcessingLogin == false))
  {
    m_bLoginAreaPressed = false;
    InvalidateRect(rcLoginButton);
    InvalidateRect(rcLoginStat);
    UpdateWindow();
    ProcessLogin();
  }
  else if((point.x >= rcForgotPass.left && point.x <= rcForgotPass.right) && (point.y >= rcForgotPass.top && point.y <= rcForgotPass.bottom))
  {
    ShellExecute(NULL, _T("open"), PRODUCT_URL, 0, 0, SW_SHOW);
  }
  else if((point.x >= (rcRememPass.left - 4) && point.x <= rcRememPass.right) && (point.y >= rcRememPass.top && point.y <= rcRememPass.bottom))
  {
    if(m_CheckBoxRemem.GetCheck() == BST_CHECKED)
      m_CheckBoxRemem.SetCheck(BST_UNCHECKED);
    else
      m_CheckBoxRemem.SetCheck(BST_CHECKED);
  }

  CDialog::OnLButtonUp(nFlags, point);
}

BOOL CiFormaterDlg::PreTranslateMessage(MSG *pMsg)
{
  if((pMsg->message == WM_KEYDOWN) || (pMsg->message == WM_KEYUP))
  {
    switch(pMsg->wParam)
    {
    case VK_ESCAPE:
      {
        return TRUE;
      }

    case VK_RETURN:
      {
        if(pMsg->message == WM_KEYUP)
        {
          if(m_bProcessingLogin == false)
          {
            m_bLoginAreaPressed = false;
            InvalidateRect(rcLoginButton);
            InvalidateRect(rcLoginStat);
            UpdateWindow();

            ProcessLogin();
          }
        }
        else if(pMsg->message == WM_KEYDOWN)
        {
          if(m_bProcessingLogin == false)
          {
            m_bLoginAreaPressed = true;
            InvalidateRect(rcLoginButton);
            InvalidateRect(rcLoginStat);
            UpdateWindow();
          }
        }
      }
      return TRUE;

    default:
      return CDialog::PreTranslateMessage(pMsg);
    }
  }
  else
  {
    return CDialog::PreTranslateMessage(pMsg);
  }
}


void CiFormaterDlg::ProcessLogin(bool bAutoLogin)
{
  if(bAutoLogin == false)
  {
    m_Edit.GetWindowText(m_cstrUser);
    m_Edit2.GetWindowText(m_cstrPass);
  }

  if(m_CheckBoxRemem.GetCheck() == BST_CHECKED)
  {
    CCryptString oCrypt;

    TCHAR szUser[32] = _T("");
    TCHAR szUserEncrypted[32] = _T("");
    _stprintf_s(szUser, sizeof(szUser), _T("%s"), m_cstrUser);
    oCrypt.EncryptString(szUser, szUserEncrypted, ENCRYPT_PASS);

    TCHAR szPass[32] = _T("");
    TCHAR szPassEncrypted[32] = _T("");
    sprintf_s(szPass, sizeof(szPass), _T("%s"), m_cstrPass);
    oCrypt.EncryptString(szPass, szPassEncrypted, ENCRYPT_PASS);

    //CString cstr;
    //cstr.Format(_T("Encrypted User: %s, Pass: %s"), szUserEncrypted, szPassEncrypted);
    //AfxMessageBox(cstr);
    CRegistryOperation::SetRegValue(HKEY_CURRENT_USER, APP_REGPATH, REG_VAL_AUTOLOGIN, _T("TRUE"));
    CRegistryOperation::SetRegValue(HKEY_CURRENT_USER, APP_REGPATH, REG_VAL_USER, szUserEncrypted);
    CRegistryOperation::SetRegValue(HKEY_CURRENT_USER, APP_REGPATH, REG_VAL_PASS, szPassEncrypted);
  }

  if(m_cstrUser.IsEmpty() || m_cstrPass.IsEmpty())
  {
    m_eLoginErr = ERROR_BLANK;
    InvalidateRect(rcLoginStat);
    UpdateWindow();
  }
  else
  {
#ifdef DEMO_VERSION
    ValidateDemoUser(m_cstrUser, m_cstrPass);
#else if
    ValidateLicensedUser(m_cstrUser, m_cstrPass);
#endif
  }
}

UINT LicensedUserValidationThread(LPVOID lParam)
{
  if(g_pLoginDlg != NULL)
    g_pLoginDlg->LicensedUserValidationThreadFunction();

  return 0;
}

void CiFormaterDlg::LicensedUserValidationThreadFunction()
{
  CT2CA szuser(m_cstrUser);
  CT2CA szpass(m_cstrPass);

  std::string user(szuser);
  std::string pass(szpass);

  // Converting Password to MD5
  char *cstr = new char[pass.length() + 1];
  strcpy(cstr, pass.c_str());
  MD5 md5 ;
  std::string md_pass = md5.digestString(cstr);

  delete [] cstr;

  // Sending it to DB TO CHECK IT !

  MySqlConn db = MySqlConn();
  if(db.initDBConnection())
  {
    int iRet = db.auth(&db , user, md_pass);

    switch(iRet)
    {
    case 0:
      //Authenticated

      g_pLoginDlg->m_vCountry = db.FetchAllCountryURL(&db , user);
      /* Check if the Request was Succesful */
      if(g_pLoginDlg->m_vCountry.at(0) == "NULL")
      {
        ::PostMessage(this->m_hWnd, UWM_AUTH_COMPLETED, 4, 0);
      }
      else
      {
        g_pLoginDlg->m_vBal = db.FetchAllBalance(&db , user);
        if(g_pLoginDlg->m_vBal.at(0) == "NULL")
        {
          ::PostMessage(this->m_hWnd, UWM_AUTH_COMPLETED, 4, 0);
        }
        else
        {
          ::PostMessage(this->m_hWnd, UWM_AUTH_COMPLETED, 0, 0);
        }
      }


      break;

    case 1:
      //Invalid username / pass
      ::PostMessage(this->m_hWnd, UWM_AUTH_COMPLETED, 1, 0);
      break;

    case 2:
      //Account Inactive. Email Not Verified
      ::PostMessage(this->m_hWnd, UWM_AUTH_COMPLETED, 2, 0);
      break;

    case 3:
      //Account Deactived. Please pay your dues to activate your account.
      ::PostMessage(this->m_hWnd, UWM_AUTH_COMPLETED, 3, 0);
      break;

    case 4:
      //Internet Connectivity Problem
      ::PostMessage(this->m_hWnd, UWM_AUTH_COMPLETED, 4, 0);
      break;

    default:
      break;
    }

    db.CloseDBConnection();
  }
  else
  {
    ::PostMessage(this->m_hWnd, UWM_AUTH_COMPLETED, 4, 0);
  }
}

LRESULT CiFormaterDlg::OnDBAuthComplete(WPARAM wParam, LPARAM lParam)
{
  m_bProcessingLogin = false;
  m_LoggingIn.UnLoad();
  m_Edit.EnableWindow(TRUE);
  m_Edit2.EnableWindow(TRUE);

  switch(wParam)
  {
  case 0:
    {
      OpenConverterDialog();
    }
    break;

  case 1:
  case 2:
  case 3:
  case 4:
    {
      if(wParam == 1)
        m_eLoginErr = ERROR_INVALID_USER_PASS;
      else if(wParam == 2)
        m_eLoginErr = ERROR_ACC_SUSPENDED;
      else if(wParam == 3)
        m_eLoginErr = ERROR_ACC_DEACTIVATED;
      else if(wParam == 4)
        m_eLoginErr = ERROR_CONNECTIVITY;

      //InvalidateRect(rcLoginButton);
      //InvalidateRect(rcLoginStat);
      InvalidateRect(rcLowerArea);
      UpdateWindow();
    }
    break;

  default:
    break;
  }

  return 0;
}

bool CiFormaterDlg::LoadResFile(int ResID, LPCTSTR ResFileName)
{
  bool bRet = false;

  HRSRC hRes = ::FindResource(NULL, MAKEINTRESOURCE(ResID), RT_RCDATA);

  HGLOBAL hResLoad = LoadResource(NULL, hRes);
  LPVOID lpResLock = ::LockResource(hResLoad);
  DWORD dwSizeRes = ::SizeofResource(NULL, hRes);
  DWORD lpNumBytes;

  HANDLE hFile = ::CreateFile(ResFileName, GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
  if(::WriteFile(hFile, lpResLock, dwSizeRes, &lpNumBytes, NULL) == TRUE)
    bRet = true;

  ::FreeResource(hRes);
  ::FreeResource(hResLoad);
  ::CloseHandle(hFile);

  return bRet;
}

void CiFormaterDlg::OnChangeUsername()
{
  if(m_eLoginErr != ERROR_NOERROR)
  {
    m_eLoginErr = ERROR_NOERROR;
    InvalidateRect(rcLoginStat);
    UpdateWindow();
  }
}

void CiFormaterDlg::OnChangePassword()
{
  if(m_eLoginErr != ERROR_NOERROR)
  {
    m_eLoginErr = ERROR_NOERROR;
    InvalidateRect(rcLoginStat);
    UpdateWindow();
  }
}

LRESULT CiFormaterDlg::OnNcHitTest(UINT uParam, LONG lParam)
{
  int xPos = LOWORD(lParam);
  int yPos = HIWORD(lParam);
  UINT nHitTest = CDialog::OnNcHitTest(CSize(xPos, yPos));

  return (nHitTest == HTCLIENT) ? HTCAPTION : nHitTest;
}

UINT UpdateCheckerThread(LPVOID lParam)
{
  UNUSED_ALWAYS(lParam);

  TCHAR chAppPath[MAX_PATH] = _T("");
  GetCurrentDirectory(MAX_PATH, chAppPath);
  CRegistryOperation::SetRegValue(HKEY_CURRENT_USER, APP_REGPATH, REG_VAL_APP_PATH, chAppPath);
  CRegistryOperation::SetRegValue(HKEY_CURRENT_USER, APP_REGPATH, REG_VAL_VER, APP_VER);
  CString cstrNewUpdater(chAppPath);	
  cstrNewUpdater += _T("\\");
  cstrNewUpdater += APP_UPDATER_NW;
  CString cstrUpdater(chAppPath);			
  cstrUpdater += _T("\\");
  cstrUpdater += APP_UPDATER;

  WIN32_FIND_DATA fndData;
  HANDLE hFile = FindFirstFile((LPCTSTR)cstrNewUpdater, &fndData);
  if(hFile != INVALID_HANDLE_VALUE)
  {
    DeleteFile(cstrUpdater);
    Sleep(400);
    MoveFile(cstrNewUpdater, cstrUpdater);
  }
  else
  {
    CString cstrUpdateStat = CRegistryOperation::GetRegValue(HKEY_CURRENT_USER, APP_REGPATH, REG_VAL_UPDATESTAT);
    if(cstrUpdateStat != _T("1"))
      ShellExecute(NULL, _T("open"), (LPCTSTR)cstrUpdater, APP_CMDLINE, chAppPath, SW_HIDE);

    g_pLoginDlg->SetForegroundWindow();
  }

  return 0;
}

void CiFormaterDlg::CreateRequiredFonts()
{
  m_UerPassFont.CreateFont(14,				 // nHeight
    0,                         // nWidth
    0,                         // nEscapement
    0,                         // nOrientation
    FW_BOLD,									 // nWeight
    FALSE,                     // bItalic
    FALSE,                     // bUnderline
    0,                         // cStrikeOut
    ANSI_CHARSET,              // nCharSet
    OUT_DEFAULT_PRECIS,        // nOutPrecision
    CLIP_DEFAULT_PRECIS,       // nClipPrecision
    DEFAULT_QUALITY,           // nQuality
    DEFAULT_PITCH | FF_DONTCARE,  // nPitchAndFamily
    _T("Serif"));

  //m_VerFont.CreateFont(12,				   // nHeight
  //  0,                         // nWidth
  //  0,                         // nEscapement
  //  0,                         // nOrientation
  //  FW_NORMAL,								 // nWeight
  //  FALSE,                     // bItalic
  //  FALSE,                     // bUnderline
  //  0,                         // cStrikeOut
  //  ANSI_CHARSET,              // nCharSet
  //  OUT_DEFAULT_PRECIS,        // nOutPrecision
  //  CLIP_DEFAULT_PRECIS,       // nClipPrecision
  //  DEFAULT_QUALITY,           // nQuality
  //  DEFAULT_PITCH | FF_DONTCARE,  // nPitchAndFamily
  //  _T("Serif"));

  m_LoginFont.CreateFont(16,				 // nHeight
    0,                         // nWidth
    0,                         // nEscapement
    0,                         // nOrientation
    FW_BOLD,									 // nWeight
    FALSE,                     // bItalic
    FALSE,                     // bUnderline
    0,                         // cStrikeOut
    ANSI_CHARSET,              // nCharSet
    OUT_DEFAULT_PRECIS,        // nOutPrecision
    CLIP_DEFAULT_PRECIS,       // nClipPrecision
    DEFAULT_QUALITY,           // nQuality
    DEFAULT_PITCH | FF_DONTCARE,  // nPitchAndFamily
    _T("Serif"));

  m_ForgotPassFont.CreateFont(14,				 // nHeight
    0,                         // nWidth
    0,                         // nEscapement
    0,                         // nOrientation
    FW_NORMAL,								 // nWeight
    FALSE,                     // bItalic
    FALSE,                     // bUnderline
    0,                         // cStrikeOut
    ANSI_CHARSET,              // nCharSet
    OUT_DEFAULT_PRECIS,        // nOutPrecision
    CLIP_DEFAULT_PRECIS,       // nClipPrecision
    DEFAULT_QUALITY,           // nQuality
    DEFAULT_PITCH | FF_DONTCARE,  // nPitchAndFamily
    _T("Serif"));

  m_ForgotPassUFont.CreateFont(14,				 // nHeight
    0,                         // nWidth
    0,                         // nEscapement
    0,                         // nOrientation
    FW_NORMAL,								 // nWeight
    FALSE,                     // bItalic
    TRUE,                      // bUnderline
    0,                         // cStrikeOut
    ANSI_CHARSET,              // nCharSet
    OUT_DEFAULT_PRECIS,        // nOutPrecision
    CLIP_DEFAULT_PRECIS,       // nClipPrecision
    DEFAULT_QUALITY,           // nQuality
    DEFAULT_PITCH | FF_DONTCARE,  // nPitchAndFamily
    _T("Serif"));
}

void CiFormaterDlg::ValidateDemoUser(CString cstrUser, CString cstrPass)
{
  if(cstrUser == DEMO_USERNAME & cstrPass == DEMO_PASSWORD)
  {
    OpenConverterDialog();
  }
  else
  {
    m_eLoginErr = ERROR_INVALID_USER_PASS;
    InvalidateRect(rcLoginStat);
    UpdateWindow();
  }
}

void CiFormaterDlg::ValidateLicensedUser(CString cstrUser, CString cstrPass)
{
  CWinThread *pThread = AfxBeginThread(LicensedUserValidationThread, NULL, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
  if(pThread != NULL)
    pThread->ResumeThread();

  m_bProcessingLogin = true;
  m_Edit.EnableWindow(FALSE);
  m_Edit2.EnableWindow(FALSE);

  if(m_LoggingIn.Load((LPCTSTR)m_cstrGIFLoader))
  {
    if(m_eLoginErr != ERROR_NOERROR)
      m_eLoginErr = ERROR_NOERROR;

    m_LoggingIn.Draw();
    //InvalidateRect(rcLoginStat);
    //InvalidateRect(rcLoginButton);
    InvalidateRect(rcLowerArea);
    UpdateWindow();
  }
}

void CiFormaterDlg::OpenConverterDialog()
{
  ShowWindow(FALSE);
  //SetWindowPos(NULL, 0, 0, 0, 0, SWP_NOACTIVATE);
  //SetWindowText(_T("iLFormater"));

  CWnd *pDeskWindow = GetDesktopWindow();
  CiFormaterConvertDlg *dlg = new CiFormaterConvertDlg();
  dlg->Create(IDD_CONVERT, pDeskWindow);
  dlg->ShowWindow(SW_SHOW);
}

void CiFormaterDlg::ProcessAutoLogin()
{
  CString cstrCheckAutoLogin = CRegistryOperation::GetRegValue(HKEY_CURRENT_USER, APP_REGPATH, REG_VAL_AUTOLOGIN);
  cstrCheckAutoLogin.MakeUpper();
  if(cstrCheckAutoLogin == _T("TRUE"))
  {
    m_CheckBoxRemem.SetCheck(BST_CHECKED);

    CCryptString oCrypt;

    CString cstrSavedUser = CRegistryOperation::GetRegValue(HKEY_CURRENT_USER, APP_REGPATH, REG_VAL_USER);
    TCHAR szUser[32] = _T("");
    sprintf_s(szUser, sizeof(szUser), _T("%s"), cstrSavedUser);
    TCHAR szUserDecrypted[32] = _T("");
    oCrypt.DecryptString(szUser, szUserDecrypted, ENCRYPT_PASS);

    CString cstrSavedPass = CRegistryOperation::GetRegValue(HKEY_CURRENT_USER, APP_REGPATH, REG_VAL_PASS);
    TCHAR szPass[32] = _T("");
    sprintf_s(szPass, sizeof(szPass), _T("%s"), cstrSavedPass);
    TCHAR szPassDecrypted[32] = _T("");
    oCrypt.DecryptString(szPass, szPassDecrypted, ENCRYPT_PASS);

    //CString cstr = _T("");
    //cstr.Format(_T("Decrypted User: %s, Pass: %s"), szUserDecrypted, szPassDecrypted);
    //AfxMessageBox(cstr);

    m_cstrUser = szUserDecrypted;
    m_cstrPass = szPassDecrypted;

    m_Edit.SetWindowText(m_cstrUser);
    m_Edit2.SetWindowText(m_cstrPass);

    ProcessLogin(true);
  }
}

void CiFormaterDlg::OnMouseMove(UINT nFlags, CPoint point)
{
  if((point.x >= rcForgotPass.left && point.x <= rcForgotPass.right) &&
     (point.y >= rcForgotPass.top && point.y <= rcForgotPass.bottom))
  {
    m_bHoverOnForgotPass = true;
    InvalidateRect(rcForgotPass);
    UpdateWindow();
  }
  else
  {
    if(m_bHoverOnForgotPass == true)
    {
      m_bHoverOnForgotPass = false;
      InvalidateRect(rcForgotPass);
      UpdateWindow();
    }
  }

  CDialog::OnMouseMove(nFlags, point);
}

BOOL CiFormaterDlg::OnSetCursor(CWnd *pWnd, UINT nHitTest, UINT message)
{
  if(m_bHoverOnForgotPass == true)
  {
    ::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_HAND));
    return TRUE;
  }

  return CDialog::OnSetCursor(pWnd, nHitTest, message);
}
