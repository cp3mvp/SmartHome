
// WTC_SrvDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "WTC_Srv.h"
#include "WTC_SrvDlg.h"
#include "afxdialogex.h"
#include "config.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void CommWatchProc(CWTC_SrvDlg *p);
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


// CWTC_SrvDlg 对话框



CWTC_SrvDlg::CWTC_SrvDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CWTC_SrvDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CWTC_SrvDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COM_SEL, com_SEL);
	DDX_Control(pDX, IDC_COM_BAUD, com_BAUD);
	DDX_Control(pDX, IDC_IP, net_IP);
	DDX_Control(pDX, IDC_PORT, net_PORT);
	DDX_Control(pDX, IDC_COM_MSG, com_MSG);
	DDX_Control(pDX, IDC_NET_MSG, net_MSG);
	DDX_Control(pDX, IDC_COM_EDIT, com_EDIT);
	DDX_Control(pDX, IDC_NET_EDIT, net_EDIT);
	DDX_Control(pDX, IDC_NET_SENDIP, net_SENDIP);
	DDX_Control(pDX, IDC_NET_SENDPORT, net_SENDPORT);
	DDX_Control(pDX, IDC_COM_CLOSE, com_CLOSE);
	DDX_Control(pDX, IDC_COM_OPEN, com_OPEN);
	DDX_Control(pDX, IDC_COM_CLR, com_CLR);
	DDX_Control(pDX, IDC_COM_SEND, com_SEND);
	DDX_Control(pDX, IDC_AUTO, btn_AUTO);
}

BEGIN_MESSAGE_MAP(CWTC_SrvDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_NET_SEND, &CWTC_SrvDlg::OnBnClickedNetSend)
	ON_MESSAGE(UM_SOCK,OnSock)
	ON_BN_CLICKED(IDC_NET_SETSEND, &CWTC_SrvDlg::OnBnClickedNetSetsend)
	ON_BN_CLICKED(IDC_NET_SETPORT, &CWTC_SrvDlg::OnBnClickedNetSetport)
	ON_BN_CLICKED(IDC_NET_CLR, &CWTC_SrvDlg::OnBnClickedNetClr)
	ON_BN_CLICKED(IDC_COM_OPEN, &CWTC_SrvDlg::OnBnClickedComOpen)
	ON_BN_CLICKED(IDC_COM_CLOSE, &CWTC_SrvDlg::OnBnClickedComClose)
	ON_BN_CLICKED(IDC_COM_SEND, &CWTC_SrvDlg::OnBnClickedComSend)
	ON_BN_CLICKED(IDC_COM_CLR, &CWTC_SrvDlg::OnBnClickedComClr)
	ON_BN_CLICKED(IDC_AUTO, &CWTC_SrvDlg::OnBnClickedAuto)
END_MESSAGE_MAP()


// CWTC_SrvDlg 消息处理程序

BOOL CWTC_SrvDlg::OnInitDialog()
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


	InitializeCriticalSection(&g_csNET);
	com_SEL.SetCurSel(0);
	com_BAUD.SetCurSel(0);
	net_PORT.SetWindowTextA("6000");
	net_SENDPORT.SetWindowTextA("6000");
	char name[128];    
	hostent* pHost;   
	gethostname(name,128);//获得主机名    
	pHost=gethostbyname(name);//获得主机结构
	
	local_IP=inet_ntoa(*((in_addr *)pHost->h_addr));
	send_IP="127.0.0.1";
	local_port=6000;
	send_port=6000;
	net_IP.SetWindowTextA(local_IP);
	net_SENDIP.SetWindowTextA(send_IP);
	com_CLOSE.EnableWindow(FALSE);
	com_SEND.EnableWindow(FALSE);
	btn_AUTO.EnableWindow(FALSE);
	InitSocket();
	HANDLE hThread_CHG;
	hThread_CHG=CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)dlg_REFRESH,this,0,NULL);
	CreateThread(NULL,0,auto_REFRESH,NULL,0,NULL);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CWTC_SrvDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CWTC_SrvDlg::OnPaint()
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
HCURSOR CWTC_SrvDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}




