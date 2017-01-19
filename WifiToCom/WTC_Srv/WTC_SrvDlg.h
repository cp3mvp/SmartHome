
// WTC_SrvDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#define UM_SOCK WM_USER+1 //定义sock事件


// CWTC_SrvDlg 对话框
class CWTC_SrvDlg : public CDialogEx
{
// 构造
public:
	CWTC_SrvDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_WTC_SRV_DIALOG };

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
	afx_msg LRESULT OnSock(WPARAM,LPARAM);
	DECLARE_MESSAGE_MAP()
public:
	SOCKET m_socket;
	BOOL CWTC_SrvDlg::InitSocket();
	CComboBox com_SEL;
	CComboBox com_BAUD;
	CIPAddressCtrl net_IP;
	CEdit net_PORT;
	CEdit com_MSG;
	CEdit net_MSG;
	CEdit com_EDIT;
	CEdit net_EDIT;
	afx_msg void OnBnClickedNetSend();
	CIPAddressCtrl net_SENDIP;
	CEdit net_SENDPORT;
	afx_msg void OnBnClickedNetSetsend();
	afx_msg void OnBnClickedNetSetport();
	afx_msg void OnBnClickedNetClr();
	CButton com_CLOSE;
	CButton com_OPEN;
	afx_msg void OnBnClickedComOpen();
	afx_msg void OnBnClickedComClose();
	afx_msg void OnBnClickedComSend();
	CButton com_CLR;
	CButton com_SEND;
	afx_msg void OnBnClickedComClr();
	afx_msg void OnBnClickedAuto();
	CButton btn_AUTO;
};
