
// Alert_ClientDlg.h : header file
//

#pragma once

#include <string>

// CAlertClientDlg dialog
class CAlertClientDlg : public CDialogEx
{
// Construction
public:
	CAlertClientDlg(CWnd* pParent = nullptr);	// standard constructor

	void add_alert(const std::string& alert_str);

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ALERT_CLIENT_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CListBox alerts_list_box;
	afx_msg void OnClickedTest();
	afx_msg void OnAlert();
};