BOOL CWTC_SrvDlg::InitSocket()
{
	m_socket=WSASocket(AF_INET,SOCK_DGRAM,0,NULL,0,0);
	if(INVALID_SOCKET==m_socket)
	{
		MessageBox("创建套接字失败");
		return FALSE;
	}
	SOCKADDR_IN addrSock;
	addrSock.sin_addr.S_un.S_addr=htonl(INADDR_ANY);
	addrSock.sin_family=AF_INET;
	addrSock.sin_port=htons(local_port);
	if(SOCKET_ERROR==bind(m_socket,(SOCKADDR*)&addrSock,sizeof(SOCKADDR)))
	{
		MessageBox("绑定失败");
		return FALSE;
	}
	if(SOCKET_ERROR==WSAAsyncSelect(m_socket,m_hWnd,UM_SOCK,FD_READ))
	{
		MessageBox("注册网络读取事件失败");
		return FALSE;
	}
	return TRUE;
}


LRESULT CWTC_SrvDlg::OnSock(WPARAM wParam,LPARAM lParam)
{
	switch (LOWORD(lParam))
	{
	case FD_READ:
		{
			WSABUF wsabuf;
			wsabuf.buf=new char[200];
			wsabuf.len=200;
			DWORD dwRead;
			DWORD dwFlag=0;
			SOCKADDR_IN addrFrom;
			int len=sizeof(SOCKADDR);
			CString str;
			CString strTemp;
			if(SOCKET_ERROR==WSARecvFrom(m_socket,&wsabuf,1,&dwRead,&dwFlag,(SOCKADDR*)&addrFrom,&len,NULL,NULL))
			{
				if(addrFrom.sin_port==local_port)
				{
					MessageBox("接收数据失败!");
					delete[] wsabuf.buf;
					return NULL;
				}
				else
				{
					//MessageBox("检查端口设置!");
					delete[] wsabuf.buf;
					return NULL;
				}
			}
			if(isAuto)
			{
				sendtocom(wsabuf.buf);
			}
			
			CString str_rec=wsabuf.buf;
			if(str_rec=="TEST")
				sendtonet("(SOK)",m_socket);
			str.Format("From %s : %s",inet_ntoa(addrFrom.sin_addr),wsabuf.buf);
			str+="\r\n";
			EnterCriticalSection(&g_csNET);
			net_edit_str.Append(str);
			LeaveCriticalSection(&g_csNET);
			dlgCHG_net=5;	
			delete[] wsabuf.buf;
		}
	default:
		break;
	}
	return NULL;
}


void CWTC_SrvDlg::OnBnClickedNetSend()
{
	if(net_MSG.GetWindowTextLengthA()!=0)
	{
		CString strSend;
		net_MSG.GetWindowTextA(strSend);
		sendtonet(strSend,m_socket);
	}
}


void CWTC_SrvDlg::OnBnClickedNetSetsend()
{
	// TODO: 在此添加控件通知处理程序代码
	net_SENDIP.GetWindowTextA(send_IP);
	send_port=GetDlgItemInt(IDC_NET_SENDPORT);

}


void CWTC_SrvDlg::OnBnClickedNetSetport()
{
	// TODO: 在此添加控件通知处理程序代码
	local_port=GetDlgItemInt(IDC_PORT);
	closesocket(m_socket);
	WSACleanup();
	InitSocket();
}


void CWTC_SrvDlg::OnBnClickedNetClr()
{
	// TODO: 在此添加控件通知处理程序代码
	EnterCriticalSection(&g_csNET);
	net_edit_str="";
	LeaveCriticalSection(&g_csNET);
	net_EDIT.SetWindowTextA("");
}


