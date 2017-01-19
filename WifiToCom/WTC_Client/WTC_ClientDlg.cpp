
// WTC_ClientDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "WTC_Client.h"
#include "WTC_ClientDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CWTC_ClientDlg 对话框



CWTC_ClientDlg::CWTC_ClientDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CWTC_ClientDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CWTC_ClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CWTC_ClientDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDCANCEL, &CWTC_ClientDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_SEND, &CWTC_ClientDlg::OnBnClickedSend)
END_MESSAGE_MAP()


// CWTC_ClientDlg 消息处理程序

BOOL CWTC_ClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
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

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	((CIPAddressCtrl*)GetDlgItem(IDC_IP))->SetAddress(127,0,0,1);
	SetDlgItemText(IDC_PORT,"6000");
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CWTC_ClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
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
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CWTC_ClientDlg::OnPaint()
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
HCURSOR CWTC_ClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}




void CWTC_ClientDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialogEx::OnCancel();
}


void CWTC_ClientDlg::OnBnClickedSend()
{
	// TODO: 在此添加控件通知处理程序代码
	SOCKET m_socket;
	DWORD dwIP;
	int port;
	SOCKADDR_IN addrSock;
	CString strSEND;

	m_socket=socket(AF_INET,SOCK_DGRAM,0);
	((CIPAddressCtrl*)GetDlgItem(IDC_IP))->GetAddress(dwIP);
	port=GetDlgItemInt(IDC_PORT);
	addrSock.sin_addr.S_un.S_addr=htonl(dwIP);
	addrSock.sin_family=AF_INET;
	addrSock.sin_port=htons(port);
	GetDlgItemText(IDC_BUF,strSEND); 
	//MessageBox(strSEND);
	
	connect(m_socket,(SOCKADDR*)&addrSock,sizeof(SOCKADDR));
	sendto(m_socket,strSEND,strSEND.GetLength()+1,0,
		(SOCKADDR*)&addrSock,sizeof(SOCKADDR));
	closesocket(m_socket);
}



