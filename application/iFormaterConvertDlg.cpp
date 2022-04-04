// iFormaterConvertDlg.cpp : implementation file
//

#include "stdafx.h"
#include "iFormater.h"
#include "iFormaterConvertDlg.h"
#include "iFormaterDlg.h"
#include "..\common\Logger.h"
#include "..\common\Registry.h"
#include <vector>

#define UWM_CONVERT_COMPLETE WM_USER + 101
#define UWM_PAINT WM_USER + 102
#define TIMER_FORMAT_DONE 1

#define TXT_CHOOSE_COUNTRY  _T("Choose Country..")
#define TXT_CHOOSE_URL      _T("Choose URL Format..")

#define INVALID_PARAM 999

extern CiFormaterDlg *g_pLoginDlg;
extern double dBalanceLeft;
double g_dRemainingBalance = 0;

CiFormaterConvertDlg *g_pConvertDlg = NULL;

IMPLEMENT_DYNAMIC(CiFormaterConvertDlg, CDialog)

#define MOVE_WINDOW_RIGHT 590
#define MOVE_WINDOW_BOTTOM 16
//CRect rcLink(400, 124, 560, 137);
CRect rcAbout(584, 4, 618, 17);
CRect rcHelp(623, 4, 645, 17);  
CRect rcLeadsLeftCount(22, 372, 480, 385);
CRect rcStart(430, 188, 570, 280);
CRect rcCompleteIcon1(389, 148, 389+24, 148+24);
CRect rcCompleteIcon2(389, 178, 389+24, 178+24);
CRect rcCompleteIcon3(389, 206, 389+24, 206+24);
CRect rcCompleteIcon4(389, 234, 389+24, 234+24);
CRect rcCompleteIcon5(389, 263, 389+24, 263+24);
CRect rcCompleteIconSet(389, 148, 389+24, 263+24);
CRect rcFilterPanel(582, 115, 582+115, 115+244);
CRect rcFilterText(598, 142, 680, 172);
//buttons
CRect rcAddFile(294, 87, 342, 100);
CRect rcDeleteFile(397, 87, 458, 100);
CRect rcDeleteAllFiles(507, 87, 560, 100);
CRect rcSetFilter(611, 87, 660, 100);

CiFormaterConvertDlg::CiFormaterConvertDlg(CWnd* pParent /*=NULL*/)
: CDialog(CiFormaterConvertDlg::IDD, pParent)
{
	m_bProcessing = false;
	m_bLoadGettingStarted = true;
	m_iFileAddedCount = 0;
	m_csURL = _T("www.example.com");
	m_bCompleteStatusShow = false;
  m_cstrLeadsLeft = _T("");
  m_cstrSelectedCountry = TXT_CHOOSE_COUNTRY;
  m_cstrSelectedURL = TXT_CHOOSE_URL;
  m_eHoverArea = AREA_NONE;
  pErrorDlg = NULL;
  m_bErrorDialogOpened = false;
  m_bFilterPanelOpened = false;
  m_iNextTop = 0;
  m_iNextBottom = 0;
}

CiFormaterConvertDlg::~CiFormaterConvertDlg()
{
	m_BodyTxtFont.DeleteObject();
	m_LinkFont.DeleteObject();
	m_LinkUFont.DeleteObject();
	//m_InfoTxtFont.DeleteObject();
	m_BottomTxtFont.DeleteObject();

  if(pErrorDlg)
  {
    delete pErrorDlg;
    pErrorDlg = NULL;
  }
}

void CiFormaterConvertDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT3, m_Edit[0]);
	DDX_Control(pDX, IDC_EDIT4, m_Edit[1]);
	DDX_Control(pDX, IDC_EDIT5, m_Edit[2]);
	DDX_Control(pDX, IDC_EDIT6, m_Edit[3]);
	DDX_Control(pDX, IDC_EDIT7, m_Edit[4]);
	DDX_Control(pDX, IDC_EDIT8, m_Edit[5]);

	DDX_Control(pDX, IDC_CHECK1, m_Check[0]);
	DDX_Control(pDX, IDC_CHECK2, m_Check[1]);
	DDX_Control(pDX, IDC_CHECK3, m_Check[2]);
	DDX_Control(pDX, IDC_CHECK4, m_Check[3]);
	DDX_Control(pDX, IDC_CHECK5, m_Check[4]);

  DDX_Control(pDX, IDC_COMBO_COUNTRY, m_ComboCountry);
  DDX_Control(pDX, IDC_COMBO_URL, m_ComboURL);
	DDX_Control(pDX, IDC_PROCESSING, m_Processing);
}


BEGIN_MESSAGE_MAP(CiFormaterConvertDlg, CDialog)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_PAINT()
	ON_WM_TIMER()
  ON_WM_MOUSEMOVE()
  ON_WM_SETCURSOR()
  ON_WM_SYSCOMMAND()
	ON_MESSAGE(UWM_CONVERT_COMPLETE, OnConvertComplete)
	ON_CBN_SELENDOK(IDC_COMBO_COUNTRY, OnCountryComboChanged)
	ON_CBN_SELENDOK(IDC_COMBO_URL, OnURLComboChanged)
END_MESSAGE_MAP()

// CiFormaterConvertDlg message handlers

BOOL CiFormaterConvertDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	g_pConvertDlg = this;

	SetIcon(g_pLoginDlg->m_hIcon, TRUE);			// Set big icon
	SetIcon(g_pLoginDlg->m_hIcon, FALSE);		// Set small icon
	
  SetWindowText(_T("iLFormater"));
	ModifyStyle(0, WS_SYSMENU | WS_MINIMIZEBOX);

	BitmapLoadFromResource();
	ResizeControls();

	CreateRequiredFonts();

	//Output folder path set from registry
	CString cstrSavedPath = CRegistryOperation::GetRegValue(HKEY_CURRENT_USER, APP_REGPATH, REG_VAL_OUTPUT);
	if(cstrSavedPath.IsEmpty() == true)
	{
		CString cstrMyDocPath = CRegistryOperation::GetRegValue(HKEY_CURRENT_USER, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders"), _T("Personal"));
		CRegistryOperation::SetRegValue(HKEY_CURRENT_USER, APP_REGPATH, REG_VAL_OUTPUT, cstrMyDocPath);
		cstrSavedPath = cstrMyDocPath;
	}
	m_Edit[0].SetWindowText(cstrSavedPath);

	//for(std::vector<std::string>::iterator itr = g_pLoginDlg->m_vCountry.begin(); itr != g_pLoginDlg->m_vCountry.end(); itr++)
	//{
	//	CString cstr((*itr).c_str());
	//	m_cArrCountry.Add(cstr);
	//	m_ComboCountry.AddString(cstr);
	//}
	//for(std::vector<std::string>::iterator itr = g_pLoginDlg->m_vUrl.begin(); itr != g_pLoginDlg->m_vUrl.end(); itr++)
	//{
	//	CString cstr((*itr).c_str());
	//	m_cArrCountryURL.Add(cstr);
	//}
	// g_pLoginDlg->m_vUrl[0] g_pLoginDlg->m_vUrl[1]

	//m_ComboCountry.SetCurSel(0);

	//if(m_cArrCountryURL.GetCount() > 0)
	//	m_csURL = m_cArrCountryURL.GetAt(m_cArrCountryURL.GetCount()-1);

	//m_ComboCountry.InsertString(0, _T("Choose Country.."));
	//m_ComboCountry.InsertString(1, _T("Sweden"));
	//m_ComboCountry.InsertString(2, _T("USA"));
	//m_ComboCountry.InsertString(3, _T("Canada"));
	//
	//m_ComboURL.AddString(_T("Choose URL Format.."));
  
  SetComboBoxValueForCountry();	

  ReadTotalLeadsConvertedFromRegistry();

  //Error dialog creation
  pErrorDlg = new CErrorDialog();
  if(pErrorDlg != NULL)
  {
    pErrorDlg->Create(CErrorDialog::IDD, this);
    pErrorDlg->ShowWindow(FALSE);
  }

	return TRUE;
}

void CiFormaterConvertDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	//CString cstr;
	//cstr.Format(_T("X: %d, Y: %d"), point.x, point.y);
	//AfxMessageBox(cstr);

  if(m_bErrorDialogOpened == true)
  {
    PlaySound((LPCSTR)SND_ALIAS_SYSTEMDEFAULT, NULL, SND_ALIAS_ID | SND_ASYNC);
    return;
  }
  
	//top area to move window
	if((point.x >= 0 && point.x <= MOVE_WINDOW_RIGHT) && (point.y >= 0 && point.y <= MOVE_WINDOW_BOTTOM))
	{
		PostMessage(WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(point.x, point.y));
	}
}

void CiFormaterConvertDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
  if(m_bErrorDialogOpened == true)
    return;

  //About area
  if((point.x >= rcAbout.left && point.x <= rcAbout.right) && (point.y >= rcAbout.top && point.y <= rcAbout.bottom))
  {
    CString cstrVerion;
    cstrVerion.Format(_T("iLeadFormatter %s \r\nCopyright (C) 2014 Camycent Technologies \r\nWebsite: www.camycent.com \r\nMail: info@camycent.com"), APP_VER);
    pErrorDlg->UpdateErrorText(cstrVerion, true);
  }

	//Help area
	else if((point.x >= rcHelp.left && point.x <= rcHelp.right) && (point.y >= rcHelp.top && point.y <= rcHelp.bottom))
	{
    ShellExecute(NULL, _T("open"), PRODUCT_URL, _T(""), 0, SW_SHOW);
	}

	//Min area
	else if((point.x >= 654 && point.x <= 673) && (point.y >= 1 && point.y <= 15))
	{
    ShowWindow(SW_MINIMIZE);
	}

	//Close area
	else if((point.x >= 674 && point.x <= 694) && (point.y >= 1 && point.y <= 15))
	{
		::PostMessage(g_pLoginDlg->m_hWnd, WM_CLOSE, 0, 0);
		return;
	}

	//Add files
	else if((point.x >= rcAddFile.left-30 && point.x <= rcAddFile.right) && (point.y >= rcAddFile.top-10 && point.y <= rcAddFile.bottom))
	{
		if(CheckForCountryAndURL() == false)
		{
      pErrorDlg->UpdateErrorText(_T("Choose Country and URL Format before adding files !"));
			return;
		}
		ClearCompleteIcons();
		CFileDialog file(TRUE, NULL, NULL, OFN_OVERWRITEPROMPT, _T("Text Files(*.txt)|*.txt||"));
		file.DoModal();

		CString cstr = file.GetPathName();
		//only for 1st time, if no file is choosen
		if(cstr.IsEmpty() == true)// && (m_iFileAddedCount == 0))
			return;

		if(m_bLoadGettingStarted == true)
		{
			m_bLoadGettingStarted = false;
			Invalidate();
			UpdateWindow();

			if(m_Check[0].IsWindowVisible() == FALSE)
				m_Check[0].ShowWindow(TRUE);
			if(m_Check[1].IsWindowVisible() == FALSE)
				m_Check[1].ShowWindow(TRUE);
			if(m_Check[2].IsWindowVisible() == FALSE)
				m_Check[2].ShowWindow(TRUE);
			if(m_Check[3].IsWindowVisible() == FALSE)
				m_Check[3].ShowWindow(TRUE);
			if(m_Check[4].IsWindowVisible() == FALSE)
				m_Check[4].ShowWindow(TRUE);

			if(m_Edit[1].IsWindowVisible() == FALSE)
				m_Edit[1].ShowWindow(TRUE);
			if(m_Edit[2].IsWindowVisible() == FALSE)
				m_Edit[2].ShowWindow(TRUE);
			if(m_Edit[3].IsWindowVisible() == FALSE)
				m_Edit[3].ShowWindow(TRUE);
			if(m_Edit[4].IsWindowVisible() == FALSE)
				m_Edit[4].ShowWindow(TRUE);
			if(m_Edit[5].IsWindowVisible() == FALSE)
				m_Edit[5].ShowWindow(TRUE);
		}

		m_iFileAddedCount++;

		if(m_iFileAddedCount > 5)
		{
			pErrorDlg->UpdateErrorText(_T("Formating more than 5 files are not allowed at a time. Order iLFormater-Pro to do more!"));
		}
		else
		{
			CString cstrEdit1Txt, cstrEdit2Txt, cstrEdit3Txt, cstrEdit4Txt, cstrEdit5Txt;
			m_Edit[1].GetWindowText(cstrEdit1Txt);
			m_Edit[2].GetWindowText(cstrEdit2Txt);
			m_Edit[3].GetWindowText(cstrEdit3Txt);
			m_Edit[4].GetWindowText(cstrEdit4Txt);
			m_Edit[5].GetWindowText(cstrEdit5Txt);

			if((cstr != cstrEdit1Txt) && (cstr != cstrEdit2Txt) && (cstr != cstrEdit3Txt) && (cstr != cstrEdit4Txt) && (cstr != cstrEdit5Txt))
			{
				if(cstrEdit1Txt.IsEmpty())
					m_Edit[1].SetWindowText(cstr);
				else if(cstrEdit2Txt.IsEmpty())
					m_Edit[2].SetWindowText(cstr);
				else if(cstrEdit3Txt.IsEmpty())
					m_Edit[3].SetWindowText(cstr);
				else if(cstrEdit4Txt.IsEmpty())
					m_Edit[4].SetWindowText(cstr);
				else if(cstrEdit5Txt.IsEmpty())
					m_Edit[5].SetWindowText(cstr);
			}
			else
			{
				m_iFileAddedCount--;
				pErrorDlg->UpdateErrorText(_T("This file is already added"));
			}
		}
	}

	//Del files
	else if((point.x >= rcDeleteFile.left-30 && point.x <= rcDeleteFile.right) && (point.y >= rcDeleteFile.top-10 && point.y <= rcDeleteFile.bottom))
	{
		if(m_bLoadGettingStarted)
			return;

		ClearCompleteIcons();

		if((m_Check[0].GetCheck() == 0) && (m_Check[1].GetCheck() == 0) && (m_Check[2].GetCheck() == 0) && (m_Check[3].GetCheck() == 0) && (m_Check[4].GetCheck() == 0))
		{
			pErrorDlg->UpdateErrorText(_T("No file has been marked for delete. Please tick one box."));
			return;
		}

		if(m_Check[0].GetCheck() == BST_CHECKED)
		{
			m_Edit[1].SetWindowText(_T(""));
			m_Check[0].SetCheck(0);
			m_iFileAddedCount--;
		}
		if(m_Check[1].GetCheck() == BST_CHECKED)
		{
			m_Edit[2].SetWindowText(_T(""));
			m_Check[1].SetCheck(0);
			m_iFileAddedCount--;
		}
		if(m_Check[2].GetCheck() == BST_CHECKED)
		{
			m_Edit[3].SetWindowText(_T(""));
			m_Check[2].SetCheck(0);
			m_iFileAddedCount--;
		}
		if(m_Check[3].GetCheck() == BST_CHECKED)
		{
			m_Edit[4].SetWindowText(_T(""));
			m_Check[3].SetCheck(0);
			m_iFileAddedCount--;
		}
		if(m_Check[4].GetCheck() == BST_CHECKED)
		{
			m_Edit[5].SetWindowText(_T(""));
			m_Check[4].SetCheck(0);
			m_iFileAddedCount--;
		}
	}

	//Del all files
	else if((point.x >= rcDeleteAllFiles.left-30 && point.x <= rcDeleteAllFiles.right) && (point.y >= rcDeleteAllFiles.top-10 && point.y <= rcDeleteAllFiles.bottom))
	{
		if(m_bLoadGettingStarted)
			return;
		ClearCompleteIcons();

		m_Edit[1].SetWindowText(_T(""));
		m_Edit[2].SetWindowText(_T(""));
		m_Edit[3].SetWindowText(_T(""));
		m_Edit[4].SetWindowText(_T(""));
		m_Edit[5].SetWindowText(_T(""));
		m_iFileAddedCount = 0;
	}
	
  //Set filter
	else if((point.x >= rcSetFilter.left-30 && point.x <= rcSetFilter.right) && (point.y >= rcSetFilter.top-10 && point.y <= rcSetFilter.bottom))
	{
		if(m_bLoadGettingStarted)
			return;

    ShowFilterPanel(true);
  }

	//Browse area
	else if((point.x >= 398 && point.x <= 481) && (point.y >= 328 && point.y <= 348))
	{
		ClearCompleteIcons();
		BROWSEINFO bi = {0};
		bi.lpszTitle = _T("Select Output folder");
		LPITEMIDLIST pidl = SHBrowseForFolder (&bi);
		TCHAR path[MAX_PATH] = _T("");
		if (pidl != 0)
		{
			// get the name of the folder
			SHGetPathFromIDList(pidl, path);

			// free memory used
			IMalloc *imalloc = 0;
			if (SUCCEEDED(SHGetMalloc (&imalloc)))
			{
				imalloc->Free(pidl);
				imalloc->Release();
			}
		}

		if(_tcscmp(path, _T("")) != 0)
		{
			m_Edit[0].SetWindowText(path);
			CRegistryOperation::SetRegValue(HKEY_CURRENT_USER, APP_REGPATH, REG_VAL_OUTPUT, path);
		}
	}
	//Open area
	else if((point.x >= 491 && point.x <= 579) && (point.y >= 328 && point.y <= 348))
	{
		ClearCompleteIcons();
		CString cstrPath;
		m_Edit[0].GetWindowText(cstrPath);
		ShellExecute(NULL, _T("open"), cstrPath, 0, cstrPath, SW_SHOW);
	}

	//Start area
	else if((point.x >= 448 && point.x <= 560) && (point.y >= 194 && point.y <= 244))
	{
		ClearCompleteIcons();
		if(m_bLoadGettingStarted == false)
		{
			//SetTimer(TIMER_FORMAT_DONE, 5*1000, NULL);
			if(m_bProcessing == false)
				ProcessFormat();
		}
	}
  
  //Filter panel area
  else if((point.x >= rcFilterPanel.left && point.x <= rcFilterPanel.right) && (point.y >= rcFilterPanel.top && point.y <= rcFilterPanel.bottom))
  {
    //intentionally left blank
  }

  else
  {
		if(m_bLoadGettingStarted)
			return;

    ShowFilterPanel(false);
  }

	//Link
	//else if((point.x >= rcLink.left && point.x <= rcLink.right) && (point.y >= rcLink.top && point.y <= rcLink.bottom))
	//{
	//	ClearCompleteIcons();
	//	if(m_bLoadGettingStarted == false)
	//		ShellExecute(NULL, _T("open"), m_csURL, 0, 0, SW_SHOW);
	//}
}

