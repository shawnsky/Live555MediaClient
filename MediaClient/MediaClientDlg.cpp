
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

/* Thread function definition */
UINT MyPlayFunction(LPVOID p);
UINT MyRTPFunction(LPVOID p);
UINT MyRTSPFunction(LPVOID p);
/* Thread state */
volatile int rtspThreadState = 0;
volatile int rtpThreadState = 0;
volatile int playerThreadState = 0;


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// CMediaClientDlg 对话框



CMediaClientDlg::CMediaClientDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MEDIACLIENT_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMediaClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMediaClientDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_PLAY, &CMediaClientDlg::OnBnClickedBtnPlay)
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

	m_filename = "tmp_audio.mp3";

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CMediaClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
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
	if (!socket.Create("TCP"))
	{
		// TODO
	}
	if (!socket.Connect("222.31.79.176", 8554))
	{
		// TODO
	}
	//string userInputUrl = "rtsp://47.102.151.23:554/xlt.mp3";
	//string userInputUrl = "rtsp://10.211.55.3:8554/mp3AudioTest";
	string userInputUrl = "rtsp://222.31.79.176:8554/mp3AudioTest";

	RTSPReqHelper rtspHelper;
	rtspHelper.setSocket(socket);
	rtspHelper.setUrl(userInputUrl);
	string resp = rtspHelper.options();
	if (resp.compare("ERROR") == 0)
	{
		// TODO
	}
	resp = rtspHelper.describe();
	if (resp.compare("ERROR") == 0)
	{
		// TODO
	}
	resp = rtspHelper.setup(4588);
	if (resp.compare("ERROR") == 0)
	{
		// TODO
	}
	resp = rtspHelper.play();
	if (resp.compare("ERROR") == 0)
	{
		// TODO
	}
	dlg->m_rtp_thread = AfxBeginThread(MyRTPFunction, dlg);
	
	rtpThreadState = 1;

	for (;;)
	{
		// Terminate thread condition
		if (!rtspThreadState)
		{
			AfxEndThread(0);
		}
		// TODO: Send GET_PARAMTER Timely
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
		// TODO
	}
	if (!socket.Bind(4588))
	{
		// TODO
	}
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
			fs.write(mediaBuff + 16 + 4 + 413, recvCount - 16 - 4 - 413);
			fs.flush();
			isFirstFrame = false;
			// Start VLC Player Thread
			dlg->m_player_thread = AfxBeginThread(MyPlayFunction, dlg, THREAD_PRIORITY_NORMAL, 0, 0, NULL);
			playerThreadState = 1;

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
	m_rtsp_thread = AfxBeginThread(MyRTSPFunction, this);
	rtspThreadState = 1;
}
