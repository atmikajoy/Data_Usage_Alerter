
// Alert_Client.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols
#include <string>

// CAlertClientApp:
// See Alert_Client.cpp for the implementation of this class
//

class CAlertClientApp : public CWinApp
{
public:
	CAlertClientApp();

	std::string get_alert_message();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CAlertClientApp theApp;
