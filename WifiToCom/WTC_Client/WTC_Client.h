
// WTC_Client.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CWTC_ClientApp:
// �йش����ʵ�֣������ WTC_Client.cpp
//

class CWTC_ClientApp : public CWinApp
{
public:
	CWTC_ClientApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CWTC_ClientApp theApp;