void CiFormaterConvertDlg::OnPaint()
{
	CPaintDC dc(this);

	CDC pdc;
	pdc.CreateCompatibleDC(&dc);
	pdc.SelectObject(&m_BmpImage[0]);
	dc.BitBlt(0, 0, m_SizBmpImage[0].cx, m_SizBmpImage[0].cy, &pdc, 0, 0, SRCCOPY);
	//dc.SelectObject(&oldBmp);

	//CBitmap *oldBmp = 
	if(m_bLoadGettingStarted == true)
	{
		pdc.SelectObject(&m_BmpImage[1]);
		dc.BitBlt(35, 140, m_SizBmpImage[1].cx, m_SizBmpImage[1].cy, &pdc, 0, 0, SRCCOPY);
		//dc.SelectObject(&oldBmp);
	}

	dc.SelectObject(&m_BodyTxtFont);
	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(RGB(255, 255, 255));

	CString cstr = _T("Add File");
	dc.DrawText(cstr, cstr.GetLength(), rcAddFile, DT_LEFT);

	(m_bLoadGettingStarted == true) ? (dc.SetTextColor(RGB(147, 147, 147))) : (dc.SetTextColor(RGB(255, 255, 255)));

	cstr = _T("Delete File");
	dc.DrawText(cstr, cstr.GetLength(), rcDeleteFile, DT_LEFT);
	cstr = _T("Delete All");
	dc.DrawText(cstr, cstr.GetLength(), rcDeleteAllFiles, DT_LEFT);
  cstr = _T("Set Filter");
  dc.DrawText(cstr, cstr.GetLength(), rcSetFilter, DT_LEFT);

	dc.SetTextColor(RGB(255, 255, 255));

	//cstr = _T("Country Format");
	//dc.DrawText(cstr, cstr.GetLength(), CRect(401, 67, 490, 80), DT_LEFT);

	cstr = _T("Output Folder");
	dc.DrawText(cstr, cstr.GetLength(), CRect(21, 309, 101, 322), DT_LEFT);

	if(m_bProcessing == true)
	{
		cstr = _T("Please wait..");
		dc.FillSolidRect(rcStart, RGB(73, 73, 73));
		dc.DrawText(cstr, cstr.GetLength(), rcStart, DT_CENTER);
	}
	else
	{
		pdc.SelectObject(&m_BmpImage[2]);
		dc.BitBlt(rcStart.left, rcStart.top, m_SizBmpImage[2].cx, m_SizBmpImage[2].cy, &pdc, 0, 0, SRCCOPY);
	}

	//cstr = APP_VER;
	//cstr = _T("v") + cstr;
	//dc.SelectObject(&m_BottomTxtFont);
	//dc.SetTextColor(RGB(147, 147, 147));
	//dc.DrawText(cstr, cstr.GetLength(), rcVersion, DT_RIGHT);

#ifdef DEMO_VERSION
	int iLeadsLeft = DEMO_LIMIT - m_iTotalLeadsConverted;
	if(iLeadsLeft <= 0)
		iLeadsLeft = 0;

	cstr.Format(_T("Demo version | Number of leads left to convert: %d"), iLeadsLeft);
	dc.SelectObject(&m_BottomTxtFont);
	dc.SetTextColor(RGB(255, 80, 0));
	dc.DrawText(cstr, cstr.GetLength(), rcLeadsLeftCount, DT_LEFT);
#else if
  if(m_cstrSelectedCountry == TXT_CHOOSE_COUNTRY)
    cstr = _T("");
  else
	  cstr.Format(_T("Leads left to convert: %s"), m_cstrLeadsLeft);
	dc.SelectObject(&m_BottomTxtFont);
	dc.SetTextColor(RGB(255, 80, 0));
  //dc.FillSolidRect(rcLeadsLeftCount, RGB());
	dc.DrawText(cstr, cstr.GetLength(), rcLeadsLeftCount, DT_LEFT);
#endif

  dc.SetTextColor(RGB(192, 192, 192));
  cstr = _T("About");
  (m_eHoverArea == AREA_ABOUT) ? dc.SelectObject(&m_LinkUFont) : dc.SelectObject(&m_LinkFont);
  //dc.FillSolidRect(rcAbout, RGB(200, 0, 0));
	dc.DrawText(cstr, cstr.GetLength(), rcAbout, DT_LEFT);

	cstr = _T("Help");
  (m_eHoverArea == AREA_HELP) ? dc.SelectObject(&m_LinkUFont) : dc.SelectObject(&m_LinkFont);
  //dc.FillSolidRect(rcHelp, RGB(0, 200, 0));
	dc.DrawText(cstr, cstr.GetLength(), rcHelp, DT_LEFT);

	if(m_bLoadGettingStarted == false)
	{
		//URL
		//cstr = m_csURL;
		//dc.DrawText(cstr, cstr.GetLength(), rcLink, DT_RIGHT);

		//Country name
		//CString cstrCountry;
		//m_ComboCountry.GetWindowText(cstrCountry);
		//if(cstrCountry.IsEmpty())
		//	cstrCountry = _T("BLANK");
		//cstr = _T("Files are compatible with ") + cstrCountry + _T(" format taken from: ");

		//dc.SelectObject(&m_InfoTxtFont);
		//dc.DrawText(cstr, cstr.GetLength(), CRect(21, 124, 380, 137), DT_LEFT);

		dc.SelectObject(&m_BodyTxtFont);
		dc.SetTextColor(RGB(255, 255, 255));

		short iY = 152;
		for(short i=1; i<6; i++)
		{
			cstr.Format(_T("%d"), i);
			cstr = _T("File ") + cstr;
			dc.DrawText(cstr, cstr.GetLength(), CRect(36, iY, 70, iY+13), DT_LEFT);
			iY += 29;
		}

		//complete status icons
		if(m_bCompleteStatusShow == true)
		{
			std::vector <int>::iterator stat = iCompleteStatus.begin();
			std::vector <int>::iterator index = iCompleteIndex.begin();
			for(stat , index ; stat != iCompleteStatus.end() , index != iCompleteIndex.end(); stat++ , index++)
			{
				switch(*index)
				{
				case 1:
					{
						if(*stat == CONVERT_SUCESS)
						{
							pdc.SelectObject(&m_BmpImage[3]);
							dc.BitBlt(rcCompleteIcon1.left, rcCompleteIcon1.top, rcCompleteIcon1.left+24, rcCompleteIcon1.top+24, &pdc, 0, 0, SRCCOPY);
						}
						else
						{
							pdc.SelectObject(&m_BmpImage[4]);
							dc.BitBlt(rcCompleteIcon1.left, rcCompleteIcon1.top, rcCompleteIcon1.left+24, rcCompleteIcon1.top+24, &pdc, 0, 0, SRCCOPY);
						}
					}

					break;
				case 2:
					{
						if(*stat == CONVERT_SUCESS)
						{
							pdc.SelectObject(&m_BmpImage[3]);
							dc.BitBlt(rcCompleteIcon2.left, rcCompleteIcon2.top, rcCompleteIcon2.left+24, rcCompleteIcon2.top+24, &pdc, 0, 0, SRCCOPY);
						}
						else
						{
							pdc.SelectObject(&m_BmpImage[4]);
							dc.BitBlt(rcCompleteIcon2.left, rcCompleteIcon2.top, rcCompleteIcon2.left+24, rcCompleteIcon2.top+24, &pdc, 0, 0, SRCCOPY);
						}
					}
					break;
				case 3:
					{
						if(*stat == CONVERT_SUCESS)
						{
							pdc.SelectObject(&m_BmpImage[3]);
							dc.BitBlt(rcCompleteIcon3.left, rcCompleteIcon3.top, rcCompleteIcon3.left+24, rcCompleteIcon3.top+24, &pdc, 0, 0, SRCCOPY);
						}
						else
						{
							pdc.SelectObject(&m_BmpImage[4]);
							dc.BitBlt(rcCompleteIcon3.left, rcCompleteIcon3.top, rcCompleteIcon3.left+24, rcCompleteIcon2.top+24, &pdc, 0, 0, SRCCOPY);
						}
					}
					break;
				case 4:
					{
						if(*stat == CONVERT_SUCESS)
						{
							pdc.SelectObject(&m_BmpImage[3]);
							dc.BitBlt(rcCompleteIcon4.left, rcCompleteIcon4.top, rcCompleteIcon4.left+24, rcCompleteIcon4.top+24, &pdc, 0, 0, SRCCOPY);
						}
						else
						{
							pdc.SelectObject(&m_BmpImage[4]);
							dc.BitBlt(rcCompleteIcon4.left, rcCompleteIcon4.top, rcCompleteIcon4.left+24, rcCompleteIcon4.top+24, &pdc, 0, 0, SRCCOPY);
						}
					}
					break;
				case 5:
					{
						if(*stat == CONVERT_SUCESS)
						{
							pdc.SelectObject(&m_BmpImage[3]);
							dc.BitBlt(rcCompleteIcon5.left, rcCompleteIcon5.top, rcCompleteIcon5.left+24, rcCompleteIcon5.top+24, &pdc, 0, 0, SRCCOPY);
						}
						else
						{
							pdc.SelectObject(&m_BmpImage[4]);
							dc.BitBlt(rcCompleteIcon5.left, rcCompleteIcon5.top, rcCompleteIcon5.left+24, rcCompleteIcon5.top+24, &pdc, 0, 0, SRCCOPY);
						}
					}
					break;
				}

			}
		}

    //draw filter panel bk img
    if(m_bFilterPanelOpened == true)
    {
			pdc.SelectObject(&m_BmpImage[5]);
			dc.BitBlt(rcFilterPanel.left, rcFilterPanel.top, rcCompleteIcon5.right, rcCompleteIcon5.bottom, &pdc, 0, 0, SRCCOPY);
	    
      dc.SelectObject(&m_LinkFont);
	    dc.SetTextColor(RGB(255, 255, 255));
	    cstr = _T("Filter leads\r\nstarting with");
      //dc.FillSolidRect(rcFilterText, RGB(200, 0, 0));
	    dc.DrawText(cstr, cstr.GetLength(), rcFilterText, DT_CENTER);
    }
	}

	CDialog::OnPaint();
}

