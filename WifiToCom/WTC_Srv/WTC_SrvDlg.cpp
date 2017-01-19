
// WTC_SrvDlg.cpp : ʵ���ļ�
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
// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CWTC_SrvDlg �Ի���



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


// CWTC_SrvDlg ��Ϣ�������

BOOL CWTC_SrvDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������


	InitializeCriticalSection(&g_csNET);
	com_SEL.SetCurSel(0);
	com_BAUD.SetCurSel(0);
	net_PORT.SetWindowTextA("6000");
	net_SENDPORT.SetWindowTextA("6000");
	char name[128];    
	hostent* pHost;   
	gethostname(name,128);//���������    
	pHost=gethostbyname(name);//��������ṹ
	
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
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CWTC_SrvDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CWTC_SrvDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}




BOOL CWTC_SrvDlg::InitSocket()
{
	m_socket=WSASocket(AF_INET,SOCK_DGRAM,0,NULL,0,0);
	if(INVALID_SOCKET==m_socket)
	{
		MessageBox("�����׽���ʧ��");
		return FALSE;
	}
	SOCKADDR_IN addrSock;
	addrSock.sin_addr.S_un.S_addr=htonl(INADDR_ANY);
	addrSock.sin_family=AF_INET;
	addrSock.sin_port=htons(local_port);
	if(SOCKET_ERROR==bind(m_socket,(SOCKADDR*)&addrSock,sizeof(SOCKADDR)))
	{
		MessageBox("��ʧ��");
		return FALSE;
	}
	if(SOCKET_ERROR==WSAAsyncSelect(m_socket,m_hWnd,UM_SOCK,FD_READ))
	{
		MessageBox("ע�������ȡ�¼�ʧ��");
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
					MessageBox("��������ʧ��!");
					delete[] wsabuf.buf;
					return NULL;
				}
				else
				{
					//MessageBox("���˿�����!");
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	net_SENDIP.GetWindowTextA(send_IP);
	send_port=GetDlgItemInt(IDC_NET_SENDPORT);

}


void CWTC_SrvDlg::OnBnClickedNetSetport()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	local_port=GetDlgItemInt(IDC_PORT);
	closesocket(m_socket);
	WSACleanup();
	InitSocket();
}


void CWTC_SrvDlg::OnBnClickedNetClr()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	EnterCriticalSection(&g_csNET);
	net_edit_str="";
	LeaveCriticalSection(&g_csNET);
	net_EDIT.SetWindowTextA("");
}


void CWTC_SrvDlg::OnBnClickedComOpen()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	
	com_SEL.GetWindowTextA(comsel);
	hCom=CreateFile((LPCSTR)comsel,GENERIC_READ|GENERIC_WRITE,  
		0,NULL,OPEN_EXISTING,0,NULL);   //�򿪴���  
	if(hCom==(HANDLE)-1)
	{
		MessageBox("��COMʧ��!");
	}
	else
	{
		com_CLOSE.EnableWindow(TRUE);
		com_OPEN.EnableWindow(FALSE);
		com_BAUD.EnableWindow(FALSE);
		com_SEL.EnableWindow(FALSE);
		com_SEND.EnableWindow(TRUE);
		btn_AUTO.EnableWindow(TRUE);
		
		//SetCommMask(hCom, EV_RXCHAR|EV_TXEMPTY );//�����¼����������� 
		SetupComm( hCom, 100,100) ; //�������롢����������Ĵ�С 
		

		COMMTIMEOUTS CommTimeOuts ;
		CommTimeOuts.ReadIntervalTimeout=MAXDWORD; 
		CommTimeOuts.ReadTotalTimeoutMultiplier=0; 
		CommTimeOuts.ReadTotalTimeoutConstant=0; 
		CommTimeOuts.WriteTotalTimeoutMultiplier=100; 
		CommTimeOuts.WriteTotalTimeoutConstant=500; 
		SetCommTimeouts(hCom, &CommTimeOuts);//��ʱ����

		baud=GetDlgItemInt(IDC_COM_BAUD); 
		DCB dcb; // �������ݿ��ƿ�ṹ
		GetCommState(hCom, &dcb ) ; //������ԭ���Ĳ�������
		dcb.BaudRate =baud;
		dcb.ByteSize =8; 
		dcb.Parity = NOPARITY;
		dcb.StopBits = ONESTOPBIT ;
		SetCommState(hCom, &dcb ) ; //���ڲ�������
		PurgeComm( hCom, PURGE_TXCLEAR | PURGE_RXCLEAR ); //��ɾ����롢��������� 
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������

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
		MessageBox("�رմ���ʧ��!");
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(isAuto)
	{
		isAuto=FALSE;
		btn_AUTO.SetWindowTextA("���Զ�ת��");
		com_CLOSE.EnableWindow(TRUE);

	}		
	else
	{
		if((local_IP==send_IP||send_IP=="127.0.0.1")&&local_port==send_port)
		{
			MessageBox("�����ַ��������ջ�,���޸�!");
			return;
		}
		else
		{
			isAuto=TRUE;
			btn_AUTO.SetWindowTextA("�ر��Զ�ת��");
			com_CLOSE.EnableWindow(FALSE);
		}
	}
}
