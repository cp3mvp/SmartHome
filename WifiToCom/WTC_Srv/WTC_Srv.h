
// WTC_Srv.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CWTC_SrvApp:
// �йش����ʵ�֣������ WTC_Srv.cpp
//

class CWTC_SrvApp : public CWinApp
{
public:
	CWTC_SrvApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CWTC_SrvApp theApp;