BOOL CiFormaterConvertDlg::PreTranslateMessage(MSG *pMsg)
{
  if((pMsg->message == WM_KEYDOWN || pMsg->message == WM_KEYUP) && (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE))
	{
		return TRUE;
	}
  else if(pMsg->message == WM_SYSKEYDOWN  && pMsg->wParam == VK_F4)
  {
    ::PostMessage(g_pLoginDlg->m_hWnd, WM_CLOSE, 0, 0);
	  return TRUE;
  }

	return CDialog::PreTranslateMessage(pMsg);
}

void CiFormaterConvertDlg::ProcessFormat()
{
	if(m_carrInputFilePaths.GetCount() > 0)
		m_carrInputFilePaths.RemoveAll();

	int iBlankCount = 0;
	for(int i=1; i<6; i++)
	{
		CString cstrEdit;
		m_Edit[i].GetWindowText(cstrEdit);
		if(cstrEdit.IsEmpty() == true)
			iBlankCount++;

		m_carrInputFilePaths.Add(cstrEdit);
	}

	if((m_carrInputFilePaths.GetCount() == 0) || (iBlankCount == 5))
	{
		pErrorDlg->UpdateErrorText(_T("No files were choosen for formatting! Please add files to proceed."));
		return;
	}

	m_bProcessing = true;

	if(g_pLoginDlg != NULL)
	{
		if(m_Processing.Load(g_pLoginDlg->m_cstrGIFLoader))
		{
			m_Processing.Draw();
			m_Processing.ShowWindow(TRUE);
		}
	}

	InvalidateRect(rcStart);
	UpdateWindow();

	CWinThread *pThread = AfxBeginThread(ProcessFormatThread, NULL, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
	if(pThread != NULL)
		pThread->ResumeThread();
}

UINT ProcessFormatThread(LPVOID lParam)
{
	UNUSED_ALWAYS(lParam);
	g_Logger.LogEvent("GUI Formatting Thread Started", 0);
	if(g_pConvertDlg != NULL)
		g_pConvertDlg->ProcessFormatThreadFunction();
	g_Logger.LogEvent("GUI Formatting Thread Exit" , 0);
	return 0;
}

void CiFormaterConvertDlg::ProcessFormatThreadFunction()
{
  CString cstrCountryFormat;
  CT2CA szCountryFormat(this->m_cstrSelectedCountry);
  std::string strCountryFormat(szCountryFormat);

  CT2CA szCountryUrl(this->m_cstrSelectedURL);
  std::string strCountryUrl(szCountryUrl);

  CString cstrUser = g_pLoginDlg->m_cstrUser;
  CT2CA szUser(cstrUser);
  std::string strUser(szUser);	

  vector <std::string> file_paths;
  vector <std::string> save_paths;

  for(int i=0; i<m_carrInputFilePaths.GetCount(); i++)
  {
    CT2CA szInputFile(m_carrInputFilePaths.GetAt(i));
    std::string strInputFile(szInputFile);
    std::string strOutputFile = GenerateOutputFileName(m_carrInputFilePaths.GetAt(i));
    file_paths.push_back(strInputFile);
    save_paths.push_back(strOutputFile);
  }

  // Main Processing Starts
  if(file_paths.size() == save_paths.size())
  {
    m_bCompleteStatusShow = true;
    int status = -1;
    CFormatterProcess fp;
    std::vector<std::string>::size_type i = 0;
    MySqlConn db_conn;
    if(db_conn.initDBConnection() == true)
    {
      //Fetching Balance 
      std::string user_id = db_conn.GetUserID(&db_conn , strUser);
      std::string country_id = db_conn.GetCountryID(&db_conn , strCountryFormat);
      g_dRemainingBalance = db_conn.get_balance(&db_conn , user_id , country_id);
      if(db_conn.internet_failed != true)
      {
        if(g_dRemainingBalance > 0)
        {
			// Geting the Filter 
			std::string FilterStr = this->GetFilterState();
			std::vector <std::string> Filters = fp.GetFilterVector(FilterStr);

          for(i = 0 ; i < file_paths.size(); i++)
          {
            if(file_paths[i].compare("") == 0)
              continue;

            Sleep(2000);
            status = fp.ProcessCSV(&db_conn , strCountryFormat , strCountryUrl , strUser , file_paths[i] , save_paths[i] , Filters);
            if(db_conn.internet_failed == true)
            {
              //Internet Connection Failed during Conversion
              m_bProcessing = false;
              DeleteFileA(save_paths[i].c_str());
              ::PostMessage(this->m_hWnd, UWM_CONVERT_COMPLETE, CONVERT_FAIL_NO_INTERNET, i);
              break;
            }
            else
            {
              if(dBalanceLeft < 0)
              {
                m_bProcessing = false;
                DeleteFileA(save_paths[i].c_str());
                ::PostMessage(this->m_hWnd, UWM_CONVERT_COMPLETE, CONVERT_FAIL_INSUFFICIENT_BAL, i);
                return;
              }
              switch(status)
              {
              case 0:
                //File formatted and inserted to db
                m_bProcessing = false;
                ::PostMessage(this->m_hWnd, UWM_CONVERT_COMPLETE, CONVERT_SUCESS, i);
                break;
              case 1:
                //Invalid input file
                m_bProcessing = false;
                DeleteFileA(save_paths[i].c_str());
                ::PostMessage(this->m_hWnd, UWM_CONVERT_COMPLETE, CONVERT_FAIL_FILE_FORMAT, i);
                break;
              case 2:
                //Database failure
                m_bProcessing = false;
                DeleteFileA(save_paths[i].c_str());
                ::PostMessage(this->m_hWnd, UWM_CONVERT_COMPLETE, CONVERT_FAIL_DB_ERROR, i);
                break;
              case 3:
                //Internet Disconnected after Insertion
                m_bProcessing = false;
                DeleteFileA(save_paths[i].c_str());
                ::PostMessage(this->m_hWnd, UWM_CONVERT_COMPLETE, CONVERT_FAIL_NO_INTERNET, i);
                break;
              default:
                break;
              }
            }
          }
        }
        else
        {
          //Please Recharge
          m_bProcessing = false;
          ::PostMessage(this->m_hWnd, UWM_CONVERT_COMPLETE, CONVERT_FAIL_RECHARGE_ACC, i);
        }
      }
      else
      {
        //Internet connection failed Before the convertion started
        m_bProcessing = false;
        ::PostMessage(this->m_hWnd, UWM_CONVERT_COMPLETE, CONVERT_FAIL_NO_INTERNET, INVALID_PARAM);
      }
    }
    else
    {
      //Internet connection failed while trying to Establish DB Connection
      m_bProcessing = false;
      ::PostMessage(this->m_hWnd, UWM_CONVERT_COMPLETE, CONVERT_FAIL_NO_INTERNET, INVALID_PARAM);
    }

    db_conn.CloseDBConnection();
  }
  else
  {
    //Input and output file no are not same
    m_bProcessing = false;
    ::PostMessage(this->m_hWnd, UWM_CONVERT_COMPLETE, CONVERT_FAIL_FILE_COUNT, INVALID_PARAM);
  }
}


LRESULT CiFormaterConvertDlg::OnConvertComplete(WPARAM wParam, LPARAM lParam)
{
  switch(wParam)
  {
  case CONVERT_SUCESS:
    {
      CString cstrBalLeft;
      cstrBalLeft.Format(_T("%f"), dBalanceLeft);
      CString cstrActualBalance;
      AfxExtractSubString(cstrActualBalance, cstrBalLeft, 0, '.');
      m_cstrLeadsLeft = cstrActualBalance;

      InvalidateRect(rcLeadsLeftCount);
      UpdateWindow();
    }
    break;

  case CONVERT_FAIL_FILE_FORMAT:
    {
      pErrorDlg->UpdateErrorText(_T("Input file not compatible with selected format"));
    }
    break;
  
  case CONVERT_FAIL_DB_ERROR:
    {
      pErrorDlg->UpdateErrorText(_T("Something went wrong during Server connection. If this continues contact support team."));
    }
    break;

  case CONVERT_FAIL_NO_INTERNET:
    {
      pErrorDlg->UpdateErrorText(_T("Couldn't connect to Internet. Please check your connection"));
    }
    break;

  case CONVERT_FAIL_INSUFFICIENT_BAL:
    {
      CString cstrErrorText = _T("");
      cstrErrorText.Format(_T("Insufficient Balance. You can Convert upto %.0f  Leads"), g_dRemainingBalance);
      pErrorDlg->UpdateErrorText(cstrErrorText);
    }
    break;

  case CONVERT_FAIL_FILE_COUNT:
    {
      pErrorDlg->UpdateErrorText(_T("File creation failed. If this continues contact Support."));
    }
    break;

  case CONVERT_FAIL_RECHARGE_ACC:
    {
      pErrorDlg->UpdateErrorText(_T("Please recharge your account for the selected country."));
    }
    break;

  default:
    break;
  }

	if(m_bProcessing == false)
		m_Processing.UnLoad();
	
	int i = (int)lParam;
	int j = (int)wParam;
	iCompleteStatus.push_back(j);
	iCompleteIndex.push_back((i+1));

  if((i+1) == 1)
    InvalidateRect(rcCompleteIcon1);
  else if((i+1) == 2)
    InvalidateRect(rcCompleteIcon2);
  else if((i+1) == 3)
    InvalidateRect(rcCompleteIcon3);
  else if((i+1) == 4)
    InvalidateRect(rcCompleteIcon4);
  else if((i+1) == 5)
    InvalidateRect(rcCompleteIcon5);

  InvalidateRect(rcStart);
  UpdateWindow();

  return 0;
}

void CiFormaterConvertDlg::CreateRequiredFonts()
{
	m_BodyTxtFont.CreateFont(14,						 // nHeight
		0,                        		 // nWidth
		0,                        		 // nEscapement
		0,                        		 // nOrientation
		FW_BOLD,											 // nWeight
		FALSE,                    		 // bItalic
		FALSE,                    		 // bUnderline
		0,                        		 // cStrikeOut
		ANSI_CHARSET,             		 // nCharSet
		OUT_DEFAULT_PRECIS,       		 // nOutPrecision
		CLIP_DEFAULT_PRECIS,      		 // nClipPrecision
		DEFAULT_QUALITY,          		 // nQuality
		DEFAULT_PITCH | FF_DONTCARE,   // nPitchAndFamily
		_T("Serif"));

	//m_InfoTxtFont.CreateFont(14,		 // nHeight
	//	0,                        		 // nWidth
	//	0,                        		 // nEscapement
	//	0,                        		 // nOrientation
	//	FW_NORMAL,										 // nWeight
	//	FALSE,                    		 // bItalic
	//	FALSE,                    		 // bUnderline
	//	0,                        		 // cStrikeOut
	//	ANSI_CHARSET,             		 // nCharSet
	//	OUT_DEFAULT_PRECIS,       		 // nOutPrecision
	//	CLIP_DEFAULT_PRECIS,      		 // nClipPrecision
	//	DEFAULT_QUALITY,          		 // nQuality
	//	DEFAULT_PITCH | FF_DONTCARE,   // nPitchAndFamily
	//	_T("Serif"));

	m_LinkFont.CreateFont(14,				 // nHeight
		0,                        		 // nWidth
		0,                        		 // nEscapement
		0,                        		 // nOrientation
		FW_NORMAL,										 // nWeight
		FALSE,                    		 // bItalic
		FALSE,                     		 // bUnderline
		0,                        		 // cStrikeOut
		ANSI_CHARSET,             		 // nCharSet
		OUT_DEFAULT_PRECIS,       		 // nOutPrecision
		CLIP_DEFAULT_PRECIS,      		 // nClipPrecision
		DEFAULT_QUALITY,          		 // nQuality
		DEFAULT_PITCH | FF_DONTCARE,	 // nPitchAndFamily
		_T("Serif"));

	m_LinkUFont.CreateFont(14,				 // nHeight
		0,                        		 // nWidth
		0,                        		 // nEscapement
		0,                        		 // nOrientation
		FW_NORMAL,										 // nWeight
		FALSE,                    		 // bItalic
		TRUE,                     		 // bUnderline
		0,                        		 // cStrikeOut
		ANSI_CHARSET,             		 // nCharSet
		OUT_DEFAULT_PRECIS,       		 // nOutPrecision
		CLIP_DEFAULT_PRECIS,      		 // nClipPrecision
		DEFAULT_QUALITY,          		 // nQuality
		DEFAULT_PITCH | FF_DONTCARE,	 // nPitchAndFamily
		_T("Serif"));

	m_BottomTxtFont.CreateFont(14,	 // nHeight
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
}

void CiFormaterConvertDlg::OnTimer(UINT_PTR nIDEvent)
{
	switch(nIDEvent)
	{
	case TIMER_FORMAT_DONE:
		{
			m_Processing.UnLoad();
			m_Processing.ShowWindow(FALSE);

			m_bProcessing = false;
			InvalidateRect(rcStart);
			UpdateWindow();
		}
		break;

	default:
		break;
	}

	return CDialog::OnTimer(nIDEvent);
}

std::string CiFormaterConvertDlg::GenerateOutputFileName(CString cstrInputFile)
{
	CString cstrOutputFolder;
	m_Edit[0].GetWindowText(cstrOutputFolder);

	int iPos = cstrInputFile.ReverseFind('\\');
	CString cstrFileName = cstrInputFile.Mid(iPos+1);
	iPos = cstrFileName.ReverseFind('.');
	CString cstrOutputFileName = cstrFileName.Mid(0, iPos);

	iPos = cstrOutputFolder.ReverseFind('\\');
	if((cstrOutputFolder.GetLength()-1) != iPos)
		cstrOutputFolder += _T("\\");

	cstrOutputFileName = cstrOutputFolder + cstrOutputFileName + _T(".csv");

	CT2CA szFileName(cstrOutputFileName);
	std::string strFile(szFileName);

	return strFile;
}

void CiFormaterConvertDlg::ResizeControls()
{
	m_Edit[0].SetWindowPos(NULL, 22, 332, 355, 19, SWP_NOACTIVATE);
	m_Edit[0].SetReadOnly();
	m_Edit[0].SetBackColor(RGB(255, 255, 255));

	m_Processing.SetWindowPos(NULL, 490, 220, 16, 16, SWP_NOACTIVATE);
	m_Processing.ShowWindow(FALSE);

	//5 input file box
	m_Edit[1].SetReadOnly();
	m_Edit[1].SetWindowPos(NULL, 78, 152, 300, 18, SWP_NOACTIVATE);
	m_Edit[1].SetBackColor(RGB(255, 255, 255));
	m_Edit[1].ShowWindow(FALSE);
	m_Check[0].SetWindowPos(NULL, 21, 152, 0, 0, SWP_NOACTIVATE | SWP_NOSIZE);
	m_Check[0].ShowWindow(FALSE);

	m_Edit[2].SetReadOnly();
	m_Edit[2].SetWindowPos(NULL, 78, 181, 300, 18, SWP_NOACTIVATE);
	m_Edit[2].SetBackColor(RGB(255, 255, 255));
	m_Edit[2].ShowWindow(FALSE);
	m_Check[1].SetWindowPos(NULL, 21, 181, 0, 0, SWP_NOACTIVATE | SWP_NOSIZE);
	m_Check[1].ShowWindow(FALSE);

	m_Edit[3].SetReadOnly();
	m_Edit[3].SetWindowPos(NULL, 78, 210, 300, 17, SWP_NOACTIVATE);
	m_Edit[3].SetBackColor(RGB(255, 255, 255));
	m_Edit[3].ShowWindow(FALSE);
	m_Check[2].SetWindowPos(NULL, 21, 210, 0, 0, SWP_NOACTIVATE | SWP_NOSIZE);
	m_Check[2].ShowWindow(FALSE);

	m_Edit[4].SetReadOnly();
	m_Edit[4].SetWindowPos(NULL, 78, 238, 300, 17, SWP_NOACTIVATE);
	m_Edit[4].SetBackColor(RGB(255, 255, 255));
	m_Edit[4].ShowWindow(FALSE);
	m_Check[3].SetWindowPos(NULL, 21, 239, 0, 0, SWP_NOACTIVATE | SWP_NOSIZE);
	m_Check[3].ShowWindow(FALSE);

	m_Edit[5].SetReadOnly();
	m_Edit[5].SetWindowPos(NULL, 78, 267, 300, 17, SWP_NOACTIVATE);
	m_Edit[5].SetBackColor(RGB(255, 255, 255));
	m_Edit[5].ShowWindow(FALSE);
	m_Check[4].SetWindowPos(NULL, 21, 268, 0, 0, SWP_NOACTIVATE | SWP_NOSIZE);
	m_Check[4].ShowWindow(FALSE);

	m_ComboCountry.SetWindowPos(NULL, 24, 65, 140, 100, SWP_NOACTIVATE);
	m_ComboCountry.SendMessage(EM_SETREADONLY, TRUE, 0);

	m_ComboURL.SetWindowPos(NULL, 24, 89, 200, 100, SWP_NOACTIVATE);
	m_ComboURL.SendMessage(EM_SETREADONLY, TRUE, 0);

  //create filter check box
  const int iTop    = 155;
  const int iBottom = 171;

  const int iR1Left   = 594;
  const int iR1Right  = 630;

  const int iR2Left   = 650;
  const int iR2Right  = 686;

  CRect rcAreaR1(iR1Left, iTop, iR1Right, iBottom);
  CRect rcAreaR2(iR2Left, iTop, iR2Right, iBottom);
  m_iNextTop = rcAreaR1.top;
  m_iNextBottom = rcAreaR1.bottom;
  CString cstrLabel;
  for(int i=0; i<10; i++)
  {
    cstrLabel = _T("");
    cstrLabel.Format(_T("%d"), i);
    
    if(i== 5)
    {
      m_iNextTop    = iTop + 35;
      m_iNextBottom = iBottom + 35;
    }
    else
    {
      m_iNextTop    += 35;
      m_iNextBottom += 35;
    }

    m_CheckFilter[i] = new CButton;
    if(i>=0 && i<=4)
    {
      rcAreaR1.SetRect(iR1Left, m_iNextTop, iR1Right, m_iNextBottom);
      m_CheckFilter[i]->Create(cstrLabel, WS_CHILD | BS_AUTOCHECKBOX, rcAreaR1, this, 2001+i);
    }
    else
    {
      rcAreaR2.SetRect(iR2Left, m_iNextTop, iR2Right, m_iNextBottom);
      m_CheckFilter[i]->Create(cstrLabel, WS_CHILD | BS_AUTOCHECKBOX, rcAreaR2, this, 2001+i);
    }
    
    m_CheckFilter[i]->ShowWindow(FALSE);
  }
}


void CiFormaterConvertDlg::BitmapLoadFromResource()
{
	BITMAP bm = {0};
	m_BmpImage[0].LoadBitmap(IDB_BITMAP4);
	m_BmpImage[0].GetBitmap(&bm);
	m_SizBmpImage[0] = CSize(bm.bmWidth, bm.bmHeight);

	//resiz winodow
	SetWindowPos(NULL, 0, 0, bm.bmWidth, bm.bmHeight, SWP_NOMOVE);
	CenterWindow(GetDesktopWindow());
	//ModifyStyle(0, WS_SYSMENU | WS_MINIMIZEBOX);

	//getting started
	m_BmpImage[1].LoadBitmap(IDB_BITMAP5);
	m_BmpImage[1].GetBitmap(&bm);
	m_SizBmpImage[1] = CSize(bm.bmWidth, bm.bmHeight);

	//start button
	m_BmpImage[2].LoadBitmap(IDB_BITMAP6);
	m_BmpImage[2].GetBitmap(&bm);
	m_SizBmpImage[2] = CSize(bm.bmWidth, bm.bmHeight);

	//done icon
	m_BmpImage[3].LoadBitmap(IDB_BITMAP8);
	m_BmpImage[3].GetBitmap(&bm);
	m_SizBmpImage[3] = CSize(bm.bmWidth, bm.bmHeight);

	//fail icon
	m_BmpImage[4].LoadBitmap(IDB_BITMAP7);
	m_BmpImage[4].GetBitmap(&bm);
	m_SizBmpImage[4] = CSize(bm.bmWidth, bm.bmHeight);

	//filter panel img
	m_BmpImage[5].LoadBitmap(IDB_BITMAP12);
	m_BmpImage[5].GetBitmap(&bm);
	m_SizBmpImage[5] = CSize(bm.bmWidth, bm.bmHeight);
}


void CiFormaterConvertDlg::ClearCompleteIcons()
{
	if((m_bLoadGettingStarted == false) && (m_bCompleteStatusShow == true))
	{
		m_bCompleteStatusShow = false;
		InvalidateRect(rcCompleteIconSet);
		UpdateWindow();
	}
}

LRESULT CiFormaterConvertDlg::OnNcHitTest(UINT uParam, LONG lParam)
{
	int xPos = LOWORD(lParam);
	int yPos = HIWORD(lParam);
	UINT nHitTest = CDialog::OnNcHitTest(CSize(xPos, yPos));

	return (nHitTest == HTCLIENT) ? HTCAPTION : nHitTest;
}

void CiFormaterConvertDlg::WriteTotalLeadsConvertedToRegistry()
{
#ifdef DEMO_VERSION
	int iValue = ReverseNumber(m_iTotalLeadsConverted);
	CRegistryOperation::SetRegBinaryValue(HKEY_CURRENT_USER, DEMO_LIMIT_REG_PATH, DEMO_LIMIT_REG_VAL, iValue);

	m_rcToRefresh = rcLeadsLeftCount;
	::PostMessage(this->m_hWnd, UWM_PAINT, 0, 0);
#endif
}

void CiFormaterConvertDlg::ReadTotalLeadsConvertedFromRegistry()
{
#ifdef DEMO_VERSION
	int iRegValue = CRegistryOperation::GetRegBinaryValue(HKEY_CURRENT_USER, DEMO_LIMIT_REG_PATH, DEMO_LIMIT_REG_VAL);
	m_iTotalLeadsConverted = ReverseNumber(iRegValue);

	InvalidateRect(rcLeadsLeftCount);
	UpdateWindow();
#endif
}

int CiFormaterConvertDlg::ReverseNumber(int iNumber)
{
	int iRemainder = 0, iReversedNumber = 0;

	while(iNumber != 0)
	{
		iRemainder = iNumber % 10;
		iNumber = iNumber / 10;
		iReversedNumber = (iReversedNumber * 10) + iRemainder;
	}

	return iReversedNumber;
}

bool CiFormaterConvertDlg::CheckForCountryAndURL()
{
  if(m_ComboCountry.GetCount() > 0)
	  m_ComboCountry.GetLBText(m_ComboCountry.GetCurSel(), m_cstrSelectedCountry);
  if(m_ComboURL.GetCount() > 0)
	  m_ComboURL.GetLBText(m_ComboURL.GetCurSel(), m_cstrSelectedURL);

	if(m_cstrSelectedCountry == _T("") || m_cstrSelectedCountry == TXT_CHOOSE_COUNTRY)
		return false;

	if(m_cstrSelectedURL == _T("") || m_cstrSelectedURL == TXT_CHOOSE_URL)
		return false;

	return true;
}

void CiFormaterConvertDlg::OnCountryComboChanged()
{
	m_ComboCountry.GetLBText(m_ComboCountry.GetCurSel(), m_cstrSelectedCountry);
  if(m_cstrSelectedCountry == TXT_CHOOSE_COUNTRY)
  {
    m_ComboURL.ResetContent();
    m_ComboURL.AddString(TXT_CHOOSE_URL);
    m_ComboURL.SetCurSel(0);
    return;
  }

  m_ComboURL.ResetContent();
  if((g_pLoginDlg != NULL) && (g_pLoginDlg->m_vCountry.size() > 0))
  {
    int iNoOfURL = 0;
		for(std::vector<std::string>::iterator itr = g_pLoginDlg->m_vCountry.begin(); itr != g_pLoginDlg->m_vCountry.end(); itr++)
		{
			CString cstr((*itr).c_str());
      for(int i=0; i<cstr.GetLength(); i++)
      {
        if(cstr.GetAt(i) == '|')
          iNoOfURL++;
      }
      iNoOfURL -= 1;
		  CString cstrCountry;
		  AfxExtractSubString(cstrCountry, cstr, 0, '|');
      if(m_cstrSelectedCountry == cstrCountry)
      {
        for(int j=1; j<=iNoOfURL; j++)
        {
		      CString cstrURL;
		      AfxExtractSubString(cstrURL, cstr, j, '|');
          m_ComboURL.AddString(cstrURL);
        }
      }
		}
    m_ComboURL.SetCurSel(0);
    UpdateLeadsBalance();
  }
}

void CiFormaterConvertDlg::OnURLComboChanged()
{
	m_ComboURL.GetLBText(m_ComboURL.GetCurSel(), m_cstrSelectedURL);
}

void CiFormaterConvertDlg::SetComboBoxValueForCountry()
{
  if((g_pLoginDlg != NULL) && (g_pLoginDlg->m_vCountry.size() > 0))	
	{
    m_ComboCountry.AddString(TXT_CHOOSE_COUNTRY);
    m_ComboURL.AddString(TXT_CHOOSE_URL);
		for(std::vector<std::string>::iterator itr = g_pLoginDlg->m_vCountry.begin(); itr != g_pLoginDlg->m_vCountry.end(); itr++)
		{
			CString cstr((*itr).c_str());
		  CString cstrCountry;
		  AfxExtractSubString(cstrCountry, cstr, 0, '|');
		  m_ComboCountry.AddString(cstrCountry);
		}
  }
  else
  {
  }

	m_ComboCountry.SetCurSel(0);
	m_ComboURL.SetCurSel(0);
}

void CiFormaterConvertDlg::UpdateLeadsBalance()
{
  for(std::vector<std::string>::iterator itr = g_pLoginDlg->m_vBal.begin(); itr != g_pLoginDlg->m_vBal.end(); itr++)
  {
    CString cstr((*itr).c_str());
    CString cstrCountry;
    AfxExtractSubString(cstrCountry, cstr, 0, '|');
    if(m_cstrSelectedCountry == cstrCountry)
    {
      CString cstrFullNumber;
      AfxExtractSubString(cstrFullNumber, cstr, 1, '|');
      AfxExtractSubString(m_cstrLeadsLeft, cstrFullNumber, 0, '.');
    }
  }

  InvalidateRect(rcLeadsLeftCount);
  UpdateWindow();
}

void CiFormaterConvertDlg::OnMouseMove(UINT nFlags, CPoint point)
{
  if((point.x >= rcAbout.left && point.x <= rcAbout.right) &&
     (point.y >= rcAbout.top && point.y <= rcAbout.bottom))
  {
    if(m_bErrorDialogOpened == false)
    {
      m_eHoverArea = AREA_ABOUT;
      InvalidateRect(rcAbout);
      UpdateWindow();
    }
  }
  else
  {
    if(m_eHoverArea == AREA_ABOUT)
    {
      m_eHoverArea = AREA_NONE;
      InvalidateRect(rcAbout);
      UpdateWindow();
    }
  }
  if((point.x >= rcHelp.left && point.x <= rcHelp.right) &&
          (point.y >= rcHelp.top && point.y <= rcHelp.bottom))
  {
    if(m_bErrorDialogOpened == false)
    {
      m_eHoverArea = AREA_HELP;
      InvalidateRect(rcHelp);
      UpdateWindow();
    }
  }
  else
  {
    if(m_eHoverArea == AREA_HELP)
    {
      m_eHoverArea = AREA_NONE;
      InvalidateRect(rcHelp);
      UpdateWindow();
    }
  }

  CDialog::OnMouseMove(nFlags, point);
}

BOOL CiFormaterConvertDlg::OnSetCursor(CWnd *pWnd, UINT nHitTest, UINT message)
{
  if(m_eHoverArea == AREA_ABOUT || m_eHoverArea == AREA_HELP)
  {
    ::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_HAND));
    return TRUE;
  }

  return CDialog::OnSetCursor(pWnd, nHitTest, message);
}

