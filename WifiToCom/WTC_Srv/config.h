extern CString local_IP;
extern CString send_IP;
extern int local_port;
extern int send_port;
extern long baud;
extern CString comsel;
extern HANDLE hCom; // 准备打开的串口的句柄
extern HANDLE hCommWatchThread;//辅助线程的全局函数
extern CString test(long x);
extern CString test(int x);
extern CString readdata();
extern BOOL sendtocom(CString sendBuffer);
extern BOOL sendtonet(CString strSend,SOCKET m_socket);
extern CRITICAL_SECTION g_csNET;
extern CRITICAL_SECTION g_csCOM_edit;
extern CRITICAL_SECTION g_csCOM_rw;
extern BOOL isAuto;
extern CString com_edit_str;
extern CString net_edit_str;
extern BOOL dlgCHG_com;
extern BOOL dlgCHG_net;
extern BOOL isCOMOPEN;
extern int CHG_com;
extern int CHG_net;
extern void dlg_REFRESH(CWTC_SrvDlg *p);
extern DWORD WINAPI auto_REFRESH(LPVOID lpParameter);
