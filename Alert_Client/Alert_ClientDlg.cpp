
// Alert_ClientDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "Alert_Client.h"
#include "Alert_ClientDlg.h"
#include "afxdialogex.h"
#include <string>
#include <ctime>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// Mon Apr  3 20:26:26 2017
// 012345678901234567890123456789

namespace
{
	std::string curr_time()
	{
		std::time_t t = std::time(nullptr);
		std::string str = std::ctime(std::addressof(t));
		return str.substr(11, 8);
	}
}
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CAlertClientDlg dialog



CAlertClientDlg::CAlertClientDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_ALERT_CLIENT_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CAlertClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ALERTS_LIST, alerts_list_box);
}

BEGIN_MESSAGE_MAP(CAlertClientDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_TEST, &CAlertClientDlg::OnClickedTest)
	ON_COMMAND(ID_ALERT, &CAlertClientDlg::OnAlert)
END_MESSAGE_MAP()


// CAlertClientDlg message handlers

BOOL CAlertClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here


	return TRUE;  // return TRUE  unless you set the focus to a control
}

namespace
{
	std::wstring to_wstring(const std::string& str)
	{
		std::wstring wstr;
		// crude, but adequate for our purposes (our utf-8 is just US ascii)
		for (char c : str) wstr += wchar_t(c);
		return wstr;
	}
}

void CAlertClientDlg::add_alert(const std::string& alert_str)
{
	const std::wstring walert_str = to_wstring( curr_time() ) + L"  " +
		to_wstring(alert_str);
	const int index = alerts_list_box.AddString( walert_str.c_str() );
	alerts_list_box.SetCurSel(index);
	UpdateData(FALSE);
}

void CAlertClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CAlertClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CAlertClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CAlertClientDlg::OnClickedTest()
{
	// TODO: Add your control notification handler code here
	static int n = 0;

	// add_alert( curr_time() + " this is a test alert #" + std::to_string(++n));
}

// handler for menu item ID_ALERT 
// the socket code posts a WM_COMMAND message with the WPARAM == ID_ALERT.
// (thus it fakes a menu command)
void CAlertClientDlg::OnAlert()
{
	// TODO: Add your command handler code here
	add_alert( ((CAlertClientApp*)AfxGetApp())->get_alert_message() );
}
