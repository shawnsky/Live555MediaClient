﻿
// MediaClientDlg.h: 头文件
//

#pragma once
#include"MySocket.h"


// CMediaClientDlg 对话框
class CMediaClientDlg : public CDialogEx
{
// 构造
public:
	CMediaClientDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MEDIACLIENT_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

private:
	
	
public:
	afx_msg void OnBnClickedBtnPlay();
	CWinThread* m_rtsp_thread;
	CWinThread* m_rtp_thread;
	CWinThread* m_player_thread;
	MySocket m_rtsp_socket;
	MySocket m_rtp_socket;
	std::string m_filename;
	std::string m_url;

	/* Controls */
	CButton m_openBtn;
	CButton m_playBtn;
	CString m_urlText;
	CString m_msgText;

	
	


protected:
	afx_msg LRESULT OnUpdateMsg(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnBnClickedBtnOpen();
protected:
	afx_msg LRESULT OnReadyToPlay(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnEnChangeUrl();
	afx_msg void OnBnClickedBtnQuit();
};
