﻿
// MediaClientDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "MediaClient.h"
#include "MediaClientDlg.h"
#include "afxdialogex.h"
#include "RTSPReqHelper.h"
#include "MyVlcPlayer.h"
#include <iostream>
#include <fstream>

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/* User messages definition */
#define WM_UPDATE_MSG WM_USER + 100
#define WM_READY_TO_PLAY WM_UPDATE_MSG + 1

/* Thread function definition */
UINT MyPlayFunction(LPVOID p);
UINT MyRTPFunction(LPVOID p);
UINT MyRTSPFunction(LPVOID p);
/* Thread state */
volatile int rtspThreadState = 0;
volatile int rtpThreadState = 0;
volatile int playerThreadState = 0;



// CMediaClientDlg 对话框



CMediaClientDlg::CMediaClientDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MEDIACLIENT_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMediaClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BTN_PLAY, m_playBtn);
	DDX_Control(pDX, IDC_BTN_OPEN, m_openBtn);
	DDX_Text(pDX, IDC_URL, m_urlText);
	DDX_Text(pDX, IDC_MSG, m_msgText);
}

BEGIN_MESSAGE_MAP(CMediaClientDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_PLAY, &CMediaClientDlg::OnBnClickedBtnPlay)
	ON_MESSAGE(WM_UPDATE_MSG, &CMediaClientDlg::OnUpdateMsg)
	ON_BN_CLICKED(IDC_BTN_OPEN, &CMediaClientDlg::OnBnClickedBtnOpen)
	ON_MESSAGE(WM_READY_TO_PLAY, &CMediaClientDlg::OnReadyToPlay)
	ON_EN_CHANGE(IDC_URL, &CMediaClientDlg::OnEnChangeUrl)
	ON_BN_CLICKED(IDC_BTN_QUIT, &CMediaClientDlg::OnBnClickedBtnQuit)
END_MESSAGE_MAP()


// CMediaClientDlg 消息处理程序

BOOL CMediaClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	m_filename = "tmp_audio";
	GetDlgItem(IDC_BTN_PLAY)->EnableWindow(false);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CMediaClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMediaClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMediaClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
