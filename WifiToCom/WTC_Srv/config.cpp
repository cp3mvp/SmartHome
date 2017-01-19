#include "stdafx.h"
#include "WTC_Srv.h"
#include "WTC_SrvDlg.h"
#include "afxdialogex.h"
#include "config.h"

BOOL isAuto=FALSE;

//����
CString local_IP;
CString send_IP;
int local_port;
int send_port;
CString net_edit_str="";
//����
long baud;
CString comsel;
CString com_edit_str="";
HANDLE hCom; // ׼���򿪵Ĵ��ڵľ��
HANDLE hCommWatchThread;//�����̵߳�ȫ�ֺ���
CRITICAL_SECTION g_csNET;
CRITICAL_SECTION g_csCOM_edit;
CRITICAL_SECTION g_csCOM_rw;
BOOL dlgCHG_net=FALSE;
BOOL dlgCHG_com=FALSE;
BOOL isCOMOPEN=FALSE;
int CHG_com=0;
int CHG_net=0;
CString test(int x)
{
	CString t;
	t.Format("%d",x);
	return t;
}
CString test(long x)
{
	CString t;
	t.Format("%ld",x);
	return t;
}



CString readdata(){
	CString temp;
	char str[16];
	memset(str,'\0',16);
	DWORD wCount;//��ȡ���ֽ���
	BOOL bReadStat;
	bReadStat=ReadFile(hCom,str,16,&wCount,NULL);
	if(!bReadStat){
		AfxMessageBox("���ڶ�ȡʧ��");
		exit(0);
	}
	PurgeComm(hCom, PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);
	temp.Format("%s",str);
	return temp;
}

BOOL sendtocom(CString sendBuffer)
{
	int len=sendBuffer.GetLength();
	DWORD dwBytesWrite=len;
	COMSTAT ComStat;
	DWORD dwErrorFlags;
	BOOL bWriteStat;
	ClearCommError(hCom,&dwErrorFlags,&ComStat);
	bWriteStat=WriteFile(hCom,(LPCVOID)sendBuffer,dwBytesWrite,&dwBytesWrite,NULL);
	if(!bWriteStat)
	{
		AfxMessageBox("д����ʧ��!");
		return FALSE;
	}
	CString temp;
	temp.Format("\r\n%s\r\n",sendBuffer);
	EnterCriticalSection(&g_csCOM_edit);
	com_edit_str.Append(temp);
	LeaveCriticalSection(&g_csCOM_edit);
	dlgCHG_com=5;
	return TRUE;
}

BOOL sendtonet(CString strSend,SOCKET m_socket)
{
		WSABUF wsabuf;
		DWORD dwSend;
		int len;
		SOCKADDR_IN addrTo;

		addrTo.sin_addr.S_un.S_addr=inet_addr(send_IP);
		addrTo.sin_family=AF_INET;
		addrTo.sin_port=htons(send_port);
		len=strSend.GetLength();
		wsabuf.buf=strSend.GetBuffer(len);
		wsabuf.len=len+1;

		if(SOCKET_ERROR==WSASendTo(m_socket,&wsabuf,1,&dwSend,0,(SOCKADDR*)&addrTo,sizeof(SOCKADDR),NULL,NULL))
		{
			AfxMessageBox("����ʧ��");
			return FALSE;
		}
		else 
		{
			CString st1;
			st1.Format("  To %s : %s\r\n",send_IP,strSend);

			EnterCriticalSection(&g_csNET);
			net_edit_str.Append(st1);
			LeaveCriticalSection(&g_csNET);
			dlgCHG_net=5;
			return TRUE;
		}
}

void dlg_REFRESH(CWTC_SrvDlg* p)
{
	while(TRUE)
	{
		Sleep(1);
		if(dlgCHG_com&&isCOMOPEN)
		{
			int row;
			CString str;
			EnterCriticalSection(&g_csCOM_edit);
			str=com_edit_str;
			LeaveCriticalSection(&g_csCOM_edit);
			p->com_EDIT.SetWindowTextA(str);
			row=p->com_EDIT.GetLineCount();
			p->com_EDIT.LineScroll(row);
			dlgCHG_com--;
		}
		if(dlgCHG_net)
		{
			int row;
			CString str;
			EnterCriticalSection(&g_csNET);
			str=net_edit_str;
			LeaveCriticalSection(&g_csNET);
			p->net_EDIT.SetWindowTextA(str);
			row=p->net_EDIT.GetLineCount();
			p->net_EDIT.LineScroll(row);
			dlgCHG_net--;
		}
		
	}
}

DWORD WINAPI auto_REFRESH(LPVOID lpParameter)
{
	while(TRUE)
	{
		Sleep(50);
		//dlgCHG_com=TRUE;
		//dlgCHG_net=TRUE; 
	}
	return NULL;
}