
// UDPServer.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.


// CUDPServerApp:
// �� Ŭ������ ������ ���ؼ��� UDPServer.cpp�� �����Ͻʽÿ�.
//

class CUDPServerApp : public CWinApp
{
public:
	CUDPServerApp();

// �������Դϴ�.
public:
	virtual BOOL InitInstance();

// �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};

extern CUDPServerApp theApp;