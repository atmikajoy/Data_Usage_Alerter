
// Alert_Client.cpp : Defines the class behaviors for the application.
//

#include "pch.h"
#include "framework.h"
#include "Alert_Client.h"
#include "Alert_ClientDlg.h"
#include "../util/sock_util.h"
#include <mutex>
#include <queue>
#include"../util/config.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace
{
	static const std::string service = atmik::config::get<std::string>("alerter.alert_port", "12345");; // port

	std::queue<std::string> alert_messages;
	std::mutex mutex;

	void error_message( const std::string& msg, std::string prefix = "error" )
	{
		{
			std::lock_guard lock(mutex);
			alert_messages.push( "*** " + prefix + " *** " + msg);
		}

		auto app = (CAlertClientApp*) ::AfxGetApp();
		if (app->m_pMainWnd)
			app->m_pMainWnd->PostMessageW(WM_COMMAND, ID_ALERT);
	}

	void init_socket()
	{
		// this runs in s separate thread because we can't block a ui thread
		// when an alert is recd on the socket, it sends a WM_COMMAND message to 
		// the main window of the app
		WSADATA wsad;
		if (WSAStartup(MAKEWORD(2, 0), &wsad) != 0) return;

		addrinfo hints{};
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM; // tcp/ip
		hints.ai_flags = AI_PASSIVE;

		addrinfo* server_address;
		int result = getaddrinfo("127.0.0.1", service.c_str(), &hints, &server_address);

		if (result != 0) return error_message( "getaddrinfo failed" );

		int sock = socket(server_address->ai_family, server_address->ai_socktype, server_address->ai_protocol);

		if (sock == INVALID_SOCKET) return error_message("socket creation failed");

		result = connect(sock, server_address->ai_addr, server_address->ai_addrlen);

		if (result == SOCKET_ERROR) return error_message("connect failed");;

		
		// this needs to be in a seperate thread

		auto app = (CAlertClientApp*) ::AfxGetApp();

		std::string msg;
		while (recv_string(sock, msg) == 1)
		{
			{
				std::lock_guard lock(mutex);
				alert_messages.push(msg);
		    }

			// post a message to the dialog when an alert arrives
			// (a WM_COMMAND message with the WPARAM == IS_ALERT)
			if(app->m_pMainWnd)
			    app->m_pMainWnd->PostMessageW(WM_COMMAND, ID_ALERT);
			
		}

		error_message( "server stopped", "info" );

		closesocket(sock);
		
		/*//////////////////////////////////////////////////////////////////*/

		freeaddrinfo(server_address);
		// windows specific
		WSACleanup();
	}
}

// CAlertClientApp

BEGIN_MESSAGE_MAP(CAlertClientApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()

std::string CAlertClientApp::get_alert_message()
{
	std::lock_guard lock(mutex);
	if (alert_messages.empty()) return {};
	std::string amsg = std::move(alert_messages.front());
	alert_messages.pop();
	return amsg;
}


// CAlertClientApp construction

CAlertClientApp::CAlertClientApp()
{
	// support Restart Manager
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CAlertClientApp object

CAlertClientApp theApp;


// CAlertClientApp initialization

BOOL CAlertClientApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	AfxEnableControlContainer();

	// Create the shell manager, in case the dialog contains
	// any shell tree view or shell list view controls.
	CShellManager *pShellManager = new CShellManager;

	// Activate "Windows Native" visual manager for enabling themes in MFC controls
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	std::thread(init_socket).detach();

	CAlertClientDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "Warning: dialog creation failed, so application is terminating unexpectedly.\n");
		TRACE(traceAppMsg, 0, "Warning: if you are using MFC controls on the dialog, you cannot #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS.\n");
	}

	// Delete the shell manager created above.
	if (pShellManager != nullptr)
	{
		delete pShellManager;
	}

#if !defined(_AFXDLL) && !defined(_AFX_NO_MFC_CONTROLS_IN_DIALOGS)
	ControlBarCleanUp();
#endif

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