/*
	RTSP Socket Thread Function
*/
UINT MyRTSPFunction(LPVOID p)
{
	for (;;) if (rtspThreadState) break;
	
	CMediaClientDlg* dlg = (CMediaClientDlg*)p;
	MySocket socket = dlg->m_rtsp_socket;
	string url = dlg->m_url;
	if (!socket.Create("TCP"))
	{
		CString* ps = new CString("[ERROR] Fail to initialize!\r\n");
		PostMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_UPDATE_MSG, NULL, (LPARAM)ps);
		AfxEndThread(0);
	}
	if (url.size() == 0)
	{
		CString* ps = new CString("[ERROR] Invalid url!\r\n");
		PostMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_UPDATE_MSG, NULL, (LPARAM)ps);
		AfxEndThread(0);
	}
	// Extract IP & Port from url
	int ipBeg = url.find_first_of("//");
	int ipEnd = url.find_first_of(":", ipBeg);
	int portEnd = url.find_first_of("/", ipEnd);
	string str1, str2;
	char *serverIp = "";
	int serverPort;
	try
	{
		str1 = url.substr(ipBeg + 2, ipEnd - ipBeg - 2);
		str2 = url.substr(ipEnd + 1, portEnd - ipEnd - 1);
		serverIp = (char*)str1.c_str();
		serverPort = std::stoi(str2);
	}
	catch (const std::exception&)
	{
		CString* ps = new CString("[ERROR] Invalid url!\r\n");
		PostMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_UPDATE_MSG, NULL, (LPARAM)ps);
		AfxEndThread(0);
	}

	// Resolve domain to IP
	gethostname(serverIp, sizeof(serverIp));
	hostent *host = gethostbyname(serverIp);
	char *ip = inet_ntoa(*(in_addr*)host->h_addr_list[0]);

	if (!socket.Connect(ip, serverPort))
	{
		CString* ps = new CString("[ERROR] Can not connect to server!\r\n");
		PostMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_UPDATE_MSG, NULL, (LPARAM)ps);
		AfxEndThread(0);
	}
	
	RTSPReqHelper rtspHelper;
	rtspHelper.setSocket(socket);
	rtspHelper.setUrl(dlg->m_url);

	string resp = rtspHelper.options();
	if (resp.compare("ERROR") == 0)
	{
		CString* ps = new CString("[ERROR] Failed to OPTIONS!\r\n");
		PostMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_UPDATE_MSG, NULL, (LPARAM)ps);
		AfxEndThread(0);
	}
	resp = rtspHelper.describe();
	if (resp.compare("ERROR") == 0)
	{
		CString* ps = new CString("[ERROR] Failed to DESCRIBE! Please try other url.\r\n");
		PostMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_UPDATE_MSG, NULL, (LPARAM)ps);
		AfxEndThread(0);
	}
	resp = rtspHelper.setup(4588);
	if (resp.compare("ERROR") == 0)
	{
		CString* ps = new CString("[ERROR] Failed to SETUP!\r\n");
		PostMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_UPDATE_MSG, NULL, (LPARAM)ps);
		AfxEndThread(0);
	}
	resp = rtspHelper.play();
	if (resp.compare("ERROR") == 0)
	{
		CString* ps = new CString("[ERROR] Failed to PLAY!\r\n");
		PostMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_UPDATE_MSG, NULL, (LPARAM)ps);
		AfxEndThread(0);
	}

	// Start RTP packet receiver Thread
	dlg->m_rtp_thread = AfxBeginThread(MyRTPFunction, dlg);
	

	CString* ps = new CString("[INFO] Request media resource successful!\r\n");
	PostMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_UPDATE_MSG, NULL, (LPARAM)ps);

	rtpThreadState = 1;



	for (;;)
	{
		// Terminate thread condition
		if (!rtspThreadState)
		{
			AfxEndThread(0);
		}
		// TODO: Create a new thread to Send GET_PARAMTER Timely
		Sleep(1000 * 30);
		resp = rtspHelper.getParameter();
	}
	
	return 0;
}
/*
	RTP Socket Thread Function
*/
UINT MyRTPFunction(LPVOID p)
{
	for (;;) if (rtpThreadState) break;

	CMediaClientDlg* dlg = (CMediaClientDlg*)p;
	MySocket socket = dlg->m_rtsp_socket;
	fstream fs(dlg->m_filename, ios_base::binary | ios_base::out | ios_base::trunc);
	char mediaBuff[2048] = { 0 };
	bool isFirstFrame = true;
	if (!socket.Create("UDP"))
	{
		CString* ps = new CString("[ERROR] Failed to setup UDP socket!\r\n");
		PostMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_UPDATE_MSG, NULL, (LPARAM)ps);
		AfxEndThread(0);
	}
	if (!socket.Bind(4588))
	{
		CString* ps = new CString("[ERROR] Failed to bind UDP address!\r\n");
		PostMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_UPDATE_MSG, NULL, (LPARAM)ps);
		AfxEndThread(0);
	}

	CString* ps = new CString("[INFO] Begin to receive stream...\r\n");
	PostMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_UPDATE_MSG, NULL, (LPARAM)ps);

	for (;;)
	{
		// Terminate thread condition
		if (!rtpThreadState)
		{
			AfxEndThread(0);
		}
		int recvCount = socket.UDPRecv(mediaBuff, sizeof(mediaBuff));
		if (isFirstFrame)
		{
			fs.write(mediaBuff + 16, recvCount - 16);
			fs.flush();
			isFirstFrame = false;
			// Start VLC Player Thread
			dlg->m_player_thread = AfxBeginThread(MyPlayFunction, dlg, THREAD_PRIORITY_NORMAL, 0, 0, NULL);
			CString* ps = new CString("[INFO] Press [Play] to play!\r\n");
			PostMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_UPDATE_MSG, NULL, (LPARAM)ps);
			PostMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_READY_TO_PLAY, NULL, NULL);

		}
		else
		{
			fs.write(mediaBuff + 16, recvCount - 16);
			fs.flush();
		}
	}
	fs.close();
	return 0;
}
/*
	VLC Play Media file Thread Function
*/
UINT MyPlayFunction(LPVOID p)
{
	for (;;) if (playerThreadState) break;

	CString* ps = new CString("[INFO] Start playing...\r\n");
	PostMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_UPDATE_MSG, NULL, (LPARAM)ps);

	CMediaClientDlg* dlg = (CMediaClientDlg*)p;

	MyVlcPlayer player;
	player.Initialize(dlg->m_filename);
	player.Play();


	for (;;)
	{
		// Terminate thread condition
		if (!playerThreadState)
		{
			AfxEndThread(0);
		}
	}
	
	return 0;
}




void CMediaClientDlg::OnBnClickedBtnPlay()
{
	// TODO: 在此添加控件通知处理程序代码
	playerThreadState = 1;
	
}


afx_msg LRESULT CMediaClientDlg::OnUpdateMsg(WPARAM wParam, LPARAM lParam)
{
	CString* msg = (CString*)lParam;
	CString str;
	GetDlgItem(IDC_MSG)->GetWindowText(str);
	str += *msg;
	GetDlgItem(IDC_MSG)->SetWindowText(str);

	
	return 0;
}


void CMediaClientDlg::OnBnClickedBtnOpen()
{
	// TODO: 在此添加控件通知处理程序代码
	m_rtsp_thread = AfxBeginThread(MyRTSPFunction, this);

	rtspThreadState = 1;

}





afx_msg LRESULT CMediaClientDlg::OnReadyToPlay(WPARAM wParam, LPARAM lParam)
{
	GetDlgItem(IDC_BTN_PLAY)->EnableWindow(true);
	return 0;
}


void CMediaClientDlg::OnEnChangeUrl()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	CString str;
	GetDlgItem(IDC_URL)->GetWindowText(str);
	m_url = CStringA(str);

	
}


void CMediaClientDlg::OnBnClickedBtnQuit()
{
	// TODO: 在此添加控件通知处理程序代码

	rtpThreadState = 0;
	playerThreadState = 0;
	m_rtsp_socket.Close();
	m_rtp_socket.Close();
	PostMessage(WM_QUIT, 0, 0);
}