void CWTC_SrvDlg::OnBnClickedComOpen()
{
	// TODO: 在此添加控件通知处理程序代码
	
	com_SEL.GetWindowTextA(comsel);
	hCom=CreateFile((LPCSTR)comsel,GENERIC_READ|GENERIC_WRITE,  
		0,NULL,OPEN_EXISTING,0,NULL);   //打开串口  
	if(hCom==(HANDLE)-1)
	{
		MessageBox("打开COM失败!");
	}
	else
	{
		com_CLOSE.EnableWindow(TRUE);
		com_OPEN.EnableWindow(FALSE);
		com_BAUD.EnableWindow(FALSE);
		com_SEL.EnableWindow(FALSE);
		com_SEND.EnableWindow(TRUE);
		btn_AUTO.EnableWindow(TRUE);
		
		//SetCommMask(hCom, EV_RXCHAR|EV_TXEMPTY );//设置事件驱动的类型 
		SetupComm( hCom, 100,100) ; //设置输入、输出缓冲区的大小 
		

		COMMTIMEOUTS CommTimeOuts ;
		CommTimeOuts.ReadIntervalTimeout=MAXDWORD; 
		CommTimeOuts.ReadTotalTimeoutMultiplier=0; 
		CommTimeOuts.ReadTotalTimeoutConstant=0; 
		CommTimeOuts.WriteTotalTimeoutMultiplier=100; 
		CommTimeOuts.WriteTotalTimeoutConstant=500; 
		SetCommTimeouts(hCom, &CommTimeOuts);//超时设置

		baud=GetDlgItemInt(IDC_COM_BAUD); 
		DCB dcb; // 定义数据控制块结构
		GetCommState(hCom, &dcb ) ; //读串口原来的参数设置
		dcb.BaudRate =baud;
		dcb.ByteSize =8; 
		dcb.Parity = NOPARITY;
		dcb.StopBits = ONESTOPBIT ;
		SetCommState(hCom, &dcb ) ; //串口参数配置
		PurgeComm( hCom, PURGE_TXCLEAR | PURGE_RXCLEAR ); //清干净输入、输出缓冲区 
		InitializeCriticalSection(&g_csCOM_edit);
		isCOMOPEN=TRUE;
		hCommWatchThread=CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)CommWatchProc,this,0,NULL);
	}

}

void CommWatchProc(CWTC_SrvDlg *p)
{
	CString str;
	CString str_previous;
	int flag=0;
	while(1){
		Sleep(50);
		str=readdata();
		if(str.GetLength()>0)
		{
			if(isAuto)
			{
				CString s1,s2,s3,s4="";
				s1=str.SpanIncluding("(");
				s2=str.SpanIncluding(")");
				if(s1=="("&&s2!=")")
				{
					flag=4;
					while(flag--)
					{
						Sleep(10);
						CString s3=readdata();
						str+=s3;
						if(s3.SpanIncluding(")")==")")flag=1;
					}
				}
					
				sendtonet(str,p->m_socket);				
			}
			EnterCriticalSection(&g_csCOM_edit);
			com_edit_str.Append(str);
			LeaveCriticalSection(&g_csCOM_edit);
			dlgCHG_com=5;			
		}
	}
}

void CWTC_SrvDlg::OnBnClickedComClose()
{
	// TODO: 在此添加控件通知处理程序代码

	if(CloseHandle(hCom)==TRUE)
	{
		TerminateThread(hCommWatchThread,0);
		com_CLOSE.EnableWindow(FALSE);
		com_OPEN.EnableWindow(TRUE);
		com_SEL.EnableWindow(TRUE);
		com_BAUD.EnableWindow(TRUE);
		com_SEND.EnableWindow(FALSE);
		btn_AUTO.EnableWindow(FALSE);
		isCOMOPEN=FALSE;
		DeleteCriticalSection(&g_csCOM_edit);
		
	}
	else{
		MessageBox("关闭串口失败!");
		exit(0);
	}
}


void CWTC_SrvDlg::OnBnClickedComSend()
{
	CString sendBuffer;
	GetDlgItemTextA(IDC_COM_MSG,sendBuffer);
	sendtocom(sendBuffer);
}


void CWTC_SrvDlg::OnBnClickedComClr()
{
	// TODO: 在此添加控件通知处理程序代码
	if(isCOMOPEN)
	{
		EnterCriticalSection(&g_csCOM_edit);
		com_edit_str="";
		LeaveCriticalSection(&g_csCOM_edit);
	}
	else com_edit_str="";
	com_EDIT.SetWindowTextA("");
}


void CWTC_SrvDlg::OnBnClickedAuto()
{
	// TODO: 在此添加控件通知处理程序代码
	if(isAuto)
	{
		isAuto=FALSE;
		btn_AUTO.SetWindowTextA("打开自动转发");
		com_CLOSE.EnableWindow(TRUE);

	}		
	else
	{
		if((local_IP==send_IP||send_IP=="127.0.0.1")&&local_port==send_port)
		{
			MessageBox("网络地址设置引起闭环,请修改!");
			return;
		}
		else
		{
			isAuto=TRUE;
			btn_AUTO.SetWindowTextA("关闭自动转发");
			com_CLOSE.EnableWindow(FALSE);
		}
	}
}