void CiFormaterConvertDlg::SetErrorDialogVisible(bool bVisible)
{
  m_bErrorDialogOpened = bVisible;
}

void CiFormaterConvertDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
  if(nID == SC_CLOSE)
  {
    ::PostMessage(g_pLoginDlg->m_hWnd, WM_CLOSE, 0, 0);
  }

  return CDialog::OnSysCommand(nID, lParam);
}

void CiFormaterConvertDlg::ShowFilterPanel(bool bShow)
{
  if(bShow == true)
  {
    if(m_bFilterPanelOpened != true)
    {
      m_bFilterPanelOpened = true;
      InvalidateRect(rcFilterPanel);
      UpdateWindow();

      for(int i=0; i<10; i++)
      {
        m_CheckFilter[i]->ShowWindow(TRUE);
      }
    }
  }
  else
  {
    if(m_bFilterPanelOpened == true)
    {
      m_bFilterPanelOpened = false;
      InvalidateRect(rcFilterPanel);
      UpdateWindow();

      for(int i=0; i<10; i++)
      {
        m_CheckFilter[i]->ShowWindow(FALSE);
      }
    }
  }
}

std::string CiFormaterConvertDlg::GetFilterState()
{
  CString cstrState = _T("");
  
  CString cstrState_0 = _T("0");
  CString cstrState_1 = _T("1");

  for(int i=0; i<10; i++)
  {
    UINT uState = m_CheckFilter[i]->GetCheck();
    if(uState == BST_UNCHECKED)
    {
      cstrState = cstrState + cstrState_0;
    }
    else
    {
      cstrState = cstrState + cstrState_1;
    }
  }

  CT2CA szState(cstrState);
  std::string strState(szState);

  return strState;
}
